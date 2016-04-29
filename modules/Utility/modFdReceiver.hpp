/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modFdReceiver.hpp
 * @author Tad Kollar  
 *
 * $Id: modFdReceiver.hpp 2366 2013-09-24 18:45:54Z tkollar $
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _MOD_FD_RECEIVER_HPP_
#define _MOD_FD_RECEIVER_HPP_

#include "BaseTrafficHandler.hpp"
#include "devFile.hpp"

namespace nFdReceiver {

//=============================================================================
/**
* @class modFdReceiver
* @author Tad Kollar  
* @brief Read data from a file descriptor and move it to the FIFO of the next handler.
*/
//=============================================================================
class modFdReceiver: public BaseTrafficHandler {

public:
	/// Primary constructor.
	/// @param newName An identifier for the module that's unique within the channel.
	/// @param newChannelName An identifier for the containing channel.
	/// @param newDLLPtr Pointer to the DLL management structure.
    modFdReceiver(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr);

    /// Destructor.
    ~modFdReceiver();

	/// Read in data and copy it to the FIFO of the next object in the channel on our side.
	int svc();

	/// Select the pre-configured device to read frames from.
	/// @param device A EthernetDevice pointer polymorphed as a CE_Device.
	void connectDevice(CE_Device* device);

	/// Break the connection with the hardware device.
	/// @param updateDevice If true, call setReader(0) on the associated device.
	/// @param updateCfg If true, remove the deviceName setting from the config file.
	/// @throw DeviceInUse When the device cannot be disconnected.
	void disconnectDevice(const bool updateDevice = true, const bool updateCfg = false);

	/// Wrapper for getMRU().
	size_t getMaxRead() const { return getMRU(); }

	/// Wrapper for setMRU().
	/// @param newMax The largest read size in bytes; will happen most of the time for a continuous stream.
	void setMaxRead(const size_t newMax) { setMRU(newMax); }

	/// Ready-only accessor to _repeatCount.
	int getRepeatCount() const { return _repeatCount; }

	/// Ready-only accessor to _repeatMax.
	int getRepeatMax() const { return _repeatMax; }

	/// Write-only accessor to _repeatMax.
	/// @param newVal The number of times to re-read/send a file. Negative indicates infinity.
	void setRepeatMax(const int newVal) { _repeatCount = newVal; }

	/// Read-only accessor to _device.
    CE_Device* getDevice() const { return _device; }

private:
	/// A pre-initialized file "device" that we read from.
	CE_Device* _device;

	/// The number of times to read/send a regular file. A negative number indicates infinity.
	Setting& _repeatMax;

	/// The number of times that a regular file has been read/sent.
	int _repeatCount;

}; // class modFdReceiver


} // namespace nFdReceiver

#endif // _MOD_FD_RECEIVER_HPP_
