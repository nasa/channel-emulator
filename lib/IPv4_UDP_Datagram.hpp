/* -*- C++ -*- */

//=============================================================================
/**
 * @file   IPv4_UDP_Datagram.hpp
 * @author Tad Kollar  
 *
 * $Id: IPv4_UDP_Datagram.hpp 1670 2012-08-28 13:25:28Z tkollar $ 
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _NASA_UDP_DATAGRAM_HPP_
#define _NASA_UDP_DATAGRAM_HPP_

#include "IPv4Packet.hpp"

namespace nasaCE {

//=============================================================================
/**
	@class IPv4_UDP_Datagram
	@author Tad Kollar  
	@brief Implements the structural unit of the Universal Datagram Protocol.
	
<p><b>UDP Datagram Structure</b></p>
<pre>
+-----+-------------+-----------------+
|bits |0 - 15       |16 - 31          |
+-----+-------------+-----------------+
|0    |Source Port  |Destination Port |
+-----+-------------+-----------------+
|32   |Length       |Checksum         |
+-----+-------------+-----------------+
|64   |Data                           |
+-----+-------------------------------+
</pre>
*/
//=============================================================================
class IPv4_UDP_Datagram: public IPv4Packet {

public:
	static const size_t spanUDPSrcPort;
	static const size_t spanUDPDstPort;
	static const size_t spanUDPLength;
	static const size_t spanUDPChecksum;

	/// Default constructor.
    IPv4_UDP_Datagram(): IPv4Packet() {}
    
    /// Initialize by copying from a buffer of the specified size.
    /// @param bufferSize The amount of data to copy from the buffer.
    /// @param buffer Pointer to the area to copy from. If 0, just allocate the space.
    /// @param isOutside True if this is the outermost container of the buffer.
    IPv4_UDP_Datagram(const size_t bufferSize, const ACE_UINT8* buffer = 0):
    	IPv4Packet(bufferSize, buffer) { }
    
    /// Copy constructor.
	IPv4_UDP_Datagram(const NetworkData* other, const bool doDeepCopy = true, 
    	const size_t offset = 0, const size_t newLen = 0): IPv4Packet(other, doDeepCopy, offset, newLen) { }

	/// Destructor.
    ~IPv4_UDP_Datagram() {
	//	AND_DEBUG((LM_DEBUG, "Deleting IPv4_UDP_Datagram.\n"));
    }
	
	// *** Unique accessors ***
	
	/// Get a pointer to the source port field.
	ACE_UINT8* ptrSrcPort() const { return IPv4Packet::ptrData(); }
	
	/// Get the source port field value.
	ACE_UINT16 getSrcPort() const { return to_u16_(ptrSrcPort()); }
	
	/// Set the source port field value.
	/// @param newPort Identifies the sending port when meaningful and should be
	/// assumed to be the port to reply to if needed. If not used, then it should be zero.
	void setSrcPort(const ACE_UINT16 newPort) { from_u16_(newPort, ptrSrcPort()); }
	
	/// Get a pointer to the destination port field.
	ACE_UINT8* ptrDstPort() const { return ptrSrcPort() + spanUDPSrcPort; }
	
	/// Get the destination port field value.
	ACE_UINT16 getDstPort() const { return to_u16_(ptrDstPort()); }
	
	/// Set the destination port field value.
	/// @param newPort Identifies the destination port and is required.
	void setDstPort(const ACE_UINT16 newPort) { from_u16_(newPort, ptrDstPort()); }
	
	/// Get a pointer to the length field.
	ACE_UINT8* ptrUDPLength() const { return ptrDstPort() + spanUDPDstPort; }
	
	/// Get the value of the length field.
	ACE_UINT16 getUDPLength() const { return to_u16_(ptrUDPLength()); }
	
	/// Set the value of the length field.
	/// @param newLength Specifies the length in bytes of the entire datagram: header and data.
	/// The minimum length is 8 bytes since that's the length of the header.
	void setUDPLength(const ACE_UINT16 newLength) { from_u16_(newLength, ptrUDPLength()); }
	
	/// Get a pointer to the checksum field.
	ACE_UINT8 *ptrUDPChecksum() const { return ptrUDPLength() + spanUDPLength; }
	
	/// Get the value of the checksum field.
	ACE_UINT16 getUDPChecksum() const { return to_u16_(ptrUDPChecksum()); }
	
	/// Determine whether the UDP checksum is set or not.
	bool hasUDPChecksum() const { return ( getUDPChecksum() > 0 ); }
	
	/// Set the value of the UDP checksum field.
	/// @param newChecksum used for error-checking of the header and data. The algorithm for
	/// computing the checksum is different for transport over IPv4 and IPv6.
	void setUDPChecksum(const ACE_UINT16 newChecksum) { from_u16_(newChecksum, ptrUDPChecksum()); }
	
	/// Return the total size of the header.
	size_t getHeaderLength() const {
		return IPv4Packet::getHeaderLength() + spanUDPSrcPort + 
			spanUDPDstPort + spanUDPLength + spanUDPChecksum;
	}
	
	// *** Overloaded data field functions ***
	
	/// Get a pointer to the data field.
	ACE_UINT8* ptrData() const { return ptrUDPChecksum() + spanUDPChecksum; }
	
	/// Get the length of the actual payload in the data field.
	size_t getDataLength() const { return getUnitLength() - getHeaderLength(); }
	
	/// Calculate (total length - header) and return the length of the payload,
	/// which will be wrong in an incomplete packet.
	size_t getExpectedDataLength() const { return getUDPLength() - (spanUDPSrcPort + 
		spanUDPDstPort + spanUDPLength + spanUDPChecksum); }
	
	/// If the expected data length doesn't match the actual data length, it's not a
	/// complete unit.
	bool isTruncated() const { return (getExpectedDataLength() - getDataLength() != 0); }
	
	/// Copy the provided network data into this packet.
	void setData(NetworkData* data);
	
	/// String representation of the contained data type.
	std::string typeStr() const { return "IPv4 UDP Datagram"; }
	
	/// Generate a IPv4/UDP packet containing the provided data. Fragmentation
	/// is not performed here. Cannot be used on a packet with existing data.
	/// @param srcIPAddr The address of the packet's sender.
	/// @param srcPort The port number of the packet's sender.
	/// @param dstIPAddr The address of the packet's target.
	/// @param dstPort The port number of the target.
	/// @param data Optional payload to insert into the packet.
	/// @param ttl The number of hops before the packet expires.
	/// @throw runtime_error If data already exists in the buffer.
	void build(const IPv4Addr& srcIPAddr, const ACE_UINT16 srcPort,
		const IPv4Addr& dstIPAddr, const ACE_UINT16 dstPort, NetworkData* data = 0,
		const ACE_UINT8 ttl = 63);
		
	/// @brief Compute a checksum over a pseudo-header and actual datagram.
	///
	/// @param protocol The protocol specified in the pseudo-header, usually 0x11 for UDP.
	/// @see Adapted from http://www.netfor2.com/udpsum.htm
	ACE_UINT16 computeUDPChecksum(const ACE_UINT16 protocol = IPv4Packet::UDP) const;
	
	/// Compute the checksum with the stored value included. It should return 0.
	/// @return True if valid OR if the field is not in use.
	bool hasValidUDPChecksum() const {
		if ( getUDPChecksum() == 0 ) return true;
		
		return ( computeUDPChecksum() == 0 );
	}	
	
	/// The minimum length to be a valid datagram.
	size_t minimumLength() const { return getHeaderLength(); }
	
	/// Test the datagram in different ways to determine if it's inviolate.
	/// @param checkIPv4 Whether to run IPv4Packet::throwIfInvalid, also.
	/// @throw IPv4_UDP_Datagram::UnderMinLength If the datagram is shorter than minimumLength().
	/// @throw IPv4_UDP_Datagram::WrongSize If the UDP length field doesn't match the actual length.
	/// @throw IPv4_UDP_Datagram::InvalidChecksum If the UDP checksum is wrong.
	void throwIfInvalid(const bool checkIPv4 = true) const;
	
}; // class IPv4_UDP_Datagram

} // namespace nasaCE

#endif // _NASA_UDP_DATAGRAM_HPP_
