/* -*- C++ -*- */

//=============================================================================
/**
 * @file   DelayedResults.cpp
 * @author Tad Kollar  
 *
 * $Id: DelayedResults.cpp 1671 2012-08-28 15:19:00Z tkollar $ 
 * Copyright (c) 2008.
 *      NASA Glenn Research Center.  All rights reserved.
 */
//=============================================================================

#include "DelayedResults.hpp"
#include "SettingsManager.hpp"

#include <ace/Log_Msg.h>
#include <ace/OS_main.h>
#include <ace/Date_Time.h>

using namespace nasaCE;

/// Erase the oldest result if ResultRetentionLimit is not set to -1
void DelayedResults::_trim() {
	int maxSize = 100; // Initialize in case it's not in the config file
	if (CEcfg::instance()->exists("Server.resultRetentionLimit"))
		maxSize = CEcfg::instance()->get("Server.resultRetentionLimit");
	
	ACE_Process_Mutex mutex("delayed_results");
	mutex.acquire();
	if ( maxSize > 0 && (int) _delayed_results.size() > maxSize ) {
		// _delayed_results.sort(); // shouldn't be necessary unless there are race problems
		// const char *tname = _delayed_results.front().timer_name.c_str();
		const ACE_Date_Time ctime(_delayed_results.front().time_completed);

		// Dump the entire contents of the result list to debug and make sure 
		// that the oldest is really the one getting dropped.
		// ResultList::iterator pos;		
		// for (pos = _delayed_results.begin(); pos != _delayed_results.end(); ++pos) {
		// 	AND_DEBUG((LM_DEBUG, "Name: %s, completed: %d\n", pos->timer_name.c_str(), pos->time_completed.sec()));	
		// }	
		
		ND_DEBUG("Trimming oldest result (completed at %02d:%02d:%02d.%02d).\n",
			ctime.hour(), ctime.minute(), ctime.second(), ctime.microsec());
		
		_delayed_results.pop_front();
	}
	mutex.release();
}

void DelayedResults::add_delayed_result(const std::string& timer_name,
	xmlrpc_c::value* retvalP) {
	ACE_Time_Value now(ACE_OS::gettimeofday());
	ACE_Date_Time ctime(now);
	Result res(timer_name, now , retvalP);
	ACE_Process_Mutex mutex("delayed_results");
	
	mutex.acquire();
	_delayed_results.push_back(res);
	mutex.release();
	
	ND_DEBUG("Added completed result for %s at %02d:%02d:%02d.%02d (%d unretrieved total)\n",
		timer_name.c_str(),	ctime.hour(), ctime.minute(), ctime.second(),
		ctime.microsec(), _delayed_results.size());
	_trim();
}

void DelayedResults::yank_delayed_result(const std::string& timer_name,
	xmlrpc_c::value* const retvalP) {

	ACE_Process_Mutex mutex("delayed_results");
	mutex.acquire();
	
	ResultList::iterator pos = _delayed_results.find(timer_name);
	if ( pos == _delayed_results.end() ) {
		mutex.release();
		std::string msg = "Cannot get result for timer identified by " + timer_name +
			": not found (timer not triggered or non-existant, or too many " + 
			"results built up).";

		throw new Exception(msg);
	}

	*retvalP = *pos->retvalP;
	_delayed_results.erase(pos);

	mutex.release();
}

void DelayedResults::yank_delayed_result(xmlrpc_c::paramList const& paramList,
	xmlrpc_c::value* const retvalP) {
	std::string timer_name = (std::string) paramList.getString(0);
	ND_DEBUG("Got request for delayed result with timer name %s.\n",
		timer_name.c_str());

	yank_delayed_result(timer_name, retvalP);
}
