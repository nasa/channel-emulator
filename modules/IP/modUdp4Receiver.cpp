/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modUdp4Receiver.cpp
 * @author Tad Kollar  
 *
 * $Id: modUdp4Receiver.cpp 2509 2014-02-14 19:31:00Z tkollar $
 * Copyright (c) 2011.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "modUdp4Receiver.hpp"
#include <ace/Reactor.h>

namespace nUdp4Receiver {

using namespace nasaCE;

modUdp4Receiver::modUdp4Receiver(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr):
	FdInputHandler(newName, newChannelName, newDLLPtr),
	_udp4_dev(0), _buffer(0),
	_receiveMax(CEcfg::instance()->getOrAddBool(cfgKey("receiveMax"), false)),
	_flags(0), _src_addr(), _src_addr_len(sizeof(_src_addr)) {

	reactor(ACE_Reactor::instance());
	ACE_OS::memset(&_src_addr, 0, _src_addr_len);

	if (! getBufferSize() ) setBufferSize(4096);
	_buffer = new ACE_UINT8[getBufferSize()];

	_flags = (getReceiveMax()) ? MSG_WAITALL : 0;
}

modUdp4Receiver::~modUdp4Receiver() {
	MOD_DEBUG("Running ~modUdp4Receiver().");
	disconnectDevice();

	delete _buffer;
}

void modUdp4Receiver::setBufferSize(size_t newVal) {
	if (_buffer) delete _buffer;

	_buffer = new ACE_UINT8[newVal];
	setMRU(newVal);
}

int modUdp4Receiver::handle_input(ACE_HANDLE fd /* = ACE_INVALID_HANDLE */) {
	if ( fd == ACE_INVALID_HANDLE || ! _udp4_dev || svcDone_ ) return -1;

	// Accept a new connection
	if ( fd == _udp4_dev->getSocket() ) {
		MOD_DEBUG("Receiving data.");

		ssize_t receivedOctets = 0;
		receivedOctets = ACE_OS::recvfrom(
			_udp4_dev->getSocket(),
			reinterpret_cast<char *>(_buffer),
			getBufferSize(),
			_flags,
			reinterpret_cast<sockaddr*>(&_src_addr),
			&_src_addr_len
		);

		if ( receivedOctets < 0 ) {
			MOD_ERROR("Error receiving data: %p", "Reason");
			return -1;
		}

		if ( receivedOctets > 0 ) {
			MOD_DEBUG("Received %d octets from %s:%d.", receivedOctets,
			IPv4Addr(_src_addr.sin_addr.s_addr).get_quad().c_str(),
				ntohs(_src_addr.sin_port));

			incReceivedUnitCount(1, HandlerLink::PrimaryInput);
			incReceivedOctetCount(receivedOctets, HandlerLink::PrimaryInput);

			if ( links_[PrimaryOutputLink] ) {
				NetworkData* data = new NetworkData(receivedOctets, _buffer);
				links_[PrimaryOutputLink]->send(data);
			}
			else {
				MOD_NOTICE("No output target defined yet, dropping data.");
			}
		}
		else {
			MOD_INFO("Connection closed.");
			return -1;
		}

	}
	else {
		MOD_DEBUG("Received input on unrecognized descriptor, ignoring.");
	}

	return 0;
}

void modUdp4Receiver::connectDevice(CE_Device* device) {
	MOD_INFO("Connecting to IPv4 UDP Device %s.", device->getName().c_str());

	device->setReader(this);
	_udp4_dev = dynamic_cast<devUdp4*>(device);
	set_handle(_udp4_dev->getSocket());

	// If the TCP device is already connected, set up the handler ourselves.
	setHandlerBits(ACE_Event_Handler::READ_MASK);

	if ( svcDone_ ) this->open();
}

void modUdp4Receiver::disconnectDevice(const bool updateDevice /* = true */, const bool updateCfg /* = false */) {
	stopTraffic();

	if ( ! _udp4_dev ) {
		MOD_INFO("Received request to disconnect from absent device.");
		return;
	}

	if ( ! svcDone_ ) this->close();
	set_handle(ACE_INVALID_HANDLE);

	MOD_DEBUG("Disconnecting from UDP device %s.", _udp4_dev->getName().c_str());
	if ( updateDevice ) _udp4_dev->setReader(0);
	_udp4_dev = 0;

	if ( updateCfg ) CEcfg::instance()->remove(cfgKey("deviceName"));
}

}
