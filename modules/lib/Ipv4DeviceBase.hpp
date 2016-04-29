/* -*- C++ -*- */

//=============================================================================
/**
 * @file   Ipv4DeviceBase.hpp
 * @author Tad Kollar  
 *
 * $Id: Ipv4DeviceBase.hpp 2143 2013-06-17 19:25:40Z tkollar $
 * Copyright (c) 2011.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _IPV4_DEVICE_BASE_HPP_
#define _IPV4_DEVICE_BASE_HPP_

#include "CE_Device.hpp"
#include "IPAddr.hpp"
#include "SettingsManager.hpp"

namespace nasaCE {


//=============================================================================
/**
	@author Tad Kollar  
*/
//=============================================================================
class Ipv4DeviceBase : public CE_Device {
public:

	/// Primary constructor.
	Ipv4DeviceBase(const std::string& newName = "", const CE_DLL* newDLLPtr = 0);

	/// Destructor.
    virtual ~Ipv4DeviceBase();

	/// Report an error if address isn't a numerical IPv4 address.
	bool checkValidAddress(const std::string address);

	/// Report an error if port isn't between 0 and 65535.
	bool checkValidPort(const int port);

	/// Close open sockets.
	virtual void closeSocket(ACE_HANDLE sockfd);

	/// Set up a socket for two-way stream IPv4 communication.
	ACE_HANDLE createSocket(int socketType = SOCK_STREAM);

}; // class Ipv4DeviceBase

} // namespace nasaCE

#endif // _IPV4_DEVICE_BASE_HPP_
