/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modTM_MC_Demux.cpp
 * @author Tad Kollar  
 *
 * $Id: modTM_MC_Demux.cpp 2403 2013-12-12 19:08:39Z tkollar $
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "modTM_MC_Demux.hpp"
#include "TM_Transfer_Frame.hpp"

using namespace nasaCE;
using namespace nTM_Demux;
using namespace nTM_MC_Demux;

namespace nTM_MC_Demux {

modTM_MC_Demux::modTM_MC_Demux(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr):
	TM_Demux_Base(newName, newChannelName, newDLLPtr) {
}

modTM_MC_Demux::~modTM_MC_Demux() {
	MOD_DEBUG("Running ~modTM_MC_Demux().");
}

int modTM_MC_Demux::svc() {
	svcStart_();

	TM_Transfer_Frame* frame = 0;
	HandlerLinkMap::iterator outputLink;

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

		MOD_DEBUG("Received %d octets to demultiplex.", frame->getTotalUnitLength());

		// Test expected values.
		if ( ! validateFrame(frame) ) {
			if ( getDropBadFrames() ) continue;
		}
		else incValidFrameCount();

		if ( ( outputLink = primaryOutputLinks_.find(frame->getMCID())) != primaryOutputLinks_.end() ) {

			MOD_DEBUG("Sending %d octets.", frame->getTotalUnitLength());

			outputLink->second->send(frame);
			frame = 0; // important
		}
		else {
			MOD_NOTICE("No output target defined for MCID %d, dropping data.", frame->getMCID());
		}
	}

	return svcEnd_();
}

HandlerLink* modTM_MC_Demux::getLink(const LinkType ltype, const ACE_UINT8 MCID /* = 0 */) {
	if ( ltype != BaseTrafficHandler::PrimaryOutputLink )
		return BaseTrafficHandler::getLink(ltype);

	HandlerLinkMap::iterator outputLink;
	if ( ( outputLink = primaryOutputLinks_.find(MCID)) != primaryOutputLinks_.end() ) {
		return outputLink->second;
	}
	else return 0;
}

}
