/* -*- C++ -*- */

//=============================================================================
/**
 * @file   AOS_Multiplexing_PDU.hpp
 * @author Tad Kollar  
 *
 * $Id: AOS_Multiplexing_PDU.hpp 1670 2012-08-28 13:25:28Z tkollar $ 
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef NASA_AOS_MULTIPLEXING_PDU_HPP
#define NASA_AOS_MULTIPLEXING_PDU_HPP

#include "NetworkData.hpp"

namespace nasaCE {

//=============================================================================
/**
 * @class AOS_Multiplexing_PDU
 * @author Tad Kollar  
 * @brief Manipulates a buffer holding an AOS Multiplexing Protocol Data Unit.
 * @see [1] CCSDS 732.0-B-2, "AOS SPACE DATA LINK PROTOCOL", Blue Book 

<p><b>Structure of the AOS Multiplexing Protocol Data Unit</b></p>
<pre>
+---------------+------------------------------------------+
| M_PDU Header  | M_PDU Packet Zone                        |
+-------+-------+-----------+--------+---+------+----------+
|Rsvd. |First   | End of    | Packet |   |Packet| Start of |
|Spare |Header  | previous  | #k+1   |...| #m   | packet   |
|      |Pointer | packet #k |        |   |      | #m+1     |
+------+--------+-----------+--------+---+------+----------+
| 5    | 11     | Varies                                   |
+------+--------+------------------------------------------+
</pre>

 */
//=============================================================================
 
class AOS_Multiplexing_PDU: public NetworkData {

public:

	static const unsigned char spanHeader;
	static const ACE_UINT16 HasIdleVal;
	static const ACE_UINT16 HasHeaderlessVal;

	/// The primary constructor.
	AOS_Multiplexing_PDU():	NetworkData() { }
	
	/// Construct from a buffer.
	/// @param buffer A pointer to the start of the buffer.
	/// @param bufLen The amount of data to copy.
	AOS_Multiplexing_PDU(const size_t bufLen, const ACE_UINT8* buffer = 0):
		NetworkData(bufLen, buffer) { }
		
	/// Copy constructor.
	AOS_Multiplexing_PDU(const NetworkData* other, const bool doDeepCopy = true, 
    	const size_t offset = 0, const size_t newLen = 0): NetworkData(other, doDeepCopy, offset, newLen) { }
	
	~AOS_Multiplexing_PDU() {
		// AND_DEBUG((LM_DEBUG, "Deleting M_PDU.\n"));
	}
	
	// *** Unique functions ***
	
	/// Return a pointer to the start of the header.
	ACE_UINT8* ptrHeader() const { return ptrUnit(); }
	
	/// Get the value of the complete header as a 16-bit unsigned integer.
	ACE_UINT16 getHeader() const { return to_u16_(ptrHeader()); }
	
	/// The Reserved Spare field is currently undefined by CCSDS; by
	/// convention, it shall therefore be set to the reserved value of 00000.
	
	/// @return The value of the Reserved Spare field.
	ACE_UINT8 getRsvdSpare() const { return (*ptrHeader() & 0xF8) >> 3; }
	
	/// Set the value of the Reserved Spare field.
	/// @param newVal An new integer value from 0 to 31 (0x1F).
	void setRsvdSpare(const ACE_UINT8 newVal) { 
		*ptrHeader() = (newVal << 3) | (*ptrHeader() & 0x7);
	}
	
	/// Contains the index (starting at 0) within the Packet Zone of the first
	/// byte of the first packet header, if it exists.
	/// If no Packet starts in the M_PDU Packet Zone, the First Header Pointer shall be
	/// set to "all ones" (11111111111 == 0x7FF)
	/// If the M_PDU Packet Zone contains only Idle Data, the First Header Pointer
	/// shall be set to ‘all ones minus one’(11111111110 = 0x7FE)
	/// @return The offset of the start of the first header.
	ACE_UINT16 getFirstHeaderIndex() const { return getHeader() & 0x7FF; }
	
	/// Set the value of the First Header Pointer field.
	/// @param newVal The offset of the start of the first header within the Packet Zone.
	void setFirstHeaderIndex(const ACE_UINT16 newVal) {
		from_u16_((getRsvdSpare() << 11) | (newVal & 0x7FF), ptrHeader());
	}
	
	/// Return true if there is non-idle data in this Packet Zone but no headers.
	bool hasHeaderlessData() const { return ( getFirstHeaderIndex() == HasHeaderlessVal ); }
	
	/// Return true if a Packet starts somewhere in this Packet Zone.
	bool hasHeader() const { return ( getFirstHeaderIndex() < HasIdleVal ); }
	
	/// Return true if the Packet Zone begins with either the middle or tail of another packet.
	bool hasContinuedData() const { 
		return ( hasHeaderlessData() || (hasHeader() && getFirstHeaderIndex() > 0) );
	}
	
	/// Return true if there is nothing but idle data (all ones) in this Packet Zone.
	bool isIdle() const { return ( getFirstHeaderIndex() == HasIdleVal ); }
	
	/// Get a pointer to the first byte in the Packet Zone.
	ACE_UINT8* ptrPacketZone() const { return ptrHeader() + spanHeader; }
	
	/// Subtract the header size from the allocated buffer size.
	size_t getPacketZoneLength() const { return getUnitLength() - spanHeader; }
	
	/// Returns a pointer (to memory) of the first header in the Packet Zone.
	/// @throw MissingField If there is no first header.
	ACE_UINT8* ptrFirstHeader() const {
		if ( ! hasHeader() ) {
			throw MissingField(MissingField::msg("AOS M_PDU", "Packet Zone Header"));
		}
		return ptrPacketZone() + getFirstHeaderIndex();
	}
	
	// *** Important overloads ***
	
	ACE_UINT8 *ptrData() const { return ptrPacketZone(); }
	
	size_t getHeaderLength() const { return spanHeader; }
	
	size_t getDataLength() const { return getPacketZoneLength(); }

	/// A short string describing the data type (usually just the name).
	std::string typeStr() const { return "AOS Multiplexing Protocol Data Unit"; }
	
	/// Fill out an M_PDU.
	/// @param firstHeaderIdx The starting location of the first header.
	/// @param data The data buffer to copy
	/// @param packetZoneSize The length to make the Packet Zone, if larger than data->size().
	/// @param fill The pattern to do the fill with.
	/// @throw BufferOverflow If packetZoneSize is less than data->size().
	void build(const ACE_UINT16 firstHeaderIdx, NetworkData* data = 0,
		const size_t packetZoneSize = 0, const NetworkData* fill = 0);
	
	/// Copy a value to every location in the Packet Zone past the specified point.
	/// @param fillIdx Where to start writing.
	/// @param fill The buffer to fill with.
	/// @return The number of bytes filled.
	size_t addFill(const size_t fillIdx, const NetworkData* fill);
	
	/// Determine if this M_PDU matches expected parameters.
	/// @param expectedSize The intended size of the unit.
	/// @throw WrongSize If getUnitLength() is different than expectedSize.
	void throwIfInvalid(const size_t expectedSize) {
		if ( expectedSize != getUnitLength() )
			throw WrongSize(WrongSize::msg(typeStr(), expectedSize, getUnitLength()));
	}
};

} // namespace nasaCE

#endif // NASA_AOS_MULTIPLEXING_PDU_HPP
