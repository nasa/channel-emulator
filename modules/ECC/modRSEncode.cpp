/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modRSEncode.cpp
 * @author Tad Kollar  
 *
 * $Id: modRSEncode.cpp 2344 2013-08-30 20:24:16Z tkollar $
 * Copyright (c) 2011.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "modRSEncode.hpp"

namespace nRSEncode {

const int modRSEncode::primitivePoly = 0x187;

const std::size_t modRSEncode::codeLength = 255;

const std::size_t modRSEncode::bitsPerSymbol = 8;

const Sym modRSEncode::primitiveRootIdx = 11;

const bool modRSEncode::doBasisTransform = true;

modRSEncode::modRSEncode(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr):
	BaseTrafficHandler(newName, newChannelName, newDLLPtr),
	_maxErrorsPerCodeword(16),
	_maxErrorsPerCodewordSetting(CEcfg::instance()->getOrAddInt(cfgKey("maxErrorsPerCodeword"), _maxErrorsPerCodeword)),
	_interleavingDepth(1),
	_interleavingDepthSetting(CEcfg::instance()->getOrAddInt(cfgKey("interleavingDepth"), _interleavingDepth)),
	_dataLength(0), _rsEncoder(0) {

	setMaxErrorsPerCodeword(_maxErrorsPerCodewordSetting);
	setInterleavingDepth(_interleavingDepthSetting);

	rebuildEncoder();
}


modRSEncode::~modRSEncode() {
	MOD_DEBUG("Running ~modRSEncode().");
	stopTraffic();
	delete _rsEncoder;
}

int modRSEncode::svc() {
	svcStart_();
	int dataLen;
	SymVec parity;

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
		dataLen = data->getUnitLength();

		MOD_DEBUG("Received %d octets to encode.", dataLen);

		if ( links_[PrimaryOutputLink] ) {
			if ( dataLen > _dataLength * _interleavingDepth ) {
				MOD_WARNING("Unit length %d is greater than maximum message length %d, dropping.",
					dataLen, _dataLength * _interleavingDepth);
				ndSafeRelease(data);
			}
			else {
				parity = _rsEncoder->encode(data);
				data->append(&parity[0], parity.size());

				MOD_DEBUG("Sending %d octets.", data->getUnitLength());
				links_[PrimaryOutputLink]->send(data);
			}
		}
		else {
			MOD_NOTICE("No output target defined yet, dropping data.");
			ndSafeRelease(data);
		}

	}

	return svcEnd_();
}

void modRSEncode::rebuildEncoder() {
	_resetLengths();

	MOD_DEBUG("Constructing a CCSDS RS(%d,%d) encoder with an interleaving depth of %d.",
		codeLength,	_dataLength, _interleavingDepth);

	if (_maxErrorsPerCodeword != 8 && _maxErrorsPerCodeword != 16)
		throw BadValue(BadValue::msg("Max errors per codeword", _interleavingDepth));

	_rsEncoder = new RSEncoder(primitivePoly,
		_maxErrorsPerCodeword * 2,
		codeLength,
		bitsPerSymbol,
		primitiveRootIdx,
		(_maxErrorsPerCodeword == 8)? 120 : 112 ,
		_interleavingDepth,
		doBasisTransform);

}

}
