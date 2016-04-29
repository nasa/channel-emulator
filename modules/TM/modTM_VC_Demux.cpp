/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modTM_VC_Demux.cpp
 * @author Tad Kollar  
 *
 * $Id: modTM_VC_Demux.cpp 2403 2013-12-12 19:08:39Z tkollar $
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "modTM_VC_Demux.hpp"
#include "TM_Transfer_Frame.hpp"

using namespace nasaCE;
using namespace nTM_Demux;
using namespace nTM_VC_Demux;

namespace nTM_VC_Demux {

modTM_VC_Demux::modTM_VC_Demux(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr):
	TM_Demux_Base(newName, newChannelName, newDLLPtr) {
}

modTM_VC_Demux::~modTM_VC_Demux() {
	MOD_DEBUG("Running ~modTM_VC_Demux().");
}

int modTM_VC_Demux::svc() {
	svcStart_();
	HandlerLinkMap::iterator outputLink;
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

		if ( ! frame ) {
			MOD_ERROR("Received %d-octet buffer in %s wrapper (not TM_Transfer_Frame)! Must discard.",
				queueTop.first->getUnitLength(), queueTop.first->typeStr().c_str());
			ndSafeRelease(queueTop.first);
			continue;
		}

		MOD_DEBUG("Received %d-octet frame to demultiplex.", frame->getTotalUnitLength());

		// Test expected values.
		if ( ! validateFrame(frame) ) {
			frameIsValid = false;
			if ( getDropBadFrames() ) continue;
		}

		if ( frame->getMCID() != getMCID() ) {
			MOD_WARNING("Received Frame with wrong MCID, 0x%X instead of 0x%X.", frame->getMCID(), getMCID());
			frameIsValid = false;
			incBadMCIDCount();
			if ( getDropBadFrames() ) continue;
		}

		if ( frameIsValid ) incValidFrameCount();

		if ( ( outputLink = primaryOutputLinks_.find(frame->getVCID())) != primaryOutputLinks_.end() ) {
			MOD_DEBUG("Sending %d-octet buffer.", frame->getTotalUnitLength());
			outputLink->second->send(frame);
			frame = 0; // important
		}
		else {
			MOD_NOTICE("No output target defined for VCID %d, dropping data.", frame->getVCID());
		}
	}

	return svcEnd_();
}

HandlerLink* modTM_VC_Demux::getLink(const LinkType ltype, const ACE_UINT8 VCID /* = 0 */) {
	if ( ltype != BaseTrafficHandler::PrimaryOutputLink )
		return BaseTrafficHandler::getLink(ltype);

	HandlerLinkMap::iterator outputLink;
	if ( ( outputLink = primaryOutputLinks_.find(VCID)) != primaryOutputLinks_.end() ) {
		return outputLink->second;
	}
	else return 0;
}

}
