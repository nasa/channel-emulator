/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modTM_FSH_Insert.cpp
 * @author Tad Kollar  
 *
 * $Id: modTM_FSH_Insert.cpp 2403 2013-12-12 19:08:39Z tkollar $
 * Copyright (c) 2012.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "modTM_FSH_Insert.hpp"
#include "TM_Transfer_Frame.hpp"

using namespace nasaCE;
using namespace nTM_FSH_Insert;

namespace nTM_FSH_Insert {

modTM_FSH_Insert::modTM_FSH_Insert(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr):
	TM_MasterChannel_Service(newName, newChannelName, newDLLPtr),
	_receivedFshUnitCount(0),
	_receivedFshOctetCount(0),
	_receivedFrameUnitCount(0),
	_receivedFrameOctetCount(0) {

	auxInputQueue_ = new AuxMessageQueue();

	MOD_DEBUG("Opening auxiliary input queue to receive FSH units.");
	getAuxQueue()->open(); // can set high/low water mark here, too.

}

modTM_FSH_Insert::~modTM_FSH_Insert() {
	MOD_DEBUG("Running ~modTM_FSH_Insert().");
	stopTraffic();

	delete auxInputQueue_;
	auxInputQueue_ = 0;
}

void modTM_FSH_Insert::stopTraffic(bool shutdown) {
	MOD_DEBUG("Closing auxiliary message queue.");
	getAuxQueue()->close();

	PeriodicTrafficHandler::stopTraffic(shutdown);
}

bool modTM_FSH_Insert::_insertWaitingFSHUnit(TM_Transfer_Frame* frame) {
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
			NetworkData* fsh = dynamic_cast<NetworkData*>(mb);

			if ( ! fsh || mb->length() != static_cast<size_t>(getFSHDataFieldSize()) ) {
				MOD_WARNING("Unrecognized %d-octet, non-FSH data unit received on auxiliary queue.", mb->length());
				ndSafeRelease(mb);
			}
			else {
				MOD_DEBUG("%d-octet FSH unit is waiting, attempting to copy to the %d-octet Secondary Header field.",
					fsh->getUnitLength(), frame->getSecondaryHeaderDataLength());

				_receivedFshUnitCount += 1;
				_receivedFshOctetCount += fsh->getUnitLength();

				try {
					frame->setSecondaryHeaderData(fsh);
					success = true;
				}
				catch(const BufferOverflow& e) {
					MOD_WARNING("%d-octet FSH unit could not fit into %d-octet FSH field, dropping.",
						fsh->getUnitLength(), frame->getSecondaryHeaderDataLength());
				}

				ndSafeRelease(fsh);
			}
		}
	}

	return success;
}


int modTM_FSH_Insert::svc() {
	svcStart_();

	TM_Transfer_Frame* frame = 0;

	while ( continueService() ) {
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

		if ( (frame = dynamic_cast<TM_Transfer_Frame*>(queueTop.first)) ) {
			MOD_DEBUG("Received %d byte TM_Transfer_Frame to add a Secondary Header to.", frame->getTotalUnitLength());

			// Test expected values.
			if (static_cast<int>(frame->getTotalUnitLength()) != getFrameSize()) {
				MOD_WARNING("TM Transfer Frame bad length: received %d octets, needed %d.", frame->getTotalUnitLength(), getFrameSize());
				incBadLengthCount();
				if ( getDropBadFrames() ) continue;
			}
			else incValidFrameCount();

			_receivedFrameUnitCount += 1;
			_receivedFrameOctetCount += frame->getUnitLength();

			_insertWaitingFSHUnit(frame);

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
			MOD_INFO("Received %d octet buffer in non-TM TF wrapper! Dropping.", queueTop.first->getUnitLength());
			ndSafeRelease(queueTop.first);
			continue;
		}
	}

	return svcEnd_();

}

}
