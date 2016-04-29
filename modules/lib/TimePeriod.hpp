/* -*- C++ -*- */

//=============================================================================
/**
 * @file   TimePeriod.hpp
 * @author Tad Kollar  
 *
 * $Id: TimePeriod.hpp 1670 2012-08-28 13:25:28Z tkollar $ 
 * Copyright (c) 2011.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _TIME_PERIOD_HPP_
#define _TIME_PERIOD_HPP_

#include <cmath>
#include <ace/Time_Value.h>
#include "nd_error.hpp"

namespace nasaCE {

struct NegativeTimeValue: public nd_error {
	NegativeTimeValue(const std::string& e): nd_error(e, "NegativeTimeValue") { }
	
	static std::string msg(const std::string& field, const double& given) {
		std::ostringstream os;
	
		os << field << " specified as disallowed negative value " << given << ".";
			
		return os.str();
	}
};

//=============================================================================
/**
 * @class TimePeriod
 * @author Tad Kollar  
 * @brief Represent a period with specified start and end times.
 */
//=============================================================================
class TimePeriod {
public:
	/// Construct with ACE_Time_Values.
	TimePeriod(const ACE_Time_Value& startTime = ACE_Time_Value::zero,
		const ACE_Time_Value& stopTime = ACE_Time_Value::zero):
		_startTime(startTime), _stopTime(stopTime) {

	}
	
	/// Construct with doubles.
	/// @throw NegativeTimeValue If either startTime or stopTime is less than 0.0.
	TimePeriod(const double& startTime, const double& stopTime = 0.0):
		_startTime(ACE_Time_Value::zero), _stopTime(ACE_Time_Value::zero) {
		setStartTime(startTime);
		setStopTime(stopTime);
	}
	
	/// Copy constructor.
	TimePeriod(const TimePeriod& otherPeriod): _startTime(otherPeriod._startTime),
		_stopTime(otherPeriod._stopTime) {		}

	/// Write-only accessor to _startTime.
	void setStartTime(const ACE_Time_Value& newVal) { _startTime = newVal; }

	/// Write-only accessor to _startTime.
	/// @throw NegativeTimeValue If newVal is less than 0.0.
	void setStartTime(const double& newVal) { 
		if ( newVal < 0.0 )
			throw NegativeTimeValue(NegativeTimeValue::msg("Start time", newVal));

		_startTime.set(newVal);
	}
	
	/// Read-only accessor to _startTime.
	ACE_Time_Value getStartTime() const { return _startTime; }

	/// Write-only accessor to _stopTime.
	void setStopTime(const ACE_Time_Value& newVal) { _stopTime = newVal; }

	/// Write-only accessor to _stopTime.
	/// @throw NegativeTimeValue If newVal is less than 0.0.
	void setStopTime(const double& newVal) {
		if ( newVal < 0.0 )
			throw NegativeTimeValue(NegativeTimeValue::msg("Stop time", newVal));
			
			_stopTime.set(newVal);
	}
	
	/// Read-only accessor to _stopTime.
	ACE_Time_Value getStopTime() const { return _stopTime; }		

	/// True if the provided time comes before _startTime.
	bool isBefore(const ACE_Time_Value& testTime) { return ( testTime < _startTime ); }
	
	/// True if the provided time is at or after _startTime and at or before _stopTime.
	bool isDuring(const ACE_Time_Value& testTime) {
		return ( testTime >= _startTime && testTime <= _stopTime);
	}
	
	/// True if the provided time is after _stopTime.
	bool isAfter(const ACE_Time_Value& testTime) { return ( testTime > _stopTime ); }

	/// Convert an ACE_Time_Value to decimal seconds.
	static double aceTimeToDouble(const ACE_Time_Value& atime) {
		return atime.sec() + atime.usec()/1000000.0;
	}
	
	/// Return a new time value one microsecond before our start time.
	ACE_Time_Value getTimeBeforeStart() const {
		ACE_Time_Value preTime(ACE_Time_Value::zero);
	
		if ( _startTime != preTime) {
			preTime = _startTime;
			preTime--;
		}
		
		return preTime; 
	}
	
	/// Return a new time value one microsecond after our stop time.
	ACE_Time_Value getTimeAfterStop() const {
		ACE_Time_Value postTime(ACE_Time_Value::zero);
	
		if ( _stopTime != postTime) {
			postTime = _stopTime;
			postTime++;
		}
		
		return postTime; 
	}
	
private:
	/// The beginning of the time period.
	ACE_Time_Value _startTime;
	
	/// The end of the time period.
	ACE_Time_Value _stopTime;
};

} // namespace nasaCE

#endif // _TIME_PERIOD_HPP_