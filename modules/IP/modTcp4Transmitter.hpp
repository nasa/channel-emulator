/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modTcp4Transmitter.hpp
 * @author Tad Kollar  
 *
 * $Id: modTcp4Transmitter.hpp 2366 2013-09-24 18:45:54Z tkollar $
 * Copyright (c) 2010.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _MOD_TCP4_TRANSMITTER_HPP_
#define _MOD_TCP4_TRANSMITTER_HPP_

#include "BaseTrafficHandler.hpp"
#include "Tcp4DeviceBase.hpp"
#include <ace/Event_Handler.h>

namespace nTcp4Transmitter {

//=============================================================================
/**
* @class modTcp4Transmitter
* @author Tad Kollar  
* @brief Send data to a TCP4 client device.
*/
//=============================================================================
class modTcp4Transmitter: public BaseTrafficHandler {

public:
	/// Primary constructor.
	/// @param newName An identifier for the module that's unique within the channel.
	/// @param newChannelName An identifier for the containing channel.
	/// @param newDLLPtr Pointer to the DLL management structure.
    modTcp4Transmitter(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr);

    /// Destructor.
    ~modTcp4Transmitter();

	/// Read data from the message queue and send it to the TCP4 client device.
	int svc();

	/// Select the pre-configured device to send data through.
	/// @param device A EthernetDevice pointer polymorphed as a CE_Device.
	void connectDevice(CE_Device* device);

	/// Break the relationship with the device.
	/// @param updateDevice If true, call setWriter(0) on the associated device.
	/// @param updateCfg If true, remove the deviceName setting from the config file.
	/// @throw DeviceInUse When the device cannot be disconnected.
	void disconnectDevice(const bool updateDevice = true, const bool updateCfg = false);

	/// Read-only accessor to _tcp4_dev.
    CE_Device* getDevice() const { return _tcp4_dev; }

	/// Return true if svc() should keep processing.
	bool continueService() const {
		return ( BaseTrafficHandler::continueService() && _tcp4_dev && _tcp4_dev->isConnected() );
	}

private:

	/// The pre-initialized TCP4 "device" that we send through.
	Tcp4DeviceBase* _tcp4_dev;

}; // class modTcp4Transmitter


} // namespace nTcp4Transmitter

#endif // _MOD_TCP4_TRANSMITTER_HPP_
