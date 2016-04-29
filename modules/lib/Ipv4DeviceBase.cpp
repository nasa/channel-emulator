/* -*- C++ -*- */

//=============================================================================
/**
 * @file   Ipv4DeviceBase.cpp
 * @author Tad Kollar  
 *
 * $Id: Ipv4DeviceBase.cpp 2542 2014-02-20 18:19:20Z tkollar $
 * Copyright (c) 2011.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "Ipv4DeviceBase.hpp"
#include "utilityMacros.hpp"

namespace nasaCE {

Ipv4DeviceBase::Ipv4DeviceBase(const std::string& newName /* = "" */, const CE_DLL* newDLLPtr /* = 0 */):
	CE_Device(newName, newDLLPtr) {

	MOD_DEBUG("Running Ipv4DeviceBase constructor.");

}

Ipv4DeviceBase::~Ipv4DeviceBase() {
	MOD_DEBUG("Running ~Ipv4DeviceBase().");
}

bool Ipv4DeviceBase::checkValidAddress(const std::string address) {
	if (IPv4Addr::isProperAddress(address)) return true;
	else MOD_ERROR("Invalid IP address provided (%s); must be numerical IPv4 or IPv6 format.\n",
			address.c_str());

	return false;
}

bool Ipv4DeviceBase::checkValidPort(const int port) {
	if ( port < 0 || port > 65535 ) {
		MOD_ERROR("Invalid port provided (%d); must be between 0 and 65535.\n", port);
		return false;
	}
	return true;
}

ACE_HANDLE Ipv4DeviceBase::createSocket(int socketType /* = SOCK_STREAM */) {
	ACE_HANDLE newSocket;

	if ( (newSocket = ACE_OS::socket(AF_INET, socketType, 0)) == ACE_INVALID_HANDLE ) {
		MOD_ERROR("Could not create a socket: %p", "Reason");
	}

	return newSocket;
}

void Ipv4DeviceBase::closeSocket(ACE_HANDLE sockfd) {
	if ( sockfd == ACE_INVALID_HANDLE ) {
		MOD_DEBUG("Not running shutdown/close on invalid handle.");
		return;
	}

	MOD_DEBUG("Running close on socket %d.", sockfd);

	if ( ACE_OS::close(sockfd) < 0 ) {
		if ( errno == EBADF ) return;
		MOD_NOTICE("Close of socket %d failed. %p", sockfd, "Reason");
	}

}

}
