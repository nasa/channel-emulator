/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modPseudoRandomize.cpp
 * @author Tad Kollar  
 *
 * $Id: modPseudoRandomize.cpp 2344 2013-08-30 20:24:16Z tkollar $
 * Copyright (c) 2008-2013.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "modPseudoRandomize.hpp"
#include "NetworkData.hpp"

namespace nPseudoRandomize {

const int modPseudoRandomize::seqBufLen = 65536;

modPseudoRandomize::modPseudoRandomize(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr):
	BaseTrafficHandler(newName, newChannelName, newDLLPtr), _seqBuf(0) {

	/// Generate the sequence.
	_seqBuf = new uint8_t[seqBufLen];
	ACE_OS::memset(_seqBuf, 0, seqBufLen);

	uint8_t gen = 0xff;
	int col = 0;
	uint8_t curBit = 0;

	for (int x = 0; x < seqBufLen * 8; x++) {
		curBit = gen & 1;
		_seqBuf[x/8] |= curBit << (7 - x%8);
		curBit ^= ((gen & 8) >> 3) ^ ((gen & 32) >> 5) ^ ((gen & 128) >> 7);
		++col;
		if ( x > 0 && ! x % 255 ) { gen = 255; }
		else { gen >>= 1; gen |= curBit << 7; }
	}
}


modPseudoRandomize::~modPseudoRandomize() {
	MOD_DEBUG("Running ~modPseudoRandomize().");

	stopTraffic();

	if (_seqBuf != 0) delete[] _seqBuf;
}

int modPseudoRandomize::svc() {
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

		MOD_DEBUG("Received a %d-octet data unit to randomize.", data->getUnitLength());

		for ( size_t x = 0; x < data->getUnitLength(); ++x ) {
			*data->ptrUnit(x) = *data->ptrUnit(x) ^ _seqBuf[x];
		}

		if ( links_[PrimaryOutputLink] ) {
			MOD_DEBUG("Sending %d octets via primary output link.", data->getUnitLength());
			links_[PrimaryOutputLink]->send(data);
		}
		else {
			MOD_NOTICE("No primary output target defined yet, dropping data.");
			ndSafeRelease(data);
		}
	}

	return svcEnd_();

}

}
