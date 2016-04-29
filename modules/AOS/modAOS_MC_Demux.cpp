/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modAOS_MC_Demux.cpp
 * @author Tad Kollar  
 *
 * $Id: modAOS_MC_Demux.cpp 2403 2013-12-12 19:08:39Z tkollar $
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "modAOS_MC_Demux.hpp"
#include "AOS_Transfer_Frame.hpp"

namespace nAOS_MC_Demux {

modAOS_MC_Demux::modAOS_MC_Demux(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr):
	AOS_Demux_Base(newName, newChannelName, newDLLPtr) {

}

modAOS_MC_Demux::~modAOS_MC_Demux() {
	MOD_DEBUG("Running ~modAOS_MC_Demux().");
}

int modAOS_MC_Demux::svc() {
	svcStart_();

	AOS_Transfer_Frame* aos = 0;
	HandlerLinkMap::iterator outputLink;

	while ( continueService() ) {
		ndSafeRelease(aos);

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

		MOD_DEBUG("Received %d octets to demultiplex.", aos->getUnitLength());

		// Test expected values.
		if ( ! validateFrame(aos) ) {
			if (getDropBadFrames()) continue;
		}
		else { incValidFrameCount(); }

		if ( ( outputLink = primaryOutputLinks_.find(aos->getSpacecraftID())) != primaryOutputLinks_.end() ) {
			MOD_DEBUG("Sending %d octets.", aos->getUnitLength());
			outputLink->second->send(aos);
			aos = 0; // important
		}
		else {
			MOD_NOTICE("No output target defined for MCID %d, dropping data.", aos->getMCID());
		}
	}

	return svcEnd_();
}

HandlerLink* modAOS_MC_Demux::getLink(const LinkType ltype, const ACE_UINT8 SCID /* = 0 */)  {
	if ( ltype != BaseTrafficHandler::PrimaryOutputLink )
		return BaseTrafficHandler::getLink(ltype);

	HandlerLinkMap::iterator outputLink;

	if ( ( outputLink = primaryOutputLinks_.find(SCID)) != primaryOutputLinks_.end() ) {
		return outputLink->second;
	}
	else return 0;
}

}

