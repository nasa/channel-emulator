/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modAOS_VC_Frame.cpp
 * @author Tad Kollar  
 *
 * $Id: modAOS_VC_Frame.cpp 2403 2013-12-12 19:08:39Z tkollar $
 * Copyright (c) 2010.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "modAOS_VC_Frame.hpp"
#include "AOS_Transfer_Frame.hpp"

namespace nAOS_VC_Frame {

modAOS_VC_Frame::modAOS_VC_Frame(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr):
	AOS_VirtualChannel_Service(newName, newChannelName, newDLLPtr) {
	rebuildIdleUnitTemplate_();
}

modAOS_VC_Frame::~modAOS_VC_Frame() {
	MOD_DEBUG("Running ~modAOS_VC_Frame().");
}

int modAOS_VC_Frame::svc() {
	svcStart_();

	NetworkData* data = 0;
	AOS_Transfer_Frame* aos = 0;

	while ( continueService() ) {
		bool frameIsValid = true;
		ndSafeRelease(data); // Handle all cases from previous iteration

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

		data = queueTop.first;
		queueTop.first = 0;

		MOD_DEBUG("Channel GVCID %X: Received a %d-octet VC frame.", getGVCID(), data->getTotalUnitLength());

		if ( data->getTotalUnitLength() != getMRU() ) {
			MOD_ERROR("Received %d-octet frame when exactly %d octets are required.", data->getTotalUnitLength(), getMRU());
			incBadLengthCount();
			frameIsValid = false;
			if ( getDropBadFrames() ) continue;
		}

		if ( links_[PrimaryOutputLink] ) {
			ndSafeRelease(aos);
			aos = new AOS_Transfer_Frame(
				getFrameSize(), // all frames in MC have the same size
				data->ptrUnit() // copy the data buffer
			);

			if ( ! aos ) {
				MOD_ALERT("Failed to allocate an AOS_Transfer_Frame!", getName().c_str());
			}
			else if ( aos->getGVCID() != getGVCID() ) {
				ND_WARNING("[%s] Incoming frame GVCID %X does not match assigned GVCID %X.", aos->getGVCID(), getGVCID());
				incBadGVCIDCount();
				frameIsValid = false;
				if ( getDropBadFrames() ) continue;
			}

			if ( frameIsValid ) incValidFrameCount();

			if (aos) {
				MOD_DEBUG("Sending %-octet frame to next segment.", aos->getUnitLength());
				links_[PrimaryOutputLink]->send(aos);
				aos = 0; // important
			}
		}
		else {
			MOD_ERROR("No output target defined, dropping packet.");
		}
	}

	return svcEnd_();
}

NetworkData* modAOS_VC_Frame::getIdleUnit() {
	if (!idleUnitTemplate_)
		throw NoIdleUnit("The AOS transfer frame idle template has not been constructed yet.");

	idleUnitTemplateCreation_.acquire();
	AOS_Transfer_Frame* idleDup = new AOS_Transfer_Frame(dynamic_cast<AOS_Transfer_Frame*>(idleUnitTemplate_));
	idleUnitTemplateCreation_.release();
	return idleDup;
}

void modAOS_VC_Frame::rebuildIdleUnitTemplate_() {
	MOD_DEBUG("Rebuilding Idle AOS Transfer Frame template.");
	idleUnitTemplateCreation_.acquire();

	AOS_Transfer_Frame* newFrame = new AOS_Transfer_Frame(static_cast<size_t>(getFrameSize()));
	newFrame->makeIdle(getSCID());

	NetworkData* idleData = new NetworkData(idlePattern_.getLength());
	for (int i = 0; i < idlePattern_.getLength(); i++ ) {
		*(idleData->ptrUnit() + i) = static_cast<int>(idlePattern_[i]) & 0xFF;
	}

	newFrame->setDataToPattern(idleData);
	ndSafeRelease(idleData);
	ndSafeRelease(idleUnitTemplate_);
	idleUnitTemplate_ = newFrame;

	idleUnitTemplateCreation_.release();
}

}
