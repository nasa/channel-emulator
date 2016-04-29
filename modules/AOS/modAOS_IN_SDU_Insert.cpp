/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modAOS_IN_SDU_Insert.cpp
 * @author Tad Kollar  
 *
 * $Id: modAOS_IN_SDU_Insert.cpp 2403 2013-12-12 19:08:39Z tkollar $
 * Copyright (c) 2012.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "modAOS_IN_SDU_Insert.hpp"
#include "AOS_Transfer_Frame.hpp"

using namespace nasaCE;
using namespace nAOS_IN_SDU_Insert;

namespace nAOS_IN_SDU_Insert {

modAOS_IN_SDU_Insert::modAOS_IN_SDU_Insert(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr):
	AOS_PhysicalChannel_Service(newName, newChannelName, newDLLPtr),
	_receivedInSduUnitCount(0),
	_receivedInSduOctetCount(0),
	_receivedFrameUnitCount(0),
	_receivedFrameOctetCount(0) {

	auxInputQueue_ = new AuxMessageQueue();

	MOD_DEBUG("Opening auxiliary input queue to receive IN_SDU units.");
	getAuxQueue()->open(); // can set high/low water mark here, too.

}

modAOS_IN_SDU_Insert::~modAOS_IN_SDU_Insert() {
	MOD_DEBUG("Running ~modAOS_IN_SDU_Insert().");
	stopTraffic();

	delete auxInputQueue_;
	auxInputQueue_ = 0;
}

void modAOS_IN_SDU_Insert::stopTraffic(bool shutdown) {
	MOD_DEBUG("Closing auxiliary message queue.");
	getAuxQueue()->close();

	PeriodicTrafficHandler::stopTraffic(shutdown);
}

bool modAOS_IN_SDU_Insert::_insertWaitingInSduUnit(AOS_Transfer_Frame* frame) {
	bool success = false;

	if ( ! getAuxQueue()->is_empty() ) {
		std::pair<NetworkData*, int> auxQueueTop = getAuxData_();

		if ( auxQueueTop.second < 0 ) {
			MOD_ERROR("getAuxData_() call failed.");
		}
		else if ( ! auxQueueTop.first ) {
			MOD_ERROR("getAuxData_() returned with null data.");
		}
		else {
			ACE_Message_Block* mb = auxQueueTop.first;
			NetworkData* inSdu = dynamic_cast<NetworkData*>(mb);

			if ( ! inSdu || mb->length() != static_cast<size_t>(getInsertZoneSize()) ) {
				MOD_WARNING("Unrecognized %d-octet, non-IN_SDU data unit received on auxInput queue (inSdu ptr: %X, IZ size: %d)",
							mb->length(), inSdu, getInsertZoneSize());
				ndSafeRelease(mb);
			}
			else {
				MOD_DEBUG("%d-octet IN_SDU unit is waiting, attempting to copy to the %d-octet Insert Zone.",
					inSdu->getUnitLength(), frame->getInSduLen());

				_receivedInSduUnitCount += 1;
				_receivedInSduOctetCount += inSdu->getUnitLength();

				try {
					frame->setInsertZone(inSdu);
					success = true;
				}
				catch(const BufferOverflow& e) {
					MOD_WARNING("%d-octet IN_SDU unit could not fit into %d-octet Insert Zone, dropping.",
						inSdu->getUnitLength(), frame->getInSduLen());
				}

				ndSafeRelease(inSdu);
			}
		}
	}

	return success;
}


int modAOS_IN_SDU_Insert::svc() {
	svcStart_();

	AOS_Transfer_Frame* frame = 0;

	while ( continueService() ) {
		ndSafeRelease(frame);

		std::pair<NetworkData*, int> queueTop = getData_();

		if ( msg_queue()->deactivated() ) break;

		if ( queueTop.second < 0 ) {
			MOD_ERROR("getData_() call failed.");
			continue;
		}
		else if ( ! queueTop.first ) {
			MOD_ERROR("getData_() returned with null data.");
			continue;
		}

		if ( (frame = dynamic_cast<AOS_Transfer_Frame*>(queueTop.first)) ) {
			MOD_DEBUG("Received %d byte AOS_Transfer_Frame to add a Secondary Header to.", frame->getTotalUnitLength());

			// Test expected values.
			if (static_cast<int>(frame->getTotalUnitLength()) != getFrameSize()) {
				MOD_WARNING("AOS Transfer Frame bad length: received %d octets, needed %d.", frame->getTotalUnitLength(), getFrameSize());
				incBadLengthCount();
				if ( getDropBadFrames() ) continue;
			}
			else {
				incValidFrameCount();
			}

			_receivedFrameUnitCount += 1;
			_receivedFrameOctetCount += frame->getUnitLength();

			_insertWaitingInSduUnit(frame);

			if ( links_[PrimaryOutputLink] ) {
				MOD_DEBUG("Sending %d octets.", frame->getTotalUnitLength());

				links_[PrimaryOutputLink]->send(frame);
				frame = 0; // important
			}
			else {
				MOD_NOTICE("No output target defined yet, dropping data.");

			}
		}
		else {
			MOD_INFO("Received %d octets of unrecognized data! Dropping.", queueTop.first->getUnitLength());
			ndSafeRelease(queueTop.first);
			queueTop.first = 0;
			continue;
		}
	}

	return svcEnd_();

}

}
