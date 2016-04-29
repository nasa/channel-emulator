/* -*- C++ -*- */

//=============================================================================
/**
 * @file   ClientService.cpp
 * @author Tad Kollar  
 *
 * $Id: ClientService.cpp 2237 2013-06-27 15:21:53Z tkollar $
 * Copyright (c) 2010.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "ClientService.hpp"
#include "HttpResponse.hpp"
#include "HttpManager.hpp"
#include "Rfc1123Date.hpp"
#include "SettingsManager.hpp"
#include "string_nocase.hpp"
#include "XML_RPC_Server.hpp"

#include <ace/INET_Addr.h>
#include <ace/OS_main.h>
#include <ace/SOCK_Acceptor.h>
#include <ace/Reactor.h>
#include <ace/SOCK_Stream.h>

#include <xmlrpc-c/base.hpp>
#include <xmlrpc-c/xml.hpp>
#include <xmlrpc-c/girerr.hpp>
#include <xmlrpc-c/timeout.hpp>
#include <xmlrpc-c/registry.hpp>

using namespace std;

namespace nasaCE {


ClientService::ClientService(const int connectionNumber /* = 0 */): ACE_Event_Handler(),
	req_(), buflen_(32768), buffer_(0), _connectionNumber(connectionNumber) {
	buffer_ = new ACE_UINT8[buflen_];
	reactor(ACE_Reactor::instance());
}

ClientService::~ClientService() {
	req_.clear();
	delete[] buffer_;
}

int ClientService::open(void) {
	if (sock_.get_remote_addr(peer_addr_) == 0 &&
		peer_addr_.addr_to_string(peer_name_, MAXHOSTNAMELEN) == 0)
		ND_INFO("[HTTP][Conn %07d] New client is at %s.\n", _connectionNumber, peer_name_);

	return reactor()->register_handler(this, ACE_Event_Handler::READ_MASK);
}

int ClientService::handle_input(ACE_HANDLE fd /* = ACE_INVALID_HANDLE */) {
	if ( fd != get_handle() ) {
		ND_DEBUG("[HTTP][Conn %07d] Received input for unmanaged handle.\n");
		return 0;
	}

	ssize_t recv_cnt;

	// ACE_Time_Value zeroTime(ACE_Time_Value::zero);
/*
	while ( (recv_cnt = sock_.recv(buffer_, buflen_)) > 0 ) {
		ND_DEBUG("[HTTP][Conn %07d] Received %d octets of request data.\n", _connectionNumber, recv_cnt);
		req_.addData(buffer_, recv_cnt);
	}

	int closeErr = errno;
	if (recv_cnt < 0 && closeErr != EWOULDBLOCK) {
		ND_DEBUG("[HTTP][Conn %07d] Connection closed to %s. %p.\n", _connectionNumber, peer_name_, "Reason");
		req_.clear();
		return -1;
	}
*/
	if ( (recv_cnt = sock_.recv(buffer_, buflen_)) < 1 ) {
		ND_DEBUG("[HTTP][Conn %07d] Connection closed to %s. %p.\n", _connectionNumber, peer_name_, "Reason");
		req_.clear();
		return -1;
	}

	ND_DEBUG("[HTTP][Conn %07d] Received %d octets of request data.\n", _connectionNumber, recv_cnt);
	req_.addData(buffer_, recv_cnt);

	// std::string reqStr(req_.getAsString());

	try {
		req_.parse();
	}
	catch (const nd_error& e) {
		ND_CRITICAL("[HTTP][Conn %07d] %s\n", _connectionNumber, e.what());
		req_.clear();
		return 0;
	}

	// ND_DEBUG("[HTTP][Conn %07d] Received %d-octet request:\n%s\n", _connectionNumber, req_.getRequestLength(), reqStr.c_str());

	if (req_.isComplete()) {
		HttpResponse response;

		httpManager::instance()->handleRequest(req_, response);

		string responseStr = response.makeClientStr((req_.getMethod() != HttpRequest::MethodHEAD));
		ND_DEBUG("[HTTP][Conn %07d] Sending response (%d %s, %s, %d octet contents).\n", _connectionNumber,
			response.getStatusCode(),
			response.getStatusStr().c_str(),
			response.getContentType().c_str(),
			response.getContent().length());

		sock_.send(responseStr.c_str(), responseStr.length());

		double ver = req_.getProtocolVersion();
		req_.clear();

		if ( ver == 1.0 ) {
			ND_DEBUG("[HTTP][Conn %07d] Closing HTTP 1.0 connection.\n", _connectionNumber);
			return -1;
		}
	}
	else {
		if ( req_.hasContent() )
			ND_DEBUG("[HTTP][Conn %07d] Received %d octets of %d expected, waiting for more.\n", _connectionNumber,
				req_.getContentLength(), req_.getReportedContentLength());
	}

	return 0;
}

int ClientService::handle_close(ACE_HANDLE, ACE_Reactor_Mask mask) {
	ND_INFO("[HTTP][Conn %07d] Closing connection to %s.\n", _connectionNumber, peer_name_);
	if (mask == ACE_Event_Handler::WRITE_MASK) return 0;
	mask = ACE_Event_Handler::ALL_EVENTS_MASK |	ACE_Event_Handler::DONT_CALL;
	reactor ()->remove_handler (this, mask);

	if ( ACE_OS::shutdown(sock_.get_handle(), SHUT_RDWR) < 0 ) {
		int errCode = errno;

		if ( errCode != EBADF )
			ND_NOTICE("[HTTP][Conn %07d] Shutdown of client socket %d failed: %p.\n", _connectionNumber,
				sock_.get_handle(), "Reason");
	}

	sock_.close();
	ND_DEBUG("[HTTP][Conn %07d] Client connection successfully closed.\n", _connectionNumber);
	delete this;
	return 0;
}

}
