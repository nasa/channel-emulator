/* -*- C++ -*- */

//=============================================================================
/**
 * @file   devTcp4Server.hpp
 * @author Tad Kollar  
 *
 * $Id: devTcp4Server.hpp 1670 2012-08-28 13:25:28Z tkollar $ 
 * Copyright (c) 2010.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _TCP4_SERVER_DEVICE_HPP_
#define _TCP4_SERVER_DEVICE_HPP_

#include "Tcp4DeviceBase.hpp"

namespace nasaCE {

//=============================================================================
/**
 * @class devTcp4Server
 * @brief Communicate with a remote system over an IPv4 TCP connction.
 */
//=============================================================================
class devTcp4Server: public Tcp4DeviceBase {
public:

	/// Primary constructor.
	devTcp4Server(const std::string& newName = "", const CE_DLL* newDLLPtr = 0);

	/// Destructor.
    ~devTcp4Server();
	
	/// Write-only accessor to _ipAddress.
	/// @param newVal A stirng containing a numerical IPv4 or IPv6 address.
	/// @throw LiveSocket If _listenSocket or _connectSocket is in use.
	/// @throw InvalidIPv4Address If the IP address was not in numerical 0.0.0.0 format.
	void setAddress(const std::string& newVal) {
		if ( isServing() || isConnected() )
			throw LiveSocket("Cannot change address with an open socket.");
			
		Tcp4DeviceBase::setAddress(newVal);
	}

	/// Write-only accessor to _ipPort.
	/// @param newVal The TCP port between 0 and 65535.
	/// @throw LiveSocket If _listenSocket or _connectSocket is in use.
	/// @throw InvalidPort If the port is out of range.
	void setPort(const int& newVal) {
		if ( isServing() || isConnected() )
			throw LiveSocket("Cannot change port with an open socket.");
		Tcp4DeviceBase::setPort(newVal);
	}

	
	/// Open a socket, bind to it, and begin listening for client connections.
	/// @return True if configured as a listener and all calls were successful, false otherwise.
	/// @throw LiveSocket If _listenSocket is already in use.
	bool startListening();
	
	/// Determine whether the device is acting as a server or not.
	bool isServing() { return (_listeningSocket != ACE_INVALID_HANDLE); }

	/// Write-only accessor to _listeningSocket.
	void setListeningSocket(const ACE_HANDLE& newVal) { _listeningSocket = newVal; }
	
	/// Read-only accessor to _listeningSocket.
	ACE_HANDLE getListeningSocket() const { return _listeningSocket; }
	
	/// Accept a new connection.
	int handle_input(ACE_HANDLE handle = ACE_INVALID_HANDLE);
	
	/// When a connection is closed, start listening again.
	int handle_exception(ACE_HANDLE);
	
	std::string getType() const { return "Tcp4Server"; }

private:
	/// If this device is configured as a listener, this socket will accept new connections.
	/// Unused if not a listerner.
	ACE_HANDLE _listeningSocket;

}; // class devTcp4Server

} // namespace nasaCE

#endif // _TCP4_SERVER_DEVICE_HPP_
