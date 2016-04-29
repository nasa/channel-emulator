/* -*- C++ -*- */

//=============================================================================
/**
 * @file   Time_Handler.hpp
 * @author Tad Kollar  
 *
 * $Id: Time_Handler.hpp 1671 2012-08-28 15:19:00Z tkollar $
 * Copyright (c) 2008.
 *      NASA Glenn Research Center.  All rights reserved.
 */
//=============================================================================

#ifndef _TIME_HANDLER_HPP_
#define _TIME_HANDLER_HPP_

#include "XML_RPC_Server.hpp"

#include <boost/regex.hpp>

#include <ace/Event_Handler.h>
#include <ace/High_Res_Timer.h>
#include <ace/Timer_Queue_T.h>
#include <ace/Singleton.h>

#include <xmlrpc-c/xml.hpp>

namespace nasaCE {

/**
 * @class ScenarioTime
 *
 * @brief Calculates time based on one of several user-specified formats.
 *
 * If the std::string is prefaced with:
 *   - <i>@@</i> represents an absolute scenario time.
 *     e.g. @@2040-03-25 13:00:00.00 == at 1 PM on March 24, 2040 (but relative to scenario time)
 *   - <i>=</i> represents an absolute wall clock time
 *     e.g. =2008-03-25 13:00:00.000 means at 1 PM today (i.e. if today is March 25, 2008)
 *   - <i>T</i> means relative to zero time
 *     e.g. T13:05:01.500 = 13 hours, 5 mins, 1 second, 500 microseconds from the start of the scenario
 *   - <i>+</i> only means relative to *now*
 *     e.g. +13:00:00.000 = 13 hours from time command was received
 */
class ScenarioTime {
private:
	/// @brief Scheduled events are timed from this value.
	///
	/// If not set by the user, it is equal to the time that the daemon started.
	ACE_Time_Value _zeroTime;

	/// @brief The simulated time that _true_zeroTime represents.
	///
	/// For example, zero time may be set to July 1, 2008. That may actually
	/// represent March 15, 2040 in the scenario - that second value is stored
	/// here. This is the value that the client provides relative times from.
	/// It <i>defaults</i> to _zeroTime.
	ACE_Time_Value _scenarioWallclockAtZeroTime;

public:

	enum TimeType { scenario = 1, wall = 2, scenario_offset = 3, wall_offset = 4, generic = 5, error = 6 };

	/// Default constructor.
	ScenarioTime() { }

	~ScenarioTime() {
		ACE_TRACE("ScenarioTime::~ScenarioTime");
	}

	void initialize() {
		_zeroTime = now();
		_scenarioWallclockAtZeroTime = _zeroTime;
	}

	/// Convert the string pointed to be the iterator to an int.
	int it2int(const boost::sub_match<std::string::const_iterator>& sm) {
		return ACE_OS::atoi(sm.str().c_str());
	}

	const ACE_Time_Value& zeroTime() { return _zeroTime; }
	const ACE_Time_Value& simulatedTime() { return _scenarioWallclockAtZeroTime; }

	void zeroTime(const ACE_Time_Value& newZTime, bool sTimeIsSame = false) {
		_zeroTime = newZTime;
		if ( sTimeIsSame ) { _scenarioWallclockAtZeroTime = newZTime; }
	}

	void zeroTime(const ACE_Time_Value& new_ztime_tv, const ACE_Time_Value& new_stime_tv) {
		_zeroTime = new_ztime_tv;
		_scenarioWallclockAtZeroTime = new_stime_tv;
	}

	void zeroTime(const std::string& new_ztime_str, const std::string& new_stime_str) {
		ACE_Time_Value new_ztime_tv, new_stime_tv;

		parseTime(new_ztime_str, new_ztime_tv);
		parseTime(new_stime_str, new_stime_tv);

		zeroTime(new_ztime_tv, new_stime_tv);
	}

	void simulatedTime(const ACE_Time_Value& new_stime) { _scenarioWallclockAtZeroTime = new_stime; }

	ACE_Time_Value now() { return ACE_Time_Value(ACE_High_Res_Timer::gettimeofday_hr()); }

	/// Return the amount of time that's passed since the start of the scenario.
	ACE_Time_Value elapsed() {
		ACE_Time_Value elapsed(ACE_High_Res_Timer::gettimeofday_hr());
		elapsed -= _zeroTime;
		return elapsed;
	}

	ACE_Time_Value scenarioNow() {
		return _scenarioWallclockAtZeroTime + elapsed();
	}

	TimeType parseTime(const std::string&, ACE_Time_Value&);

	ACE_Time_Value calculateAlarmTime(TimeType, const ACE_Time_Value&);
	ACE_Time_Value calculateAlarmTime(const std::string&);
};

typedef ACE_Singleton<ScenarioTime, ACE_Recursive_Thread_Mutex> scenarioTimeKeeper;

/**
 * @class Time_Handler
 *
 * @brief Passed to schedule_timer() in the Reactor.
 *
 * Knows how to type cast the void pointer and call the appropriate hook.
 */
class Time_Handler: public ACE_Event_Handler {
private:
	std::string _method_name;
	std::string _call_XML;

	void _do_call();

public:
	Time_Handler(const std::string& mname, const std::string& callXml):
		_method_name(mname), _call_XML(callXml) {}

    /// Method which is called back by the Reactor when timeout occurs.
	virtual int handle_timeout (const ACE_Time_Value &, const void*);
};

} // namespace nasaCE

#endif // _TIME_HANDLE_HPP_
