/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modResolvePhaseAmbiguity.cpp
 * @author Tad Kollar  
 *
 * $Id: modResolvePhaseAmbiguity.cpp 2344 2013-08-30 20:24:16Z tkollar $
 * Copyright (c) 2011.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "modResolvePhaseAmbiguity.hpp"
#include <bitset>

using namespace nasaCE;

namespace nResolvePhaseAmbiguity {

const size_t modResolvePhaseAmbiguity::_markerTableRows = 16;

modResolvePhaseAmbiguity::modResolvePhaseAmbiguity(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr /* = 0 */):
	BaseTrafficHandler(newName, newChannelName, newDLLPtr),
	_leftShiftBits(0),
	_leftShiftBitsSetting(CEcfg::instance()->getOrAddInt(cfgKey("leftShiftBits"), _leftShiftBits)),
	_invert(false),
	_invertSetting(CEcfg::instance()->getOrAddBool(cfgKey("invert"), _invert)),
	_waitForNewDataInterval(ACE_Time_Value::zero),
	_maxUsecsForNewData(CEcfg::instance()->getOrAddInt(cfgKey("maxUsecsForNewData"), -1)),
	_marker(CEcfg::instance()->getOrAddArray(cfgKey("markerPattern"))),
	_rebuildMarkerTable(true),
	_markerTable(0),
	_markerLen(0),
	_detectShift(true),
	_detectShiftSetting(CEcfg::instance()->getOrAddBool(cfgKey("detectShift"), _detectShift)),
	_allowedMarkerBitErrors(0),
	_allowedMarkerBitErrorsSetting(CEcfg::instance()->getOrAddInt(cfgKey("allowedMarkerBitErrors"), _allowedMarkerBitErrors)) {

	_markerTable = new ACE_UINT8*[_markerTableRows];
	ACE_OS::memset(_markerTable, 0, _markerTableRows);

	setLeftShiftBits(_leftShiftBitsSetting);
	_invert = _invertSetting;

	ACE_Time_Value oneMillisec(0, 1000);
	if ( static_cast<int>(_maxUsecsForNewData) < 0 ) setWaitForNewDataInterval(oneMillisec);
	else {
		_waitForNewDataInterval.sec(getMaxUsecsForNewData()/1000000);
		_waitForNewDataInterval.usec(getMaxUsecsForNewData()%1000000);
	}

	if ( _marker.getLength() == 0 ) {
		uint8_t defaultPattern[] = { 0x1A, 0xCF, 0xFC, 0x1D };
		setMarker(defaultPattern, 4);
	}
	MOD_DEBUG("Using a %d-octet marker.", _marker.getLength());

	_detectShift = _detectShiftSetting;
	_allowedMarkerBitErrors = _allowedMarkerBitErrorsSetting;
}

modResolvePhaseAmbiguity::~modResolvePhaseAmbiguity() {
	MOD_DEBUG("Running ~modResolvePhaseAmbiguity().");
	stopTraffic();

	if (_markerTable[0]) {
		for ( size_t i = 0; i < _markerTableRows; ++i ) {
			delete _markerTable[i];
			_markerTable[i] = 0;
		}
	}

	delete[] _markerTable;
}

void modResolvePhaseAmbiguity::setMarker(const uint8_t* asmBuf, const int bufLen) {
	for (int i = 0; i < bufLen; i++) {
		// Add slots when needed
		if ( _marker.getLength() < i + 1 ) _marker.add(Setting::TypeInt);
		_marker[i] = asmBuf[i];
	}

	// Eliminate extra slots
	while (_marker.getLength() > bufLen) _marker.remove(bufLen);

	_rebuildMarkerTable = true;
}

void modResolvePhaseAmbiguity::setMarker(const std::vector<uint8_t>& newMarker) {
	for (unsigned i = 0; i < newMarker.size(); i++) {
		// Add slots when needed
		if ( static_cast<unsigned>(_marker.getLength()) < i + 1 )
			_marker.add(Setting::TypeInt);

		_marker[i] = newMarker[i];
	}

	// Eliminate extra slots
	while (static_cast<unsigned>(_marker.getLength()) > newMarker.size())
		_marker.remove(newMarker.size());

	_rebuildMarkerTable = true;
}

void modResolvePhaseAmbiguity::getMarker(std::vector<uint8_t>& marker) const {
	marker.clear();

	for ( int i = 0; i < _marker.getLength(); i++ )
		marker.push_back(static_cast<int>(_marker[i]) & 0xFF);
}

void modResolvePhaseAmbiguity::_updateMarkerTable() {
	if ( ! _rebuildMarkerTable ) return;

	_markerLen = _marker.getLength();

	if (_markerTable[0]) {
		for ( size_t i = 0; i < _markerTableRows; ++i ) {
			delete _markerTable[i];
			_markerTable[i] = 0;
		}
	}

	size_t row, leftShiftBits, rightShiftBits;

	for ( row = 0; row < _markerTableRows/2; ++row ) {
		_markerTable[row] = new ACE_UINT8[_markerLen];
		_markerTable[row+8] = new ACE_UINT8[_markerLen];

		leftShiftBits = row;
		rightShiftBits = 8 - leftShiftBits;
		for ( size_t offset = 0; offset < _markerLen; ++offset ) {
			if ( leftShiftBits == 0 ) { // no shift
				_markerTable[row][offset] = static_cast<int>(_marker[offset]) & 0xff;
			}
			else if ( offset < _markerLen - 1 ) { // shift first octets
				_markerTable[row][offset] = (static_cast<int>(_marker[offset]) & 0xff) << leftShiftBits |
					(static_cast<int>(_marker[offset + 1]) & 0xff) >> rightShiftBits;
			}
			else { // shift last octet
				_markerTable[row][offset] = (static_cast<int>(_marker[offset]) & 0xff) << leftShiftBits;
			}

			_markerTable[row+8][offset] = ~(_markerTable[row][offset]); // make another row for inverted values
		}
	}

	_rebuildMarkerTable = false;

	#ifdef DEFINE_DEBUG

	std::ostringstream s;
	for ( row = 0; row < _markerTableRows; ++row ) {
		s << "Row " << std::setw(2) << std::setfill(' ') << std::dec << row << ": ";
		for ( unsigned offset = 0; offset < _markerLen; ++offset ) {
			s	<< std::hex << std::setw(2) << std::setfill('0')
				<< static_cast<int>(_markerTable[row][offset]) << ' ';
		}
		s << std::endl;
	}

	std::string tableStr(s.str());
	MOD_DEBUG("Marker table:\n%s", tableStr.c_str());

	#endif

	MOD_DEBUG("Updated shifted marker table with %d-octet marker.", _markerLen);
}

bool modResolvePhaseAmbiguity::_inTableAt(const ACE_UINT8* buffer,
	const uint8_t& leftShiftBits, const bool isInverted) {

	size_t offset = 0;
	const size_t row = isInverted? leftShiftBits + 8 : leftShiftBits;
	const uint8_t finalOctetMask = 0xff << leftShiftBits;
	int currentBitErrors = 0;

	// Compare markerLen - 1 full octets
	for ( offset = 0; offset < _markerLen - 1; ++offset ) {
		if ( _markerTable[row][offset] != buffer[offset] && _allowedMarkerBitErrors == 0 ) return false;

		// Use XOR to get mismatched bits.
		std::bitset<8> testOctet(buffer[offset] ^ _markerTable[row][offset]);
		currentBitErrors += testOctet.count();

		if ( currentBitErrors > _allowedMarkerBitErrors ) return false;
	}

	// Compare 1-8 bits in final octet
	if ( _allowedMarkerBitErrors == 0 ) {
		if ( ( _markerTable[row][offset] & finalOctetMask ) != ( buffer[offset] & finalOctetMask)) return false;
	}
	else {
		std::bitset<8> testOctet(( buffer[offset] & finalOctetMask) ^ ( _markerTable[row][offset] & finalOctetMask ));
		currentBitErrors += testOctet.count();

		if ( currentBitErrors > _allowedMarkerBitErrors ) return false;
	}

	MOD_DEBUG("_inTableAt: Bit left shift is %d, inversion is %s (ASM table row %d). %d bit errors.",
		leftShiftBits, isInverted? "true" : "false", row, currentBitErrors);

	return true;
}

bool modResolvePhaseAmbiguity::_findInTable(const ACE_UINT8* buffer,
	uint8_t& leftShiftBits, uint8_t& isInverted) {

	// First try with given values
	if ( _inTableAt(buffer, leftShiftBits, isInverted)) return true;

	// That didn't succeed, so check every permutation.
	uint8_t testLeftShiftBits, testIsInverted;

	for ( testLeftShiftBits = 0; testLeftShiftBits < 8; ++testLeftShiftBits )
		for ( testIsInverted = 0; testIsInverted < 2; ++testIsInverted )
			if ( _inTableAt(buffer, testLeftShiftBits, testIsInverted)) {
				leftShiftBits = testLeftShiftBits;
				isInverted = testIsInverted;
				return true;
			}

	// Failed to find anything so reset the values
	return false;
}

void modResolvePhaseAmbiguity::_findShift(NetworkData*& data) {
	uint8_t leftShiftBits, isInverted;
	int counters[8][2] = {{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}};
	bool foundSomething = false;

	for ( size_t offset = 0; offset < data->getUnitLength() - _markerLen; ++offset ) {
		// Set these to configured/previously found values to hopefully skip a few iterations
		leftShiftBits = getLeftShiftBits();
		isInverted = getInversion();

		if ( _findInTable(data->ptrUnit() + offset, leftShiftBits, isInverted ) ) {
			++counters[leftShiftBits][isInverted];
			foundSomething = true;
		}
	}

	if ( ! foundSomething ) {
		MOD_NOTICE("_findShift: Could not locate a shifted and/or inverted ASM in the data block.");
		return;
	}

	uint8_t mostLeftShiftBits = 0, mostIsInverted = 0;

	for ( leftShiftBits = 0; leftShiftBits < 8; ++leftShiftBits )
		for ( isInverted = 0; isInverted < 2; ++isInverted )
			if ( counters[leftShiftBits][isInverted] > counters[mostLeftShiftBits][mostIsInverted] ) {
				mostLeftShiftBits = leftShiftBits;
				mostIsInverted = isInverted;
			}

	setLeftShiftBits(mostLeftShiftBits);
	setInversion(mostIsInverted);

	MOD_DEBUG("_findShift: Bit left shift is %d, inversion is %s.",
		mostLeftShiftBits, mostIsInverted? "true" : "false");
}

int modResolvePhaseAmbiguity::svc() {
	svcStart_();

	ACE_Time_Value timeToWaitUntil;
	ACE_Message_Block* mb = 0;
	ACE_UINT8 carry = 0; // Bits left over from the previous data unit.
	bool haveCarry = false;

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

		size_t incomingBlockLen = data->getUnitLength();
		MOD_DEBUG("Received %d octets to shift and/or invert.", incomingBlockLen);

		if ( incomingBlockLen && links_[PrimaryOutputLink] ) {
			if ( getDetectShift() ) {
				_updateMarkerTable();
				_findShift(data);
			}

			NetworkData* shifted = new NetworkData(incomingBlockLen + 1);
			MOD_DEBUG("Created %d octet buffer to shift into.", shifted->getUnitLength());

			ACE_UINT8 rightShiftBits = 8 - _leftShiftBits;
			int carryOffset = 0;

			for (unsigned offset = 0; offset < incomingBlockLen; ++offset ) {
				if ( _leftShiftBits > 0 ) {

					if ( offset == 0 ) { // first octet
						if ( haveCarry ) {
							*(shifted->ptrUnit()) = carry | (*(data->ptrUnit()) >> _leftShiftBits);
							carry = 0;
							haveCarry = false;
							carryOffset = 0;
						}
						else {
							MOD_DEBUG("First octet, no carry, changing unit length to %d.",
								incomingBlockLen - 1);
							carryOffset = -1;
							shifted->setUnitLength(incomingBlockLen - 1, true);
							continue;
						}
					}
					else {
						*(shifted->ptrUnit() + offset + carryOffset) =
							(*(data->ptrUnit() + offset - 1) << rightShiftBits) |
							(*(data->ptrUnit() + offset) >> _leftShiftBits);
					}
				}
				else *(shifted->ptrUnit() + offset + carryOffset) = *(data->ptrUnit() + offset);

				if ( _invert ) *(shifted->ptrUnit() + offset + carryOffset) =
					~ *(shifted->ptrUnit() + offset + carryOffset);
			}

			if ( _leftShiftBits > 0 ) {
				carry = *(data->ptrUnit() + incomingBlockLen - 1 ) << rightShiftBits;
				haveCarry = true;
			}

			// Before sending, peek into the queue for up to the configured microseconds.
			// If nothing shows up, send the carry with the current block; otherwise
			// save it for the next.
			timeToWaitUntil = _getWaitUntilTime();
			if ( _leftShiftBits > 0 && msg_queue()->peek_dequeue_head(mb, &timeToWaitUntil) == -1 ) {
				MOD_DEBUG("Including carry on current block after not seeing a new block for %d microseconds.",
					getMaxUsecsForNewData());

				// Nothing else is coming, drop the carry.
				// *(shifted->ptrUnit() + shifted->getUnitLength() - 1 + carryOffset) = _invert? ~carry : carry;
				carry = 0;
				haveCarry = false;
			}

			if (shifted->getUnitLength() > incomingBlockLen) {
				// Either there's no left shift, or another block is waiting, send the carry with it.
				// Reduce the size of the unit by 1 octet (buffer size is unmodified).
				shifted->setUnitLength(incomingBlockLen, true);
			}

			MOD_DEBUG("Sending %d unit/%d buffer octets.", shifted->getUnitLength(),
				shifted->getBufferSize());
			links_[PrimaryOutputLink]->send(shifted);
		}
		else {
			if ( incomingBlockLen )
				MOD_NOTICE("No output target defined yet, dropping data.");
			else
				MOD_INFO("Received null data.");
		}

		ndSafeRelease(data);

		MOD_DEBUG("Finished processing incoming data block.");
	}

	return svcEnd_();

}

} // namespace nResolvePhaseAmbiguity
