/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modEmulateDelay.hpp
 * @author Tad Kollar  
 *
 * $Id: modEmulateDelay.hpp 1580 2012-06-08 19:48:58Z tkollar $
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _MOD_EMULATE_DELAY_HPP_
#define _MOD_EMULATE_DELAY_HPP_

#include <BaseTrafficHandler.hpp>
#include <ace/Event_Handler.h>
#include "TimeLine.hpp"
#include "Time_Handler.hpp"

namespace nEmulateDelay {

typedef std::pair<double, double> DelayAndJitterT;

//=============================================================================
/**
 * @class modEmulateDelay
 * @author Tad Kollar  
 * @brief Holds units for a specified amount of time and then send on.
 */
//=============================================================================
class modEmulateDelay : public BaseTrafficHandler {

public:

	/// Primary constructor.
	/// @param newName An identifier for the module that's unique within the channel.
	/// @param newChannelName An identifier for the containing channel.
	/// @param newDLLPtr Pointer to the DLL management structure.
    modEmulateDelay(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr);

	/// Destructor.
    ~modEmulateDelay();

	/// Reads in data, possibly introduces error(s), and sends it to the output handler.
	int svc();

	/// Read-only accessor to _delaySeconds.
	double getDelaySeconds() const {
		return ( _timeLine ) ?
			_timeLine->get(scenarioTimeKeeper::instance()->elapsed(), _timeLineCycleSecs).first :
			_delaySeconds;
	}

	/// Return the delay at the specified time (relative to the start of the simulation).
	/// @param atTime The time value to check the delay for.
	/// @return The delay in seconds at the specified time.
	double getDelaySecondsAtTime(const double& atTime) {
		return ( _timeLine) ?
			_timeLine->get(ACE_Time_Value(atTime), _timeLineCycleSecs).first :
			_delaySeconds;
	}

	/// Read-only accessor to _jitterSeconds.
	double getJitterSeconds() const {
		return ( _timeLine ) ?
			_timeLine->get(scenarioTimeKeeper::instance()->elapsed(), _timeLineCycleSecs).second :
			_jitterSeconds;
	}

	/// Return the jitter at the specified time (relative to the start of the simulation).
	/// @param atTime The time value to check the jitter for.
	/// @return The jitter in seconds at the specified time.
	double getJitterSecondsAtTime(const double& atTime) {
		return ( _timeLine ) ?
			_timeLine->get(ACE_Time_Value(atTime), _timeLineCycleSecs).second :
			_jitterSeconds;
	}

	/// Write-only accessor to _delaySeconds and _delaySecondsSetting.
	/// @param newDelay The value in seconds to set the new delay to.
	/// @param newJitter The value in seconds to set the new jitter to.
	/// @param startTime The scenario time in seconds when the new delay/jitter should go into effect. If not provided or not > 0.0, the default delay/jitter is set instead.
	/// @throw ValueTooSmall If either value is negative.
	void setDelayAndJitter(const double& newDelay, const double& newJitter = 0.0, const double& startTime = -1.0 );

	/// Read-only accessor to _allowJitterReorder.
	bool getAllowJitterReorder() const { return static_cast<bool>(_allowJitterReorder); }

	/// Write-only accessor to _allowJitterReorder and _allowJitterReorderSetting.
	void setAllowJitterReorder(const bool newVal) {
		_allowJitterReorder = newVal;
		_allowJitterReorderSetting = _allowJitterReorder;
	}

	/// Read-only accessor to _expectedKbits.
	int getExpectedKbits() const { return static_cast<int>(_expectedKbits); }

	/// Write-only accessor to _expectedKbits and _expectedKbitsSetting.
	void setExpectedKbits(const int& newVal) {
		_expectedKbits = newVal;
		_expectedKbitsSetting = _expectedKbits;
	}

	/// Read-only accessor to _useTimeStamp.
	bool getUseTimeStamp() const { return static_cast<bool>(_useTimeStamp); }

	/// Write-only accessor to _useTimeStamp and _useTimeStampSetting.
	void setUseTimeStamp(const bool newVal) {
		_useTimeStamp = newVal;
		_useTimeStampSetting = _useTimeStamp;
	}

	int handle_timeout(const ACE_Time_Value &currentTime, const void *dataPtr);

	/// Insert a message block into the specified message queue after computing delay.
	/// @param data The message to insert.
	int putData(NetworkData* data, const HandlerLink::InputRank inputRank = HandlerLink::PrimaryInput);

	/// Read-only accessor to _timeLineCycleSecs.
	double getTimeLineCycleSecs() const { return _timeLineCycleSecs; }

	/// Write-only accessor to _timeLineCycleSecs and _timeLineCycleSecsSetting.
	/// @throw ValueTooSmall If newVal is negative.
	void setTimeLineCycleSecs(const double& newVal) {
		if ( newVal < 0 ) throw ValueTooSmall(ValueTooSmall::msg("Time Line Cycle Seconds", newVal, 0));

		_timeLineCycleSecs = newVal;
		_timeLineCycleSecsSetting = newVal;
	}

	/// Erase the delay time line and only use _delaySeconds.
	void clearTimeLine();

private:
	/// @brief Retrieve a message from an input queue with a delay.
	/// @param inputRank Whether to dequeue from the primary or auxiliary queue.
	/// @param timeout The amount of time to wait for a message.
	/// @return A std::pair, where .first is a pointer to the message data and
	/// .second is the number of messages left in the queue.
	std::pair<NetworkData*, int> getData_(
		const HandlerLink::InputRank inputRank = HandlerLink::PrimaryInput,
		ACE_Time_Value* timeout = 0);

	/// Double-precision floating point value that represents the seconds to hold each unit.
	double _delaySeconds;

	/// Config file reference to _delaySeconds.
	Setting& _delaySecondsSetting;

	/// The amount to randomly add to the delay of each unit.
	double _jitterSeconds;

	/// Config file reference to _jitterSeconds.
	Setting& _jitterSecondsSetting;

	/// Whether to allow jitter to cause unit reordering.
	bool _allowJitterReorder;

	/// Config file reference to _allowJitterReorder.
	Setting& _allowJitterReorderSetting;

	/// Used to calculate the bandwidth delay product to resize the water marks for the message queue.
	/// If this is set, the high/low water marks config settings will be overridden.
	int _expectedKbits;

	/// Config file reference to _expectedKbits.
	Setting& _expectedKbitsSetting;

	/// Either use the unit's timestamp to calculate the delay, or the current time.
	bool _useTimeStamp;

	/// Config file reference to _useTimeStamp.
	Setting& _useTimeStampSetting;

	/// How long to wait before allowing messages to be retrieved.
	timespec_t _nanoSleepSecs;

	/// If nanosleep was interrupted, the leftover seconds it would have waited.
	timespec_t _remainingSleepSecs;

	/// List of time, delay/jitter pairs that defines what delay/jitter applies starting at what time.
	TimeLine<DelayAndJitterT>* _timeLine;

	/// How often does the time line repeat.
	double _timeLineCycleSecs;

	/// Config file reference to _timeLineCycleLen.
	Setting& _timeLineCycleSecsSetting;

}; // class modEmulateDelay

} // namespace nEmulateDelay

#endif // _MOD_EMULATE_DELAY_HPP_
