/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modEmulateRate.hpp
 * @author Tad Kollar  
 *
 * $Id: modEmulateRate.hpp 1577 2012-06-08 19:03:34Z tkollar $
 * Copyright (c) 2010.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _MOD_EMULATE_RATE_HPP_
#define _MOD_EMULATE_RATE_HPP_

#include <BaseTrafficHandler.hpp>
#include <ace/Event_Handler.h>
#include "TimeLine.hpp"
#include "Time_Handler.hpp"

namespace nEmulateRate {

//=============================================================================
/**
 * @class modEmulateRate
 * @author Tad Kollar  
 * @brief Wait after sending depending on size of previous traffic unit.
 */
//=============================================================================
class modEmulateRate : public BaseTrafficHandler {

public:

	/// Primary constructor.
	/// @param newName An identifier for the module that's unique within the channel.
	/// @param newChannelName An identifier for the containing channel.
	/// @param newDLLPtr Pointer to the DLL management structure.
    modEmulateRate(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr);

	/// Destructor.
    ~modEmulateRate();

	/// Reads in data, slows it down, and sends it to the output handler.
	int svc();

	/// Read-only accessor to _rateLimit if there is no time line. Otherwise,
	/// return the rate corresponding to the current time.
	int getRateLimit() const {
		return ( _rateTimeLine ) ? _rateTimeLine->get(scenarioTimeKeeper::instance()->elapsed(), _timeLineCycleSecs) : _rateLimit;
	}

	/// Return the rate at the specified time (relative to the start of the simulation).
	/// @param atTime The time value to check the rate for.
	/// @return The rate limit in kbit/s at the specified time.
	int getRateLimitAtTime(const double& atTime) {
		return ( _rateTimeLine ) ? _rateTimeLine->get(ACE_Time_Value(atTime), _timeLineCycleSecs) : _rateLimit;
	}

	/// Write-only accessor to _rateLimit and _rateLimitSetting.
	/// @param newVal The value in kbit/s to set the new rate to.
	/// @param startTime The scenario time in seconds when the new rate should go into effect. If not provided or not > 0.0, the default rate is set instead.
	/// @throw ValueTooSmall If newVal is negative.
	void setRateLimit(const int& newVal, const double& startTime = -1.0);

	/// Read-only accessor to _unlimitedThroughput.
	int getUnlimitedThroughput() const { return _unlimitedThroughput; }

	/// Write-only accessor to _unlimitedThroughput and _unlimitedThroughputSetting.
	/// @throw ValueTooSmall If newVal is negative.
	void setUnlimitedThroughput(const int& newVal) {
		if ( newVal < 0 ) throw ValueTooSmall(ValueTooSmall::msg("Unlimited Throughput", newVal, 0));

		_unlimitedThroughput = newVal;
		_unlimitedThroughputSetting = static_cast<int>(_unlimitedThroughput);
	}

	/// Read-only accessor to _timeLineCycleSecs.
	double getTimeLineCycleSecs() const { return _timeLineCycleSecs; }

	/// Write-only accessor to _timeLineCycleSecs and _timeLineCycleSecsSetting.
	/// @throw ValueTooSmall If newVal is negative.
	void setTimeLineCycleSecs(const double& newVal) {
		if ( newVal < 0 ) throw ValueTooSmall(ValueTooSmall::msg("Time Line Cycle Seconds", newVal, 0));

		_timeLineCycleSecs = newVal;
		_timeLineCycleSecsSetting = newVal;
	}

	/// Erase the time line and only use _rateLimit.
	void clearTimeLine();

private:
	/// The maximum rate in kbit/s at which to release data.
	int _rateLimit;

	/// Config file setting associated with _rateLimit.
	Setting& _rateLimitSetting;

	/// What the throughput would be in kbit/s without this segment.
	int _unlimitedThroughput;

	/// Config file setting associated with _unlimitedThroughput.
	Setting& _unlimitedThroughputSetting;

	/// List of time, rate pairs that defines what rate applies starting at what time.
	TimeLine<int>* _rateTimeLine;

	/// How often does the time line repeat.
	double _timeLineCycleSecs;

	/// Config file reference to _timeLineCycleLen.
	Setting& _timeLineCycleSecsSetting;

}; // class modEmulateRate

} // namespace nEmulateRate

#endif // _MOD_EMULATE_RATE_HPP_

