/* -*- C++ -*- */

//=============================================================================
/**
 * @file   Time_Handler.cpp
 * @author Tad Kollar  
 *
 * $Id: Time_Handler.cpp 2082 2013-06-13 13:22:28Z tkollar $
 * Copyright (c) 2008.
 *      NASA Glenn Research Center.  All rights reserved.
 */
//=============================================================================

#include "Time_Handler.hpp"
#include "DelayedResults.hpp"

#include <ace/Date_Time.h>

using namespace std;
using namespace nasaCE;

ScenarioTime::TimeType ScenarioTime::parseTime(const string& user_time,
	ACE_Time_Value& tv) {
	using namespace boost;

	TimeType tt = generic;

	// Absolute time examples:
	// @2009-01-20 10:20:01.231
	// =2008-06-15 13:15:05.500
	// Match results:   1=tt  2=year    3=month  4=day       5=hour   6=minute 7=second    8=microsecond
	regex abs_time_re("^([@=])(\\d{4})-(\\d{2})-(\\d{2})\\s+(\\d{2}):(\\d{2}):(\\d{2})\\.?(\\d+)*$");

	// Offset time examples:
	// T30:10:05.250
	// +10:15:10.700
	// Match results: 1=tt    2=hours  3=mins   4=seconds   5=microseconds
	regex offset_re("^([T\\+])(\\d{2}):(\\d{2}):(\\d{2})\\.?(\\d+)*$");

	match_results<string::const_iterator> what;

	if (regex_match(user_time, what, abs_time_re)) {
		ND_DEBUG("User provided absolute time (%s).\n",	user_time.c_str());

		tt = (what[1].str() == "@") ? scenario : wall;

		std::tm dt;
		dt.tm_year = it2int(what[2]) - 1900;
		dt.tm_mon = it2int(what[3]) - 1;
		dt.tm_mday = it2int(what[4]);
		dt.tm_hour = it2int(what[5]);
		dt.tm_min = it2int(what[6]);
		dt.tm_sec = it2int(what[7]);
		tv.sec(ACE_OS::mktime(&dt));
		tv.usec(it2int(what[8]));

		ND_DEBUG("Parsed: yr=%d mo=%d day=%d hr=%d min=%d s=%d us=%d. Double-checking asctime(): %s",
			dt.tm_year + 1900, dt.tm_mon + 1, dt.tm_mday, dt.tm_hour,
			dt.tm_min, dt.tm_sec, tv.usec(), ACE_OS::asctime(&dt));
	}
	else if (regex_match(user_time, what, offset_re)) {
		ND_DEBUG("User provided offset time (%s).\n", user_time.c_str());
		tt = (what[1].str() == "T") ? scenario_offset : wall_offset;
		tv.sec(it2int(what[2])*3600 + it2int(what[3])*60 + it2int(what[4]));
		tv.usec(it2int(what[5]));
	}
	else {
		throw Exception("User provided time '" + user_time +
			"' did not match accepted time patterns.");
	}

	ND_DEBUG("tv updated to %ds, %dus\n", tv.sec(), tv.usec());

	return tt;
}

ACE_Time_Value ScenarioTime::calculateAlarmTime(TimeType time_type,
	const ACE_Time_Value& relative_tv) {
	ACE_Time_Value actual_tv;

	switch (time_type) {
		case scenario:
			actual_tv = relative_tv - scenarioNow();
			break;
		case scenario_offset:
			actual_tv = relative_tv - _scenarioWallclockAtZeroTime;
			break;
		case wall:
			ND_DEBUG("Now: %ds, %dus; Then: %ds, %dus\n",
				now().sec(), now().usec(), relative_tv.sec(),
				relative_tv.usec());
			actual_tv = relative_tv - now();
			break;
		case wall_offset:
		case generic:
			actual_tv = relative_tv;
			break;
		case error:
			break;
	}

	if ( actual_tv.sec() < 0 )
		throw Exception("Execution time in the past, not scheduling alarm.");

	ND_INFO("Alarm will execute %d.%d seconds from now.\n",
		actual_tv.sec(), actual_tv.usec());

	return actual_tv;
}

ACE_Time_Value ScenarioTime::calculateAlarmTime(const string& user_time) {
	ACE_Time_Value relative_tv;
	TimeType time_type = parseTime(user_time, relative_tv);

	return calculateAlarmTime(time_type, relative_tv);
}

int Time_Handler::handle_timeout (const ACE_Time_Value &tv, const void *arg) {
		string* timer_name = (string*) arg;
		ND_DEBUG("Executing delayed request using timer named %s.\n",
			timer_name->c_str());
		ACE_Recursive_Thread_Mutex mutex(_method_name.c_str());
		string responseXml;

		xmlrpc_c::value* retvalP;
		ND_INFO("Timed command %s executing now.\n", _method_name.c_str());
		try {
			retvalP = new xmlrpc_c::value;

			mutex.acquire();
			xmlrpc_server::instance()->rpcRegistry->processCall(_call_XML,
				&responseXml);
			mutex.release();

			xmlrpc_c::xml::parseSuccessfulResponse(responseXml, retvalP);

			delayed_results::instance()->add_delayed_result(*timer_name, retvalP);
		}
		catch(exception& e) {
			ND_ERROR("Timed command %s failed: %s\n", _method_name.c_str(), e.what());
		}

		xmlrpc_server::instance()->clearTimer(*timer_name, true);

		return -1;
}
