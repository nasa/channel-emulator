/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modUdp4Receiver.hpp
 * @author Tad Kollar  
 *
 * $Id: modUdp4Receiver.hpp 2509 2014-02-14 19:31:00Z tkollar $
 * Copyright (c) 2011.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _MOD_UDP4_RECEIVER_HPP_
#define _MOD_UDP4_RECEIVER_HPP_

#include "FdInputHandler.hpp"
#include "devUdp4.hpp"

namespace nUdp4Receiver {

//=============================================================================
/**
* @class modUdp4Receiver
* @author Tad Kollar  
* @brief Read data from a TCP socket and move it to the FIFO of the next handler.
*/
//=============================================================================
class modUdp4Receiver: public FdInputHandler {

public:
	/// Primary constructor.
	/// @param newName An identifier for the module that's unique within the channel.
	/// @param newChannelName An identifier for the containing channel.
	/// @param newDLLPtr Pointer to the DLL management structure.
    modUdp4Receiver(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr);

    /// Destructor.
    ~modUdp4Receiver();

	/// Select the pre-configured UDP4 device to read data from.
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

	/// Read-only accessor to _udp4_dev.
    CE_Device* getDevice() const { return _udp4_dev; }

	/// Write-only accessor to _receiveMax.
	void setReceiveMax(const bool newVal) {
		_receiveMax = newVal;
		_flags = (newVal) ? MSG_WAITALL : 0;
	}

	/// Read-only accessor to _receiveMax.
	bool getReceiveMax() const { return static_cast<bool>(_receiveMax); }

private:
	/// A pre-initialized UDP4 "device" that we read from.
	devUdp4* _udp4_dev;

	/// The block of memory to copy incoming data to.
	ACE_UINT8* _buffer;

	/// Whether to wait for a full MRU with each recv().
	Setting& _receiveMax;

	/// The flags to use with recvfrom().
	int _flags;

	/// Address information for latest received datagram.
	sockaddr_in _src_addr;

	/// Size of _src_addr.
	int _src_addr_len;

}; // class modUdp4Receiver


} // namespace nUdp4Receiver

#endif // _MOD_UDP4_RECEIVER_HPP_