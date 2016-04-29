/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modTM_MC_Gen.cpp
 * @author Tad Kollar  
 *
 * $Id: modTM_MC_Gen.cpp 2403 2013-12-12 19:08:39Z tkollar $
 * Copyright (c) 2012.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "modTM_MC_Gen.hpp"
#include "TM_Transfer_Frame.hpp"

namespace nTM_MC_Gen {

modTM_MC_Gen::modTM_MC_Gen(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr):
	TM_VirtualChannel_Service(newName, newChannelName, newDLLPtr) {
	rebuildIdleUnitTemplate_();
}

modTM_MC_Gen::~modTM_MC_Gen() {
	MOD_DEBUG("Running ~modTM_MC_Gen().");
}

int modTM_MC_Gen::svc() {
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

		queueTop.first = 0;

		if ( frame->getUnitLength() != getFrameSizeU() ) {
			MOD_ERROR("Received %d-octet unit when exactly %d octets are required.",
				frame->getUnitLength(), getFrameSizeU());
			incBadLengthCount();
			frameIsValid = false;
			if ( getDropBadFrames() ) continue;
		}

		if ( frame->getSpacecraftID() != getSCID() ) {
			MOD_WARNING("Spacecraft identifier (SCID) mismatch: Want %d, frame has %d.",
						getSCID(), frame->getSpacecraftID());
			incBadMCIDCount();
			frameIsValid = false;
			if ( getDropBadFrames() ) continue;
		}

		MOD_DEBUG("Received a %d-octet Transfer Frame with MCID %X.", frame->getUnitLength(), frame->getMCID());
		if (frameIsValid) incValidFrameCount();

		if ( links_[PrimaryOutputLink] ) {
			frame->setMCFrameCount(getCurrentFrameNumber());
			incCurrentFrameNumber(TM_Transfer_Frame::maxMCFrameCount);
			links_[PrimaryOutputLink]->send(frame);
			frame = 0; // important
			MOD_DEBUG("Finished processing incoming frame.");
		}
		else {
			MOD_ERROR("No output target defined, dropping frame.");
		}

	}

	return svcEnd_();
}

}
