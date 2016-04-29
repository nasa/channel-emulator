/* -*- C++ -*- */

//=============================================================================
/**
 * @file   ClientAcceptor.hpp
 * @author Tad Kollar  
 *
 * $Id: ClientAcceptor.hpp 2210 2013-06-21 20:43:20Z tkollar $
 * Copyright (c) 2010.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _NASA_CLIENT_ACCEPTOR_HPP_
#define _NASA_CLIENT_ACCEPTOR_HPP_

#include <ace/SOCK_Acceptor.h>
#include <ace/Event_Handler.h>

namespace nasaCE {

//=============================================================================
/**
 * @class ClientAcceptor
 * @brief An event handler to accept and manage new TCP connections.
*/
//=============================================================================

class ClientAcceptor: public ACE_Event_Handler {
public:
	/// Constructor.
	ClientAcceptor();

	/// Destructor.
	~ClientAcceptor();

	/// Begin listening on the specified port.
	/// @param listenAddr Contains the TCP port number, IP address, and socket type (AF_INET).
	/// @return The result of reactor()->register_handler() on this object.
	int open(const ACE_INET_Addr &listenAddr);

	/// Get this handler's I/O handle.
	ACE_HANDLE get_handle(void) const { return acceptor_.get_handle(); }

	/// Called when a connection is ready to accept.
	int handle_input(ACE_HANDLE fd = ACE_INVALID_HANDLE);

	/// Called when this handler is removed from the ACE_Reactor.
	int handle_close(ACE_HANDLE handle, ACE_Reactor_Mask close_mask);

	/// Stop accepting connections until unpaused.
	/// @return The result of reactor()->suspend_handler().
	int pause();

	/// Allow new connections again.
	/// @return The result of reactor()->resume_handler().
	int unpause();

protected:
	/// The object that creates sockets for new connections.
	ACE_SOCK_Acceptor acceptor_;

	/// Tally of new connections accepted.
	int connectionCount_;
};

} // namespace nasaCE

#endif
