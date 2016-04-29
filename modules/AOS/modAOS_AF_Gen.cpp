/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modAOS_AF_Gen.cpp
 * @author Tad Kollar  
 *
 * $Id: modAOS_AF_Gen.cpp 2403 2013-12-12 19:08:39Z tkollar $
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "modAOS_AF_Gen.hpp"
#include "AOS_Transfer_Frame.hpp"

namespace nAOS_AF_Gen {

modAOS_AF_Gen::modAOS_AF_Gen(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr):
	AOS_PhysicalChannel_Service(newName, newChannelName, newDLLPtr),
	_aosHeaderEncoder(new RSEncoder(0x13, 4, 10, 4, 1, 6, 1, false))
{

}

modAOS_AF_Gen::~modAOS_AF_Gen() {
	MOD_DEBUG("Running ~modAOS_AF_Gen().");
	stopTraffic();
	delete _aosHeaderEncoder;
}

int modAOS_AF_Gen::svc() {
	svcStart_();

	AOS_Transfer_Frame* aos = 0;

	while ( continueService() ) {
		ndSafeRelease(aos);

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

		if ( !aos ) {
			MOD_ERROR("Received data (%s) that was not an AOS Transfer Frame.", queueTop.first->typeStr().c_str());
			ndSafeRelease(queueTop.first);
			continue;
		}

		queueTop.first = 0;

		MOD_DEBUG("Received a %d-octet Transfer Frame with GVCID %X.", aos->getUnitLength(), aos->getGVCID());

		if ( aos->getUnitLength() != getMRU() ) {
			MOD_ERROR("Received %d-octet message when exactly %d octets are required.",
				aos->getUnitLength(), getFrameSize());
			incBadLengthCount();

			if (getDropBadFrames()) continue;
		}
		else {
			incValidFrameCount();
		}

		if ( links_[PrimaryOutputLink] ) {
			addErrorControlIfPossible(aos);
			links_[PrimaryOutputLink]->send(aos);
			aos = 0; // Important so that ndSafeRelease at top doesn't actually delete.

			MOD_DEBUG("Finished processing incoming packet.");
		}
		else {
			MOD_ERROR("No output target defined, dropping packet.");
		}
	}

	return svcEnd_();
}

void modAOS_AF_Gen::addErrorControlIfPossible(AOS_Transfer_Frame* aos) {

	if ( getUseHeaderErrorControl() && aos->hasHeaderErrorControl() ) {

		Sym headerData[3];

		// Extract the 4-bit symbols to generate parity for.
		ACE_OS::memcpy(headerData, aos->ptrUnit(), 2);
		headerData[2] = aos->getSignalingField();
		SymVec headerDataUnpacked = _aosHeaderEncoder->unpack(headerData, 3, 6);

		// Generate the parity symbols and insert them back into the frame.
		SymVec headerParity = _aosHeaderEncoder->encode(headerDataUnpacked);
		_aosHeaderEncoder->pack(headerParity, 4, aos->ptrHeaderErrorControl(), 2);
	}

	if ( getUseFrameErrorControl() && aos->hasFrameErrorControl() ) aos->initializeCRC();
}

}
