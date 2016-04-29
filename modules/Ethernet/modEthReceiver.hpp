/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modEthReceiver.hpp
 * @author Tad Kollar  
 *
 * $Id: modEthReceiver.hpp 2514 2014-02-14 20:03:58Z tkollar $
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _NASA_MOD_ETH_RECEIVER_HPP_
#define _NASA_MOD_ETH_RECEIVER_HPP_

#include "FdInputHandler.hpp"
#include "EthernetFrame.hpp"
#include "devEthernet.hpp"
#include <ace/Event_Handler.h>

using namespace nasaCE;

namespace nEthReceiver {

//=============================================================================
/**
 * @class modEthReceiver
 * @author Tad Kollar  
 * @brief Read frames from an initialized ethernet device and send them on to the next module.
 */
//=============================================================================
class modEthReceiver : public FdInputHandler {
public:

	/// Primary constructor.
	/// @param newName An identifier for the module that's unique within the channel.
	/// @param newChannelName An identifier for the containing channel.
	/// @param newDLLPtr Pointer to the DLL management structure.
	modEthReceiver(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr);

    /// Destructor. Needs to close down PCapLive::loop() as cleanly as possible.
    virtual ~modEthReceiver();

    /// Read-only accessor to _etherDev.
    devEthernet* iface() const { return _etherDev; }

	/// Hand off the received frame to the next module.
	void process_frame(EthernetFrame* eFrame);

	/// Select the pre-configured device to read frames from.
	/// @param device A devEthernet pointer polymorphed as a CE_Device.
 	void connectDevice(CE_Device* device);

	/// Break the connection with the hardware device.
	/// @param updateDevice If true, call setReader(0) on the associated device.
	/// @param updateCfg If true, remove the deviceName setting from the config file.
	/// @throw DeviceInUse When the device cannot be disconnected.
	void disconnectDevice(const bool updateDevice = true, const bool updateCfg = false);

	/// Called by the reactor when there is input available.
	int handle_input(ACE_HANDLE fd = ACE_INVALID_HANDLE);

	/// Wrapper for setMRU().
	void setBufferSize(const size_t newVal = 0);

	/// Wrapper for getMRU().
	size_t getBufferSize() const { return getMRU(); }

	/// Read-only accessor to _etherDev.
    CE_Device* getDevice() const { return _etherDev; }

private:
	devEthernet* _etherDev; /// The pre-initialized ethernet device that we operate on.

	/// The block of memory to copy incoming data to.
	ACE_UINT8* _buffer;

}; // class modEthReceiver

} // namespace nEthReceiver

#endif // _NASA_MOD_ETH_RECEIVER_HPP_
