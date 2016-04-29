/* -*- C++ -*- */

//=============================================================================
/**
 * @file   ClientAcceptor.cpp
 * @author Tad Kollar  
 *
 * $Id: ClientAcceptor.cpp 2210 2013-06-21 20:43:20Z tkollar $
 * Copyright (c) 2010.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "ClientAcceptor.hpp"
#include "ClientService.hpp"
#include "nd_macros.hpp"

#include <ace/INET_Addr.h>
#include <ace/OS_main.h>
#include <ace/Reactor.h>
#include <ace/SOCK_Stream.h>

namespace nasaCE {

ClientAcceptor::ClientAcceptor(): connectionCount_(0) {
	reactor(ACE_Reactor::instance());
}

ClientAcceptor::~ClientAcceptor() {
	handle_close(ACE_INVALID_HANDLE, 0);
}

int ClientAcceptor::open(const ACE_INET_Addr &listenAddr) {
	if (acceptor_.open (listenAddr, 1) == -1) {
		ACE_ERROR_RETURN((LM_ERROR, "%p\n", "acceptor_.open"), -1);
	}

	return reactor()->register_handler(this, ACE_Event_Handler::ACCEPT_MASK);
}

int ClientAcceptor::handle_input(ACE_HANDLE) {
	ClientService *client;
	++connectionCount_;

	ACE_NEW_RETURN(client, ClientService(connectionCount_), -1);

	if (acceptor_.accept(client->peer()) == -1) {
		delete client;
		ACE_ERROR_RETURN ((LM_ERROR, "(%P|%t) %p\n", "Failed to accept ", "client connection"), -1);
	}

	// client->peer().enable(ACE_NONBLOCK);

	ND_INFO("[HTTP][Conn %07d] Accepted new connection.\n", connectionCount_);
	return client->open();
}

int ClientAcceptor::handle_close(ACE_HANDLE, ACE_Reactor_Mask) {
	ND_DEBUG("[HTTP] Closing the listening socket.\n");

	if (acceptor_.get_handle() != ACE_INVALID_HANDLE) {
		ACE_Reactor_Mask m = ACE_Event_Handler::ACCEPT_MASK | ACE_Event_Handler::DONT_CALL;
		reactor ()->remove_handler(this, m);
		acceptor_.close();
	}
	else {
		ND_DEBUG("[HTTP] Listening socket already closed.\n");
	}
	return 0;
}

int ClientAcceptor::pause() {
	return reactor()->suspend_handler(this);
}

int ClientAcceptor::unpause() {
	return reactor()->resume_handler(this);
}

}
