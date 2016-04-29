/* -*- C++ -*- */

//=============================================================================
/**
 * @file   ArpPacket.hpp
 * @author Tad Kollar  
 *
 * $Id: ArpPacket.hpp 1670 2012-08-28 13:25:28Z tkollar $ 
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef NASAARPPACKET_HPP
#define NASAARPPACKET_HPP

#include "EthernetFrame.hpp"
#include "IPAddr.hpp"
#include "MacAddress.hpp"
#include "nd_error.hpp"

#include <map>
#include <string>

extern "C" {
	#include <linux/if_ether.h>
}

namespace nasaCE {

// ping from 10.250.1.2 (0A FA 01 02)/00:30:48:57:6C:27 
//  to       10.250.1.3 (0A FA 01 03)/00:30:48:57:6C:3F

// ARP Conversation (ethernet header removed)
// 0                          1                             2                             3                             4                   
//  1  2  3  4  5  6  7  8  9  0  1  2  3  4  5  6  7  8  9  0  1  2  3  4  5  6  7  8  9  0  1  2  3  4  5  6  7  8  9  0  1  2  3  4  5  6 
// 00 01 08 00 06 04 00 01 00 30 48 57 6C 27 0A FA 01 02 00 00 00 00 00 00 0A FA 01 03 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
// 00 01 08 00 06 04 00 01 00 30 48 57 6C 27 0A FA 01 02 00 00 00 00 00 00 0A FA 01 03 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
// 00 01 08 00 06 04 00 02 00 30 48 57 6C 3F 0A FA 01 03 00 30 48 57 6C 27 0A FA 01 02 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
// 00 01 08 00 06 04 00 01 00 30 48 57 6C 27 0A FA 01 02 00 00 00 00 00 00 0A FA 01 03 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
// 00 01 08 00 06 04 00 02 00 30 48 57 6C 3F 0A FA 01 03 00 30 48 57 6C 27 0A FA 01 02 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00

/*
    +--------+ 
    |Hardware| 2 bytes
    |MAC     |
    |Address |
    |Type    |
    +--------+
    |Protocol| 2 bytes
    |Address |
    |Type    |
    +--------+
    |Hardware| 1 byte
    |MAC     |
    |Address |
    |Size    |
    +--------+
    |Protocol| 1 byte
    |Address |
    |Size    |
    +--------+
    |Op      | 2 bytes
    +--------+ 
    |Sender  | 6 bytes (depends on the above size field)
    |MAC     |
    |Address |
    +--------+
    |Sender  | 4 bytes (depends on the above size field)
    |IP      |
    |Address |
    +--------+
    |Target  | 6 bytes (depends on the above size field)
    |MAC     |
    |Address |
    +--------+
    |Target  | 4 bytes (depends on the above size field)
    |IP      |
    |Address |
    +--------+
*/

class ArpPacket: public NetworkData {
private:
	ACE_UINT16 _expectedHwAddrType;
	ACE_UINT16 _expectedProtAddrType;
	ACE_UINT16 _expectedHwAddrSize;
	ACE_UINT16 _expectedProtAddrSize;

public:
	enum Operation { Request = 1, Reply = 2};
	enum { DefaultSize = 46 };
	enum HardwareTypes { EthernetHardware = 1 };
	enum ProtocolTypes { IPv4Protocol = 0x0800 };
	enum HardwareAddrLengths { EthernetLength = 6 };
	enum ProtocolAddrLength { IPv4Length = 4 };
	
	/// Default constructor.
	ArpPacket(ACE_UINT16 hwType = EthernetHardware, ACE_UINT16 protType = IPv4Protocol,
		ACE_UINT16 hwLen = EthernetLength, ACE_UINT16 protLen = IPv4Length): NetworkData(),
		_expectedHwAddrType(hwType), _expectedProtAddrType(protType), 
		_expectedHwAddrSize(hwLen),  _expectedProtAddrSize(protLen) { }
	
	/// Copy constructor.
	ArpPacket(ArpPacket* pkt, const bool doDeepCopy = true, 
    	const size_t offset = 0, const size_t newLen = 0): NetworkData(pkt, doDeepCopy, offset, newLen) {
		_expectedHwAddrType = pkt->_expectedHwAddrType;
		_expectedProtAddrType = pkt->_expectedProtAddrType;
		_expectedHwAddrSize = pkt->_expectedHwAddrSize;
		_expectedProtAddrSize = pkt->_expectedProtAddrSize;
	}
	
	/// Pointer to the hardware address type in the packet buffer.
	ACE_UINT8* hardwareAddrTypePtr() { return ptrUnit(); }
	/// Type of the hardware MAC address which is being mapped. For Ethernet, this is 1.
	ACE_UINT16 hardwareAddrType() { return to_u16_(hardwareAddrTypePtr()); }
	
	/// Pointer to the protocol address type in the packet buffer.
	ACE_UINT8* protocolAddrTypePtr() { return hardwareAddrTypePtr() + 2; }
	/// Type of the protocol address to which the MAC address is mapped. For IP, this is 0x0800.
    ACE_UINT16 protocolAddrType() { return to_u16_(protocolAddrTypePtr()); }
    
    /// Pointer to the hardware address size in the packet buffer.
    ACE_UINT8* hardwareAddrSizePtr() { return protocolAddrTypePtr() + 2; }
    /// Size of the hardware MAC address. For Ethernet, this is 6.
    ACE_UINT8 hardwareAddrSize() { return *(hardwareAddrSizePtr()); }
    
	/// Pointer to the protocol address size in the packet buffer.
    ACE_UINT8* protocolAddrSizePtr() { return hardwareAddrSizePtr() + 1; }
    /// Size of the protocol address. For IP, this is 4.
    ACE_UINT8 protocolAddrSize() { return *(protocolAddrSizePtr()); }

	/// Pointer to the operation in the packet buffer.
    ACE_UINT8* operationPtr() { return protocolAddrSizePtr() + 1; }
    /// Type of operation being performed. This can be 1 (ARP request), 2 (ARP reply).
    ACE_UINT16 operation() { return to_u16_(operationPtr()); }
    bool isReply() { return operation() == Reply; }
    bool isRequest() { return operation() == Request; }
    
    /// Pointer to the hardware MAC address of the host sending the ARP request or reply.
    ACE_UINT8* senderMACPtr() { return operationPtr() + 2; }
    /// The hardware MAC address of the host sending the ARP request or reply.
    MacAddress senderMAC() { return MacAddress(senderMACPtr()); }
    
    /// Pointer to the IP address of the host sending the ARP request or reply.
    ACE_UINT8* senderIPPtr() { return senderMACPtr() + hardwareAddrSize(); }
    /// The IP address of the host sending the ARP request or reply.  
    IPv4Addr senderIP() { return IPv4Addr(to_u32_(senderIPPtr(), false)); }
    void senderIP(ACE_UINT32 newSenderIP) {
    	from_u32_(newSenderIP, senderIPPtr(), false);
    }
    
    /// Pointer to the hardware MAC address of the host receiving the ARP request or reply.
    ACE_UINT8* targetMACPtr() { return senderIPPtr() + protocolAddrSize(); }
    /// The hardware MAC address of the host receiving the ARP request or reply.
    MacAddress targetMAC() { return MacAddress(targetMACPtr()); }
    
   	/// The IP address of the host receiving the ARP request or reply.
	ACE_UINT8* targetIPPtr() { return targetMACPtr() + hardwareAddrSize(); }
	/// The IP address of the host receiving the ARP request or reply.
	IPv4Addr targetIP() { return IPv4Addr(to_u32_(targetIPPtr(), false)); }
    void targetIP(ACE_UINT32 newTargetIP) {
    	from_u32_(newTargetIP, targetIPPtr(), false);
    }	

    /// Combine the values from the supplied request and lookup from the table to respond.
    /// @param request The received request to respond to.
    /// @param requestedMac The MAC address the requester is looking for.
    void buildReply(ArpPacket*, MacAddress&);
    
    /// Use this when an IP address is encountered that's not in the ARP table.
    /// @param requestedIP The IP address who's hardware address we'd like to know.
    /// @param myMac The hardware address of the interface that's sending the request.
    /// @param myIp The IPv4 address of the interface sending the request.
    void buildRequest(const IPv4Addr&, MacAddress&, const IPv4Addr&);
    
    ACE_UINT16 enetType() const { return EthernetFrame::Eth_ARP; }
    unsigned long long expectedLength() { return DefaultSize; }
    ACE_UINT32 maxSize() { return DefaultSize; }
    static ACE_UINT8 minBytesToDetermineLength() { return 8; }
    std::string typeStr() const { return "ARP"; }

    bool isExpectedType() {
    	ACE_TRACE("ArpPacket::isExpectedType");
    	
    	if ( getUnitLength() < minBytesToDetermineLength() )
    		throw nd_error("Not enough bytes loaded to determine packet type validity.");
    
    	return ( hardwareAddrType() == _expectedHwAddrType ) &&
    		( protocolAddrType() == _expectedProtAddrType ) &&
    		( hardwareAddrSize() == _expectedHwAddrSize ) &&
    		( protocolAddrSize() == _expectedProtAddrSize ) &&
    		( isReply() || isRequest() );
    }

};

} // namespace nasaCE

#endif // NASAARPPACKET_HPP
