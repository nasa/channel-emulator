/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modAOS_VC_Demux.cpp
 * @author Tad Kollar  
 *
 * $Id: modAOS_VC_Demux.cpp 2403 2013-12-12 19:08:39Z tkollar $
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "modAOS_VC_Demux.hpp"
#include "AOS_Transfer_Frame.hpp"

namespace nAOS_VC_Demux {

modAOS_VC_Demux::modAOS_VC_Demux(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr):
	AOS_Demux_Base(newName, newChannelName, newDLLPtr) {
}

modAOS_VC_Demux::~modAOS_VC_Demux() {
	MOD_DEBUG("Running ~modAOS_VC_Demux().");
}

int modAOS_VC_Demux::svc() {
	svcStart_();
	AOS_Transfer_Frame* aos = 0;
	HandlerLinkMap::iterator outputLink;

	while ( continueService() ) {
		ndSafeRelease(aos);

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

		aos = dynamic_cast<AOS_Transfer_Frame*>(queueTop.first);

		if ( ! aos ) {
			MOD_INFO("Received %d octets of non-AOS data! Dropping.", queueTop.first->getUnitLength());
			ndSafeRelease(queueTop.first);
			continue;
		}

		MOD_DEBUG("Received %d octets to demultiplex.", aos->getTotalUnitLength());

		// Test expected values.
		if ( ! validateFrame(aos) ) continue;

		if ( aos->getMCID() != getMCID() ) {
			MOD_WARNING("Received Frame with wrong MCID, 0x%X instead of 0x%X. Dropping.", aos->getMCID(), getMCID());
			incBadMCIDCount();
			frameIsValid = false;
			if ( getDropBadFrames() ) continue;
		}

		if ( frameIsValid ) incValidFrameCount();

		if ( ( outputLink = primaryOutputLinks_.find(aos->getVirtualChannelID())) != primaryOutputLinks_.end() ) {
			MOD_DEBUG("Sending %d octets.", aos->getTotalUnitLength());
			outputLink->second->send(aos);
			aos = 0; // important
		}
		else {
			MOD_NOTICE("No output target defined for VCID %d, dropping data.", aos->getVirtualChannelID());
		}
	}

	return svcEnd_();
}

HandlerLink* modAOS_VC_Demux::getLink(const LinkType ltype, const ACE_UINT8 VCID /* = 0 */) {
	if ( ltype != BaseTrafficHandler::PrimaryOutputLink )
		return BaseTrafficHandler::getLink(ltype);

	HandlerLinkMap::iterator outputLink;
	if ( ( outputLink = primaryOutputLinks_.find(VCID)) != primaryOutputLinks_.end() ) {
		return outputLink->second;
	}
	else return 0;
}

}
