/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modAOS_VC_Gen.cpp
 * @author Tad Kollar  
 *
 * $Id: modAOS_VC_Gen.cpp 2406 2013-12-12 20:09:02Z tkollar $
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "modAOS_VC_Gen.hpp"
#include "AOS_Transfer_Frame.hpp"
#include "TC_Comm_Link_Control_Word.hpp"

namespace nAOS_VC_Gen {

modAOS_VC_Gen::modAOS_VC_Gen(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr):
	AOS_VirtualChannel_Service(newName, newChannelName, newDLLPtr) {

	rebuildIdleUnitTemplate_();
}

modAOS_VC_Gen::~modAOS_VC_Gen() {
	MOD_DEBUG("Running ~modAOS_VC_Gen().");
}

int modAOS_VC_Gen::svc() {
	svcStart_();

	// If we don't have a CLCW to insert, call the OCF a Type-2 Report.
	const ACE_UINT8 reportType = 2;

	while ( continueService() ) {
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

		NetworkData* data = queueTop.first;
		queueTop.first = 0;

		MOD_DEBUG("Received %d octets to frame into Virtual Channel with GVCID %X.", data->getUnitLength(), getGVCID());

		if ( data->getUnitLength() != getMRU() ) {
			MOD_ERROR("Received %d-octet message when exactly %d octets are required, dropping.", data->getUnitLength(), getMRU());
		}
		else if ( links_[PrimaryOutputLink] ) {

			AOS_Transfer_Frame* aos = new AOS_Transfer_Frame(
				getFrameSize(), // all frames in MC have the same size
				0, // we have the contents the data zone, but no TF header/trailer yet
				getUseHeaderErrorControl(), // all frames in MC use header error control or not.
				getInsertZoneSize(), // all frames in MC have Insert Zone, or not
				getUseOperationalControl(), // existance of OCF is VC dependent
				getUseFrameErrorControl() // all frames in MC have frame CRC, or not
			);

			if (! aos) throw nd_error("Unable to allocate an AOS_Transfer_Frame!");

			aos->build(
				getSCID(),
				getVCID(),
				getFrameCount(),
				false, // Replay flag TODO: Consider usage in testing
				getUseVCFrameCycle(),
				getFrameCountCycle(),
				reportType,
				data
			);

			links_[PrimaryOutputLink]->send(aos);
			aos = 0;

			if (incFrameCount() > AOS_Transfer_Frame::maxVCFrameCount) {
				// Max frame count hit, increment the cycle counter and flip to 0
				setFrameCount(0);
				if ( getUseVCFrameCycle() ) incFrameCountCycle();
			}

			MOD_DEBUG("Finished processing incoming packet.");
		}
		else {
			MOD_ERROR("No output target defined, dropping packet.");
		}

		ndSafeRelease(data);

	}

	return svcEnd_();
}

size_t modAOS_VC_Gen::getMRU() const { return getFrameSize() -
	AOS_Transfer_Frame::getNonDataLength(getUseHeaderErrorControl(),
		getUseOperationalControl(), getUseFrameErrorControl()) - getInsertZoneSize(); }

void modAOS_VC_Gen::setMRU(const size_t newVal) {
	const size_t newFrameSize = newVal +
		AOS_Transfer_Frame::getNonDataLength(getUseHeaderErrorControl(),
			getUseOperationalControl(), getUseFrameErrorControl()) + getInsertZoneSize();

	setFrameSize(newFrameSize);
}

NetworkData* modAOS_VC_Gen::getIdleUnit() {
	if (!idleUnitTemplate_)
		throw NoIdleUnit("The AOS transfer frame idle template has not been constructed yet.");

	idleUnitTemplateCreation_.acquire();
	AOS_Transfer_Frame* idleDup = new AOS_Transfer_Frame(dynamic_cast<AOS_Transfer_Frame*>(idleUnitTemplate_));
	idleUnitTemplateCreation_.release();
	return idleDup;
}

void modAOS_VC_Gen::rebuildIdleUnitTemplate_() {
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
