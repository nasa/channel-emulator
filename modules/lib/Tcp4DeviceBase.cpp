/* -*- C++ -*- */

//=============================================================================
/**
 * @file   Tcp4DeviceBase.cpp
 * @author Tad Kollar  
 *
 * $Id: Tcp4DeviceBase.cpp 2085 2013-06-13 15:06:01Z tkollar $
 * Copyright (c) 2010.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "Tcp4DeviceBase.hpp"
#include "utilityMacros.hpp"

namespace nasaCE {

Tcp4DeviceBase::Tcp4DeviceBase(const std::string& newName /* = "" */, const CE_DLL* newDLLPtr /* = 0 */):
	Ipv4DeviceBase(newName, newDLLPtr),
	_ipAddress(CEcfg::instance()->getOrAddString(cfgKey("address"))),
	_ipPort(CEcfg::instance()->getOrAddInt(cfgKey("port"), 0)),
	_connectedSocket(ACE_INVALID_HANDLE),
	_connectedPeer(0) {

	MOD_DEBUG("Running Tcp4DeviceBase constructor.");

	if ( strcmp(_ipAddress.c_str(), "") == 0) setAddress(IPv4Addr::IPv4_Any);

	_connectedPeer = new sockaddr_in();
	ACE_OS::memset(_connectedPeer, 0, sizeof(*_connectedPeer));
}

Tcp4DeviceBase::~Tcp4DeviceBase() {
	MOD_DEBUG("Running ~Tcp4DeviceBase().");
	delete _connectedPeer;
	_connectedPeer = 0;

	closeSocket(_connectedSocket);
	_connectedSocket = ACE_INVALID_HANDLE;
	MOD_DEBUG("~Tcp4DeviceBase() complete.");
}

bool Tcp4DeviceBase::isConnected() {
	if (_connectedSocket == ACE_INVALID_HANDLE) return false;

	int len = sizeof(sockaddr_in);
	if ( ACE_OS::getpeername(_connectedSocket,
		reinterpret_cast<sockaddr*>(_connectedPeer), &len) == -1 )
		return false;

	return true;
}

void Tcp4DeviceBase::closeSocket(ACE_HANDLE sockfd) {
	if ( sockfd == ACE_INVALID_HANDLE ) {
		MOD_DEBUG("Not running shutdown/close on invalid handle.");
		return;
	}

	MOD_DEBUG("Running shutdown on socket %d.", sockfd);

	if ( ACE_OS::shutdown(sockfd, SHUT_RDWR) < 0 ) {
		if ( errno == EBADF ) return;
		MOD_NOTICE("Shutdown of socket %d failed: %s.", sockfd, ACE_OS::strerror(errno));
	}

	Ipv4DeviceBase::closeSocket(sockfd);
}

}
