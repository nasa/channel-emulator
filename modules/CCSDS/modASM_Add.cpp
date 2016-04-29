/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modASM_Add.cpp
 * @author Tad Kollar  
 *
 * $Id: modASM_Add.cpp 2532 2014-02-20 15:23:53Z tkollar $
 * Copyright (c) 2010.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================


#include "modASM_Add.hpp"
#include "NetworkData.hpp"

namespace nASM_Add {

modASM_Add::modASM_Add(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr):
	BaseTrafficHandler(newName, newChannelName, newDLLPtr),
	_marker(CEcfg::instance()->getOrAddArray(cfgKey("markerPattern"))),
	_rebuildMarker(true), _asmCount(0) {

	if ( _marker.getLength() == 0 ) {
		uint8_t defaultPattern[] = { 0x1A, 0xCF, 0xFC, 0x1D };
		setMarker(defaultPattern, 4);
	}

	MOD_DEBUG("Initializing with a %d-octet sync marker.",
		_marker.getLength());
}

modASM_Add::~modASM_Add() {
	MOD_DEBUG("Running ~modASM_Add().");
}

void modASM_Add::setMarker(const uint8_t* asmBuf, const int bufLen) {
	for (int i = 0; i < bufLen; i++) {
		// Add slots when needed
		if ( _marker.getLength() < i + 1 ) _marker.add(Setting::TypeInt);
		_marker[i].setFormat(Setting::FormatHex);
		_marker[i] = asmBuf[i];
	}

	// Eliminate extra slots
	while (_marker.getLength() > bufLen) _marker.remove(bufLen);

	_rebuildMarker = true;
}

void modASM_Add::setMarker(const std::vector<uint8_t>& newMarker) {
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

	_rebuildMarker = true;
}

void modASM_Add::getMarker(std::vector<uint8_t>& marker) const {
	marker.clear();

	for ( int i = 0; i < _marker.getLength(); i++ )
		marker.push_back(static_cast<int>(_marker[i]) & 0xFF);
}

void modASM_Add::_updateMarker(ACE_UINT8*& buffer, size_t& bufferLen) {
	if ( ! _rebuildMarker ) return;

	if (buffer) { delete[] buffer; buffer = 0; }

	bufferLen = _marker.getLength();
	buffer = new ACE_UINT8[bufferLen];

	for (unsigned i = 0; i < bufferLen; i++) {
		buffer[i] = static_cast<int>(_marker[i]);
	}

	_rebuildMarker = false;

	MOD_DEBUG("Updated marker to %d-octet buffer.", bufferLen);
}

int modASM_Add::svc() {
	svcStart_();

	ACE_UINT8* markerBuf = 0;
	size_t markerLen = 0;
	_rebuildMarker = true;

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

		if ( data->getUnitLength() ) {

			_updateMarker(markerBuf, markerLen);

			MOD_DEBUG("Received a %d-octet %s to prepend a %d-octet ASM to.",
				data->getUnitLength(), data->typeStr().c_str(), markerLen);

			if (markerLen) {
				// Create a new unit with the ASM at the beginning.
				NetworkData* newData = new NetworkData(data->getUnitLength() + markerLen);
				if ( ! newData ) throw OutOfMemory("modASM_Add::svc(): Failed to allocate memory for NetworkData object.");
				newData->copyUnit(newData->ptrUnit(), markerBuf, markerLen);
				newData->copyUnit(newData->ptrUnit() + markerLen, data);
				ndSafeRelease(data);
				data = newData;
				_asmCount++;
			}

			if ( links_[PrimaryOutputLink] ) {
				MOD_DEBUG("Sending %d octets.", data->getUnitLength());
				links_[PrimaryOutputLink]->send(data);
			}
			else {
				MOD_NOTICE("No output target defined yet, dropping data.");
				ndSafeRelease(data);
			}
		}
		else {
			MOD_INFO("Received null data.");
		}
	}

	delete[] markerBuf;

	return svcEnd_();

}

}
