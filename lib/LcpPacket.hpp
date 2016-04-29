/* -*- C++ -*- */

//=============================================================================
/**
 * @file   LcpPacket.hpp
 * @author Tad Kollar  
 *
 * $Id: LcpPacket.hpp 1670 2012-08-28 13:25:28Z tkollar $ 
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef NASALCPPACKET_HPP
#define NASALCPPACKET_HPP

#include "PppConfPacket.hpp"

namespace nasaCE {

/* LCP Packet structure:
    0                   1                   2                   3
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |     Code      |  Identifier   |            Length             |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   | Data ...
   +-+-+-+-+
*/

/**
 * @class LcpPacket
 *
 * @brief Holds and operates on a single PPP Link Control Protocol (LCP) packet.
 */
class LcpPacket: public PppConfPacket {

public:
	enum OptionTypes {
		Vendor      		= 0,	/*!< Vendor Specific */
		MRU         		= 1,	/*!< Maximum Receive Unit */
		AsyncMap    		= 2,	/*!< Async Control Character Map */
		AuthType    		= 3,	/*!< Authentication Type */
		Quality     		= 4,	/*!< Quality Protocol */
		MagicNumber 		= 5,	/*!< Magic Number */
		Undef6				= 6,
		PCompression		= 7,	/*!< Protocol Field Compression */
		ACCompression		= 8,	/*!< Address/Control Field Compression */
		FCSAltern   		= 9,	/*!< FCS-Alternatives */
		SDP         		= 10,	/*!< Self-Describing-Pad */
		Numbered    		= 11,	/*!< Numbered-Mode */
		Undef12				= 12,
		Callback    		= 13,	/*!< callback */
		Undef14				= 14,
		Undef15				= 15,		
		Undef16				= 16,
		MRRU        		= 17,	/*!< max reconstructed receive unit; multilink */
		SSNHF       		= 18,	/*!< short sequence numbers for multilink */
		EPDisc      		= 19,	/*!< endpoint discriminator */
		Undef20				= 20,
		Undef21				= 21,
		MPPlus      		= 22,	/*!< Multi-Link-Plus-Procedure */
		LDisc       		= 23,	/*!< Link-Discriminator */
		LCPAuth     		= 24,	/*!< LCP Authentication */
		COBS        		= 25,	/*!< Consistent Overhead Byte Stuffing */
		PrefElis    		= 26,	/*!< Prefix Elision */
		MPHdrFmt    		= 27,	/*!< MP Header Format */
		I18n        		= 28,	/*!< Internationalization */
		SDL          		= 29,	/*!< Simple Data Link */
		OptionTypesCount	= 30
	};
	
	static std::string optTypeStr[OptionTypesCount];
	
	/// Default constructor.
	LcpPacket(): PppConfPacket() { }
	  
	/// Initialize the packet with buffer contents.
	/// @param buf The buffer holding the data.
	/// @param bufLen Size of the buffer.
	LcpPacket(ACE_UINT8* buf, unsigned bufLen) { replicateBuffer(buf, bufLen); }
	  
	/// Copy constructor.
	/// @param other Pointer to the LcpPacket to copy.
	LcpPacket(const LcpPacket* other): PppConfPacket(other) { }
   
	/// String representation of the contained data type.
	std::string typeStr() const { return "LCP Packet"; }
}; 
   
} // namespace nasaCE

#endif // NASALCPPACKET_HPP