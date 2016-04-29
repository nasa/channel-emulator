/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modAOS_VC_Mux.cpp
 * @author Tad Kollar  
 *
 * $Id: modAOS_VC_Mux.cpp 2344 2013-08-30 20:24:16Z tkollar $
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "modAOS_VC_Mux.hpp"
#include "AOS_Transfer_Frame.hpp"

using namespace nAOS_Mux;
namespace nAOS_VC_Mux {

modAOS_VC_Mux::modAOS_VC_Mux(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr):
	AOS_Mux_Base(newName, newChannelName, newDLLPtr) {
}

modAOS_VC_Mux::~modAOS_VC_Mux() {
	MOD_DEBUG("Running ~modAOS_VC_Mux().");
}

int modAOS_VC_Mux::svc() {
	svcStart_();

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

		AOS_Transfer_Frame* aos = dynamic_cast<AOS_Transfer_Frame*>(queueTop.first);

		if ( ! aos ) {
			MOD_INFO("Received %d octets of non-AOS data! Dropping.", queueTop.first->getUnitLength());
			ndSafeRelease(queueTop.first);
			queueTop.first = 0;
			continue;
		}

		MOD_DEBUG("Received %d bytes to multiplex.", aos->getTotalUnitLength());

		// Test expected values.
		if (static_cast<int>(aos->getTotalUnitLength()) != getFrameSize()) {
			MOD_WARNING("AOS Transfer Frame bad length: received %d octets, needed %d.", aos->getTotalUnitLength(), getFrameSize());
			incBadLengthCount();
			continue;
		}

		if ( aos->getMCID() != getMCID() ) {
			MOD_WARNING("Received Frame with wrong MCID, 0x%X instead of 0x%X. Dropping.", aos->getMCID(), getMCID());
			incBadMCIDCount();
			if (getDropBadFrames()) continue;
		}

		incValidFrameCount();
		if ( links_[PrimaryOutputLink] ) {
			MOD_DEBUG("Sending %d octets.", aos->getTotalUnitLength());

			links_[PrimaryOutputLink]->send(aos);
		}
		else {
			MOD_NOTICE("No output target defined yet, dropping data.");
			ndSafeRelease(aos);
		}
	}

	return svcEnd_();

}

ACE_UINT8 modAOS_VC_Mux::getIdentifier_(ACE_Message_Block* mb) const {
	return dynamic_cast<AOS_Transfer_Frame*>(mb)->getVirtualChannelID();
}

}
