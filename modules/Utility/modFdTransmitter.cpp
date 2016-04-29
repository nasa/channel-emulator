/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modFdTransmitter.cpp
 * @author Tad Kollar  
 *
 * $Id: modFdTransmitter.cpp 2366 2013-09-24 18:45:54Z tkollar $
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "modFdTransmitter.hpp"

namespace nFdTransmitter {

using namespace nasaCE;

modFdTransmitter::modFdTransmitter(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr):
	BaseTrafficHandler(newName, newChannelName, newDLLPtr), _device(0) {
}

modFdTransmitter::~modFdTransmitter() {
	MOD_DEBUG("Shutting down modFdTransmitter.");
	disconnectDevice();
}

int modFdTransmitter::svc() {
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

		MOD_DEBUG("Received %d bytes to write to a descriptor.", data->getUnitLength());

		if ( _device) {
			int octetsToWrite = data->getUnitLength();

			if ( _device->getHandle() == ACE_INVALID_HANDLE ) {
				MOD_ERROR("Target device %s has invalid descriptor.",
					_device->getName().c_str());
				ndSafeRelease(data);
				shutdown_ = true;
				continue;
			}

			int writeCount = ACE_OS::write(_device->getHandle(), data->ptrUnit(), octetsToWrite);

			if ( writeCount < 0 ) {
				int err = errno;
				MOD_ERROR("Problem writing %d octets to descriptor (data references %d). Errno: %d (%s).",
					octetsToWrite, data->reference_count(), err, ACE_OS::strerror(err));
				continue;
			}
			else if ( writeCount != octetsToWrite) {
				MOD_ERROR("Wrote the wrong number of bytes to the descriptor: %d instead of %d.",
					writeCount, octetsToWrite);
			}
			else {
				MOD_DEBUG("Successfully wrote %d bytes.", writeCount);
			}
		}
		else {
			MOD_ERROR("No device object stored in pointer, cannot write.");
		}

		ndSafeRelease(data);
	}

	return svcEnd_();
}

void modFdTransmitter::connectDevice(CE_Device* device) {
	MOD_DEBUG("Connecting to File Device %s.", device->getName().c_str());

	device->setWriter(this);
	_device = device;
}

void modFdTransmitter::disconnectDevice(const bool updateDevice /* = true */, const bool updateCfg /* = false */) {
	stopTraffic();
	if ( ! _device ) return;
	if ( updateDevice ) _device->setWriter(0);
	_device = 0;

	if ( updateCfg ) CEcfg::instance()->remove(cfgKey("deviceName"));
}

}
