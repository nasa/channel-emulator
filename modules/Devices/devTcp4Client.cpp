/* -*- C++ -*- */

//=============================================================================
/**
 * @file   devTcp4Client.cpp
 * @author Tad Kollar  
 *
 * $Id: devTcp4Client.cpp 2514 2014-02-14 20:03:58Z tkollar $
 * Copyright (c) 2010.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "devTcp4Client.hpp"
#include <ace/Thread_Manager.h>
#include <ace/Reactor.h>

namespace nasaCE {

devTcp4Client::devTcp4Client(const std::string& newName /* = "" */, const CE_DLL* newDLLPtr /* = 0 */):
	Tcp4DeviceBase(newName, newDLLPtr), _shutdown(false),
	_msBetweenAttempts(CEcfg::instance()->getOrAddInt(cfgKey("msBetweenAttempts"), 2500)),
	_timerId(-1) {
	ND_DEBUG("[%s] Creating devTcp4Client.\n", newName.c_str());
	CEcfg::instance()->getOrAddString(cfgKey("devType")) = "Tcp4Client";

	reactor(ACE_Reactor::instance());
	if ( static_cast<int>(_msBetweenAttempts) < 0 ) {
		MOD_WARNING("Negative value (%d) specified for _msBetweenAttempts, setting to 2500.",
			static_cast<int>(_msBetweenAttempts));
		_msBetweenAttempts = 2500;
	}

	if ( getPort() > 0) openConnection();
}

devTcp4Client::~devTcp4Client() {
	MOD_DEBUG("Running ~devTcp4Client().");
	_shutdown = true;

	if ( _timerId >= 0 ) {
		MOD_INFO("Cancelled connection-attempting timer with id %d.", _timerId);
		reactor()->cancel_timer(_timerId);
		_timerId = -1;
	}

	MOD_DEBUG("Removing connection handler.");
	reactor()->remove_handler(getConnectedSocket(), ACE_Event_Handler::CONNECT_MASK);

	// Close here, first even though it will be called by the base classes.
	closeSocket(getConnectedSocket());
	setConnectedSocket(ACE_INVALID_HANDLE);

	MOD_DEBUG("~devTcp4Client() complete.");
}

bool devTcp4Client::canOpenNew() {
	if ( getPort() < 1) {
		MOD_ERROR("Cannot open connection with IPv4 port setting of %d.", getPort());
		return false;
	}

	if ( exit_requested || _shutdown ) {
		MOD_WARNING("Cannot attempt new connection during shutdown.");
		return false;
	}

	if ( isConnected() ) {
		MOD_WARNING("Cannot re-open an existing connection.");
		return false;
	}

	if ( getConnectedSocket() != ACE_INVALID_HANDLE ) {
		MOD_WARNING("Already attempting to open connection.");
		return false;
	}

	return true;
}

void devTcp4Client::openConnection() {
	MOD_DEBUG("Initiating new connection to TCP server.");

	if ( ! canOpenNew() ) {
		MOD_WARNING("Not possible to connect to TCP server.");
		return;
	};

	ACE_HANDLE newSocket;
	if ( (newSocket = createSocket()) != ACE_INVALID_HANDLE ) setConnectedSocket(newSocket);
	else {
		MOD_WARNING("Failed to create new socket: %p.", "Reason");
		return;
	}

	getConnectedPeer()->sin_family = AF_INET;
	getConnectedPeer()->sin_addr.s_addr = IPv4Addr(getAddress()).get();
	getConnectedPeer()->sin_port = htons(getPort());

	ACE_OS::fcntl(newSocket, F_SETFL, O_NONBLOCK);

	if ( ACE_OS::connect(newSocket, reinterpret_cast<sockaddr *>(getConnectedPeer()), sizeof(sockaddr_in)) == -1 ) {
		if ( errno != EINPROGRESS ) {
			MOD_WARNING("Connection to server %s:%d failed. %p.",
				IPv4Addr(getConnectedPeer()->sin_addr.s_addr).get_quad().c_str(),
				ntohs(getConnectedPeer()->sin_port), "Reason");
			Ipv4DeviceBase::closeSocket(getConnectedSocket());
			setConnectedSocket(ACE_INVALID_HANDLE);
		}
		else {
			setHandlerBits(ACE_Event_Handler::CONNECT_MASK);
			reactor()->register_handler(newSocket, this, ACE_Event_Handler::CONNECT_MASK);
		}
	}
	else { _setupConnection(); }
}

void devTcp4Client::_setupConnection() {
	MOD_INFO("Connected to server %s:%d.",
		IPv4Addr(getConnectedPeer()->sin_addr.s_addr).get_quad().c_str(),
		ntohs(getConnectedPeer()->sin_port));

	// Send input events to the reader.
	if ( getReader() ) {
		getReader()->setHandlerBits(ACE_Event_Handler::READ_MASK);
		getReader()->set_handle(getConnectedSocket());
		getReader()->open();
	}

	// Allow the writer to begin sending output.
	if ( getWriter() ) getWriter()->open();
}

int devTcp4Client::handle_output(ACE_HANDLE fd /* = ACE_INVALID_HANDLE */) {
	MOD_DEBUG("Running handle_output(%d).", fd);

	if ( _shutdown || exit_requested || fd == ACE_INVALID_HANDLE ) return -1;

	if ( fd == getConnectedSocket() ) {
		// Make sure the connection is OK.
		char testBuf[2] = "";
		ssize_t ret = ACE_OS::send(getConnectedSocket(), testBuf, MSG_DONTWAIT);

		if ( ret > -1 || ret == EWOULDBLOCK ) { _setupConnection(); }
		else {
			MOD_WARNING("Connection to server %s:%d failed. %p.",
				IPv4Addr(getConnectedPeer()->sin_addr.s_addr).get_quad().c_str(),
				ntohs(getConnectedPeer()->sin_port), "Reason");
			Ipv4DeviceBase::closeSocket(getConnectedSocket());
			setConnectedSocket(ACE_INVALID_HANDLE);
			ACE_Time_Value msToNextTry(0, static_cast<int>(_msBetweenAttempts) * 1000);
			setHandlerBits(ACE_Event_Handler::TIMER_MASK);
			_timerId = reactor()->schedule_timer(this, 0, msToNextTry);
		}
	}
	else {
		MOD_WARNING("Received output event for unmanaged socket %d.", fd);
	}

	return -1;
}

// Reader/writer removes handler for us on CONNECT_MASK to call this
int devTcp4Client::handle_close(ACE_HANDLE fd, ACE_Reactor_Mask close_mask) {
	MOD_DEBUG("Running handle_close(%d, %X).", fd, close_mask);

	if ( fd == getConnectedSocket() ) {

		if ( close_mask == ACE_Event_Handler::READ_MASK ) {
			MOD_WARNING("Server at %s:%d closed connection on socket %d.\n",
						IPv4Addr(getConnectedPeer()->sin_addr.s_addr).get_quad().c_str(),
						ntohs(getConnectedPeer()->sin_port),
						getConnectedSocket());
			ACE_Reactor_Mask m = ACE_Event_Handler::READ_MASK | ACE_Event_Handler::DONT_CALL;

			if ( getReader() ) reactor()->remove_handler(getReader(), m);
			if ( getWriter() ) getWriter()->stopTraffic(false);

			setConnectedSocket(ACE_INVALID_HANDLE);

			if ( ! ( _shutdown || exit_requested) ) {
				ACE_Time_Value msToNextTry(0, static_cast<int>(_msBetweenAttempts) * 1000);
				setHandlerBits(ACE_Event_Handler::TIMER_MASK);
				_timerId = reactor()->schedule_timer(this, 0, msToNextTry);
			}
		}
	}
	else {
		MOD_DEBUG("Received close event for unmanaged socket %d.", fd);
	}

	return BaseTask::handle_close(fd, close_mask);
}

int devTcp4Client::handle_timeout(const ACE_Time_Value &, const void*) {
	_timerId = -1;
	MOD_DEBUG("Re-attempting connection after previous failure.");
	openConnection();
	return -1;
}

}
