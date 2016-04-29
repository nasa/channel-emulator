/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modAOS_OCF_Extract.cpp
 * @author Tad Kollar  
 *
 * $Id: modAOS_OCF_Extract.cpp 2403 2013-12-12 19:08:39Z tkollar $
 * Copyright (c) 2012.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "modAOS_OCF_Extract.hpp"
#include "AOS_Transfer_Frame.hpp"

using namespace nAOS_OCF_Extract;

namespace nAOS_OCF_Extract {

modAOS_OCF_Extract::modAOS_OCF_Extract(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr):
	AOS_MasterChannel_Service(newName, newChannelName, newDLLPtr) {

}

modAOS_OCF_Extract::~modAOS_OCF_Extract() {
	MOD_DEBUG("Running ~modAOS_OCF_Extract().");
}

void modAOS_OCF_Extract::_extractAndSendOCF(AOS_Transfer_Frame* frame) {
	if ( frame->hasOperationalControl() ) {
		if ( links_[AuxOutputLink] ) {
			MOD_DEBUG("Sending a Type-%d Report.", frame->getReportType());
			switch (frame->getReportType()) {
				case 1:
					links_[AuxOutputLink]->send(frame->getCLCW());
					break;
				case 2:
					links_[AuxOutputLink]->send(frame->wrapInnerPDU<NetworkData>(AOS_Transfer_Frame::spanOperationalControlField,
						frame->ptrOperationalControl()));
					break;
			}
		}
		else {
			MOD_WARNING("No auxiliary output, cannot send OCF unit.");
		}
	}
	else {
		MOD_DEBUG("The Operational Control Field Flag is unset, not extracting.");
	}
}

int modAOS_OCF_Extract::svc() {
	svcStart_();

	AOS_Transfer_Frame* frame = 0;

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

		frame = dynamic_cast<AOS_Transfer_Frame*>(queueTop.first);

		if ( ! frame ) {
			MOD_INFO("Received %d octets of non-AOS data! Dropping.", queueTop.first->getUnitLength());
			ndSafeRelease(queueTop.first);
			continue;
		}

		queueTop.first = 0;

		MOD_DEBUG("Received %d-octet frame.", frame->getUnitLength());
		frame->useOperationalControl(true);

		// Test expected values.
		if (frame->getUnitLength() != getFrameSizeU()) {
			MOD_WARNING("AOS Transfer Frame bad length: received %d octets, needed %d.", frame->getUnitLength(), getFrameSize());
			incBadLengthCount();
			if ( getDropBadFrames() ) continue;
		}
		else incValidFrameCount();

		_extractAndSendOCF(frame);

		if ( links_[PrimaryOutputLink] ) {
			MOD_DEBUG("Sending a %d-octet AOS Transfer Frame.", frame->getUnitLength());
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
