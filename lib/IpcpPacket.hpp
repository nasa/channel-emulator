/* -*- C++ -*- */

//=============================================================================
/**
 * @file   IpcpPacket.hpp
 * @author Tad Kollar  
 *
 * $Id: IpcpPacket.hpp 1670 2012-08-28 13:25:28Z tkollar $ 
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef NASAIPCPPACKET_HPP
#define NASAIPCPPACKET_HPP

#include "PppConfPacket.hpp"
#include "IPAddr.hpp"

namespace nasaCE {

/**
 * @class IpcpPacket
 *
 * @brief Holds and operates on a single PPP Internet Protocol Control Protocol (IPCP) packet.
 */
class IpcpPacket: public PppConfPacket {
private:
	static PppConfOption* _buildIPOption(const ACE_UINT32 ip) {
		return new PppConfOption(IP_Address, 4, (ACE_UINT8*) &ip);
	}

public:
	enum OptionTypes {
		IP_Addresses = 1,
		IP_Compression_Protocol = 2,
		IP_Address = 3,
		OptionTypesCount = 4
	};
	
	static std::string optTypeStr[OptionTypesCount];

	IpcpPacket(bool allocate = false): PppConfPacket(allocate) { }
	IpcpPacket(ACE_UINT8* buf, unsigned bufLen): PppConfPacket(buf, bufLen) { }
	IpcpPacket(const IpcpPacket* other): PppConfPacket(other) { }
	
	/// If this packet contains an IP-Address option, return the IP
	IPv4Addr getIP();

	/// @p ipAddr A reference to an IPv4Addr object, containing IP address to add.
	void addIP(IPv4Addr&);
	
	/// String representation of the contained data type.
	std::string typeStr() const { return "IPCP Packet"; }
	
	/// Genererate a fresh packet.
	/// @p code Value identifying the kind of packet.
	/// @p identifier Helps matching requests and replies.
	/// @p ipAddr A reference to an IPv4Addr object, containing IP address to add.
	void buildWithIP(const ACE_UINT8, const ACE_UINT8, IPv4Addr&);
	
}; // class IpcpPacket

} // namespace nasaCE

#endif // NASAIPCPPACKET_HPP
