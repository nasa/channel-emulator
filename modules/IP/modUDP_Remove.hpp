/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modUDP_Remove.hpp
 * @author Tad Kollar  
 *
 * $Id: modUDP_Remove.hpp 1482 2012-05-04 13:59:08Z tkollar $ 
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _MOD_UDP_REMOVE_HPP_
#define _MOD_UDP_REMOVE_HPP_

#include "BaseTrafficHandler.hpp"
#include "IPv4_UDP_Datagram.hpp"

namespace nUDP_Remove {

//=============================================================================
/**
	@class modUDP_Remove
	@author Tad Kollar  
	@brief Remove the IPv4 UDP datagram header/trailer from a packet and send the data on.
*/
//=============================================================================
class modUDP_Remove: public BaseTrafficHandler {
public:
	/// Primary constructor.
	/// @param newName An identifier for the module that's unique within the channel.
	/// @param newChannelName An identifier for the containing channel.
	/// @param newDLLPtr Pointer to the DLL management structure.
    modUDP_Remove(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr);

	/// Destructor.
    ~modUDP_Remove();

	/// Reads in a UDP datagram, removes the UDP information, and sends it to the output handler.
	int svc();
	
	/// Read-only accessor to _check_CRC.
	bool isCheckingCRC() const { return _check_CRC; }
	
	/// Write-only accessor to _check_CRC.
	void setCheckCRC(const bool doCheckCRC = true) { _check_CRC = doCheckCRC; }

	/// Write-only accessor to _badIPCRCCount.
	void setBadIPCRCCount(const ACE_UINT32& newVal) { _badIPCRCCount = newVal; }
	
	/// Increment _badIPCRCCount by 1.
	void incBadIPCRCCount() { _badIPCRCCount++; }
	
	/// Read-only accessor to _badIPCRCCount.
	ACE_UINT32 getBadIPCRCCount() const { return _badIPCRCCount; }

	/// Write-only accessor to _badUDPCRCCount.
	void setBadUDPCRCCount(const ACE_UINT32& newVal) { _badUDPCRCCount = newVal; }

	/// Increment _badUDPCRCCount by 1.
	void incBadUDPCRCCount() { _badUDPCRCCount++; }

	/// Read-only accessor to _badUDPCRCCount.
	ACE_UINT32 getBadUDPCRCCount() const { return _badUDPCRCCount; }
	
private:
	/// Disable default constructor.
	modUDP_Remove();

	Setting& _check_CRC; /// Whether to drop datagrams with bad IPv4 or UDP checksums.

	/// Check the datagram's CRC if enabled; always return true if not.
	/// @param dgm The UDP datagram to validate.
	bool _goodCRC(IPv4_UDP_Datagram* dgm);
	
	ACE_UINT32 _badIPCRCCount; /// The tally of incorrect IPv4 CRCs detected.
	
	ACE_UINT32 _badUDPCRCCount; /// The tally of incorrect UDP CRCs detected.
	
}; // class modUDP_Remove

} // namespace nUDP_Remove

#endif // _MOD_UDP_REMOVE_HPP_
