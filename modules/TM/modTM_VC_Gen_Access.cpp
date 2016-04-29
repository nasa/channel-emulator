/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modTM_VC_Gen_Access.cpp
 * @author Tad Kollar  
 *
 * $Id: modTM_VC_Gen_Access.cpp 2403 2013-12-12 19:08:39Z tkollar $
 * Copyright (c) 2012.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "modTM_VC_Gen_Access.hpp"
#include "TM_Transfer_Frame.hpp"

using namespace nTM_VC_Gen_Access;

namespace nTM_VC_Gen_Access {

modTM_VC_Gen_Access::modTM_VC_Gen_Access(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr):
	TM_VirtualChannel_Service(newName, newChannelName, newDLLPtr) {
	rebuildIdleUnitTemplate_();
}

modTM_VC_Gen_Access::~modTM_VC_Gen_Access() {
	MOD_DEBUG("Running ~modTM_VC_Gen_Access().");
}

int modTM_VC_Gen_Access::svc() {
	svcStart_();
	NetworkData* data = 0;

	while ( continueService() ) {
		ndSafeRelease(data);
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

		data = queueTop.first;
		queueTop.first = 0;

		MOD_DEBUG("Received %d octets to frame into TM Virtual Channel with VCID %X.", data->getUnitLength(), getVCID());

		if ( data->getUnitLength() != getMRU() ) {
			MOD_ERROR("Received %d-octet message when exactly %d octets are required.",
					  data->getUnitLength(), getMRU());
			frameIsValid = false;
			if ( getDropBadFrames() ) continue;
		}

		TM_Transfer_Frame* frame = new TM_Transfer_Frame(
			getFrameSize(), // all frames in MC have the same size
			0, // we have the contents the data zone, but no TF header/trailer yet
			getUseOperationalControl(), // existance of OCF is VC dependent
			getUseFrameErrorControl(), // all frames in MC have frame CRC, or not
			getFSHSize() // size of secondary header
		);

		if ( ! frame ) {
			MOD_ALERT("Failed to allocate a TM_Transfer_Frame object! Must discard unit.");
			continue;
		}

		frame->build(
			getSCID(),
			getVCID(),
			0, // MC frame count
			getCurrentFrameNumber(),
			data
		);

		frame->setSynchronizationFlag(true);

		if (frameIsValid) incValidFrameCount();
		incCurrentFrameNumber(TM_Transfer_Frame::maxVCFrameCount);

		if ( links_[PrimaryOutputLink] ) {
			links_[PrimaryOutputLink]->send(frame);
			frame = 0; // important
			MOD_DEBUG("Finished processing incoming packet.");
		}
		else {
			MOD_ERROR("No output target defined, dropping frame.");
		}

	}

	return svcEnd_();
}

size_t modTM_VC_Gen_Access::getMRU() const {
	return getFrameSize() -
	TM_Transfer_Frame::getNonDataLength(getUseOperationalControl(), getUseFrameErrorControl(), getFSHSize());
}

void modTM_VC_Gen_Access::setMRU(const size_t newVal) {
	const size_t newFrameSize = newVal +
		TM_Transfer_Frame::getNonDataLength(getUseOperationalControl(), getUseFrameErrorControl(), getFSHSize());

	setFrameSize(newFrameSize);
}

NetworkData* modTM_VC_Gen_Access::getIdleUnit() {
	if (!idleUnitTemplate_)
		throw NoIdleUnit("The TM transfer frame idle template has not been constructed yet.");

	idleUnitTemplateCreation_.acquire();
	TM_Transfer_Frame* idleDup = new TM_Transfer_Frame(dynamic_cast<TM_Transfer_Frame*>(idleUnitTemplate_));
	idleUnitTemplateCreation_.release();
	return idleDup;
}

void modTM_VC_Gen_Access::rebuildIdleUnitTemplate_() {
	MOD_DEBUG("Rebuilding Idle TM Transfer Frame template.");
	idleUnitTemplateCreation_.acquire();

	TM_Transfer_Frame* newFrame = new TM_Transfer_Frame(static_cast<size_t>(getFrameSize()));

	NetworkData* idleData = new NetworkData(idlePattern_.getLength());
	for (int i = 0; i < idlePattern_.getLength(); i++ ) {
		*(idleData->ptrUnit() + i) = static_cast<int>(idlePattern_[i]) & 0xFF;
	}

	newFrame->makeIdle(getSCID(), getVCID(), idleData);

	ndSafeRelease(idleData);
	ndSafeRelease(idleUnitTemplate_);
	idleUnitTemplate_ = newFrame;

	idleUnitTemplateCreation_.release();
}

}
