/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modTcp4Transmitter.cpp
 * @author Tad Kollar  
 *
 * $Id: modTcp4Transmitter.cpp 2366 2013-09-24 18:45:54Z tkollar $
 * Copyright (c) 2010.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "modTcp4Transmitter.hpp"
#include <ace/Reactor.h>

namespace nTcp4Transmitter {

using namespace nasaCE;

modTcp4Transmitter::modTcp4Transmitter(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr):
	BaseTrafficHandler(newName, newChannelName, newDLLPtr),
	_tcp4_dev(0) {
	reactor(ACE_Reactor::instance());
}

modTcp4Transmitter::~modTcp4Transmitter() {
	MOD_DEBUG("Shutting down modTcp4Transmitter.");
	disconnectDevice();
}

int modTcp4Transmitter::svc() {
	svcStart_();

	msg_queue()->activate();

	while ( continueService() ) {
		std::pair<NetworkData*, int> queueTop = getData_();

		if ( msg_queue()->deactivated() ) continue;

		if ( queueTop.second < 0 ) {
			MOD_ERROR("Failed to read from message queue, checking connection and trying again.");
			continue;
		}
		else if ( ! queueTop.first ) {
			MOD_WARNING("Returned from message queue with 0-length data, checking connection and trying again.");
			continue;
		}

		NetworkData* data = queueTop.first;

		ssize_t octetsToSend = data->getUnitLength();
		ND_DEBUG("[%s] Received %d bytes to write to a TCP4 device.\n",
			getName().c_str(), octetsToSend);

		if ( _tcp4_dev ) {

			int sentOctets = 0;

			if ( ( sentOctets = ACE_OS::send(_tcp4_dev->getConnectedSocket(),
				(const char *) data->ptrUnit(), data->getUnitLength(), MSG_NOSIGNAL) ) < 0 ) {
				int errCode = errno;

				if ( errCode == EPIPE || errCode == EBADF )	{
					MOD_NOTICE("Connection closed, flushing message queue.");

					// Notify the device that the connection is gone.
					reactor()->notify(_tcp4_dev);
					msg_queue()->flush();
				}
				else  {
					MOD_ERROR("Error sending %d-octet buffer: %s. Dropping data.",
						data->getUnitLength(), ACE_OS::strerror(errno));
				}

				ndSafeRelease(data);
				continue;
			}

			if ( sentOctets < 0 ) {
				MOD_ERROR("Problem sending %d octets to %s: %s.", octetsToSend,
					_tcp4_dev->getName().c_str(), ACE_OS::strerror(errno));
				continue;
			}
			else if ( sentOctets != octetsToSend) {
				MOD_ERROR("Sent the wrong number of octets to the TCP4 device: %d instead of %d.",
					sentOctets, octetsToSend);
			}
			else {
				MOD_DEBUG("Successfully sent %d octets.", sentOctets);
			}
		}
		else {
			MOD_ERROR("No device object stored in pointer, cannot write.");
		}

		ndSafeRelease(data);
	}

	return svcEnd_();
}

void modTcp4Transmitter::connectDevice(CE_Device* device) {
	MOD_DEBUG("Connecting to IPv4 TCP Device %s.", device->getName().c_str());

	device->setWriter(this);
	_tcp4_dev = dynamic_cast<Tcp4DeviceBase*>(device);
}

void modTcp4Transmitter::disconnectDevice(const bool updateDevice /* = true */, const bool updateCfg /* = false */) {
	stopTraffic();

	if ( ! _tcp4_dev ) {
		MOD_DEBUG("Received request to disconnect from absent device.");
		return;
	}

	MOD_DEBUG("Disconnecting from TCP device %s.", _tcp4_dev->getName().c_str());

	if ( updateDevice ) _tcp4_dev->setWriter(0);
	_tcp4_dev = 0;

	if ( updateCfg ) CEcfg::instance()->remove(cfgKey("deviceName"));
}


}
