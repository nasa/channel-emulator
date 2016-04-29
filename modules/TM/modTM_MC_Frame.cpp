/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modTM_MC_Frame.cpp
 * @author Tad Kollar  
 *
 * $Id: modTM_MC_Frame.cpp 2403 2013-12-12 19:08:39Z tkollar $
 * Copyright (c) 2012.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "modTM_MC_Frame.hpp"
#include "TM_Transfer_Frame.hpp"

namespace nTM_MC_Frame {

modTM_MC_Frame::modTM_MC_Frame(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr):
	TM_VirtualChannel_Service(newName, newChannelName, newDLLPtr) {
	rebuildIdleUnitTemplate_();
}

modTM_MC_Frame::~modTM_MC_Frame() {
	MOD_DEBUG("Running ~modTM_MC_Frame().");
}

int modTM_MC_Frame::svc() {
	svcStart_();

	NetworkData* data = 0;
	TM_Transfer_Frame* frame = 0;

	while ( continueService() ) {
		ndSafeRelease(data);
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

		data = queueTop.first;
		queueTop.first = 0;

		MOD_DEBUG("Channel MCID %X: Received a %d-octet MC frame.", getMCID(), data->getTotalUnitLength());

		if ( data->getTotalUnitLength() != getMRU() ) {
			MOD_ERROR("Received %d-octet frame when exactly %d octets are required.", data->getTotalUnitLength(), getMRU());
			incBadLengthCount();
			if ( getDropBadFrames() ) { ndSafeRelease(data); continue; }
		}

		TM_Transfer_Frame* frame = new TM_Transfer_Frame(
			getFrameSize(), // all frames in MC have the same size
			data->ptrUnit(), // copy the data
			getUseOperationalControl(), // existance of OCF is VC dependent
			getUseFrameErrorControl(), // all frames in MC have frame CRC, or not
			getFSHSize() // size of secondary header
		);

		if ( ! frame ) {
			MOD_ALERT("Failed to allocate a TM_Transfer_Frame!", getName().c_str());
			continue;
		}

		if ( frame->getMCID() != getMCID() ) {
			ND_WARNING("[%s] Incoming frame MCID %X does not match assigned MCID %X.", frame->getMCID(), getMCID());
			incBadMCIDCount();
			if ( getDropBadFrames() ) continue;
		}
		else {
			incValidFrameCount();
		}

		if ( links_[PrimaryOutputLink] ) {
			MOD_DEBUG("Sending %-octet frame to next segment.", frame->getUnitLength());
			links_[PrimaryOutputLink]->send(frame);
			frame = 0; // important
		}
		else {
			MOD_ERROR("No output target defined, dropping packet.");
		}
	}

	return svcEnd_();
}

NetworkData* modTM_MC_Frame::getIdleUnit() {
	if (!idleUnitTemplate_)
		throw NoIdleUnit("The TM transfer frame idle template has not been constructed yet.");

	idleUnitTemplateCreation_.acquire();
	TM_Transfer_Frame* idleDup = new TM_Transfer_Frame(dynamic_cast<TM_Transfer_Frame*>(idleUnitTemplate_));
	idleUnitTemplateCreation_.release();
	return idleDup;
}

void modTM_MC_Frame::rebuildIdleUnitTemplate_() {
	MOD_DEBUG("Rebuilding Idle TM Transfer Frame template.");
	idleUnitTemplateCreation_.acquire();

	TM_Transfer_Frame* newFrame = new TM_Transfer_Frame(static_cast<size_t>(getFrameSize()));

	NetworkData* idleData = new NetworkData(idlePattern_.getLength());
	for (int i = 0; i < idlePattern_.getLength(); i++ ) {
		*(idleData->ptrUnit() + i) = static_cast<int>(idlePattern_[i]) & 0xFF;
	}

	newFrame->makeIdle(getSCID(), getVCID(), idleData);

	newFrame->setDataToPattern(idleData);
	ndSafeRelease(idleData);
	ndSafeRelease(idleUnitTemplate_);
	idleUnitTemplate_ = newFrame;

	idleUnitTemplateCreation_.release();
}

}

