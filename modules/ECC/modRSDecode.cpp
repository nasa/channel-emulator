/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modRSDecode.cpp
 * @author Tad Kollar  
 *
 * $Id: modRSDecode.cpp 2344 2013-08-30 20:24:16Z tkollar $
 * Copyright (c) 2011.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "modRSDecode.hpp"

namespace nRSDecode {

const int modRSDecode::primitivePoly = 0x187;

const std::size_t modRSDecode::codeLength = 255;

const std::size_t modRSDecode::bitsPerSymbol = 8;

const Sym modRSDecode::primitiveRootIdx = 11;

const bool modRSDecode::doBasisTransform = true;

modRSDecode::modRSDecode(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr):
	BaseTrafficHandler(newName, newChannelName, newDLLPtr),
	_maxErrorsPerCodeword(16),
	_maxErrorsPerCodewordSetting(CEcfg::instance()->getOrAddInt(cfgKey("maxErrorsPerCodeword"), _maxErrorsPerCodeword)),
	_interleavingDepth(1),
	_interleavingDepthSetting(CEcfg::instance()->getOrAddInt(cfgKey("interleavingDepth"), _interleavingDepth)),
	_dataLength(0),
	_correctedErrorCount(0),
	_uncorrectedErrorCount(0),
	_errorlessUnitCount(0),
	_correctedUnitCount(0),
	_uncorrectedUnitCount(0),
	_rsDecoder(0) {

	setMaxErrorsPerCodeword(_maxErrorsPerCodewordSetting);
	setInterleavingDepth(_interleavingDepthSetting);

	rebuildDecoder();
}


modRSDecode::~modRSDecode() {
	MOD_DEBUG("Running ~modRSDecode().");
	stopTraffic();
	delete _rsDecoder;
}

int modRSDecode::svc() {
	svcStart_();
	NetworkData *correctedData;
	std::size_t dataLen, errs;
	SymVec corrected;

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

		MOD_DEBUG("Received %d octets to decode", dataLen);

		if ( links_[PrimaryOutputLink] ) {
			if ( dataLen > codeLength * _interleavingDepth ) {
				MOD_WARNING("Encoded unit length %d is greater than maximum interleaved codeword length %d, dropping.",
					dataLen, codeLength * _interleavingDepth);
				ndSafeRelease(data);
			}
			else {
				try {
					corrected = _rsDecoder->decode(data);
					correctedData = new NetworkData(corrected.size(), &corrected[0]);

					errs = _rsDecoder->getLastCorrectedErrors();
					if ( errs == 0 ) ++_errorlessUnitCount;
					else ++_correctedUnitCount;

					_correctedErrorCount += errs;
					MOD_DEBUG("Corrected %d errors. Now sending %d octets.", errs, correctedData->getUnitLength());
					links_[PrimaryOutputLink]->send(correctedData);
				}
				catch (RSDecoder::TooManyErrors& tme) {
					++_uncorrectedUnitCount;
					_uncorrectedErrorCount += tme.errors;
					MOD_NOTICE("Failed to decode: %s. Dropping data.", tme.what());
				}
				catch (std::logic_error& e) {
					MOD_WARNING("Failed to decode: %s. Dropping data.\nMax errors/codeword: %d\nInterleaving depth: %d\nMessage length: %d",
						e.what(), _maxErrorsPerCodeword, _interleavingDepth, data->getUnitLength());
				}

				ndSafeRelease(data);

			}
		}
		else {
			MOD_NOTICE("No output target defined yet, dropping data.");
			ndSafeRelease(data);
		}

	}

	return svcEnd_();
}

void modRSDecode::rebuildDecoder() {
	_resetLengths();

	MOD_DEBUG("Constructing a RS(%d,%d) code with an interleaving depth of %d.",
		codeLength, _dataLength, _interleavingDepth);

	if ( _maxErrorsPerCodeword != 8 && _maxErrorsPerCodeword != 16)
		throw BadValue(BadValue::msg("Max errors per codeword", _maxErrorsPerCodeword));

	_rsDecoder = new RSDecoder(primitivePoly,
		_maxErrorsPerCodeword * 2,
		codeLength,
		bitsPerSymbol,
		primitiveRootIdx,
		(_maxErrorsPerCodeword == 8)? 120 : 112 ,
		_interleavingDepth,
		doBasisTransform);
}

}
