/* -*- C++ -*- */

//=============================================================================
/**
 * @file   ClientService.hpp
 * @author Tad Kollar  
 *
 * $Id: ClientService.hpp 2207 2013-06-21 20:10:25Z tkollar $
 * Copyright (c) 2010.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _NASA_CLIENT_SERVICE_HPP_
#define _NASA_CLIENT_SERVICE_HPP_

#include "HttpRequest.hpp"

#include <ace/Event_Handler.h>
#include <ace/SOCK_Stream.h>

namespace nasaCE {

//=============================================================================
/**
 * @class ClientAcceptor
 * @brief Adapted from ACE Programmer's Guide
*/
//=============================================================================
class ClientService: public ACE_Event_Handler {
public:
	/// Constructor.
	/// @param connectionNumber The numerical identifier of this connection.
	ClientService(const int connectionNumber = 0);

	/// Destructor.
	~ClientService();

	/// Accessor to sock_.
	ACE_SOCK_Stream &peer (void) { return sock_; }

	int open (void);

	/// Get this handler's I/O handle.
	ACE_HANDLE get_handle (void) const {
		return sock_.get_handle ();
	}

	/// Called when input is available from the client.
	int handle_input (ACE_HANDLE fd = ACE_INVALID_HANDLE);

	/// Called when this handler is removed from the ACE_Reactor.
	int handle_close (ACE_HANDLE handle, ACE_Reactor_Mask close_mask);

protected:
	/// The socket connected to the client.
	ACE_SOCK_Stream sock_;

	/// The current request being received from the client.
	HttpRequest req_;

	/// The FQDN, if it is possible to obtain, or IPv4 address if not.
	ACE_TCHAR peer_name_[MAXHOSTNAMELEN];

	/// The IPv4 address of the client.
	ACE_INET_Addr peer_addr_;

	/// The length of buffer_.
	const size_t buflen_;

	/// Where new requests are read into.
	uint8_t* buffer_;

	/// The numerical identifier of this connection.
	int _connectionNumber;
};

} // namespace nasaCE

#endif
