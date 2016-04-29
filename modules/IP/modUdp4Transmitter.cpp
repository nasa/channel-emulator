/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modUdp4Transmitter.cpp
 * @author Tad Kollar  
 *
 * $Id: modUdp4Transmitter.cpp 2366 2013-09-24 18:45:54Z tkollar $
 * Copyright (c) 2011.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "modUdp4Transmitter.hpp"
#include <ace/Reactor.h>

namespace nUdp4Transmitter {

using namespace nasaCE;

modUdp4Transmitter::modUdp4Transmitter(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr):
	BaseTrafficHandler(newName, newChannelName, newDLLPtr),
	_destAddress(CEcfg::instance()->getOrAddString(cfgKey("destAddress"))),
	_destPort(CEcfg::instance()->getOrAddInt(cfgKey("destPort"), -1)),
	_udp4_dev(0), _dest_addr(), _dest_addr_len(sizeof(_dest_addr)) {
	reactor(ACE_Reactor::instance());

	ACE_OS::memset(&_dest_addr, 0, _dest_addr_len);
	_dest_addr.sin_family = AF_INET;

	// TODO: Add checks on config file values
	if ( strcmp(_destAddress.c_str(), "") == 0) setDestAddress(IPv4Addr::IPv4_Any);
	else _dest_addr.sin_addr.s_addr = IPv4Addr(getDestAddress()).get();

	if ( static_cast<int>(_destPort) < 0) setDestPort(0);
	else _dest_addr.sin_port = htons(getDestPort());
}

modUdp4Transmitter::~modUdp4Transmitter() {
	MOD_DEBUG("Shutting down modUdp4Transmitter.");
	disconnectDevice();
}

int modUdp4Transmitter::svc() {
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
		ND_DEBUG("[%s] Received %d bytes to write to an IPv4 UDP device.\n",
			getName().c_str(), octetsToSend);

		if ( _udp4_dev ) {

			int sentOctets = 0;

			if ( ( sentOctets = ACE_OS::sendto(
					_udp4_dev->getSocket(),
					reinterpret_cast<const char *>(data->ptrUnit()),
					data->getUnitLength(),
					0,
					reinterpret_cast<sockaddr*>(&_dest_addr),
					_dest_addr_len
				) ) < 0 ) {

				MOD_ERROR("Error sending %d-octet buffer: %s. Dropping data.",
					data->getUnitLength(), ACE_OS::strerror(errno));

				ndSafeRelease(data);
				continue;
			}

			if ( sentOctets != octetsToSend) {
				MOD_ERROR("Sent the wrong number of octets to the UDP4 device: %d instead of %d.",
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

void modUdp4Transmitter::connectDevice(CE_Device* device) {
	MOD_DEBUG("Connecting to IPv4 UDP Device %s.", device->getName().c_str());

	device->setWriter(this);
	_udp4_dev = dynamic_cast<devUdp4*>(device);
}

void modUdp4Transmitter::disconnectDevice(const bool updateDevice /* = true */, const bool updateCfg /* = false */) {
	stopTraffic();

	if ( ! _udp4_dev ) {
		MOD_DEBUG("Received request to disconnect from absent device.");
		return;
	}

	MOD_DEBUG("Disconnecting from UDP device %s.", _udp4_dev->getName().c_str());

	if ( updateDevice ) _udp4_dev->setWriter(0);
	_udp4_dev = 0;

	if ( updateCfg ) CEcfg::instance()->remove(cfgKey("deviceName"));

}


}
