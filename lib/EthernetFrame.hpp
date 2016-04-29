/* -*- C++ -*- */

//=============================================================================
/**
 * @file   EthernetFrame.hpp
 * @author Tad Kollar  
 *
 * $Id: EthernetFrame.hpp 1670 2012-08-28 13:25:28Z tkollar $ 
 * Copyright (c) 2008.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _ETHERNET_FRAME_HPP_
#define _ETHERNET_FRAME_HPP_

#include "NetworkData.hpp"
#include "MacAddress.hpp"

/*
    +-------------------+
    |Tap Preamble       | 2 bytes
    +-------------------+
    |Tap Payload Type   | 2 bytes
    +-------------------+
    |Target MAC Address | 6 bytes
    +-------------------+
    |Source MAC Address | 6 bytes
    +-------------------+
    |Payload Type/Length| 2 (normal) or 6 (802.1q) bytes
    +-------------------+
    | Payload           | Variable
    +-------------------+
*/

// ping from 10.250.1.2 (0A FA 01 02)/00:30:48:57:6C:27 
//  to       10.250.1.3 (0A FA 01 03)/00:30:48:57:6C:3F

// ARP Conversation
// 0                          1                             2                             3                             4                             5                             6                            
//  1  2  3  4  5  6  7  8  9  0  1  2  3  4  5  6  7  8  9  0  1  2  3  4  5  6  7  8  9  0  1  2  3  4  5  6  7  8  9  0  1  2  3  4  5  6  7  8  9  0  1  2  3  4  5  6  7  8  9  0  1  2  3  4  5  6  7  8  9
// 00 00 08 06 FF FF FF FF FF FF 00 30 48 57 6C 27 08 06 00 01 08 00 06 04 00 01 00 30 48 57 6C 27 0A FA 01 02 00 00 00 00 00 00 0A FA 01 03 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
// 00 00 08 06 FF FF FF FF FF FF 00 30 48 57 6C 27 08 06 00 01 08 00 06 04 00 01 00 30 48 57 6C 27 0A FA 01 02 00 00 00 00 00 00 0A FA 01 03 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
// 00 00 08 06 00 30 48 57 6C 27 00 30 48 57 6C 3F 08 06 00 01 08 00 06 04 00 02 00 30 48 57 6C 3F 0A FA 01 03 00 30 48 57 6C 27 0A FA 01 02 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
// 00 00 08 06 FF FF FF FF FF FF 00 30 48 57 6C 27 08 06 00 01 08 00 06 04 00 01 00 30 48 57 6C 27 0A FA 01 02 00 00 00 00 00 00 0A FA 01 03 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
// 00 00 08 06 00 30 48 57 6C 27 00 30 48 57 6C 3F 08 06 00 01 08 00 06 04 00 02 00 30 48 57 6C 3F 0A FA 01 03 00 30 48 57 6C 27 0A FA 01 02 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00

// IPv4/ICMP
// 0                          1                             2                             3                             4                             5                             6                             7                             8                             9                            10                         
//  1  2  3  4  5  6  7  8  9  0  1  2  3  4  5  6  7  8  9  0  1  2  3  4  5  6  7  8  9  0  1  2  3  4  5  6  7  8  9  0  1  2  3  4  5  6  7  8  9  0  1  2  3  4  5  6  7  8  9  0  1  2  3  4  5  6  7  8  9  0  1  2  3  4  5  6  7  8  9  0  1  2  3  4  5  6  7  8  9  0  1  2  3  4  5  6  7  8  9  0  1  2  3  4  5  6  7  8  9
// 00 00 08 00 00 30 48 57 6C 3F 00 30 48 57 6C 27 08 00 45 00 00 54 00 00 40 00 40 01 22 B1 0A FA 01 02 0A FA 01 03 08 00 16 FF B8 6B 00 01 A7 63 59 48 00 00 00 00 5B 15 0E 00 00 00 00 00 10 11 12 13 14 15 16 17 18 19 1A 1B 1C 1D 1E 1F 20 21 22 23 24 25 26 27 28 29 2A 2B 2C 2D 2E 2F 30 31 32 33 34 35 36 37
// 00 00 08 00 00 30 48 57 6C 3F 00 30 48 57 6C 27 08 00 45 00 00 54 00 00 40 00 40 01 22 B1 0A FA 01 02 0A FA 01 03 08 00 B2 FD B8 6B 00 02 A8 63 59 48 00 00 00 00 BE 15 0E 00 00 00 00 00 10 11 12 13 14 15 16 17 18 19 1A 1B 1C 1D 1E 1F 20 21 22 23 24 25 26 27 28 29 2A 2B 2C 2D 2E 2F 30 31 32 33 34 35 36 37
// 00 00 08 00 00 30 48 57 6C 3F 00 30 48 57 6C 27 08 00 45 00 00 54 00 00 40 00 40 01 22 B1 0A FA 01 02 0A FA 01 03 08 00 D1 FC B8 6B 00 03 A9 63 59 48 00 00 00 00 9E 15 0E 00 00 00 00 00 10 11 12 13 14 15 16 17 18 19 1A 1B 1C 1D 1E 1F 20 21 22 23 24 25 26 27 28 29 2A 2B 2C 2D 2E 2F 30 31 32 33 34 35 36 37
// 00 00 08 00 00 30 48 57 6C 27 00 30 48 57 6C 3F 08 00 45 00 00 54 C9 5E 00 00 40 01 99 52 0A FA 01 03 0A FA 01 02 00 00 1E FF B8 6B 00 01 A7 63 59 48 00 00 00 00 5B 15 0E 00 00 00 00 00 10 11 12 13 14 15 16 17 18 19 1A 1B 1C 1D 1E 1F 20 21 22 23 24 25 26 27 28 29 2A 2B 2C 2D 2E 2F 30 31 32 33 34 35 36 37
// 00 00 08 00 00 30 48 57 6C 27 00 30 48 57 6C 3F 08 00 45 00 00 54 C9 5F 00 00 40 01 99 51 0A FA 01 03 0A FA 01 02 00 00 BA FD B8 6B 00 02 A8 63 59 48 00 00 00 00 BE 15 0E 00 00 00 00 00 10 11 12 13 14 15 16 17 18 19 1A 1B 1C 1D 1E 1F 20 21 22 23 24 25 26 27 28 29 2A 2B 2C 2D 2E 2F 30 31 32 33 34 35 36 37
// 00 00 08 00 00 30 48 57 6C 27 00 30 48 57 6C 3F 08 00 45 00 00 54 C9 60 00 00 40 01 99 50 0A FA 01 03 0A FA 01 02 00 00 D9 FC B8 6B 00 03 A9 63 59 48 00 00 00 00 9E 15 0E 00 00 00 00 00 10 11 12 13 14 15 16 17 18 19 1A 1B 1C 1D 1E 1F 20 21 22 23 24 25 26 27 28 29 2A 2B 2C 2D 2E 2F 30 31 32 33 34 35 36 37
// 00 00 08 00 00 30 48 57 6C 3F 00 30 48 57 6C 27 08 00 45 00 00 54 00 00 40 00 40 01 22 B1 0A FA 01 02 0A FA 01 03 08 00 E9 EE B8 6B 00 04 AA 63 59 48 00 00 00 00 85 22 0E 00 00 00 00 00 10 11 12 13 14 15 16 17 18 19 1A 1B 1C 1D 1E 1F 20 21 22 23 24 25 26 27 28 29 2A 2B 2C 2D 2E 2F 30 31 32 33 34 35 36 37
// 00 00 08 00 00 30 48 57 6C 27 00 30 48 57 6C 3F 08 00 45 00 00 54 C9 61 00 00 40 01 99 4F 0A FA 01 03 0A FA 01 02 00 00 F1 EE B8 6B 00 04 AA 63 59 48 00 00 00 00 85 22 0E 00 00 00 00 00 10 11 12 13 14 15 16 17 18 19 1A 1B 1C 1D 1E 1F 20 21 22 23 24 25 26 27 28 29 2A 2B 2C 2D 2E 2F 30 31 32 33 34 35 36 37
// 00 00 08 00 00 30 48 57 6C 3F 00 30 48 57 6C 27 08 00 45 00 00 54 00 00 40 00 40 01 22 B1 0A FA 01 02 0A FA 01 03 08 00 03 E1 B8 6B 00 05 AB 63 59 48 00 00 00 00 6A 2F 0E 00 00 00 00 00 10 11 12 13 14 15 16 17 18 19 1A 1B 1C 1D 1E 1F 20 21 22 23 24 25 26 27 28 29 2A 2B 2C 2D 2E 2F 30 31 32 33 34 35 36 37
// 00 00 08 00 00 30 48 57 6C 27 00 30 48 57 6C 3F 08 00 45 00 00 54 C9 62 00 00 40 01 99 4E 0A FA 01 03 0A FA 01 02 00 00 0B E1 B8 6B 00 05 AB 63 59 48 00 00 00 00 6A 2F 0E 00 00 00 00 00 10 11 12 13 14 15 16 17 18 19 1A 1B 1C 1D 1E 1F 20 21 22 23 24 25 26 27 28 29 2A 2B 2C 2D 2E 2F 30 31 32 33 34 35 36 37

namespace nasaCE {


/**
* @class EthernetFrame
*
* @brief A single Ethernet frame read from or destined for an Ethernet device.
*
* This is an abstract class with several pure virtual functions defined in
* its child EthernetFrameT, a template class. It exists to provide a
* common base for template-derived objects to allow polymorphism.

@htmlonly
<table border="1" style="border-color: black;">
<tr><td>Tap Preamble</td><td>2 bytes</td></tr>
<tr><td>Tap Payload Type</td><td>2 bytes</td></tr>
<tr><td>Target MAC Address</td><td>6 bytes</td></tr>
<tr><td>Source MAC Address</td><td>6 bytes</td></tr>
<tr><td>Payload Type/Length</td><td>2 (normal) or 6 (802.1q) bytes</td></tr>
<tr><td>Payload</td><td>Variable</td></tr>
</table>
@endhtmlonly
*/

class EthernetFrame: public NetworkData {
public:

	struct PreambleLengthError: public nd_error {
		PreambleLengthError(const std::string e): nd_error(e) { }
	};
	
	enum TypeLength { typeNormalLen = 2, type8021qLen = 6 };
	
	/// @enum Magic
	/// Values and comments copied from "linux/if_ether.h".
	enum Magic {
		Eth_AddrLen		= 6,	/*!< Octets in one ethernet addr   */
		Eth_MinLen		= 60,	/*!< Min. octets in frame sans FCS */
		Eth_MinPayload	= 46,	/*!< Min. octets in data field */
		Eth_FcsLen		= 4		/*!< Octets in the FCS             */
	};
	
	/// @enum PayloadType
	/// Types of payloads supported by this class.
	enum PayloadType {
		Eth_IPv4	= 0x0800,
		Eth_ARP		= 0x0806,
		Eth_AOS		= 0x0A05, /*!< This value is a local, not public assignment. */
		Eth_8021q	= 0x8100,
		Eth_IPv6	= 0x86DD,
		Eth_MaxType	= 0xFFFF
	};
	
	/// @enum SupportedFrameTypes
	/// Depending on the source, Ethernet frames can have different preamble lengths.
	/// These are the ones accounted for.
	enum SupportedFrameTypes {
		PCap = 0,
		LinuxTap = 4,
		IEEE = 8
	};
	
	/// Default constructor.
	EthernetFrame(const SupportedFrameTypes frameType = PCap,
		const TypeLength newTypeLen = typeNormalLen): 
		NetworkData(), preambleLength_(frameType), typeLen_(newTypeLen) { }
	
	/// Initialize with an allocated buffer.
	EthernetFrame(const size_t bufferSize,
		const SupportedFrameTypes frameType = PCap,
		const TypeLength newTypeLen = typeNormalLen,
		ACE_UINT8* buffer = 0):
		NetworkData(bufferSize, buffer), preambleLength_(frameType), typeLen_(newTypeLen) { }
	
	/// Copy constructor.
	explicit EthernetFrame(EthernetFrame* other,
		const TypeLength newTypeLen = typeNormalLen,
		const bool doDeepCopy = true, 
    	const size_t offset = 0, const size_t newLen = 0): 
    		NetworkData(other, doDeepCopy, offset, newLen),
    		preambleLength_(other->preambleLength_), typeLen_(newTypeLen) { }
    		
	/// Copy constructor.
	EthernetFrame(NetworkData* other,
		const bool doDeepCopy = true, 
    	const size_t offset = 0,
    	const size_t newLen = 0,
    	const SupportedFrameTypes frameType = PCap,
		const TypeLength newTypeLen = typeNormalLen): 
    		NetworkData(other, doDeepCopy, offset, newLen),
    		preambleLength_(frameType), typeLen_(newTypeLen) { }
	
	/// Destructor.
    virtual ~EthernetFrame() { }

	/// Pointer to the preamble, if it exists.
	ACE_UINT8* ptrPreamble() const { testPreambleLength_(); return ptrUnit(); }
	
	/// This value is normally 0x0000 for tap or 0xAAAAAAAAAAAAAAAB for IEEE.
	/// @return The integer value of the preamble if in use.
	ACE_UINT64 getPreamble() const;
	
	/// Set the preamble only if one is supposed to be there.
	/// @throw PreambleLengthError If the preamble is not a recognized length or 0.
	void setPreamble(const ACE_UINT64 newVal = 0xAAAAAAAAAAAAAAABllu );
	
	/// Pointer to the type field in the "tap" preamble. Does not exist in other types.
	ACE_UINT8* ptrPreambleType() const { testPreambleLength_(LinuxTap); return ptrPreamble() + 2; }
	
	/// Value of the type field in the "tap" preamble.
	/// @return The type of payload embedded in the frame, may be byte-swapped.
	ACE_UINT16 getPreambleType() const { return to_u16_(ptrPreambleType()); }
	
	/// Set the type field in the "tap" preamble.
	/// @param newType The 16-bit Ethernet type value to set, may be byte-swapped.
	void setPreambleType(const ACE_UINT16 newType) { from_u16_(newType, ptrPreambleType()); }
	
	/// Pointer to the destination MAC address field.
	ACE_UINT8* ptrDstAddr() const;
	
	/// Set the destination MAC address field.
	/// @param dstMacPtr A pointer to a 6-byte array which will be copied into the field.
	void setDstAddr(ACE_UINT8* dstMacPtr) { copyUnit(ptrDstAddr(), dstMacPtr, Eth_AddrLen); }
	
	/// Set the destination MAC address field using a MacAddress object.
	void setDstAddr(MacAddress& dstMac) { setDstAddr(dstMac.address()); }	
	
	/// Pointer to the source MAC address field.
	ACE_UINT8* ptrSrcAddr() const { return ptrDstAddr() + Eth_AddrLen; }
	
	/// Set the source MAC address field.
	/// @param srcMacPtr A pointer to a 6-byte array which will be copied into the field.
	void setSrcAddr(ACE_UINT8* srcMacPtr) { copyUnit(ptrSrcAddr(), srcMacPtr, Eth_AddrLen); }
	
	/// Set the source MAC address field using a MacAddress object.	
	void setSrcAddr(MacAddress& srcMac) { setSrcAddr(srcMac.address()); }
	
	/// Pointer to the Ethernet type-or-length field.
	ACE_UINT8* ptrEtherType() const { return ptrSrcAddr() + Eth_AddrLen; }
	
	/// Value of the Ethernet type-or-length field.
	ACE_UINT16 getEtherType() const { return to_u16_(ptrEtherType()); }
	
	/// Set the value of the Ethernet type-or-length field.
	void setEtherType(const ACE_UINT16 newType) { from_u16_(newType, ptrEtherType()); }
	
	/// If the type is recognized, return a string representation of it.
	std::string getEtherTypeStr();

	/// Determine the type of the payload regardless if this is an 802.1q frame or not.
	virtual ACE_UINT16 getPayloadType() const { return getEtherType(); }

	/// Determine if this payload is an ARP packet.
	bool isArp() const { return (getPayloadType() == Eth_ARP); }
		
	/// Test the Ethernet type-or-length field for an 802.1q type.
	bool is8021q() const { return getEtherType() == Eth_8021q; }
	
	/// Add the field header widths together to determine the entire length
	/// of the header.	
	static size_t calculateHeaderLength(const size_t preambleLen,
		const size_t typeLen = typeNormalLen) {
			return preambleLen + (Eth_AddrLen * 2) + typeLen;
	}
	
	/// Add the field header widths together to determine the entire length
	/// of the header.
	size_t calculateHeaderLength() const {
		return calculateHeaderLength(preambleLength_, getTypeLen());
	}
	
	/// Pointer to the start of the payload packet.
	ACE_UINT8* ptrPayload() const { return ptrEtherType() + getTypeLen(); }
	
	/// Copy in a new payload from a buffer.
	void setPayload(ACE_UINT8 *buffer, unsigned bufLen) {
		setData(buffer, bufLen);
	}
	
	/// Copy in a new payload from another NetworkData object.
	void setPayload(NetworkData* newPayload) {
		if (newPayload->getUnitLength() < Eth_MinPayload)
			throw ValueOutOfRange(ValueOutOfRange::msg("Ethernet payload length",
				newPayload->getUnitLength(), Eth_MinPayload, maxSize()));
	
		setData(newPayload);
		setEtherType(newPayload->enetType());
	}
	
	/// Subtract the header length from the entire frame length to get the
	/// size of the payload. Only works for fully assembled frames.
	unsigned calculatePayloadLength() const {
		return getUnitLength() - calculateHeaderLength();
	}
		
	/// The type field may be 2 bytes for a normal Ethernet frame or 6 bytes
	/// for an 802.1q frame.
	unsigned getTypeLen() const { return typeLen_; }
	
	/// Take a look into the payload to determine its length. Only works for
	/// recognized payload types.
	unsigned extractPayloadLength() const;
	
	/// Calculate how long the frame should be based on its header length
	/// plus the extracted payload length.
	size_t getExpectedLength() const { 
		return calculateHeaderLength() + extractPayloadLength();
	}
	
	// *** Important overloads ***
	ACE_UINT8* ptrData() const { return ptrPayload(); }
	
	size_t getDataLength() const { return calculatePayloadLength(); }
	
	size_t getExpectedDataLength() const { return extractPayloadLength(); }
	
	size_t getHeaderLength() const { return calculateHeaderLength(); }
	
	/// The largest size frame to ever expect.
	static ACE_UINT32 maxSize() { return 9500; }
	
	/// A string describing the type of data represented by this structure.
	/// @return Will always be "Ethernet".
	std::string typeStr() const { 
		switch (preambleLength_) {
			case LinuxTap: return "Tap Ethernet Frame";
			case IEEE: return "IEEE Ethernet Frame";
			default: return "PCap Ethernet Frame";
		}
	}
	
	/// Make sure the "tap" preamble is empty and the "tap" payload type agrees
	/// with the type-or-length field.
	/// @throw runtime_error When not enough bytes are present to make the determination.
	bool isExpectedType() const;

	/// Throw an exception if the payload type is unrecognized.
	void throwIfUnhandled() const {
		if ( ! ( getPayloadType() == Eth_IPv4 || getPayloadType() == Eth_ARP || 
			getPayloadType() == Eth_IPv6 ) ) 
			throw std::runtime_error("Unhandled packet type encountered.");
	}
	
	/// Compute the length of a frame with the provided payload length.
	static ACE_UINT32 estimate_length(const size_t payloadLen,
		const size_t preambleLen = PCap, const size_t typeLen = typeNormalLen) {
		return payloadLen + preambleLen + typeLen + (Eth_AddrLen * 2);
	}
	
	/// Calculate the MTU minus the header length.
	static ACE_UINT16 max_payload(const ACE_UINT16 mtu) {
		return mtu - estimate_length(0);
	}
		
	/// For frames that are not delimited by special markers that are coming
    /// in serially, return the minimum amount of data needed to extract the
    /// length from internal parameters. In this case the minimum size of
    /// an Ethernet frame is used.
    static ACE_UINT8 minBytesToDetermineLength() { return Eth_MinLen; }
    
    /// Operate on a small partial frame to determine the type.
    /// @param ptrSrc The front of the buffer to copy from.
	void setFront(const ACE_UINT8* ptrSrc) {
		replicateBuffer(ptrSrc, minBytesToDetermineLength());
	}
	
	/// Complete the partial frame.
	unsigned setBack(ACE_UINT8*, const unsigned);
	
	/// Override NetworkData::copyUnit to detect the Ethernet type field length.
	unsigned replicateBuffer(const ACE_UINT8* ptrSrc, const unsigned dataLen) {
		NetworkData::replicateBuffer(ptrSrc, dataLen);
		return getUnitLength();
	}
	
	/// Construct a new Ethernet frame.
	void build(MacAddress& dstMac, MacAddress& srcMac, NetworkData* newPayload = 0,
		const PayloadType newPayloadType = Eth_MaxType);
		
protected:
	/// The number of octets preceding the destination address.
	size_t preambleLength_;
	
	/// The number of octets in the Ethernet type length field.
	size_t typeLen_;	
	
	/// Write-only accessor to typeLen_.
	void setTypeLen_(const size_t newTypeLen) { typeLen_ = newTypeLen; }
	
	/// Make sure the preamble length is recognized.
	/// @throw PreambleLengthError If the requested length != set length.
	void testPreambleLength_(const int length = -1 ) const;
};

} // namespace nasaCE

#endif // NASAETHERNETFRAME_HPP
