/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modAOS_VC_Rcv.cpp
 * @author Tad Kollar  
 *
 * $Id: modAOS_VC_Rcv.cpp 2405 2013-12-12 19:56:04Z tkollar $
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "modAOS_VC_Rcv.hpp"
#include "AOS_Transfer_Frame.hpp"
#include "AOS_Multiplexing_PDU.hpp"
#include "AOS_Bitstream_PDU.hpp"
#include "printers.hpp"

namespace nAOS_VC_Rcv {

modAOS_VC_Rcv::modAOS_VC_Rcv(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr):
	AOS_VirtualChannel_Service(newName, newChannelName, newDLLPtr) {

}

modAOS_VC_Rcv::~modAOS_VC_Rcv() {
	MOD_DEBUG("Running ~modAOS_VC_Rcv().");
}

int modAOS_VC_Rcv::svc() {
	svcStart_();

	AOS_Transfer_Frame* aos = 0;


	while ( continueService() ) {
		ndSafeRelease(aos); // Handle all cases from previous iteration

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

		aos = dynamic_cast<AOS_Transfer_Frame*>(queueTop.first);

		if ( ! aos ) {
			MOD_INFO("Received %d octets of non-AOS data! Dropping.", queueTop.first->getUnitLength());
			ndSafeRelease(queueTop.first);
			queueTop.first = 0;
			continue;
		}

		aos->useOperationalControl(getUseOperationalControl());
		aos->setVCFrameCountCycleUsageFlag(getUseVCFrameCycle());

		queueTop.first = 0;

		MOD_DEBUG("Received %d octet frame in VC# %d.", aos->getUnitLength(), getVCID());

		// Test expected values.
		if (aos->getTotalUnitLength() != getFrameSizeU()) {
			MOD_WARNING("AOS Transfer Frame bad length: received %d octets, needed %d.", aos->getUnitLength(), getFrameSize());
			incBadLengthCount();
			frameIsValid = false;
			continue;
		}

		if ( ! validateFrame(aos) ) {
			frameIsValid = false;
			if (getDropBadFrames()) continue;
		}

		if ( aos->getMCID() != getMCID() ) {
			MOD_WARNING("Received Frame with wrong MCID, 0x%X instead of 0x%X. Dropping.", aos->getMCID(), getMCID());
			incBadMCIDCount();
			frameIsValid = false;
			if (getDropBadFrames()) continue;
		}

		if ( aos->getVirtualChannelID() != getVCID() ) {
			MOD_WARNING("Received Frame with wrong VCID, 0x%X instead of 0x%X. Dropping.", aos->getVirtualChannelID(), getVCID());
			incBadGVCIDCount();
			frameIsValid = false;
			if (getDropBadFrames()) continue;
		}

		if (aos->isIdle()) continue; // If this is idle data, move along. Checking frame count is optional.

		if ( frameIsValid ) incValidFrameCount();

		// If we get an incorrect frame count here, reset the frame count and keep going.
		// TODO: Find the accepted behavior for getting a bad frame count
		if (aos->getVCFrameCount() != getFrameCount() ) {
			incFrameCountMisses();
			MOD_WARNING("Bad VC frame count: Got %d, expected %d. Missing %d frames.", aos->getVCFrameCount(), getFrameCount(),
				aos->getVCFrameCount() - getFrameCount());
			setFrameCount(aos->getVCFrameCount());
		}

		if (incFrameCount() > AOS_Transfer_Frame::maxVCFrameCount) {
			// Max frame count hit, increment the cycle counter and flip to 0
			setFrameCount(0);
			if ( getUseVCFrameCycle() ) incFrameCountCycle();
		}

		if ( links_[PrimaryOutputLink] ) {
			switch (getServiceType()) {
			case ServicePacket:
				MOD_DEBUG("Sending %d-octet M_PDU.",
					aos->getDataLength());
				links_[PrimaryOutputLink]->send(aos->wrapInnerPDU<AOS_Multiplexing_PDU>());
				break;
			case ServiceBitstream:
				MOD_DEBUG("Sending %d-octet B_PDU.",
					aos->getDataLength());

				links_[PrimaryOutputLink]->send(aos->wrapInnerPDU<AOS_Bitstream_PDU>());
				break;
			case ServiceAccess:
				MOD_DEBUG("Sending %d-octet VCA_SDU.",
					aos->getDataLength());

				links_[PrimaryOutputLink]->send(aos->wrapInnerPDU<NetworkData>());
				break;
			case ServiceIdle:
				// Shouldn't get here.
				MOD_WARNING("Encountered an Idle Transfer Frame that should have been ignored!");
				break;
			case ServiceInvalid:
				// Shouldn't get here.
				MOD_ERROR("Encountered an invalid Transfer Frame!");
				break;
		}
		}
		else {
			MOD_ERROR("No output target defined, dropping frame.");
		}

		MOD_DEBUG("Finished processing incoming frame.");
	}

	return svcEnd_();

}

size_t modAOS_VC_Rcv::getMTU() const { return getFrameSize() -
	AOS_Transfer_Frame::getNonDataLength(getUseHeaderErrorControl(),
		getUseOperationalControl(), getUseFrameErrorControl()) - getInsertZoneSize(); }

void modAOS_VC_Rcv::setMTU(const size_t newVal) {
	const size_t newFrameSize = newVal +
		AOS_Transfer_Frame::getNonDataLength(getUseHeaderErrorControl(),
			getUseOperationalControl(), getUseFrameErrorControl()) + getInsertZoneSize();

	setFrameSize(newFrameSize);
}

}
