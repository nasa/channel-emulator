/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modTM_AF_Gen.cpp
 * @author Tad Kollar  
 *
 * $Id: modTM_AF_Gen.cpp 2403 2013-12-12 19:08:39Z tkollar $
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "modTM_AF_Gen.hpp"
#include "TM_Transfer_Frame.hpp"

namespace nTM_AF_Gen {

modTM_AF_Gen::modTM_AF_Gen(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr):
	TM_PhysicalChannel_Service(newName, newChannelName, newDLLPtr) {

}

modTM_AF_Gen::~modTM_AF_Gen() {
	MOD_DEBUG("Running ~modTM_AF_Gen().");
}

int modTM_AF_Gen::svc() {
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

		frame = dynamic_cast<TM_Transfer_Frame*>(queueTop.first);

		if ( !frame ) {
			MOD_ERROR("Received %d-octet buffer in %s wrapper (not TM_Transfer_Frame)! Must discard.",
				queueTop.first->getUnitLength(), queueTop.first->typeStr().c_str());
			ndSafeRelease(queueTop.first);
			continue;
		}

		queueTop.first = 0;

		MOD_DEBUG("Received a %d-octet Transfer Frame with GVCID %X.", frame->getUnitLength(), frame->getGVCID());

		if ( frame->getUnitLength() != getFrameSizeU() ) {
			MOD_ERROR("Received %d-octet message when exactly %d octets are required.", frame->getUnitLength(), getFrameSizeU());
			incBadLengthCount();
			if ( getDropBadFrames() ) continue;
		}
		else { incValidFrameCount(); }

		if ( links_[PrimaryOutputLink] ) {
			addErrorControlIfPossible(frame);
			links_[PrimaryOutputLink]->send(frame);
			frame = 0; // important
			MOD_DEBUG("Finished processing incoming frame.");
		}
		else {
			MOD_ERROR("No output target defined, dropping frame.");
		}
	}

	svcStart_();
	return svcEnd_();
}

}
