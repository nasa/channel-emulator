/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modTM_VC_Rcv_Access.cpp
 * @author Tad Kollar  
 *
 * $Id: modTM_VC_Rcv_Access.cpp 2405 2013-12-12 19:56:04Z tkollar $
 * Copyright (c) 2012.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "modTM_VC_Rcv_Access.hpp"
#include "TM_Transfer_Frame.hpp"

namespace nTM_VC_Rcv_Access {

modTM_VC_Rcv_Access::modTM_VC_Rcv_Access(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr):
	TM_VirtualChannel_Service(newName, newChannelName, newDLLPtr) {

}

modTM_VC_Rcv_Access::~modTM_VC_Rcv_Access() {
	MOD_DEBUG("Running ~modTM_VC_Rcv_Access().");
}

int modTM_VC_Rcv_Access::svc() {
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

		MOD_DEBUG("Received %d octet frame in VC# %d.", frame->getUnitLength(), getVCID());

		// Test expected values.
		if (frame->getUnitLength() != getFrameSizeU()) {
			MOD_WARNING("TM Transfer Frame bad length: received %d octets, needed %d.", frame->getUnitLength(), getFrameSize());
			incBadLengthCount();
			frameIsValid = false;
			if ( getDropBadFrames() ) continue;
		}

		if ( ! validateFrame(frame, true) ) {
			frameIsValid = false;
			if ( getDropBadFrames() ) continue;
		}

		if ( frame->getMCID() != getMCID() ) {
			MOD_WARNING("Received Frame with wrong MCID, 0x%X instead of 0x%X.", frame->getMCID(), getMCID());
			frameIsValid = false;
			incBadMCIDCount();
			if ( getDropBadFrames() ) continue;
		}

		if ( frame->getVCID() != getVCID() ) {
			MOD_WARNING("Received Frame with wrong VCID, 0x%X instead of 0x%X.", frame->getVCID(), getVCID());
			incBadVCIDCount();
			frameIsValid = false;
			if ( getDropBadFrames() ) continue;
		}

		if ( ! frame->getSynchronizationFlag() ) {
			MOD_WARNING("Synchronization flag is unset, a VCA_SDU is not indicated.");
			frameIsValid = false;
			if ( getDropBadFrames() ) continue;
		}

		if (frame->onlyIdleData()) continue; // If this is idle data, move along. Checking frame count is optional.

		// If we get an incorrect frame count here, reset the frame count and keep going.
		// TODO: Find the accepted behavior for getting a bad frame count
		if (frame->getVCFrameCount() != ( getCurrentFrameNumber()) ) {
			incFrameCountMisses();
			MOD_WARNING("Bad VC frame count: Got %d, expected %d.", frame->getVCFrameCount(),
				getCurrentFrameNumber());
			setCurrentFrameNumber(frame->getVCFrameCount());
		}

		if (frameIsValid) incValidFrameCount();
		incCurrentFrameNumber(TM_Transfer_Frame::maxVCFrameCount);

		if ( links_[PrimaryOutputLink] ) {
			links_[PrimaryOutputLink]->send(frame->wrapInnerPDU<NetworkData>());
		}
		else {
			MOD_ERROR("No output target defined, dropping frame.");
		}

		MOD_DEBUG("Finished processing incoming frame.");
	}

	return svcEnd_();


}

size_t modTM_VC_Rcv_Access::getMTU() const {
	return getFrameSize() -
	TM_Transfer_Frame::getNonDataLength(getUseOperationalControl(), getUseFrameErrorControl(), getFSHSize());
}

void modTM_VC_Rcv_Access::setMTU(const size_t newVal) {
	const size_t newFrameSize = newVal +
		TM_Transfer_Frame::getNonDataLength(getUseOperationalControl(), getUseFrameErrorControl(), getFSHSize());

	setFrameSize(newFrameSize);
}

}
