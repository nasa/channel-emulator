/* -*- C++ -*- */

//=============================================================================
/**
 * @file   TimeLine.hpp
 * @author Tad Kollar  
 *
 * $Id: TimeLine.hpp 1670 2012-08-28 13:25:28Z tkollar $
 * Copyright (c) 2011.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _TIME_LINE_HPP_
#define _TIME_LINE_HPP_

#include <list>
#include "TimePeriod.hpp"

namespace nasaCE {

struct AppendFailed: public nd_error {
	AppendFailed(const std::string& e): nd_error(e, "AppendFailed") { }

	static std::string msg(const ACE_Time_Value& existingTime,
		const ACE_Time_Value& attemptedTime) {
		std::ostringstream os;

		os	<< "Timeline append failed: existing period starting at "
			<< std::setprecision(6) << std::fixed
			<< TimePeriod::aceTimeToDouble(existingTime)
			<< " is later than new period starting at "
			<< TimePeriod::aceTimeToDouble(attemptedTime) << ".";

		return os.str();
	}
};

//=============================================================================
/**
 * @class TimeLine
 * @author Tad Kollar  
 * @brief Manage a list of TimePeriod, value pairs.
 */
//=============================================================================
template<typename valT>
class TimeLine {
public:
	typedef std::pair<TimePeriod, valT> TimeLineDataT;
	typedef std::list<TimeLineDataT> TimeLineListT;

	/// Initialize the time line with a default value covering all times.
	TimeLine(const valT& defaultVal): _defaultVal(defaultVal) {
		reset();
	}

	/// Add a new value for a new start time to the end of the list, truncating the
	/// timespan of the value before it.
	/// @param startTime When the new value is to take effect.
	/// @param newVal The value at that time.
	/// @throw AppendFailed When an attempt is made to insert a time period before one that already exists.
	void append(const ACE_Time_Value& startTime, const valT& newVal) {
		TimeLineDataT newData(TimePeriod(startTime, ACE_Time_Value::max_time), newVal);
		TimePeriod& lastPeriod = _timeLine.back().first;
		TimePeriod& newPeriod = newData.first;

		// Only allowed to insert after the start time of the last time period.
		if (lastPeriod.getStartTime() < newPeriod.getStartTime()) {
			ACE_Time_Value newStopTime = newPeriod.getTimeBeforeStart();
			lastPeriod.setStopTime(newStopTime);
			_timeLine.push_back(newData);
		}
		else {
			throw AppendFailed(AppendFailed::msg(lastPeriod.getStartTime(), startTime));
		}
	}

	/// Retrieve the value that is mapped to the time period indicated.
	/// @param timePoint The time value to match to a time period.
	valT get(const ACE_Time_Value& timePoint, const double modulus = 0.0) {
		ACE_Time_Value workingTimePoint(timePoint);

		if ( modulus > 0.0 ) {
			ACE_UINT64 micros;
			workingTimePoint.to_usec(micros);
			double moddedTime = (micros % static_cast<ACE_UINT64>(modulus * 1000000)) / 1000000.0;
			workingTimePoint.set(moddedTime);
		}

		typename TimeLineListT::iterator it;

		for ( it = _timeLine.begin() ; it != _timeLine.end(); ++it ) {
			if ( workingTimePoint >= it->first.getStartTime() &&
				workingTimePoint <= it->first.getStopTime() )
			return it->second;
		}

		return _defaultVal;
	}

	/// Retrieve the value that is mapped to the time period indicated; synonym for get().
	/// @param timePoint The time value to match to a time period.
	valT operator[] (const ACE_Time_Value& timePoint) { return get(timePoint); }

	/// Get rid of all time period, value pairs and load only the default value.
	void reset() {
		_timeLine.clear();
		TimeLineDataT defaultData (TimePeriod(ACE_Time_Value::zero, ACE_Time_Value::max_time), _defaultVal);
		_timeLine.push_back(defaultData);
	}

private:

	/// What to initialize the list with, and return if no matching value can be
	/// found (which should never happen).
	valT _defaultVal;

	/// Using default constructor is forbidden because we always need a default value.
	TimeLine() { }

	/// The map of time periods.
	TimeLineListT _timeLine;

};

} // namespace nasaCE

#endif // _TIME_LINE_HPP_