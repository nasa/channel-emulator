/* -*- C++ -*- */

//=============================================================================
/**
 * @file   EncapsulationPacket.hpp
 * @author Tad Kollar  
 *
 * $Id: EncapsulationPacket.hpp 2311 2013-07-30 15:47:37Z tkollar $
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef NASA_ENCAPSULATION_PACKET_HPP
#define NASA_ENCAPSULATION_PACKET_HPP

#include "NetworkData.hpp"
#include <sstream>

namespace nasaCE {

//=============================================================================
/**
 * @class EncapsulationPacketBase
 * @author Tad Kollar  
 * @brief Implementation of the Encapsulation Packet.
 * @see [1] CSSDS 133.1-B-2, "Encapsulation Service", Blue Book
 * @see [2] CCSDS 135.0-B-4, "SPACE LINK IDENTIFIERS", Blue Book

<p><i>From [1], page 1-1:</i></p>
<p><b>1.1 PURPOSE</b></p>
<p>The purpose of this Recommended Standard is to specify the Encapsulation Service. This
service is to be used by space missions to transfer data units that are not directly transferred
by the Space Data Link Protocols over a ground-to-space or space-to-
space communications link.</p>
<p><i>From [1], page 4-3:</i></p>
@htmlonly
<style type='text/css'>
<!--
bits { font-size: 8pt; }
-->
</style>
@endhtmlonly
<table border='1'>
<tr>
<th>Packet Version Number<br><span class='bits'>3 bits</span></th>
<th>Protocol ID<br><span class='bits'>3 bits</span></th>
<th>Length of Length<br><span class='bits'>2 bits</span></th>
<th>User Defined Field<br><span class='bits'>0 or 4 bits</span></th>
<th>Protocol ID Extension<br><span class='bits'>0 or 4 bits</span></th>
<th>CSSDS Defined Field<br><span class='bits'>0 or 2 octets</span></th>
<th>Packet Length<br><span class='bits'>0 to 4 octets</span></th>
</tr>
<tr><td>'111'</td><td>'XXX'</td><td>'00'</td><td>0 bits</td><td>0 bits</td><td>0 octets</td><td>0 octets</td></tr>
<tr><td>'111'</td><td>'XXX'</td><td>'01'</td><td>0 bits</td><td>0 bits</td><td>0 octets</td><td>1 octets</td></tr>
<tr><td>'111'</td><td>'XXX'</td><td>'10'</td><td>4 bits</td><td>4 bits</td><td>0 octets</td><td>2 octets</td></tr>
<tr><td>'111'</td><td>'XXX'</td><td>'11'</td><td>4 bits</td><td>4 bits</td><td>2 octets</td><td>4 octets</td></tr>
</table>
<p><b>Figure 4-2: Packet Header</b></p>
*/
//=============================================================================
class EncapsulationPacketBase: public NetworkData {
public:
	static const ACE_UINT8 maxProtocolID;
	static const ACE_UINT8 maxLengthOfLength;
	static const ACE_UINT8 maxUserDefinedField;
	static const ACE_UINT8 maxProtocolIDExtension;

	/// The number of bytes required to extract the expected length
	/// from the encapsulated payload.
	static ACE_UINT8 minBytesToDetermineLength(ACE_UINT8 firstOctet = 0) {
		return getHeaderOctets(firstOctet);
	}

	/// Default constructor.
	EncapsulationPacketBase(): NetworkData() { }

	/// Initializing constructor.
	EncapsulationPacketBase(const ACE_UINT32 bufLen, const ACE_UINT8* frameBuf = 0):
		NetworkData(bufLen, frameBuf) { }

	/// Copy constructor.
	EncapsulationPacketBase(const NetworkData* other, const bool doDeepCopy = true,
    	const size_t offset = 0, const size_t newLen = 0): NetworkData(other, doDeepCopy, offset, newLen) { }

	/// Destructor.
	virtual ~EncapsulationPacketBase() { }

	/// A pointer to the Packet Version Number field at the start of the packet.
	ACE_UINT8* ptrPacketVersionNumber() const { return ptrUnit(); }

	/// The 3-bit version number field value, which should always be binary 111, or decimal/hex 7.
	ACE_UINT8 getPacketVersionNumber() const { return *ptrPacketVersionNumber() >> 5; }

	/// Set the Packet Version number to binary '111'.
	void setPacketVersionNumber() { *ptrPacketVersionNumber() |= 0xE0; }

	/// The 3-bit protocol ID field value.
	ACE_UINT8 getProtocolID() const { return (*ptrPacketVersionNumber() & 0x1C) >> 2; }

	/// Set the Protocol Identifier field value.
	/// @throw ValueTooLarge If newProtID is greater than maxProtocolID.
	void setProtocolID(const ACE_UINT8 newProtID) {
		if ( newProtID <= maxProtocolID) setFrontOctet_(newProtID, getLengthOfLength());
		else throw ValueTooLarge(ValueTooLarge::msg("Protocol ID", newProtID, maxProtocolID));
	}

	/// Determine if this is a one-byte packet.
	bool isFill() const { return getProtocolID() == 0; }

	/// Determine if the provided byte constitutes a one-byte packet.
	static bool isFill(const ACE_UINT8 firstOctet) { return firstOctet == 0xE0; }

	/// Get the value of the Length of Length field.
	ACE_UINT8 getLengthOfLength() const { return *ptrUnit() & 0x3; }

	/// Set the value of the Length of Length field.
	/// @throw ValueTooLarge If newLoL is greater than maxLengthOfLength.
	void setLengthOfLength(const ACE_UINT8 newLoL) {
		if ( newLoL <= maxLengthOfLength ) setFrontOctet_(getProtocolID(), newLoL);
		else throw ValueTooLarge(ValueTooLarge::msg("Length of Length", newLoL, maxLengthOfLength));
	}

	/// Determine if the User Defined Field is intended to be used.
	bool hasUserDefinedField() const { return ( getLengthOfLength() > 1 )? true : false; }

	/// Get a pointer to the User Defined Field, if it exists.
	/// @throw MissingField If the field is unused.
	ACE_UINT8* ptrUserDefinedField() const {
		if ( hasUserDefinedField() ) return ptrPacketVersionNumber() + 1;
		else throw MissingField(MissingField::msg("Encapsulation Packet", "User Defined Field"));
	}

	/// Get the value of the User Defined Field.
	ACE_UINT8 getUserDefinedField() const { return *ptrUserDefinedField() >> 4;	}

	/// Set the value of the User Defined Field.
	/// @throw ValueTooLarge If newUDF is greater than maxUserDefinedField.
	void setUserDefinedField(const ACE_UINT8 newUDF) {
		if ( newUDF <= maxUserDefinedField )
			*ptrUserDefinedField() =  (newUDF << 4) | getProtocolIDExtension();
		else throw ValueTooLarge(ValueTooLarge::msg("User Defined", newUDF, maxUserDefinedField));
	}

	/// Determine if the Protocol ID Extension Field is intended to be used.
	bool hasProtocolIDExtension() const { return hasUserDefinedField(); }

	/// Get the value of the Protocol ID Extension field.
	/// @throw MissingField If the field is unused.
	ACE_UINT8 getProtocolIDExtension() const {
		// Do this check here instead of falling through to ptrUserDefinedField()
		// so that we can specify that it's this field.
		if ( hasProtocolIDExtension() ) return *ptrUserDefinedField() & 0xF;
		else throw MissingField(MissingField::msg("Encapsulation Packet", "Protocol ID Extension"));
	}

	/// Set the value of the Protocol ID Extension field.
	/// @throw MissingField If the field is unused.
	/// @throw ValueTooLarge If newProtIDExt is greater than maxProtocolIDExtension.
	void setProtocolIDExtension(const ACE_UINT8 newProtIDExt) {
		// Do this check here instead of falling through to ptrUserDefinedField()
		// so that we can specify that it's this field.
		if ( hasProtocolIDExtension() ) {
			if ( newProtIDExt <= maxProtocolIDExtension )
				*ptrUserDefinedField() = (getUserDefinedField() << 4) | newProtIDExt;
			else throw ValueTooLarge(ValueTooLarge::msg("Protocol ID Extension", newProtIDExt,
				maxProtocolIDExtension));
		}
		else throw MissingField(MissingField::msg("Encapsulation Packet", "Protocol ID Extension"));
	}

	/// Determine if the CSSDS Defined Field is intended to be used.
	bool hasCssdsDefinedField() const { return ( getLengthOfLength() == 3 )? true : false; }

	/// Get a pointer to the CSSDS Defined Field, if it exists.
	/// @throw MissingField If the field is unused.
	ACE_UINT8* ptrCssdsDefinedField() const {
		if ( hasCssdsDefinedField() ) return ptrUserDefinedField() + 1;
		else throw MissingField(MissingField::msg("Encapsulation Packet", "CSSDS Defined"));
	}

	/// Get the value of the CSSDS Defined Field.
	ACE_UINT16 getCcsdsDefinedField() const { return to_u16_(ptrCssdsDefinedField()); }

	/// Set the value of the CSSDS Defined Field.
	void setCssdsDefinedField(const ACE_UINT16 newCDF) {
		from_u16_(newCDF, ptrCssdsDefinedField());
	}

	/// Determine if the Packet Length field is intended for use.
	bool hasPacketLength() const { return ( getLengthOfLength() > 0 )? true : false; }

	/// Get a pointer to the Packet Length field.
	/// @throw MissingField If the field is unused.
	/// @throw LogicError If a bad Length of Length is encountered (should never happen).
	ACE_UINT8* ptrPacketLength() const;

	/// Get the value of the Packet Length field.
	/// @throw MissingField If the field is unused.
	/// @throw LogicError If a bad Length of Length is encountered (should never happen).
	ACE_UINT32 getPacketLength() const;

	/// Set the value of the Packet Length field.
	/// @throw MissingField If the field is unused.
	/// @throw ValueTooLarge If the given value is too large for the field according to Length of Length.
	/// @throw LogicError If a bad Length of Length is encountered (should never happen).
	void setPacketLength(const ACE_UINT32 newPacketLen);

	/// Calculate the size of the packet header based on the provided Length of Length.
	/// @throw LogicError If a bad Length of Length is encountered.
	static unsigned char getHeaderOctets(const ACE_UINT8 lengthOfLength);

	/// Calculate the size of the packet header based on the contained Length of Length.
	unsigned char getHeaderOctets() const {
		return getHeaderOctets(getLengthOfLength());
	}

	/// Extract the intended length of the packet from header fields. The entire buffer may not yet be allocated.
	size_t expectedLength() const { return getPacketLength(); }

	/// Determine if the packet has an Internet Protocol Extension shim.
	virtual bool hasIPE() const = 0;

	// *** Overloaded NetworkData data field functions ***

	/// Determine if the packet is intended to have a data field.
	bool hasData() const { return getLengthOfLength()? true : false; }

	/// Determine if this Encapsulation Packet is valid.
	/// @throw WrongSize If the length field value doesn't match the packet length.
	void throwIfInvalid() const {
		if ( getPacketLength() != getUnitLength() )
			throw WrongSize(WrongSize::msg(typeStr(), getPacketLength(), getUnitLength()));
	}

protected:
 	/// Set the value of the first octet in the frame.
 	/// @param newVal The new unsigned 8-bit integer value to store.
 	void setFrontOctet_(const ACE_UINT8 newVal) { *ptrUnit() = newVal; }

 	/// Set the value of the first octet in the frame. Bits 0-2 are always '111'.
 	/// @param protocol_id Store these 3 bits as bits 3-5 in the octet.
 	/// @param length_of_length Store these 2 bits as bits 6-7 in the octet.
 	void setFrontOctet_(const ACE_UINT8 protocol_id, const ACE_UINT8 length_of_length) {
		setFrontOctet_( 0xE0 | ((protocol_id & 0x7) << 2) | (length_of_length & 0x3) );
 	}
};

class EncapsulationPacketWithIPE: public EncapsulationPacketBase {
public:
	static const ACE_UINT64 maxIntStorableInIPE;

	enum IPE_Header_Values {
		IPv4_Datagram = 33,
		Frame_Relay_IP_HCCP = 35,
		IPv6_Datagram = 87,
		Full_Header = 97,
		Compressed_TCP = 99,
		Compressed_TCP_No_Delta = 101,
		Compressed_Non_TCP = 103,
		Compressed_RTP_8 = 105,
		Compressed_RTP_16 = 107,
		Compressed_UDP_8 = 109,
		Compressed_UDP_16 = 111,
		Context_State = 113
	};

	/// @enum DefinedProtocolIdentifiers
	/// @brief Taken from [2], Page 7-5, Table 7-7a.
	enum DefinedProtocolIdentifiers {
		Fill = 0, /*!< The encapsulation data field, if present, contains no protocol data */
		Reserved001 = 1, /*!< Reserved by CCSDS */
		InternetProtocolExtenstion = 2, /*!< Protocol identification is contained in the Internet Protocol Extension (IPE) shim*/
		CFDP = 3, /*!< CCSDS File Delivery Protocol */
		Reserved100 = 4, /*!< Reserved by CCSDS */
		Reserved101 = 5, /*!< Reserved by CCSDS */
		ProtocolExtension = 6, /*!< Signals the use of the Extended Protocol ID for Encapsulation Service */
		ArbitraryAggregationsOfOctets = 7 /*!< */
	};

	/// Default constructor.
	EncapsulationPacketWithIPE(): EncapsulationPacketBase() { }

	/// Initializing constructor.
	EncapsulationPacketWithIPE(const ACE_UINT32 bufLen, const ACE_UINT8* frameBuf = 0):
		EncapsulationPacketBase(bufLen, frameBuf) { }

	/// Copy constructor.
	EncapsulationPacketWithIPE(const NetworkData* other, const bool doDeepCopy = true,
    	const size_t offset = 0, const size_t newLen = 0):
    	EncapsulationPacketBase(other, doDeepCopy, offset, newLen) { }

	/// Destructor.
	virtual ~EncapsulationPacketWithIPE() { }

	bool hasIPE() const { return getProtocolID() == InternetProtocolExtenstion; }

	/// Get a pointer to the start of the Internet Protocol Extension shim.
	/// @throw MissingField If the IPE field is unused.
	ACE_UINT8* ptrIPE() const {
		if (hasIPE()) return ptrUnit() + getHeaderOctets();
		throw MissingField(MissingField::msg("Encapsulation Packet", "Internet Protocol Extension"));
	}

	/// Determine the number of octets in the IPE field.
	/// @throw nd_error If the end of the IPE is not found before the end-of-packet is reached.
	unsigned short countIPEOctets() const;

	/// Determine how many octets will be required to hold the suggested IPE value.
	/// @param testIPE An IPE value of up to 8 octets in length.
	static unsigned short calcIPEOctets(ACE_UINT64 testIPE) {
		unsigned short count = 1;
		while ((testIPE >>= 8) ) { ++count; }
		return count;
	}

	/// Determine whether the supplied number will work as an IPE, i.e. the LSB
	/// of all except the least significant octet equals 0, while the least
	/// significant octet's LSB equals 1.
	/// @param testIPE The value to be tested for proper formatting.
	/// @return True if the number is properly formatted as an IPE.
	static bool properIPE(ACE_UINT64 testIPE) { return ((testIPE & 0x0101010101010101llu) == 1); }

	/// Take any number and make an acceptable IPE value out of it by
	/// storing it across bits 0-6 of each octet of up to 8 octets.
	/// When using published IPEs this is unnecessary, because those
	/// numbers have already been selected to fit the format.
	/// @param anyNum Any unsigned integer value up to 2^56 - 1
	/// @throw ValueTooLarge If anyNum is larger than maxIntStorableInIPE.
	static ACE_UINT64 convertToIPE(ACE_UINT64 anyNum);

	/// Extract an unsigned integer from the 7 MSBs of up to 8 octets.
	/// Mostly for testing and not necessary for recognition of published IPEs.
	/// @param testIPE A properly formatted Internet Protocol Extension value.
	/// @throw BadValue If the supplied value is not an IPE.
	static ACE_UINT64 unconvertIPE(ACE_UINT64 testIPE);

	/// Aggregate bits of potentially several octets to get the complete IPE value.
	/// @return Up to 64 bits of IPE due to the 8-byte unsigned integer used to store it.
	/// @throw MissingField If the IPE field is unused.
	/// @throw nd_error If the end of the IPE is not found before the end-of-packet is reached.
	ACE_UINT64 getIPE() const;

	/// Store up to a 64-bit IPE value, so limited because of the 8-byte unsigned int storing it.
	/// Trusts that the space for the IPE has been pre-allocated!
	/// @param newIPE Up to 64-bits of IPE to set.
	/// @throw MissingField If the IPE field is unused.
	/// @throw LogicError If we've screwed up programmatically and end up with a structure the wrong size.
	/// @throw BadValue If the provided number is not a properly-formated IPE.
	void setIPE(ACE_UINT64 newIPE);

	// *** Overloaded NetworkData data field functions ***
	/// Get a pointer to the Data Field.
	/// @throw MissingField If the field is unused.
	ACE_UINT8* ptrData() const {
		if (hasData()) return ptrUnit() + getHeaderOctets() + countIPEOctets();
		throw MissingField(MissingField::msg("Encapsulation Packet", "Data"));
	}

	/// Subtract the size of the header from the size of the buffer allocated for the entire packet.
	/// @return The true length of the Data Field, as opposed to expectedLength().
	size_t getDataLength() const { return getUnitLength() - getHeaderOctets() - countIPEOctets(); }

	/// Size the packet appropriately and set all the given parameters.
	void build(
		const ACE_UINT8 protID,
		const ACE_UINT8 newLoL,
		const ACE_UINT32 pktLen = 0,
		NetworkData* data = 0,
		const ACE_UINT64 ipe = 0,
		const ACE_UINT8 UDF = 0,
		const ACE_UINT8 protIDExt = 0,
		const ACE_UINT16 ccsdsDefinedField = 0
	);

	/// A short string describing the data type (usually just the name).
	std::string typeStr() const { return "Encapsulation Packet (IPE support)"; }
};

class EncapsulationPacketPreIPE: public EncapsulationPacketBase {
public:
	/// Default constructor.
	EncapsulationPacketPreIPE(): EncapsulationPacketBase() { }

	/// Initializing constructor.
	EncapsulationPacketPreIPE(const ACE_UINT32 bufLen, const ACE_UINT8* frameBuf = 0):
		EncapsulationPacketBase(bufLen, frameBuf) { }

	/// Copy constructor.
	EncapsulationPacketPreIPE(const NetworkData* other, const bool doDeepCopy = true,
    	const size_t offset = 0, const size_t newLen = 0):
    	EncapsulationPacketBase(other, doDeepCopy, offset, newLen) { }

	/// Destructor.
	virtual ~EncapsulationPacketPreIPE() { }

	/// @enum DefinedProtocolIdentifiers
	/// @brief Taken from [3], Page 7-5, Table 7-7a.
	enum DefinedProtocolIdentifiers {
		Fill = 0, /*!< The encapsulation data field, if present, contains no protocol data */
		Reserved001 = 1, /*!< Reserved by CCSDS */
		IPv4 = 2, /*!< Data field contains an IPv4 datagram */
		CFDP = 3, /*!< CCSDS File Delivery Protocol */
		IPv6 = 4, /*!< Data field contains an IPv6 datagram */
		Reserved101 = 5, /*!< Reserved by CCSDS */
		ProtocolExtension = 6, /*!< Signals the use of the Extended Protocol ID for Encapsulation Service */
		ArbitraryAggregationsOfOctets = 7 /*!< */
	};

	/// This type of Encap Packet does not support the Internet Protocol Extension.
	bool hasIPE() const { return false; }

	// *** Overloaded NetworkData data field functions ***
	/// Get a pointer to the Data Field.
	/// @throw MissingField If the field is unused.
	ACE_UINT8* ptrData() const {
		if (hasData()) return ptrUnit() + getHeaderOctets();
		throw MissingField(MissingField::msg("Encapsulation Packet", "Data"));
	}

	/// Subtract the size of the header from the size of the buffer allocated for the entire packet.
	/// @return The true length of the Data Field, as opposed to expectedLength().
	size_t getDataLength() const { return getUnitLength() - getHeaderOctets(); }

	/// Size the packet appropriately and set all the given parameters.
	void build(
		const ACE_UINT8 protID,
		const ACE_UINT8 newLoL,
		const ACE_UINT32 pktLen = 0,
		NetworkData* data = 0,
		const ACE_UINT8 UDF = 0,
		const ACE_UINT8 protIDExt = 0,
		const ACE_UINT16 ccsdsDefinedField = 0
	);

	/// A short string describing the data type (usually just the name).
	std::string typeStr() const { return "Encapsulation Packet (no IPE support)"; }
};

} // namespace nasaCE

#endif // NASA_ENCAPSULATION_PACKET_HPP
