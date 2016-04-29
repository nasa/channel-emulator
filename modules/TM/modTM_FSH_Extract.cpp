/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modTM_FSH_Extract.cpp
 * @author Tad Kollar  
 *
 * $Id: modTM_FSH_Extract.cpp 2403 2013-12-12 19:08:39Z tkollar $
 * Copyright (c) 2012.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "modTM_FSH_Extract.hpp"
#include "TM_Transfer_Frame.hpp"

using namespace nTM_FSH_Extract;

namespace nTM_FSH_Extract {

modTM_FSH_Extract::modTM_FSH_Extract(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr):
	TM_MasterChannel_Service(newName, newChannelName, newDLLPtr) {

}

modTM_FSH_Extract::~modTM_FSH_Extract() {
	MOD_DEBUG("Running ~modTM_FSH_Extract().");
}

void modTM_FSH_Extract::_extractAndSendFSH(TM_Transfer_Frame* frame) {
	if ( frame->getSecondaryHeaderFlag() ) {
		if ( links_[AuxOutputLink] ) {
			MOD_DEBUG("Sending a %d-octet FSH unit.", frame->getSecondaryHeaderDataLength());
			links_[AuxOutputLink]->send(frame->wrapInnerPDU<NetworkData>(frame->getSecondaryHeaderDataLength(),
				frame->ptrSecondaryHeaderData()));
		}
		else {
			MOD_WARNING("No auxiliary output, cannot send FSH unit.");
		}
		MOD_DEBUG("Secondary Header Flag unset, not sending anything via aux output.");
	}
}

int modTM_FSH_Extract::svc() {
	svcStart_();

	TM_Transfer_Frame* frame = 0;

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

		frame = dynamic_cast<TM_Transfer_Frame*>(queueTop.first);

		if ( !frame ) {
			MOD_ERROR("Received %d-octet buffer in %s wrapper (not TM_Transfer_Frame)! Must discard.",
				queueTop.first->getUnitLength(), queueTop.first->typeStr().c_str());
			ndSafeRelease(queueTop.first);
			continue;
		}

		queueTop.first = 0;

		MOD_DEBUG("Received %d-octet frame.", frame->getUnitLength());
		frame->setSecondaryHeaderLen(getFSHSize());

		// Test expected values.
		if (frame->getUnitLength() != getFrameSizeU()) {
			MOD_WARNING("TM Transfer Frame bad length: received %d octets, needed %d.", frame->getUnitLength(), getFrameSize());
			incBadLengthCount();
			if ( getDropBadFrames() ) continue;
		}
		else { incValidFrameCount(); }

		_extractAndSendFSH(frame);

		if ( links_[PrimaryOutputLink] ) {
			MOD_DEBUG("Sending a %d-octet TM Transfer Frame.", frame->getUnitLength());
			links_[PrimaryOutputLink]->send(frame);
			frame = 0; // important
		}
		else {
			MOD_ERROR("No output target defined, dropping frame.");
		}

		MOD_DEBUG("Finished processing incoming frame.");
	}

	return svcEnd_();
}

}
