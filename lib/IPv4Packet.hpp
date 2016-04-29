/* -*- C++ -*- */

//=============================================================================
/**
 * @file   IPv4Packet.hpp
 * @author Tad Kollar  
 *
 * $Id: IPv4Packet.hpp 1670 2012-08-28 13:25:28Z tkollar $ 
 * Copyright (c) 2008.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef NASAIPV4PACKET_HPP
#define NASAIPV4PACKET_HPP

#include "NetworkData.hpp"
#include "IPAddr.hpp"
#include "EthernetFrame.hpp"

#include <ace/Log_Msg.h>
#include <ace/Malloc_Base.h>

extern "C" {
	#include <linux/if_ether.h>
	#include <netinet/ip.h>
}

// IPv4/ICMP
// 0                          1                             2                             3                             4                             5                             6                             7                             8
//  1  2  3  4  5  6  7  8  9  0  1  2  3  4  5  6  7  8  9  0  1  2  3  4  5  6  7  8  9  0  1  2  3  4  5  6  7  8  9  0  1  2  3  4  5  6  7  8  9  0  1  2  3  4  5  6  7  8  9  0  1  2  3  4  5  6  7  8  9  0  1  2  3  4  5  6  7  8  9  0  1  2  3  4
// 45 00 00 54 00 00 40 00 40 01 22 B1 0A FA 01 02 0A FA 01 03 08 00 16 FF B8 6B 00 01 A7 63 59 48 00 00 00 00 5B 15 0E 00 00 00 00 00 10 11 12 13 14 15 16 17 18 19 1A 1B 1C 1D 1E 1F 20 21 22 23 24 25 26 27 28 29 2A 2B 2C 2D 2E 2F 30 31 32 33 34 35 36 37
// 45 00 00 54 00 00 40 00 40 01 22 B1 0A FA 01 02 0A FA 01 03 08 00 B2 FD B8 6B 00 02 A8 63 59 48 00 00 00 00 BE 15 0E 00 00 00 00 00 10 11 12 13 14 15 16 17 18 19 1A 1B 1C 1D 1E 1F 20 21 22 23 24 25 26 27 28 29 2A 2B 2C 2D 2E 2F 30 31 32 33 34 35 36 37
// 45 00 00 54 00 00 40 00 40 01 22 B1 0A FA 01 02 0A FA 01 03 08 00 D1 FC B8 6B 00 03 A9 63 59 48 00 00 00 00 9E 15 0E 00 00 00 00 00 10 11 12 13 14 15 16 17 18 19 1A 1B 1C 1D 1E 1F 20 21 22 23 24 25 26 27 28 29 2A 2B 2C 2D 2E 2F 30 31 32 33 34 35 36 37


namespace nasaCE {

/**
 * @class IPv4Packet
 * @author Tad Kollar  
 * @brief Perform IPv4 operations on the contained buffer.
 *

<p><b>IPv4 Packet Structure</b></p>
<pre>
+----+-----------------+----------------+----------+-------+------------------+
|+   |     Bits 0-3    |       4-7      |   8-15   | 16-18 |      19-31       |
+----+-----------------+----------------+----------+-------+------------------+
|0   |     Version     | Header length  |   ToS    |       Total Length       |
+----+-----------------+----------------+----------+-------+------------------+
|32  |               Identification                | Flags | Fragment Offset  |
+----+---------------------------------------------+-------+------------------+
|64  |            Time to Live          | Protocol |     Header Checksum      |
+----+----------------------------------+----------+--------------------------+
|96  |                            Source Address                              |
+----+------------------------------------------------------------------------+
|128 |                          Destination Address                           |
+----+------------------------------------------------------------------------+
|160 |                               Options                                  | 
+----+------------------------------------------------------------------------+
|160 |                                                                        |
| or |                                 Data                                   |
|192+|                                                                        |
+----+------------------------------------------------------------------------+
</pre>

*/
class IPv4Packet : public NetworkData {
public:
	enum ProtocolTypes {
		ICMP = 1,
		IGMP = 2,
		TCP = 6,
		UDP = 17,
		OSPF = 89,
		SCTP = 132	
	};

	/// Default constructor.
	IPv4Packet(): NetworkData() { }
	
	/// Initialize with an allocated buffer.
	IPv4Packet(const size_t bufferSize, const ACE_UINT8* buffer = 0):
		NetworkData(bufferSize, buffer) { }
	
	/// Copy constructor.
	IPv4Packet(const NetworkData* other, const bool doDeepCopy = true, 
    	const size_t offset = 0, const size_t newLen = 0):
    	NetworkData(other, doDeepCopy, offset, newLen) { }
	
	/// Pointer to the IP version in the packet buffer.
	ACE_UINT8* ptrVersion() const { return ptrUnit(); }
	
	/// Value of the four-bit version field. For IPv4, this has a value of 4.
	ACE_UINT8 getVersion() const { return *ptrVersion() >> 4; }
	
	/// Set the version field.
	void setVersion(const ACE_UINT8 newVer) { *ptrVersion() = (*ptrVersion()) & (newVer << 4); }
	
	/// Value of the four-bit Internet Header Length (IHL).
	///
	/// Contains the number of 32-bit words in the header. An IPv4 header
	/// contains variable number of options, so this field specifies the size of
	/// the header - also the offset to the data. The minimum value for
	/// this field is 5 (rfc791), which is a length of 5�32 = 160 bits. Being a
	/// four-bit field, the maximum length is 15 words or 480 bits.
	size_t getHeaderWords() const { return *ptrVersion() & 0xF; }
	
	/// Perform the calculation for the bytes in the header.
	size_t getHeaderLength() const { return getHeaderWords() * 4; }
	
	/// Set the number of bytes in header.
	/// @param len Either the number of bytes or 32-bit words.
	/// @param isBytes True if len was specified with bytes instead of words.
	void setHeaderWords(const ACE_UINT16 len, const bool isBytes = true) {
		*ptrVersion() = *ptrVersion() & (isBytes)? (len/4) : len;
	}
	
	/// Pointer to the Type of Service (TOS) bits.
	ACE_UINT8* ptrTOS() const { return ptrVersion() + 1; }
	
	/// Total value of the ToS bits.
	ACE_UINT8 getTOS() const { return *ptrTOS(); }
	
	/// Change the value of the ToS field.
	void setTOS(const ACE_UINT8 newTos) { *ptrTOS() = newTos; }
	
	/// Pointer to the total length.
	ACE_UINT8* ptrTotalLength() const { return ptrTOS() + 1; }
	
	/// The entire datagram size, including header and data, in bytes.
	ACE_UINT16 getTotalLength() const { return to_u16_(ptrTotalLength()); }
	
	/// Change the total length parameter of the packet.
	void setTotalLength(const ACE_UINT16 len) { from_u16_(len, ptrTotalLength()); }
	
	/// Report how big the packet thinks it should be.
	size_t expectedLength() const { return getTotalLength(); }
	
	/// Pointer to the identification field.
	ACE_UINT8* ptrIdent() const { return ptrTotalLength() + 2; }
	
	/// Primarily used for uniquely identifying fragments of an original IP datagram.
	ACE_UINT16 getIdent() const { return to_u16_(ptrIdent()); }
	
	/// Change the packet's identification.
	void setIdent(const ACE_UINT16 id) { from_u16_(id, ptrIdent()); }
	
	/// Pointer to the flags field.
	ACE_UINT8* ptrFlags() const { return ptrIdent() + 2; }
	
	/// The ORed values of the flags and offset fields.
	ACE_UINT16 getFlags() const { return to_u16_(ptrFlags()); }
	
	/// Set the flags and offset fields together.
	void setFlags(const ACE_UINT16 flags_and_offset) { from_u16_(flags_and_offset, ptrFlags()); }
	
	/// The offset of a fragment relative to the beginning of the original unfragmented IP datagram.
	ACE_UINT16 getOffset() const { return getFlags() & IP_OFFMASK; }

	/// Pointer to the time-to-live (TTL) field.
	ACE_UINT8* ptrTTL() const { return ptrFlags() + 2; }
	
	/// Maximum number of hops that the packet is allowed to endure before expiring.
	ACE_UINT8 getTTL() const { return *ptrTTL(); }
	
	/// Set the time-to-live field value.
	void setTTL(const ACE_UINT8 newTTL) { *ptrTTL() = newTTL; }
	
	/// Pointer to the protocol field.
	ACE_UINT8* ptrProtocol() const { return ptrTTL() + 1; }
	
	/// The protocol used in the data portion of the IP datagram.
	ACE_UINT8 getProtocol() const { return *ptrProtocol(); }
	
	/// Change the value of the protocol field.
	void setProtocol(const ACE_UINT8 newProt) { *ptrProtocol() = newProt; }

	/// Pointer to the header checksum field.
	ACE_UINT8* ptrChecksum() const { return ptrProtocol() + 1; }
	
	/// Used for error-checking the header.
	ACE_UINT16 getChecksum() const { return to_u16_(ptrChecksum()); }
	
	/// Change the header checksum field.
	void setChecksum(const ACE_UINT16 chk) { from_u16_(chk, ptrChecksum()); }
	
	/// Pointer to the source address field.
	ACE_UINT8* ptrSrc() const { return ptrChecksum() + 2; }
	
	/// The address of the host that originated the packet.
	ACE_UINT32 getSrc() const { return to_u32_(ptrSrc(), false); }
	
	/// Initialize an IPv4Addr object with the 32-bit source address integer.
	IPv4Addr getSrcIP() const { return IPv4Addr(getSrc()); }
	
	/// Set the value of the source IP address field.
	void setSrc(const ACE_UINT32 newSrcIP) { from_u32_(newSrcIP, ptrSrc(), false); }
	
	/// Set the value of the source IP address field using an IPv4Addr object.
	void setSrc(const IPv4Addr& newSrcIP) { from_u32_(newSrcIP.get(), ptrSrc(), false); }
	
	/// Pointer to the destination address field.
	ACE_UINT8* ptrDst() const { return ptrSrc() + 4; }
	
	/// The address of the host for which the packet is headed.
	ACE_UINT32 getDst() const { return to_u32_(ptrDst(), false); }
	
	/// Initialize an IPv4Addr object with the 32-bit destination address integer.
	IPv4Addr getDstIP() const { return IPv4Addr(getDst()); }
	
	/// Set the value of the destination IP address field.
	void setDst(const ACE_UINT32 newDstIP) { from_u32_(newDstIP, ptrDst(), false); }
	
	/// Set the value of the destination IP address field using an IPv4Addr object.
	void setDst(const IPv4Addr& newDstIP) { from_u32_(newDstIP.get(), ptrDst(), false); }
	
	/// Pointer to the options field (which may actually be data).
	ACE_UINT8* ptrOptions() const { return ptrDst() + 4; };
	
	// *** Overloaded functions ***
	// Note: IPv4Packet:: scope is explicit here because these are overloaded
	// again by derived class. When that happens without the scope being
	// specified, the checksum is computed incorrectly and fragmentation
	// doesn't work right.
	
	/// Pointer to the data portion of the packet.
	///
	/// Calculated by extracting the header length from the packet and adding
	/// it to the value returned by versionPtr(). This is necessary in case there
	/// are no options (most of the time).
	ACE_UINT8* ptrData() const { return ptrVersion() + IPv4Packet::getHeaderLength(); }
	
	/// Calculate (total length - header) and return the length of the payload,
	/// which will be wrong in an incomplete packet.
	size_t getExpectedDataLength() const {
		return IPv4Packet::getTotalLength() - IPv4Packet::getHeaderLength();
	}
	
	/// Calculate (packet size - header) and return the actual length of the payload.
	size_t getDataLength() const { return getUnitLength() - IPv4Packet::getHeaderLength(); }
	
	size_t minimumLength() const { return IPv4Packet::getHeaderLength(); }
	
	/// If the expected data length doesn't match the actual data length, it's not a
	/// complete packet. This is different from an IPv4 fragment in that there's no
	/// information in the header of a truncated packet about segments to follow.
	bool isTruncated() const {
		return (IPv4Packet::getExpectedDataLength() - IPv4Packet::getDataLength() != 0);
	}
	
	/// Copy the provided network data into this packet, changing our buffer size if necessary.
	/// @param data The NetworkData object to entirely copy into our data area.
	void setData(NetworkData* data) {
		if ( getUnitLength() != data->getUnitLength() + IPv4Packet::getHeaderLength() )
			setUnitLength(data->getUnitLength() + IPv4Packet::getHeaderLength());
		NetworkData::setData(data);
		setTotalLength(getUnitLength());
	}
	
	/// Compare values from the first 32 bits to expected ranges for IPv4.
	bool isExpectedType() const {
		ACE_TRACE("IPv4Packet::isExpectedType");
    	if ( getUnitLength() < minBytesToDetermineLength() )
    		throw std::runtime_error("Not enough bytes loaded to determine packet type validity.");
    		
		return ( getVersion() == 4 ) &&
			( getHeaderWords() >= 5 && getHeaderWords() <= 15 ) &&
			( getTotalLength() >= (getHeaderWords() * 4) );
	}
	
	/// When encapsulated in an Ethernet frame, identify with this type.
	ACE_UINT16 enetType() const { return EthernetFrame::Eth_IPv4; }
	
	/// For packets that are not delimited by special markers that are coming
    /// in serially, return the minimum amount of data needed to extract the
    /// length from internal parameters.
	static ACE_UINT8 minBytesToDetermineLength() { return 4; }
	
	/// String representation of the contained data type.
	std::string typeStr() const { return "IPv4 Packet"; }
	
	/// If the identity field is non-zero, and either the IP_MF flag is set
	/// or the offset value is set, it's a fragment.
	bool isFragment() const { 
		return ( getIdent() && (getFlags() & IP_MF || getOffset()) );
	}
	
	/// If the identity field is set, the IP_MF flag is set, but the offset
	/// value is zero, this is the first fragment.
	bool isFirstFragment() const {
		return ( getIdent() && (getFlags() & IP_MF && ! getOffset()) );
	}
	
	/// If the identity field is set, the IP_MF flag is not set, and the
	/// offset value is non-zero, this is the last fragment.
	bool isLastFragment() const {
		return ( getIdent() && ( ! (getFlags() & IP_MF) && getOffset()) );
	}
	
	/// Rebuild the packet into an unfragmented whole.
	///
	/// If this is a fragmented packet (should be the final in the series) and
	/// we have a complete data buffer, combine the info from the current header
	/// with the data to assemble the complete packet.
	/// @param newDataBuf A pointer to the completed data buffer.
	/// @param freeBuf Free() the passed buffer when done, defaults to true.
	void defragment(ACE_UINT8*, const bool freeBuf = true);
	
	/// Add all the two-byte words in the header together and return the result.
	ACE_UINT16 computeChecksum() const;
	
	/// Test the stored checksum against a computed one.
	/// @return True if they match, false if not.
	bool hasValidChecksum() const {
		return ( getChecksum() == computeChecksum() );
	}
	
	/// Break a large packet down into smaller ones.
	/// @param frags The vector to populate with the packet fragments.
	/// @param maxLen The maximum size the new packets can be.
	void fragment(std::vector<IPv4Packet*>&, const ACE_UINT16);
	
	/// Generate a IPv4 packet containing the provided data. Fragmentation
	/// is not performed here. Cannot be used on a packet with existing data.
	/// @param srcIPAddr The address of the packet's sender.
	/// @param dstIPAddr The address of the packet's target.
	/// @param data The payload to insert into the packet.
	/// @param ttl The number of hops before the packet expires.
	/// @param prot The protocol of the payload.
	/// @throw runtime_error If data already exists in the buffer.
	void build(const IPv4Addr& srcIPAddr, const IPv4Addr& dstIPAddr, NetworkData* data,
		const ACE_UINT8 ttl = 64, const ACE_UINT8 prot = 0);

	/// Test the datagram in different ways to determine if it's inviolate.
	/// @throw IPv4Packet::UnderMinLength If the length is shorter than the minimum header size.
	/// @throw IPv4Packet::WrongSize If the packet length doesn't match the total length field.
	/// @throw IPv4Packet::InvalidChecksum If the IPv4 header checksum is wrong.
	virtual void throwIfInvalid() const;
	
	/// Get the Internet Protocol Extension header value for use in an Encapsulation Packet.
	/// @return For an IPv4 datagram, the value is always 0x21.
	ACE_INT64 getIPE_Header_Val() const { return 0x21; }
	
private:

	/// Create a single fragment of the buffered packet using the provided offset and length.
	/// @param offset The data index to start from; stored in the new fragment.
	/// @param dataLen The maximum data length in the new fragment.
	/// @param id Each fragment of this packet shares the same id.
	/// @param isLast True is this is the final fragment so that the IP_MF flag is not set.
	/// @return A pointer to the new fragment.
	IPv4Packet* _make_one_fragment(const ACE_UINT16 offset,
		const ACE_UINT16 dataLen, const ACE_UINT16 id,
		const bool isLast = false);
	
}; // class IPv4Packet

} // namespace nasaCE

#endif // NASAIPV4PACKET_HPP
