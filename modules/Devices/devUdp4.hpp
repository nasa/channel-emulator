/* -*- C++ -*- */

//=============================================================================
/**
 * @file   devUdp4.hpp
 * @author Tad Kollar  
 *
 * $Id: devUdp4.hpp 2143 2013-06-17 19:25:40Z tkollar $
 * Copyright (c) 2011.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _UDP4_DEVICE_BASE_HPP_
#define _UDP4_DEVICE_BASE_HPP_

#include "CE_Device.hpp"
#include "IPAddr.hpp"
#include "Ipv4DeviceBase.hpp"
#include "SettingsManager.hpp"
#include "common_exceptions.hpp"

namespace nasaCE {

//=============================================================================
/**
 * @class devUdp4
 * @author Tad Kollar  
*/
//=============================================================================
class devUdp4 : public Ipv4DeviceBase {
public:

	/// Primary constructor.
	devUdp4(const std::string& newName = "", const CE_DLL* newDLLPtr = 0);

	/// Destructor.
    ~devUdp4();

	/// Write-only accessor to _ipAddress. Calls bindSocket() after setting.
	/// @param newVal A string containing a numerical IPv4 or IPv6 address.
	/// @throw InvalidIPv4Address If the IP address was not in numerical 0.0.0.0 format.
	virtual void setAddress(const std::string& newVal) {
		if (newVal.empty() || checkValidAddress(_ipAddress)) _ipAddress = newVal;
		else throw InvalidIPv4Address("Improper IPv4 address given to devUdp4::setAddress");
		bindSocket();
	}

	/// Read-only accessor to _ipAddress.
	std::string getAddress() const { return _ipAddress; }

	/// Write-only accessor to _ipPort. Calls bindSocket() after setting.
	/// @param newVal The TCP port between 0 and 65535.
	/// @throw InvalidPort If the port is out of range.
	virtual void setPort(const int& newVal) {
		if ( checkValidPort(newVal) ) _ipPort = newVal;
		else throw InvalidPort("Bad port number given to devUdp4::setPort");
		bindSocket();
	}

	/// Read-only accessor to _ipPort.
	int getPort() const { return _ipPort; }

	/// Read-only accessor to _socket.
	ACE_HANDLE getSocket() const { return _socket; }

	/// The type of this device.
	std::string getType() const { return "Udp4"; }

	/// Create a new socket and bind it to _ipAddress and _ipPort. First closes
	/// existing socket if _socket is valid. Should only need to be called
	/// explicitly if the bind during construction fails or there is an update
	/// to the address or port.
	void bindSocket();

private:
	/// The IPv4 address to open a socket with.
	Setting& _ipAddress;

	/// The port to open a socket with.
	Setting& _ipPort;

	/// The socket send and receive traffic on.
	ACE_HANDLE _socket;


}; // class devUdp4

} // namespace nasaCE

#endif // _UDP4_DEVICE_BASE_HPP_
