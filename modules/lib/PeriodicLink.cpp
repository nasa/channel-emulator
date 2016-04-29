/* -*- C++ -*- */

//=============================================================================
/**
 * @file   PeriodicLink.cpp
 * @author Tad Kollar  
 *
 * $Id: PeriodicLink.cpp 2349 2013-09-03 13:51:02Z tkollar $
 * Copyright (c) 2010.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "PeriodicLink.hpp"

namespace nasaCE {

PeriodicLink::PeriodicLink( BaseTrafficHandler* source,
	BaseTrafficHandler* target,
	const ACE_Time_Value& interval /* = ACE_Time_Value::zero */,
	const OutputRank sourceOutputRank /* = PrimaryOutput */,
	const InputRank targetInputRank /* = PrimaryInput */,
	const bool openTransmitter /* = true */):
	HandlerLink(source, target, sourceOutputRank, targetInputRank),
	_periodicTX(0) {
		_periodicTX = new PeriodicTransmitter(this, interval);
		if ( ! _periodicTX ) {
			throw OutOfMemory("Could not allocate memory for a new PeriodicTransmitter object.");
		}
		if ( openTransmitter ) _periodicTX->open();
	}


PeriodicLink::~PeriodicLink() {
	delete _periodicTX;
}

int PeriodicLink::send(NetworkData* data) {
	ACE_Message_Block* mblk = dynamic_cast<ACE_Message_Block*>(data);

	if ( ! mblk ) {
		ND_ERROR("[%s] PeriodicLink::send() failed to cast from NetworkData* to ACE_Message_Block*.\n",
			getSource()->getName().c_str());
		return -1;
	}

	int ret = -1;
	if (_periodicTX) ret = _periodicTX->putq(mblk);

	return ret;
}

}
