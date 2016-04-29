/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modASM_Remove.cpp
 * @author Tad Kollar  
 *
 * $Id: modASM_Remove.cpp 2532 2014-02-20 15:23:53Z tkollar $
 * Copyright (c) 2010.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "modASM_Remove.hpp"
#include "NetworkData.hpp"
#include <bitset>

namespace nASM_Remove {

modASM_Remove::modASM_Remove(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr):
	BaseTrafficHandler(newName, newChannelName, newDLLPtr),
	_marker(CEcfg::instance()->getOrAddArray(cfgKey("markerPattern"))),
	_expectedUnitLength(256),
	_expectedUnitLengthSetting(CEcfg::instance()->getOrAddInt(cfgKey("expectedUnitLength"), _expectedUnitLength)),
	_allowedMarkerBitErrors(0),
	_allowedMarkerBitErrorsSetting(CEcfg::instance()->getOrAddInt(cfgKey("allowedMarkerBitErrors"), 0)),
	_rebuildMarker(true),
	_currentBitErrors(0),
	_asmCount(0),
	_asmValidCount(0),
	_asmMissedCount(0),
	_asmSearchCount(0),
	_asmDiscoveredCount(0),
	_asmBitErrorsAllowed(0),
	_asmBitErrorsRejected(0),
	_asmAllowedWithBitErrorsCount(0),
	_asmRejectedWithBitErrorsCount(0),
	_asmPartialMismatch(0) {

	if ( _marker.getLength() == 0 ) {
		uint8_t defaultPattern[] = { 0x1A, 0xCF, 0xFC, 0x1D };
		setMarker(defaultPattern, 4);
	}

	_allowedMarkerBitErrors = _allowedMarkerBitErrorsSetting;
	_expectedUnitLength = _expectedUnitLengthSetting;

	MOD_DEBUG("Initializing with a %d-octet sync marker.", _marker.getLength());
}

modASM_Remove::~modASM_Remove() {
	MOD_DEBUG("Running ~modASM_Remove().");
}

void modASM_Remove::setMarker(const uint8_t* asmBuf, const int bufLen) {
	for (int i = 0; i < bufLen; i++) {
		// Add slots when needed
		if ( _marker.getLength() < i + 1 ) _marker.add(Setting::TypeInt);
		_marker[i].setFormat(Setting::FormatHex);
		_marker[i] = asmBuf[i];
	}

	// Eliminate extra slots
	while (_marker.getLength() > bufLen) _marker.remove(bufLen);
}

void modASM_Remove::setMarker(const std::vector<uint8_t>& newMarker) {
	for (unsigned i = 0; i < newMarker.size(); i++) {
		// Add slots when needed
		if ( static_cast<unsigned>(_marker.getLength()) < i + 1 )
			_marker.add(Setting::TypeInt);

		_marker[i].setFormat(Setting::FormatHex);
		_marker[i] = newMarker[i];
	}

	// Eliminate extra slots
	while (static_cast<unsigned>(_marker.getLength()) > newMarker.size())
		_marker.remove(newMarker.size());
}

void modASM_Remove::getMarker(std::vector<uint8_t>& marker) const {
	marker.clear();

	for ( int i = 0; i < _marker.getLength(); i++ )
		marker.push_back(static_cast<int>(_marker[i]) & 0xFF);
}

void modASM_Remove::_updateMarker(ACE_UINT8*& buffer, size_t& bufferLen) {
	if ( ! _rebuildMarker ) return;

	delete[] buffer;

	bufferLen = _marker.getLength();
	buffer = new ACE_UINT8[bufferLen];

	for (unsigned i = 0; i < bufferLen; i++) {
		buffer[i] = static_cast<int>(_marker[i]);
	}

	_rebuildMarker = false;

	MOD_DEBUG("Updated marker to %d-octet buffer.", bufferLen);
}

void modASM_Remove::_send(NetworkData*& goodUnit) {
	if ( links_[PrimaryOutputLink] ) {
		MOD_DEBUG("About to send (target queue size %d).", links_[PrimaryOutputLink]->getTarget()->getQueuedOctetCount());
		links_[PrimaryOutputLink]->send(goodUnit);
	}
	else {
		MOD_NOTICE("No output target defined yet, dropping data.");
		ndSafeRelease(goodUnit);
	}

	goodUnit = 0;
}

int modASM_Remove::svc() {
	svcStart_();
	ACE_UINT8* markerBuf = 0;
	size_t markerLen = 0;
	_rebuildMarker = true;
	NetworkData* data = 0;
	NetworkData* goodUnit = 0;
	size_t goodUnitLength = 0;
	size_t advanceLen = 0;
	size_t remainingLen = 0;
	size_t partialMarkerLen = 0;
	size_t markerOffset = 0;
	bool searching = false;

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

		data = queueTop.first;

		_updateMarker(markerBuf, markerLen);

		MOD_DEBUG("Received a %d-octet unit to test for ASMs.", data->getUnitLength());

		while ( data->getUnitLength() ) {
			if ( ! goodUnit ) { // no unit to continue, need to find next ASM
				partialMarkerLen = markerLen - markerOffset;

				// determine if enough buffer left to find ASM
				// if not, check what's left and check for remainder in next buffer
				if ( data->getUnitLength() < partialMarkerLen ) {
					if ( _markerMatch(data->ptrUnit(), markerBuf + markerOffset, data->getUnitLength()) ) {
						// this portion is correct
						markerOffset += data->getUnitLength();
					}
					else {
						MOD_DEBUG("Missed partial ASM, resetting marker offset and bit error count.");
						incPartialMismatchCount();
						markerOffset = 0;
						_currentBitErrors = 0;
					}

					advanceLen = data->getUnitLength();
				}
				// if so, look for ASM normally
				else if ( _markerMatch(data->ptrUnit(), markerBuf + markerOffset, partialMarkerLen) ) {
					MOD_DEBUG("Found ASM.");
					_asmCount++;

					if ( searching ) _asmDiscoveredCount++;
					else _asmValidCount++;

					searching = false;
					advanceLen = partialMarkerLen;
					markerOffset = 0;
					_currentBitErrors = 0;

					// determine if entire unit can be extracted from buffer
					// if so, wrap and send
                    if ( data->getUnitLength() - partialMarkerLen >= getExpectedUnitLength() ) {
						MOD_DEBUG("Found complete unit.");
						goodUnit = data->wrapInnerPDU<NetworkData>(getExpectedUnitLength(),
							data->ptrUnit() + partialMarkerLen);
						_send(goodUnit);
						advanceLen += getExpectedUnitLength();
                    }
					// if not, create a new unit to hold it
					else {
						MOD_DEBUG("Holding partial unit for expected completion.");
						goodUnitLength = data->getUnitLength() - partialMarkerLen;
						goodUnit = new NetworkData(getExpectedUnitLength());
						goodUnit->copyUnit(goodUnit->ptrUnit(), data->ptrUnit() + partialMarkerLen, goodUnitLength);
						advanceLen += goodUnitLength;
					};
				}
				else {
					MOD_DEBUG("Missed ASM, advancing one octet at a time.");
					if (! searching ) _asmMissedCount++; // Only increment for the first miss, or after another ASM was found.
					searching = true;
					incSearchCount();

					// data->dump();
					advanceLen = 1;
					markerOffset = 0;
					_currentBitErrors = 0;
				}
			}
			else { // continuing previous unit, don't look for ASM
				remainingLen = getExpectedUnitLength() - goodUnitLength;
				// determine if remaining unit fits in buffer
				// if so, append it and send, reset goodUnit to 0
				if ( data->getUnitLength() >= remainingLen ) {
					MOD_DEBUG("Completing partial unit with newly received data.");
					goodUnit->copyUnit(goodUnit->ptrUnit() + goodUnitLength, data->ptrUnit(),
						remainingLen);
					_send(goodUnit);
					advanceLen = remainingLen;
				}

				// if not, append it and don't send
				else {
					MOD_DEBUG("Adding to partial unit but will complete later.");

					goodUnit->copyUnit(goodUnit->ptrUnit() + goodUnitLength, data->ptrUnit(),
						data->getUnitLength());
					goodUnitLength += data->getUnitLength();
					advanceLen = data->getUnitLength();
				}
			}

			// advance read pointer by length just used up
			data->rd_ptr(advanceLen);
			MOD_DEBUG("Buffer length is now %d.", data->getUnitLength());
		}

		ndSafeRelease(data);
	}

	return svcEnd_();
}

bool modASM_Remove::_markerMatch(const ACE_UINT8* ptrUnitBuf, const ACE_UINT8* ptrMarkerBuf,
	const size_t testLen) {

	// Already exceeded allowable errors, doesn't matter if the rest matches.
	if ( _currentBitErrors > _allowedMarkerBitErrors ) return false;

	// Complete match, do nothing else.
	MOD_DEBUG("Found an exact ASM match (may be partial).");
	if ( ! memcmp(ptrUnitBuf, ptrMarkerBuf, testLen)) return true;

	// If the match must be exact, nothing else to do.
	if ( _allowedMarkerBitErrors == 0 ) return false;

	// Determine if there are only a few mismatched bits in the marker.
	for ( size_t offset = 0; offset < testLen; ++offset ) {
		// Use XOR to get mismatched bits.
		std::bitset<8> testOctet(*(ptrUnitBuf + offset) ^ *(ptrMarkerBuf + offset));
		_currentBitErrors += testOctet.count();

		if ( _currentBitErrors > _allowedMarkerBitErrors ) {
			incAsmBitErrorsRejected(_currentBitErrors);
			incRejectedWithBitErrorsCount();
			MOD_DEBUG("Too many wrong ASM bits (%d allowed), signaling mismatch.", _allowedMarkerBitErrors);
			return false;
		}
	}

	// Found a match with only a few errors.
	incAsmBitErrorsAllowed(_currentBitErrors);
	incAllowedWithBitErrorsCount();
	MOD_DEBUG("Found an ASM match with bit errors (%d total so far of %d allowed).",
		_currentBitErrors, _allowedMarkerBitErrors);
	return true;
}

}
