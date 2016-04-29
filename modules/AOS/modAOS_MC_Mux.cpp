/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modAOS_MC_Mux.cpp
 * @author Tad Kollar  
 *
 * $Id: modAOS_MC_Mux.cpp 2403 2013-12-12 19:08:39Z tkollar $
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "AOS_Transfer_Frame.hpp"
#include "modAOS_MC_Mux.hpp"

using namespace nAOS_Mux;
namespace nAOS_MC_Mux {

modAOS_MC_Mux::modAOS_MC_Mux(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr):
	AOS_Mux_Base(newName, newChannelName, newDLLPtr) {
}

modAOS_MC_Mux::~modAOS_MC_Mux() {
	MOD_DEBUG("Running ~modAOS_MC_Mux().");
}

int modAOS_MC_Mux::svc() {
	svcStart_();

	AOS_Transfer_Frame* aos = 0;

	while ( continueService() ) {
		ndSafeRelease(aos);

		bool isValidFrame = true;
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

		aos = dynamic_cast<AOS_Transfer_Frame*>(queueTop.first);

		if ( ! aos ) {
			MOD_INFO("Received %d octets of non-AOS data! Dropping.", queueTop.first->getUnitLength());
			ndSafeRelease(queueTop.first);
			queueTop.first = 0;
			continue;
		}

		MOD_DEBUG("Received %d octets to multiplex into one Physical Channel.", aos->getTotalUnitLength());

		// Test expected values.
		if (static_cast<int>(aos->getUnitLength()) != getFrameSize()) {
			MOD_WARNING("AOS Transfer Frame bad length: received %d octets, needed %d.", aos->getUnitLength(), getFrameSize());
			incBadLengthCount();
			isValidFrame = false;
			if ( getDropBadFrames() ) continue;
		}

		if ( isValidFrame ) incValidFrameCount();

		if ( links_[PrimaryOutputLink] ) {
			MOD_DEBUG("Sending %d octets.", aos->getUnitLength());
			links_[PrimaryOutputLink]->send(aos);
			aos = 0; // important
		}
		else {
			MOD_NOTICE("No output target defined yet, dropping data.");
		}
	}

	return svcEnd_();
}

ACE_UINT8 modAOS_MC_Mux::getIdentifier_(ACE_Message_Block* mb) const {
	return dynamic_cast<AOS_Transfer_Frame*>(mb)->getMCID();
}

}
