/* -*- C++ -*- */

//=============================================================================
/**
 * @file   devTcp4Server.cpp
 * @author Tad Kollar  
 *
 * $Id: devTcp4Server.cpp 2542 2014-02-20 18:19:20Z tkollar $
 * Copyright (c) 2010.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "devTcp4Server.hpp"
#include <ace/INET_Addr.h>
#include <ace/Reactor.h>

namespace nasaCE {

devTcp4Server::devTcp4Server(const std::string& newName /* = "" */, const CE_DLL* newDLLPtr /* = 0 */):
	Tcp4DeviceBase(newName, newDLLPtr),
	_listeningSocket(ACE_INVALID_HANDLE) {

	ND_DEBUG("[%s] Creating devTcp4Server.\n", newName.c_str());
	CEcfg::instance()->getOrAddString(cfgKey("devType")) = "Tcp4Server";

	// TODO: Add checks for config file values

	reactor(ACE_Reactor::instance());

	if ( ! startListening() )
		MOD_INFO("Not enough information (or failure) to listen.");
}

devTcp4Server::~devTcp4Server() {
	MOD_DEBUG("Running ~devTcp4Server().");

	ACE_Reactor_Mask m = ACE_Event_Handler::READ_MASK | ACE_Event_Handler::DONT_CALL;
	reactor()->remove_handler(getListeningSocket(), m);
	closeSocket(getListeningSocket());
	setListeningSocket(ACE_INVALID_HANDLE);
}

bool devTcp4Server::startListening() {
	if ( getPort() == 0 ) return false;

	if ( isServing() ) throw LiveSocket("Socket is already listening.");

	ACE_HANDLE newSocket;
	if ( (newSocket = createSocket()) == ACE_INVALID_HANDLE ) return false;

	int optVal = 1;
	if ( (setsockopt(newSocket, SOL_SOCKET, SO_REUSEADDR, &optVal, sizeof(int)) == -1 ) ) {
		MOD_WARNING("Failed to set SO_REUSEADDR on socket %d. %p", newSocket, "Reason");
	}

	setListeningSocket(newSocket);

	sockaddr_in ceAddr;
	ceAddr.sin_addr.s_addr = IPv4Addr(getAddress()).get();
	ceAddr.sin_port = htons(getPort());
	ceAddr.sin_family = AF_INET;

	MOD_DEBUG("Attempting bind with address %d, port %d.", ceAddr.sin_addr.s_addr, ceAddr.sin_port);

	if ( ACE_OS::bind(getListeningSocket(), reinterpret_cast<sockaddr *>(&ceAddr), sizeof(ceAddr)) ) {
		MOD_ERROR("Could not bind socket %d: %s", getListeningSocket(), ACE_OS::strerror(errno));
		closeSocket(getListeningSocket());
		setListeningSocket(ACE_INVALID_HANDLE);
		return false;
	}

	if ( ACE_OS::listen(getListeningSocket(), 5) ) {
		MOD_ERROR("Could not listen on socket %d: %s", _listeningSocket, ACE_OS::strerror(errno));
		closeSocket(_listeningSocket);
		setListeningSocket(ACE_INVALID_HANDLE);
		return false;
	}

	reactor()->register_handler(_listeningSocket, this, ACE_Event_Handler::ACCEPT_MASK);

	MOD_DEBUG("Now listening for TCP4 connections on %s:%d.",
		getAddress().c_str(), getPort());

	return true;
}

int devTcp4Server::handle_input(ACE_HANDLE handle /* = ACE_INVALID_HANDLE */) {
	if ( handle == ACE_INVALID_HANDLE ) return 0;

	// Accept a new connection
	if ( handle == _listeningSocket ) {
		int len = sizeof(sockaddr_in);
		int connFd = ACE_OS::accept(_listeningSocket, reinterpret_cast<sockaddr *>(getConnectedPeer()), &len);

		if ( connFd == ACE_INVALID_HANDLE ) {
			MOD_ERROR("Accept on socket %d failed: %s.", _listeningSocket, strerror(errno));
			return -1;
		}

		reactor()->suspend_handler(_listeningSocket);

		MOD_INFO("Accepted new connection from %s:%d.",
			IPv4Addr(getConnectedPeer()->sin_addr.s_addr).get_quad().c_str(),
			ntohs(getConnectedPeer()->sin_port));

		setConnectedSocket(connFd);

		// Send input events to the reader.
		if ( getReader() ) {
			getReader()->set_handle(getConnectedSocket());
			getReader()->open();
		}

		// Allow the writer to begin sending output.
		if ( getWriter() ) {
			// getWriter()->set_handle(getConnectedSocket());
			getWriter()->open();
		}
	}

	else {
		MOD_DEBUG("Received input on unrecognized descriptor, ignoring.");
	}

	return 0;
}

// Reader/writer removes handler for us on CONNECT_MASK to call this
int devTcp4Server::handle_exception(ACE_HANDLE) {

	MOD_DEBUG("Connected socket closed.");

	ACE_Reactor_Mask m = ACE_Event_Handler::READ_MASK | ACE_Event_Handler::DONT_CALL;
	setConnectedSocket(ACE_INVALID_HANDLE);

	if ( getReader() ) reactor()->remove_handler(getReader(), m);
	if ( getWriter() ) getWriter()->stopTraffic(false);

	reactor()->resume_handler(_listeningSocket);

	return 0;
}

}
