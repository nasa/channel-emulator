/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modTM_AF_Rcv.cpp
 * @author Tad Kollar  
 *
 * $Id: modTM_AF_Rcv.cpp 2489 2014-02-13 16:44:28Z tkollar $
 * Copyright (c) 2012.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "modTM_AF_Rcv.hpp"
#include "printers.hpp"

namespace nTM_AF_Rcv {

modTM_AF_Rcv::modTM_AF_Rcv(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr):
	TM_PhysicalChannel_Service(newName, newChannelName, newDLLPtr), _frameTemplate(0) {
	updateTemplate_();
}

modTM_AF_Rcv::~modTM_AF_Rcv() {
	MOD_DEBUG("Running ~modTM_AF_Rcv().");
}

int modTM_AF_Rcv::svc() {
	svcStart_();

	TM_Transfer_Frame* frame = 0;

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

		// Be tricky - if we received a pointer to an actual TM_Transfer_Frame object,
		// continue using it. Otherwise, build a new wrapper around the received data.
		if ( ! (frame = dynamic_cast<TM_Transfer_Frame*>(queueTop.first)) ) {
			MOD_DEBUG("Generating TM_Transfer_Frame wrapper.");
			frame = new TM_Transfer_Frame(_frameTemplate);
			frame->deepCopy(queueTop.first);
			ndSafeRelease(queueTop.first);
		}
		else {
			MOD_DEBUG("Retaining existing TM_Transfer_Frame wrapper.");
		}

		frame->useFrameErrorControl(getUseFrameErrorControl());

		queueTop.first = 0;

		MOD_DEBUG("Received %d-octet TM Transfer Frame with GVCID %X.", frame->getUnitLength(), frame->getGVCID());

		if ( ! validateFrame(frame, getDropBadFrames()) ) {
			if ( getDropBadFrames() ) continue;
		}
		else if ( getUseFrameErrorControl() && ! frame->hasValidCRC() ) {
			MOD_WARNING("TM Transfer Frame has bad checksum.");
			incBadFrameChecksumCount();
			if ( getDropBadFrames() ) continue;
		}
		else {
			incValidFrameCount();
		}

		// If this is idle data, move along. Checking frame count is optional.
		if (frame->onlyIdleData()) continue;
		// TODO: Increase idle frame count?

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

void modTM_AF_Rcv::updateTemplate_() {
	ndSafeRelease(_frameTemplate);

	_frameTemplate = new TM_Transfer_Frame(
		getFrameSize(),
		0,
		false, // existance of OCF is VC dependent
		getUseFrameErrorControl()
	);

	if (! _frameTemplate ) throw nd_error("Unable to allocate an TM_Transfer_Frame!");
}

}
