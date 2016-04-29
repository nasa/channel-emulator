/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modTcp4Receiver.hpp
 * @author Tad Kollar  
 *
 * $Id: modTcp4Receiver.hpp 2514 2014-02-14 20:03:58Z tkollar $
 * Copyright (c) 2010.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _MOD_TCP4_RECEIVER_HPP_
#define _MOD_TCP4_RECEIVER_HPP_

#include "FdInputHandler.hpp"
#include "Tcp4DeviceBase.hpp"
#include <ace/Event_Handler.h>

namespace nTcp4Receiver {

//=============================================================================
/**
* @class modTcp4Receiver
* @author Tad Kollar  
* @brief Read data from a TCP socket and move it to the FIFO of the next handler.
*/
//=============================================================================
class modTcp4Receiver: public FdInputHandler {

public:
	/// Primary constructor.
	/// @param newName An identifier for the module that's unique within the channel.
	/// @param newChannelName An identifier for the containing channel.
	/// @param newDLLPtr Pointer to the DLL management structure.
    modTcp4Receiver(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr);

    /// Destructor.
    virtual ~modTcp4Receiver();

	/// Select the pre-configured TCP4 device to read data from.
	/// @param device A EthernetDevice pointer polymorphed as a CE_Device.
	void connectDevice(CE_Device* device);

	/// Break the relationship with the device.
	/// @param updateDevice If true, call setReader(0) on the associated device.
	/// @param updateCfg If true, remove the deviceName setting from the config file.
	/// @throw DeviceInUse When the device cannot be disconnected.
	void disconnectDevice(const bool updateDevice = true, const bool updateCfg = false);

	/// Wrapper for setMRU().
	void setBufferSize(size_t newVal );

	/// Wrapper for getMRU().
	size_t getBufferSize() const { return getMRU(); }

	/// Invoked by the reactor when there is incoming data on a socket.
	int handle_input(ACE_HANDLE fd = ACE_INVALID_HANDLE);

	/// When a connection is closed, notify the device.
	int handle_close(ACE_HANDLE handle, ACE_Reactor_Mask close_mask);

	/// Read-only accessor to _tcp4_dev.
    CE_Device* getDevice() const { return _tcp4_dev; }

	/// Write-only accessor to _receiveMax.
	void setReceiveMax(const bool newVal) {
		_receiveMax = newVal;
		_flags = (newVal) ? MSG_WAITALL : 0;
	}

	/// Read-only accessor to _receiveMax.
	bool getReceiveMax() const { return static_cast<bool>(_receiveMax); }

private:
	/// A pre-initialized TCP4 "device" that we read from.
	Tcp4DeviceBase* _tcp4_dev;

	/// The block of memory to copy incoming data to.
	ACE_UINT8* _buffer;

	/// Whether to wait for a full MRU with each recv().
	Setting& _receiveMax;

	/// The flags to use with recvfrom().
	int _flags;

}; // class modTcp4Receiver


} // namespace nTcp4Receiver

#endif // _MOD_TCP4_RECEIVER_HPP_