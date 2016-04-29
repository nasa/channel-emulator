/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modTM_VC_Mux.cpp
 * @author Tad Kollar  
 *
 * $Id: modTM_VC_Mux.cpp 2403 2013-12-12 19:08:39Z tkollar $
 * Copyright (c) 2012.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "modTM_VC_Mux.hpp"
#include "TM_Transfer_Frame.hpp"

using namespace nTM_Mux;
using namespace nTM_VC_Mux;

namespace nTM_VC_Mux {

modTM_VC_Mux::modTM_VC_Mux(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr):
	TM_Mux_Base(newName, newChannelName, newDLLPtr) {
}

modTM_VC_Mux::~modTM_VC_Mux() {
	MOD_DEBUG("Running ~modTM_VC_Mux().");
}

int modTM_VC_Mux::svc() {
	svcStart_();

	TM_Transfer_Frame* frame = 0;

	while ( continueService() ) {
		ndSafeRelease(frame);
		bool frameIsValid = true;

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

		MOD_DEBUG("Received %d bytes to multiplex.", frame->getTotalUnitLength());

		// Test expected values.
		if (static_cast<int>(frame->getTotalUnitLength()) != getFrameSize()) {
			MOD_WARNING("TM Transfer Frame bad length: received %d octets, needed %d.", frame->getTotalUnitLength(), getFrameSize());
			incBadLengthCount();
			frameIsValid = false;
			if ( getDropBadFrames() ) continue;
		}

		if ( frame->getMCID() != getMCID() ) {
			MOD_WARNING("Received Frame with wrong MCID, 0x%X instead of 0x%X.", frame->getMCID(), getMCID());
			incBadMCIDCount();
			frameIsValid = false;
			if ( getDropBadFrames() ) continue;
		}

		addErrorControlIfPossible(frame);
		if ( frameIsValid ) incValidFrameCount();

		if ( links_[PrimaryOutputLink] ) {
			MOD_DEBUG("Sending %d octets.", frame->getTotalUnitLength());
			links_[PrimaryOutputLink]->send(frame);
			frame = 0; // important
		}
		else {
			MOD_NOTICE("No output target defined yet, dropping data.");
		}
	}

	return svcEnd_();

}

ACE_UINT8 modTM_VC_Mux::getIdentifier_(ACE_Message_Block* mb) const {
	return dynamic_cast<TM_Transfer_Frame*>(mb)->getVCID();
}

}
