/* -*- C++ -*- */

//=============================================================================
/**
 * @file   devTcp4Client.hpp
 * @author Tad Kollar  
 *
 * $Id: devTcp4Client.hpp 2131 2013-06-17 15:24:06Z tkollar $
 * Copyright (c) 2010.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _TCP4_CLIENT_DEVICE_HPP_
#define _TCP4_CLIENT_DEVICE_HPP_

#include "Tcp4DeviceBase.hpp"

namespace nasaCE {

//=============================================================================
/**
 * @class devTcp4Client
 * @brief Communicate with a remote system over an IPv4 TCP connction.
 */
//=============================================================================
class devTcp4Client: public Tcp4DeviceBase {
public:

	/// Primary constructor.
	devTcp4Client(const std::string& newName = "", const CE_DLL* newDLLPtr = 0);

	/// Destructor.
    virtual ~devTcp4Client();

	/// Write-only accessor to _ipAddress.
	/// @param newVal A stirng containing a numerical IPv4 or IPv6 address.
	/// @throw LiveSocket If _listenSocket or _connectSocket is in use.
	/// @throw InvalidIPv4Address If the IP address was not in numerical 0.0.0.0 format.
	void setAddress(const std::string& newVal) {
		if ( isConnected() )
			throw LiveSocket("Cannot change address with an open socket.");

		Tcp4DeviceBase::setAddress(newVal);
	}

	/// Write-only accessor to _ipPort.
	/// @param newVal The TCP port between 0 and 65535.
	/// @throw LiveSocket If _listenSocket or _connectSocket is in use.
	/// @throw InvalidTcpPort If the port is out of range.
	void setPort(const int& newVal) {
		if ( isConnected() )
			throw LiveSocket("Cannot change port with an open socket.");
		Tcp4DeviceBase::setPort(newVal);
	}

	/// Return whether it's permissible to open a new connection at this time.
	bool canOpenNew();

	/// Attempt to open a new connection, if one doesn't exist.
	void openConnection();

	std::string getType() const { return "Tcp4Client"; }

	/// Called when connect() has an event.
	int handle_output(ACE_HANDLE fd = ACE_INVALID_HANDLE);

	/// Handle a failed/closed connection.
	int handle_close(ACE_HANDLE fd, ACE_Reactor_Mask close_mask);

	/// When the connection fails, a timer is set and this will start a new attempt.
	int handle_timeout (const ACE_Time_Value &, const void*);

	/// Read-only accessor to _msBetweenAttempts.
	int getMsBetweenAttempts() const {
		return _msBetweenAttempts;
	}

	/// Write-only accessor to _msBetweenAttempts.
	void setMsBetweenAttempts(const int newVal) {
		_msBetweenAttempts = newVal;
	}

private:
	/// A flag set when the device is being destroyed.
	bool _shutdown;

	/// How long to wait between failed connection attempts.
	Setting& _msBetweenAttempts;

	/// If there was a failed connection attempt, this is a reference to the timer
	/// that will reattempt.
	long _timerId;

	/// Inform the reader/writer if they exist that the connection is live.
	void _setupConnection();

}; // class devTcp4Client

} // namespace nasaCE

#endif // _TCP4_CLIENT_DEVICE_HPP_
