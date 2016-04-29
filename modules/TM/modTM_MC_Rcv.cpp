/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modTM_MC_Rcv.cpp
 * @author Tad Kollar  
 *
 * $Id: modTM_MC_Rcv.cpp 2403 2013-12-12 19:08:39Z tkollar $
 * Copyright (c) 2012.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "modTM_MC_Rcv.hpp"
#include "TM_Transfer_Frame.hpp"

namespace nTM_MC_Rcv {

modTM_MC_Rcv::modTM_MC_Rcv(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr):
	TM_VirtualChannel_Service(newName, newChannelName, newDLLPtr) {

}

modTM_MC_Rcv::~modTM_MC_Rcv() {
	MOD_DEBUG("Running ~modTM_MC_Rcv().");
}

int modTM_MC_Rcv::svc() {
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

		frame->useOperationalControl(getUseOperationalControl());
		frame->setSecondaryHeaderLen(getFSHSize());
		frame->useFrameErrorControl(getUseFrameErrorControl());

		queueTop.first = 0;

		MOD_DEBUG("Received %d octet frame in MC# %d.", frame->getUnitLength(), getMCID());

		// Test expected values.
		if (frame->getTotalUnitLength() != getFrameSizeU()) {
			MOD_WARNING("TM Transfer Frame bad length: received %d octets, needed %d.", frame->getUnitLength(), getFrameSize());
			incBadLengthCount();
			frameIsValid = false;
			if ( getDropBadFrames() ) continue;
		}

		if ( ! validateFrame(frame) ) {
			frameIsValid = false;
			if ( getDropBadFrames() ) continue;
		}

		if ( frame->getMCID() != getMCID() ) {
			MOD_WARNING("Received Frame with wrong MCID, 0x%X instead of 0x%X.", frame->getMCID(), getMCID());
			incBadMCIDCount();
			frameIsValid = false;
			if ( getDropBadFrames() ) continue;
		}

		 // If this is idle data, move along. Checking frame count is optional.
		if (frame->onlyIdleData()) continue;

		// If we get an incorrect frame count here, reset the frame count and keep going.
		// TODO: Find the recommended behavior for getting a bad frame count
		if (frame->getMCFrameCount() != getCurrentFrameNumber() ) {
			MOD_WARNING("Bad MC frame count: Got %d, expected %d.", frame->getMCFrameCount(),
				getCurrentFrameNumber());
			setCurrentFrameNumber(frame->getMCFrameCount());
		}

		if ( frameIsValid ) incValidFrameCount();
		incCurrentFrameNumber(TM_Transfer_Frame::maxMCFrameCount);

		if ( links_[PrimaryOutputLink] ) {
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
