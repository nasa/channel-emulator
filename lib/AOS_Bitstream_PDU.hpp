/* -*- C++ -*- */

//=============================================================================
/**
 * @file   AOS_Bitstream_PDU.hpp
 * @author Tad Kollar  
 *
 * $Id: AOS_Bitstream_PDU.hpp 1670 2012-08-28 13:25:28Z tkollar $ 
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef NASA_AOS_BITSTREAM_PDU_HPP
#define NASA_AOS_BITSTREAM_PDU_HPP

#include "NetworkData.hpp"

namespace nasaCE {

//=============================================================================
/**
 * @class AOS_Bitstream_PDU
 * @author Tad Kollar  
 * @brief Implements the AOS Bitstream PDU.
 * @see [1] CCSDS 732.0-B-2, "AOS SPACE DATA LINK PROTOCOL", Blue Book
 
<p><b>AOS Bitstream PDU Structure</b><p>
<pre>
+----------------+------------------------------------------+
| B_PDU Header   | B_PDU Bitstream Data Zone                |
+-------+--------+                                          |
|Rsvd. |Bitstream|                                          |
|Spare |Data     |                                          |
|      |Pointer  |                                          |
+------+---------+------------------------------------------+
| 2    | 14      | Varies                                   |
+------+---------+------------------------------------------+
</pre>
*/
//=============================================================================

class AOS_Bitstream_PDU: public NetworkData {
public:
	static const ACE_UINT16 AllOnes;
	static const ACE_UINT16	AllOnesMinusOne;
	
	static const size_t spanHeader;

	AOS_Bitstream_PDU(): NetworkData() { }
	
	/// Construct from a buffer.
	/// @param buffer A pointer to the start of the buffer.
	/// @param bufLen The amount of data to copy.
	AOS_Bitstream_PDU(const size_t bufLen, const ACE_UINT8* buffer = 0):
		NetworkData(bufLen, buffer) { }	
	
	/// Copy constructor.
	AOS_Bitstream_PDU(const NetworkData* other, const bool doDeepCopy = true, 
    	const size_t offset = 0, const size_t newLen = 0): NetworkData(other, doDeepCopy, offset, newLen) { }
	
	ACE_UINT8* ptrHeader() const { return ptrUnit(); }
	ACE_UINT16 getHeader() const { return to_u16_(ptrHeader()); }
	
	/// The Reserved Spare field is currently undefined by CCSDS; by
	/// convention, it shall therefore be set to the reserved value of 00.
	
	/// Get the value of the Reserved Spare field.
	ACE_UINT8 getRsvdSpare() const { return (*ptrHeader() & 0xC0) >> 6; }

	/// Set the value of the Reserved Spare field.
	/// @param newVal An integer value from 0 to 3.
	void setRsvdSpare(const ACE_UINT8 newVal) { 
		*ptrHeader() = (newVal << 6) | (*ptrHeader() & 0x3F);
	}
	
	/// Return the index of the last valid user data bit in the Data Zone.
	ACE_UINT16 getBitstreamDataEnd() const { return getHeader() & AllOnes; }
	
	/// Store the index of the last valid user data bit in the Data Zone.
	/// @param newVal The index value.
	void setBitstreamDataEnd(const ACE_UINT16 newVal) {
		from_u16_((getRsvdSpare() << 14) | (newVal & AllOnes), ptrHeader());
	}
	
	/// Return true if there is no idle data in the Data Zone.
	bool isAllData() const { return ( getBitstreamDataEnd() == AllOnes ); }
	
	/// Mark the B_PDU as containing 100% user data.
	void setAllData() { setBitstreamDataEnd(AllOnes); }
	
	/// Return true if there is no user data in the Data Zone.
	bool isAllIdle() const { return ( getBitstreamDataEnd() == AllOnesMinusOne ); }
	
	/// Mark the B_PDU as containing 100% idle data.
	void setAllIdle() { setBitstreamDataEnd(AllOnesMinusOne); }
	
	/// Get a pointer to the start of the Data Zone.
	ACE_UINT8* ptrDataZone() const { return ptrHeader() + spanHeader; }
	
	/// Determine the length of the Data Zone.
	size_t getDataZoneLength() const { return getUnitLength() - spanHeader; }

	// *** Important overloads ***
	
	ACE_UINT8 *ptrData() const { return ptrDataZone(); }
	
	size_t getHeaderLength() const { return spanHeader; }
	
	size_t getDataLength() const { return getDataZoneLength(); }
		
	/// A short string describing the data type (usually just the name).
	std::string typeStr() const { return "AOS BitStream Protocol Data Unit"; }

	/// Copy a value to every location in the Data Zone past the specified point.
	/// @param fillIdx Where to start writing.
	/// @param fillVal The 8-bit value to copy.
	/// @return The number of bytes filled.
	size_t addFill(const size_t fillIdx, const ACE_UINT8 fillVal = 0xE0);
};

}

#endif // NASA_AOS_BITSTREAM_PDU_HPP
