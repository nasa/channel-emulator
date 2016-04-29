/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modTcp4Receiver.cpp
 * @author Tad Kollar  
 *
 * $Id: modTcp4Receiver.cpp 2514 2014-02-14 20:03:58Z tkollar $
 * Copyright (c) 2010.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "modTcp4Receiver.hpp"
#include <ace/Reactor.h>

namespace nTcp4Receiver {

using namespace nasaCE;

modTcp4Receiver::modTcp4Receiver(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr):
	FdInputHandler(newName, newChannelName, newDLLPtr),
	_tcp4_dev(0), _buffer(0),
	_receiveMax(CEcfg::instance()->getOrAddBool(cfgKey("receiveMax"), false)),
	_flags(0) {

	reactor(ACE_Reactor::instance());

	if (! getBufferSize() ) setBufferSize(4096);
	_buffer = new ACE_UINT8[getBufferSize()];

	_flags = (getReceiveMax()) ? MSG_WAITALL : 0;
}

modTcp4Receiver::~modTcp4Receiver() {
	MOD_DEBUG("Running ~modTcp4Receiver().");
	disconnectDevice();
	delete _buffer;
}

void modTcp4Receiver::setBufferSize(size_t newVal) {
	if (_buffer) delete _buffer;

	_buffer = new ACE_UINT8[newVal];
	setMRU(newVal);
}

int modTcp4Receiver::handle_input(ACE_HANDLE fd /* = ACE_INVALID_HANDLE */) {
	if ( fd == ACE_INVALID_HANDLE || ! _tcp4_dev || svcDone_ ) return -1;

	// Accept a new connection
	if ( fd == _tcp4_dev->getConnectedSocket() ) {
		MOD_DEBUG("Receiving data.");

		ssize_t receivedOctets = 0;
		receivedOctets = ACE_OS::recv(_tcp4_dev->getConnectedSocket(),
			reinterpret_cast<char *>(_buffer), getBufferSize(), _flags);

		if ( receivedOctets < 0 ) {
			MOD_ERROR("Error receiving data: %s", ACE_OS::strerror(errno));
			return -1;
		}

		if ( receivedOctets > 0 ) {
			MOD_DEBUG("Received %d octets.", receivedOctets);

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
			MOD_INFO("Connection closed from %s:%d.",
				IPv4Addr(_tcp4_dev->getConnectedPeer()->sin_addr.s_addr).get_quad().c_str(),
				ntohs(_tcp4_dev->getConnectedPeer()->sin_port));

			return -1;
		}

	}
	else {
		MOD_DEBUG("Received input on unrecognized descriptor, ignoring.");
	}

	return 0;
}

int modTcp4Receiver::handle_close(ACE_HANDLE handle, ACE_Reactor_Mask close_mask) {

	if (_tcp4_dev) {
		MOD_INFO("Notifying device of close detected on socket.");
		reactor()->notify(_tcp4_dev);
	}

	return FdInputHandler::handle_close(handle, close_mask);
}


void modTcp4Receiver::connectDevice(CE_Device* device) {
	MOD_DEBUG("Connecting to IPv4 TCP Device %s.", device->getName().c_str());

	device->setReader(this);
	_tcp4_dev = dynamic_cast<Tcp4DeviceBase*>(device);

	// If the TCP device is already connected, set up the handler ourselves.
	if ( _tcp4_dev->isConnected() && svcDone_ ) {
		setHandlerBits(ACE_Event_Handler::READ_MASK);
		set_handle(_tcp4_dev->getConnectedSocket());
		this->open();
	}
}

void modTcp4Receiver::disconnectDevice(const bool updateDevice /* = true */, const bool updateCfg /* = false */) {
	stopTraffic();

	if ( ! _tcp4_dev ) {
		MOD_DEBUG("Received request to disconnect from absent device.");
		return;
	}

	MOD_DEBUG("Unregistering handler from reactor.");
	if ( ! svcDone_ ) this->close();
	clearHandlerBits(ACE_Event_Handler::READ_MASK);

	MOD_DEBUG("Disconnecting from TCP device %s.", _tcp4_dev->getName().c_str());
	if ( updateDevice ) _tcp4_dev->setReader(0);
	_tcp4_dev = 0;

	if ( updateCfg ) CEcfg::instance()->remove(cfgKey("deviceName"));
}


}
