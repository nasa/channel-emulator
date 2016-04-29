/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modSplitter.cpp
 * @author Tad Kollar  
 *
 * $Id: modSplitter.cpp 2344 2013-08-30 20:24:16Z tkollar $
 * Copyright (c) 2010.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "modSplitter.hpp"
#include "NetworkData.hpp"

namespace nSplitter {

modSplitter::modSplitter(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr):
	BaseTrafficHandler(newName, newChannelName, newDLLPtr) {
}

modSplitter::~modSplitter() {
	MOD_DEBUG("Running ~modSplitter().");
}

int modSplitter::svc() {
	svcStart_();

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

		MOD_DEBUG("Received a %d-octet data unit to split.", data->getUnitLength());

		NetworkData* dupData = data->wrapInnerPDU<NetworkData>(data->getUnitLength(), data->ptrUnit());

		if ( links_[PrimaryOutputLink] ) {
			MOD_DEBUG("Sending %d octets via primary output link.", data->getUnitLength());
			links_[PrimaryOutputLink]->send(data);
		}
		else {
			MOD_NOTICE("No primary output target defined yet, dropping data.");
			ndSafeRelease(data);
		}

		if ( links_[AuxOutputLink] ) {
			MOD_DEBUG("Sending %d octets via auxiliary output link.", dupData->getUnitLength());
			links_[AuxOutputLink]->send(dupData);
		}
		else {
			MOD_NOTICE("No auxiliary output target defined yet, dropping data copy.");
			ndSafeRelease(dupData);
		}
	}

	return svcEnd_();

}

}
