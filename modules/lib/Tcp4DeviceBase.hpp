/* -*- C++ -*- */

//=============================================================================
/**
 * @file   Tcp4DeviceBase.hpp
 * @author Tad Kollar  
 *
 * $Id: Tcp4DeviceBase.hpp 2091 2013-06-13 17:17:33Z tkollar $
 * Copyright (c) 2010.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _TCP4_DEVICE_BASE_HPP_
#define _TCP4_DEVICE_BASE_HPP_

#include "CE_Device.hpp"
#include "IPAddr.hpp"
#include "Ipv4DeviceBase.hpp"
#include "SettingsManager.hpp"
#include "common_exceptions.hpp"

namespace nasaCE {


//=============================================================================
/**
 *  @author Tad Kollar  
*/
//=============================================================================
class Tcp4DeviceBase : public Ipv4DeviceBase {
public:
	/// Primary constructor.
	Tcp4DeviceBase(const std::string& newName = "", const CE_DLL* newDLLPtr = 0);

	/// Destructor.
    virtual ~Tcp4DeviceBase();

	/// Write-only accessor to _ipAddress.
	/// @param newVal A stirng containing a numerical IPv4 or IPv6 address.
	/// @throw LiveSocket If _listenSocket or _connectSocket is in use.
	/// @throw InvalidIPv4Address If the IP address was not in numerical 0.0.0.0 format.
	virtual void setAddress(const std::string& newVal) {
		if (newVal.empty() || checkValidAddress(_ipAddress)) _ipAddress = newVal;
		else throw InvalidIPv4Address("Improper IPv4 address given to Tcp4DeviceBase::setAddress");
	}

	/// Read-only accessor to _ipAddress.
	std::string getAddress() const { return _ipAddress;	}

	/// Write-only accessor to _ipPort.
	/// @param newVal The TCP port between 0 and 65535.
	/// @throw LiveSocket If _listenSocket or _connectSocket is in use.
	/// @throw InvalidPort If the port is out of range.
	virtual void setPort(const int& newVal) {
		if ( checkValidPort(newVal) ) _ipPort = newVal;
		else throw InvalidPort("Bad port number given to Tcp4DeviceBase::setPort");
	}

	/// Read-only accessor to _ipPort.
	int getPort() const { return _ipPort; }

	/// Determine whether the device is connected to a remote system.
	bool isConnected();

	/// Write-only accessor to _connectedSocket.
	void setConnectedSocket(const ACE_HANDLE& newVal) { _connectedSocket = newVal; }

	/// Read-only accessor to _connectedSocket.
	ACE_HANDLE getConnectedSocket() const { return _connectedSocket; }

	/// Write-only accessor to _connectedPeer.
	void setConnectedPeer(sockaddr_in* newVal) { _connectedPeer = newVal; }

	/// Read-only accessor to _connectedPeer.
	sockaddr_in* getConnectedPeer() const { return _connectedPeer; }

	/// Defined meaningfully by child class.
	virtual void openConnection() { }

	/// Defined meaningfully by child class.
	virtual ACE_HANDLE getListeningSocket() const { return ACE_INVALID_HANDLE; }

	/// Defined meaningfully by child class.
	virtual void setListeningSocket(const ACE_HANDLE&) { }

	/// Shut down, then close socket.
	void closeSocket(ACE_HANDLE sockfd);

private:
	/// If isListener is true, listen on this address; otherwise connect to it.
	Setting& _ipAddress;

	/// If isListener is true, listen on this port; otherwise connect to it.
	Setting& _ipPort;

	/// The socket connected to a remote peer.
	ACE_HANDLE _connectedSocket;

	/// Information about the remote peer.
	sockaddr_in* _connectedPeer;

}; // class Tcp4DeviceBase

} // namespace nasaCE

#endif // _TCP4_DEVICE_BASE_HPP_
