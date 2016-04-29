/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modUdp4Transmitter.hpp
 * @author Tad Kollar  
 *
 * $Id: modUdp4Transmitter.hpp 2366 2013-09-24 18:45:54Z tkollar $
 * Copyright (c) 2011.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _MOD_UDP4_TRANSMITTER_HPP_
#define _MOD_UDP4_TRANSMITTER_HPP_

#include "BaseTrafficHandler.hpp"
#include <ace/INET_Addr.h>
#include "devUdp4.hpp"
#include "common_exceptions.hpp"


namespace nUdp4Transmitter {

//=============================================================================
/**
* @class modUdp4Transmitter
* @author Tad Kollar  
* @brief Send data to a UDP4 device.
*/
//=============================================================================
class modUdp4Transmitter: public BaseTrafficHandler {

public:

	/// Primary constructor.
	/// @param newName An identifier for the module that's unique within the channel.
	/// @param newChannelName An identifier for the containing channel.
	/// @param newDLLPtr Pointer to the DLL management structure.
    modUdp4Transmitter(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr);

    /// Destructor.
    ~modUdp4Transmitter();

	/// Read data from the message queue and send it to the UDP4 client device.
	int svc();

	/// Select the pre-configured device to send data through.
	/// @param device A EthernetDevice pointer polymorphed as a CE_Device.
	/// @throw DeviceInUse If something else is already configured as the device reader.
	void connectDevice(CE_Device* device);

	/// Break the relationship with the device.
	/// @param updateDevice If true, call setWriter(0) on the associated device.
	/// @param updateCfg If true, remove the deviceName setting from the config file.
	/// @throw DeviceInUse When the device cannot be disconnected.
	void disconnectDevice(const bool updateDevice = true, const bool updateCfg = false);

	/// Read-only accessor to _udp4_dev.
    CE_Device* getDevice() const { return _udp4_dev; }

   	/// Report an error if address isn't a numerical IPv4 address.
	bool checkValidAddress(const std::string& address) {
		if (IPv4Addr::isProperAddress(address)) return true;
		else ND_ERROR("[%s] Invalid IP address provided (%s); must be numerical IPv4 or IPv6 format.\n",
				getName().c_str(), address.c_str());

		return false;
	}

	/// Report an error if port isn't between 0 and 65535.
	bool checkValidPort(const int& port) {
		if ( port < 0 || port > 65535 ) {
			ND_ERROR("[%s] Invalid port provided (%d); must be between 0 and 65535.\n",
				getName().c_str(), port);
			return false;
		}
		return true;
	}

	/// Write-only accessor to _destAddress.
	/// @param newVal A stirng containing a numerical IPv4 or IPv6 address.
	/// @throw InvalidIPv4Address If the IP address was not in numerical 0.0.0.0 format.
	virtual void setDestAddress(const std::string& newVal) {
		if (newVal.empty() || checkValidAddress(_destAddress)) _destAddress = newVal;
		else throw InvalidIPv4Address("Improper IPv4 address given to devUdp4::setAddress");
		_dest_addr.sin_addr.s_addr = IPv4Addr(newVal).get();
	}

	/// Read-only accessor to _destAddress.
	std::string getDestAddress() const { return _destAddress;	}

	/// Write-only accessor to _destPort.
	/// @param newVal The TCP port between 0 and 65535.
	/// @throw InvalidPort If the port is out of range.
	virtual void setDestPort(const int& newVal) {
		if ( checkValidPort(newVal) ) _destPort = newVal;
		else throw InvalidPort("Bad port number given to devUdp4::setPort");
		_dest_addr.sin_port = htons(newVal);
	}

	/// Read-only accessor to _destPort.
	int getDestPort() const { return _destPort; }

	/// Return true if svc() should keep processing.
	bool continueService() const {
		return ( BaseTrafficHandler::continueService() && _udp4_dev );
	}

private:
	/// The IPv4 address to send data to.
	Setting& _destAddress;

	/// The port to send data to.
	Setting& _destPort;

	/// The pre-initialized UDP4 "device" that we send through.
	devUdp4* _udp4_dev;

	/// Values of _destAddress and _destPort converted for use with sendto().
	sockaddr_in _dest_addr;

	/// Size of _dest_addr.
	int _dest_addr_len;

}; // class modUdp4Transmitter

} // namespace nUdp4Transmitter

#endif // _MOD_UDP4_TRANSMITTER_HPP_
