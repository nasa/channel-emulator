/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modAOS_IN_SDU_Extract.cpp
 * @author Tad Kollar  
 *
 * $Id: modAOS_IN_SDU_Extract.cpp 2403 2013-12-12 19:08:39Z tkollar $
 * Copyright (c) 2012.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "modAOS_IN_SDU_Extract.hpp"
#include "AOS_Transfer_Frame.hpp"

using namespace nAOS_IN_SDU_Extract;

namespace nAOS_IN_SDU_Extract {

modAOS_IN_SDU_Extract::modAOS_IN_SDU_Extract(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr):
	AOS_PhysicalChannel_Service(newName, newChannelName, newDLLPtr) {

}

modAOS_IN_SDU_Extract::~modAOS_IN_SDU_Extract() {
	MOD_DEBUG("Running ~modAOS_IN_SDU_Extract().");
}

void modAOS_IN_SDU_Extract::_extractAndSendInSdu(AOS_Transfer_Frame* frame) {
	if ( frame->getInSduLen() ) {
		if ( links_[AuxOutputLink] ) {
			MOD_DEBUG("Sending a %d-octet IN_SDU unit.", frame->getInSduLen());
			links_[AuxOutputLink]->send(frame->wrapInnerPDU<NetworkData>(frame->getInSduLen(),
				frame->ptrInsertZone()));
		}
		else {
			MOD_WARNING("No auxiliary output, cannot send IN_SDU unit.");
		}
		MOD_DEBUG("Insert Zone unused, not sending anything via aux output.");
	}
}

int modAOS_IN_SDU_Extract::svc() {
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

		frame = dynamic_cast<AOS_Transfer_Frame*>(queueTop.first);

		if ( ! frame ) {
			MOD_INFO("Received %d octets of non-AOS data! Dropping.", queueTop.first->getUnitLength());
			ndSafeRelease(queueTop.first);
			queueTop.first = 0;
			continue;
		}

		queueTop.first = 0;

		MOD_DEBUG("Received %d-octet frame.", frame->getUnitLength());
		// frame->setSecondaryHeaderLen(getIN_SDUSize());

		// Test expected values.
		if (frame->getUnitLength() != getFrameSizeU()) {
			MOD_WARNING("AOS Transfer Frame bad length: received %d octets, needed %d.", frame->getUnitLength(), getFrameSize());
			incBadLengthCount();
			if ( getDropBadFrames() ) continue;
		}
		else incValidFrameCount();

		_extractAndSendInSdu(frame);

		if ( links_[PrimaryOutputLink] ) {
			MOD_DEBUG("Sending a %d-octet AOS Transfer Frame.", frame->getUnitLength());
			links_[PrimaryOutputLink]->send(frame);
			frame = 0; // Important
		}
		else {
			MOD_ERROR("No output target defined, dropping frame.");
		}

		MOD_DEBUG("Finished processing incoming frame.");
	}

	return svcEnd_();
}

}
