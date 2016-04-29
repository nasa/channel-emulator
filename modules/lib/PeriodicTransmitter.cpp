/* -*- C++ -*- */

//=============================================================================
/**
 * @file   PeriodicTransmitter.cpp
 * @author Tad Kollar  
 *
 * $Id: PeriodicTransmitter.cpp 1975 2013-04-26 17:45:33Z tkollar $
 * Copyright (c) 2010.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "PeriodicTransmitter.hpp"
#include <ace/Event.h>
#include <ace/High_Res_Timer.h>

namespace nasaCE {

PeriodicTransmitter::PeriodicTransmitter(HandlerLink* outputLink,
	const ACE_Time_Value& interval /* = ACE_Time_Value::zero */ ):
		BaseTask(outputLink->getSource()->getName() + "->PeriodicTransmitter"),
		_outputLink(outputLink),
		_interval(interval),
		_periodicSource(dynamic_cast<PeriodicTrafficHandler*>(outputLink->getSource())),
		_intervalUpdated(false){

		if ( ! _periodicSource )
			throw nd_error("Source segment is not a PeriodicTrafficHandler!");

		MOD_DEBUG("Configured with %d\u00B5s interval.", _interval.usec());

		// Copy message queue behavior from the source.
		msg_queue()->high_water_mark(_periodicSource->msg_queue()->high_water_mark());
		msg_queue()->low_water_mark(_periodicSource->msg_queue()->low_water_mark());
}


PeriodicTransmitter::~PeriodicTransmitter() {
	_interval.usec(0);
	stopTraffic();
}

int PeriodicTransmitter::svc() {
	svcStart_();

	ACE_Message_Block* mblk = 0;
	NetworkData* data = 0;

	ACE_Time_Value sendTime(ACE_High_Res_Timer::gettimeofday_hr());
	timespec waitLen;
	// ACE_Event waiter;
	int queueItems;
	bool isPeriodic = false;

	while ( continueService() ) {
		isPeriodic = (_interval > ACE_Time_Value::zero);

		if ( isPeriodic ) {
			sendTime = sendTime + _interval;
			queueItems = getq(mblk, &sendTime);
		}
		else queueItems = getq(mblk);

		if (msg_queue()->deactivated() || ! continueService() ) break;

		if ( _outputLink ) {
			if ( isPeriodic && (queueItems < 0 || ! mblk) ) {
				if ( _intervalUpdated ) {
					_intervalUpdated = false;
					continue;
				}

				MOD_DEBUG("Requesting idle unit from source segment.");
				try {
					data = _periodicSource->getIdleUnit();
					_periodicSource->incIdleCount();
				}
				catch (const PeriodicTrafficHandler::NoIdleUnit& noIdle) {
					MOD_NOTICE("Idle unit not available yet, cannot send one.");
					continue;
				}
			}
			else data = dynamic_cast<NetworkData*>(mblk);

			if ( sendTime > ACE_High_Res_Timer::gettimeofday_hr() ) {
				waitLen = sendTime - ACE_High_Res_Timer::gettimeofday_hr();
				ACE_OS::nanosleep(&waitLen, 0);
			}
			// waitTime = sendTime - ACE_High_Res_Timer::gettimeofday_hr();
			// waiter.wait(&sendTime, true);

			if (data) {
				MOD_DEBUG("Sending data onto the next target.");
				_outputLink->HandlerLink::send(data);
			}
			else {
				MOD_WARNING("No data to send!");
			}
		}
		else {
			MOD_DEBUG("No output target defined, dropping %d octets.", data->size());
		}

		mblk = 0;
		data = 0;
	}

	return svcEnd_();
}

void PeriodicTransmitter::setInterval(const ACE_Time_Value& newVal) {
	_interval = newVal;
	_intervalUpdated = true;

	// Wake up getq in svc() to use the new interval.
	msg_queue()->pulse();
}


void PeriodicTransmitter::setInterval(const suseconds_t usec) {
	_interval.usec(usec);
	_intervalUpdated = true;

	// Wake up getq in svc() to use the new interval.
	msg_queue()->pulse();
}

} // namespace nasaCE
