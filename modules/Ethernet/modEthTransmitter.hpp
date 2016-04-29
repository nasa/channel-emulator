/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modEthTransmitter.hpp
 * @author Tad Kollar  
 *
 * $Id: modEthTransmitter.hpp 2366 2013-09-24 18:45:54Z tkollar $
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _MOD_ETH_TRANSMITTER_HPP_
#define _MOD_ETH_TRANSMITTER_HPP_

#include "BaseTrafficHandler.hpp"
#include "EthernetFrame.hpp"
#include "devEthernet.hpp"

namespace nEthTransmitter {
//=============================================================================
/**
 * @class modEthTransmitter
 * @brief Receive frames in a message queue and forward them to an Ethernet device.
 */
//=============================================================================

class modEthTransmitter: public BaseTrafficHandler {
public:
	/// @enum LinkCheckFrequency
	/// @brief List of potential occassions to test for a link carrier.
	enum LinkCheckFrequency {
		LinkCheckNever, /*!< Don't bother checking for a link prior to sending data */
		LinkCheckOnce, /*!< Check for a link only at the invocation of svc(). */
		LinkCheckAlways /*!< Check for a link prior to sending any and every unit. */
	};

	/// Primary constructor.
	/// @param newName An identifier for the module that's unique within the channel.
	/// @param newChannelName An identifier for the containing channel.
	/// @param newDLLPtr Pointer to the DLL management structure.
	modEthTransmitter(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr);

    /// Destructor.
    virtual ~modEthTransmitter();

    /// Read-only accessor to _etherDev.
    devEthernet* iface() const { return _etherDev; }

    /// Activate the object.
    int open (void* = 0);

	/// Notified where there is data in our FIFO.
	/// Data is popped from the FIFO and written out to the ethernet device.
	int svc();

	/// Select the pre-configured device to write frames to.
	/// @param device A devEthernet pointer polymorphed as a CE_Device.
 	void connectDevice(CE_Device* device);

	/// Break the connection with the hardware device.
	/// @param updateDevice If true, call setWriter(0) on the associated device.
	/// @param updateCfg If true, remove the deviceName setting from the config file.
	/// @throw DeviceInUse When the device cannot be disconnected.
	void disconnectDevice(const bool updateDevice = true, const bool updateCfg = false);

	/// Calculate the MRU from the device's MTU minus the header length.
	size_t getMRU() const { if (_etherDev) return _etherDev->getMTU() -
		EthernetFrame::calculateHeaderLength(EthernetFrame::PCap); return 0; }

	/// Get the MTU from the Ethernet device directly.
	size_t getMTU() const { if (_etherDev) return _etherDev->getMTU(); return 0;}

	/// Read-only accessor to _linkCheckFreq.
	int getLinkCheckFreq() const { return _linkCheckFreq; }

	/// Write-only accessor to _linkCheckFreq.
	void setLinkCheckFreq(const LinkCheckFrequency newVal) { _linkCheckFreq = newVal; }

	/// Return a string value to represent _linkCheckFreq.
	std::string getLinkCheckFreqStr() const { return _linkCheckFreqStr; }

	/// Receive a string value to set _linkCheckFreq.
	/// @param newVal Either "Never", "Once", or "Always". Only the first character is examined, case-insensitively.
	/// @throw BadValue If newVal starts with something other than [NnOoAa].
	void setLinkCheckFreqStr(const std::string& newVal);

	/// Read-only accessor to _linkCheckMaxIterations.
	int getLinkCheckMaxIterations() const { return _linkCheckMaxIterations; }

	/// Write-only accessor to _linkCheckMaxIterations.
	void setLinkCheckMaxIterations(const int newVal) { _linkCheckMaxIterations = newVal; }

	/// Read-only accessor to _linkCheckSleepMSec.
	int getLinkCheckSleepMSec() const { return _linkCheckSleepMSec; }

	/// Write-only accessor to _linkCheckSleepMSec.
	void setLinkCheckSleepMSec(const int newVal) { _linkCheckSleepMSec = newVal; }

	/// Read-only accessor to _etherDev.
    CE_Device* getDevice() const { return _etherDev; }

private:
	/// Pointer to the object managing the network device we write to.
	devEthernet* _etherDev;

	/// Pause sending until we find a carrier.
	/// @return True if a link was found, false if no link was found (or shutdown was detected).
	bool _waitForLink();

	/// When to check for the presence of a link.
	LinkCheckFrequency _linkCheckFreq;

	/// Descriptive value of when to check for the presence of a link.
	Setting& _linkCheckFreqStr;

	/// The number of times to check for a link before giving up. If less than 1, check indefinitely.
	Setting& _linkCheckMaxIterations;

	/// How many milliseconds to sleep after each failed link check iteration.
	Setting& _linkCheckSleepMSec;


}; // class modEthTransmitter

} // namespace nEthTransmitter

#endif // _MOD_ETH_TRANSMITTER_HPP_
