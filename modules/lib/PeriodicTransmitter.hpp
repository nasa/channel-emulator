/* -*- C++ -*- */

//=============================================================================
/**
 * @file   PeriodicTransmitter.hpp
 * @author Tad Kollar  
 *
 * $Id: PeriodicTransmitter.hpp 1974 2013-04-26 17:13:56Z tkollar $
 * Copyright (c) 2010.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _NASA_PERIODIC_TRANSMITTER_HPP_
#define _NASA_PERIODIC_TRANSMITTER_HPP_

#include "PeriodicTrafficHandler.hpp"

#include <ace/Time_Value.h>

namespace nasaCE {

//=============================================================================
/**
 * @class PeriodicTransmitter
 * @author Tad Kollar  
 * @brief Send a data unit at every specified time interval.
 */
//=============================================================================
class PeriodicTransmitter: public BaseTask {
public:

	/// Primary constructor.
	/// @param outputLink Where to send the data to.
	/// @param interval How long to wait before transmitting the next message.
    PeriodicTransmitter(HandlerLink* outputLink,
    	const ACE_Time_Value& interval = ACE_Time_Value::zero);

	/// Destructor.
    ~PeriodicTransmitter();

    /// The functional loop that makes this an active object.
    int svc();

	/// Write-only accessor to _interval.
	/// @param newVal Obtain the microseconds from an ACE_Time_Value.
	void setInterval(const ACE_Time_Value& newVal);

	/// Set the interval.
	/// @param usec Number of microseconds between sends.
	void setInterval(const suseconds_t usec);

	/// Read-only accessor to _interval.
	ACE_Time_Value getInterval() const { return _interval; }

private:
	/// The Traffic Handler that is using this service.
	HandlerLink* _outputLink;

	/// How long to wait before transmitting the next message.
	ACE_Time_Value _interval;

	/// The segment that is us sending data.
	PeriodicTrafficHandler* _periodicSource;

	/// Internal flag set when setInterval is called.
	bool _intervalUpdated;

}; // class PeriodicTransmitter

} // namespace nasaCE

#endif // _NASA_PERIODIC_TRANSMITTER_HPP_
