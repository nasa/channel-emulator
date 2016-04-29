/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modExtractor.cpp
 * @author Tad Kollar  
 *
 * $Id: modExtractor.cpp 2033 2013-06-05 18:28:52Z tkollar $
 * Copyright (c) 2010.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "modExtractor.hpp"

namespace nExtractor {

modExtractor::modExtractor(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr):
	BaseTrafficHandler(newName, newChannelName, newDLLPtr),
	_headerLength(CEcfg::instance()->getOrAddInt(cfgKey("headerLength"))),
	_trailerLength(CEcfg::instance()->getOrAddInt(cfgKey("trailerLength"))),
	_headerOctetCount(CEcfg::instance()->getOrAddInt(cfgKey("headerOctetCount"))),
	_trailerOctetCount(CEcfg::instance()->getOrAddInt(cfgKey("trailerOctetCount"))),
	_stubCount(CEcfg::instance()->getOrAddInt(cfgKey("stubCount")))  {

}

modExtractor::~modExtractor() {
	MOD_DEBUG("Running ~modExtractor().");
}

int modExtractor::svc() {
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

		MOD_DEBUG("Received %d octets to extract from.", data->getUnitLength());

		if ( links_[PrimaryOutputLink] ) {
			if ( data->getUnitLength() < static_cast<unsigned>( getHeaderLength() + getTrailerLength() ) ) {
				MOD_INFO("%d-octet unit is too small to extract, sending unchanged.", data->getUnitLength());
				incStubCount();
			}
			else {
				size_t newUnitLength = data->getUnitLength() - getHeaderLength() - getTrailerLength();

				char* startPtr = data->rd_ptr();
				data->rd_ptr(startPtr + getHeaderLength());
				data->setUnitLength(newUnitLength);

				MOD_DEBUG("New unit length is now %d octets.", data->getUnitLength());

				incHeaderOctetCount(getHeaderLength());
				incTrailerOctetCount(getTrailerLength());
			}

			MOD_DEBUG("Sending %d octets.", data->getUnitLength());
			links_[PrimaryOutputLink]->send(data);
		}
		else {
			MOD_NOTICE("No output target defined yet, dropping data.");
		}

	}

	return svcEnd_();

}

}
