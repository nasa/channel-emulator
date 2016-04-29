/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modFdTransmitter.hpp
 * @author Tad Kollar  
 *
 * $Id: modFdTransmitter.hpp 2366 2013-09-24 18:45:54Z tkollar $
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _MOD_FD_TRANSMITTER_HPP_
#define _MOD_FD_TRANSMITTER_HPP_

#include "BaseTrafficHandler.hpp"
#include "devFile.hpp"

namespace nFdTransmitter {

//=============================================================================
/**
* @class modFdTransmitter
* @author Tad Kollar  
* @brief Read data from a file descriptor and move it to the FIFO of the next handler.
*/
//=============================================================================
class modFdTransmitter: public BaseTrafficHandler {

public:
	/// Primary constructor.
	/// @param newName An identifier for the module that's unique within the channel.
	/// @param newChannelName An identifier for the containing channel.
	/// @param newDLLPtr Pointer to the DLL management structure.
    modFdTransmitter(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr);

    /// Destructor.
    ~modFdTransmitter();

	/// Get data from our FIFO and write it to the device that we connect to.
	int svc();

	/// Select the pre-configured device to write frames to.
	/// @param device A EthernetDevice pointer polymorphed as a CE_Device.
 	void connectDevice(CE_Device* device);

	/// Break the connection with the hardware device.
	/// @param updateDevice If true, call setWriter(0) on the associated device.
	/// @param updateCfg If true, remove the deviceName setting from the config file.
	/// @throw DeviceInUse When the device cannot be disconnected.
	void disconnectDevice(const bool updateDevice = true, const bool updateCfg = false);

	/// Read-only accessor to _device.
    CE_Device* getDevice() const { return _device; }

private:
	/// A pre-initialized file "device" that we write to.
	CE_Device* _device;

}; // class modFdTransmitter

} // namespace nFdTransmitter

#endif // _MOD_FD_TRANSMITTER_HPP_
