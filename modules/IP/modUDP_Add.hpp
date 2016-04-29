/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modUDP_Add.hpp
 * @author Tad Kollar  
 *
 * $Id: modUDP_Add.hpp 1482 2012-05-04 13:59:08Z tkollar $ 
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _MOD_UDP_ADD_HPP_
#define _MOD_UDP_ADD_HPP_

#include "BaseTrafficHandler.hpp"
#include "IPv4Addr.hpp"
#include "IPv4_UDP_Datagram.hpp"

namespace nUDP_Add {

//=============================================================================
/**
	@class modUDP_Add
	@author Tad Kollar  
	@brief Add a UDP header/trailer to data received in our FIFO.
	
	@todo Add fragmentation options
*/
//=============================================================================
class modUDP_Add: public BaseTrafficHandler {
public:
	/// Primary constructor.
	/// @param newName An identifier for the module that's unique within the channel.
	/// @param newChannelName An identifier for the containing channel.
	/// @param newDLLPtr Pointer to the DLL management structure.
    modUDP_Add(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr);

	/// Destructor.
    ~modUDP_Add();

	/// Reads in data, wraps it in a UDP datagram, and sends it to the output handler.
	int svc();
	
	/// Read-only accessor to _srcIPAddr.
	const IPv4Addr getSrcIPAddr() const { std::string s = _srcIPAddrStr; return IPv4Addr(s); }
	
	/// Write-only accessor to _srcIPAddr.
	void setSrcIPAddr(const IPv4Addr& newIP) { _srcIPAddrStr = newIP.get_quad(); _updateTemplate(); }
	
	/// Write-only accessor to _srcIPAddr.
	void setSrcIPAddr(const std::string& newIP) { _srcIPAddrStr = newIP; _updateTemplate(); }	
	
	/// Read-only accessor to _srcPort.
	ACE_UINT16 getSrcPort() const { return (unsigned int) _srcPort; }
	
	/// Write-only accessor to _srcPort.
	void setSrcPort(const ACE_UINT16 newPort) { _srcPort = (int) newPort; _updateTemplate(); }
	
	/// Read-only accessor to _dstIPAddr.
	const IPv4Addr getDstIPAddr() const { std::string d = _dstIPAddrStr; return IPv4Addr(d); }
	
	/// Write-only accessor to _dstIPAddr.
	void setDstIPAddr(const IPv4Addr& newIP) { _dstIPAddrStr = newIP.get_quad(); _updateTemplate(); }
	
	/// Write-only accessor to _dstIPAddr.
	void setDstIPAddr(const std::string& newIP) { _dstIPAddrStr = newIP; _updateTemplate(); }
		
	/// Read-only accessor to _dstPort.
	ACE_UINT16 getDstPort() const { return (int) _dstPort; }
	
	/// Write-only accessor to _dstPort.
	void setDstPort(const ACE_UINT16 newPort) { _dstPort = (int) newPort; _updateTemplate(); }
	
	/// Read-only accessor to _compute_CRC.
	bool isComputingCRC() const { return _compute_CRC; }
	
	/// Write-only accessor to _compute_CRC.
	void setComputeCRC(const bool doCRC = true) { _compute_CRC = doCRC; }
	
private:
	Setting& _srcIPAddrStr; /// The address of the packet's sender.
	Setting& _srcPort; /// The port number of the packet's sender.
	
	Setting& _dstIPAddrStr; /// The address of the packet's target.
	Setting& _dstPort; /// The port number of the target.
	
	Setting& _compute_CRC; /// Whether to compute a CRC for the datagram.
	
	IPv4_UDP_Datagram* _dgmHeader; /// A preconfigured header to duplicate and make new datagrams from.
	
	void _updateTemplate(); /// Rebuild _dgmHeader.

}; // class modUDP_Add

} // namespace nUDP_Add

#endif // _MOD_UDP_ADD_HPP_
