/* -*- C++ -*- */

//=============================================================================
/**
 * @file   devUdp4.cpp
 * @author Tad Kollar  
 *
 * $Id: devUdp4.cpp 1843 2013-01-28 14:53:25Z tkollar $ 
 * Copyright (c) 2011.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "devUdp4.hpp"

namespace nasaCE {

devUdp4::devUdp4(const std::string& newName /* = "" */, const CE_DLL* newDLLPtr /* = 0 */): 
	Ipv4DeviceBase(newName, newDLLPtr),
	_ipAddress(CEcfg::instance()->getOrAddString(cfgKey("address"))),
	_ipPort(CEcfg::instance()->getOrAddInt(cfgKey("port"), 0)),
	_socket(ACE_INVALID_HANDLE) {
	
	MOD_DEBUG("Running devUdp4 constructor.");
	
	// TODO: Add checks on config file values
	if ( strcmp(_ipAddress.c_str(), "") == 0) setAddress(IPv4Addr::IPv4_Any);
	bindSocket();
}

devUdp4::~devUdp4() {
	MOD_DEBUG("Running ~devUdp4().");
	
	closeSocket(_socket);
	_socket = ACE_INVALID_HANDLE;
}

void devUdp4::bindSocket() {

	if ( _socket != ACE_INVALID_HANDLE ) closeSocket(_socket);

	_socket = createSocket(SOCK_DGRAM);

	if ( getSocket() == ACE_INVALID_HANDLE ) {
		MOD_ERROR("Cannot bind to invalid handle.");
		return;
	}

	sockaddr_in ceAddr;
	ceAddr.sin_family = AF_INET;
	ceAddr.sin_addr.s_addr = IPv4Addr(getAddress()).get();
	ceAddr.sin_port = htons(getPort());
	
	MOD_DEBUG("Attempting bind with address %d, port %d.", ceAddr.sin_addr.s_addr, ceAddr.sin_port);
	
	if ( ACE_OS::bind(getSocket(), reinterpret_cast<sockaddr *>(&ceAddr), sizeof(ceAddr)) ) {
		MOD_ERROR("Could not bind socket %d: %s", getSocket(), ACE_OS::strerror(errno));
		closeSocket(getSocket());
		_socket = ACE_INVALID_HANDLE;
	}
}

}
