/* -*- C++ -*- */

//=============================================================================
/**
 * @file   PppFrame.hpp
 * @author Tad Kollar  
 *
 * $Id: PppFrame.hpp 1670 2012-08-28 13:25:28Z tkollar $ 
 * Copyright (c) 2008.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef NASAPPPFRAME_HPP
#define NASAPPPFRAME_HPP

#include <HdlcFrame.hpp>
#include <bitset>

/*

+------+---------+---------+----------+--------------------+-----+------+
| 1    | 1       | 1       | 2        | Variable           | 2/4 | 1    |
+------+---------+---------+----------+--------------------+-----+------+
| Flag | Address | Control | Protocol | Data               | FCS | Flag |
+------+---------+---------+----------+--------------------+-----+------+

Flag: A single byte that indicates the beginning or end of a frame.
      The flag field consists of the binary sequence 01111110 (7E).

Address: A single byte that contains the binary sequence 11111111 (FF),
      the standard broadcast address. PPP does not assign individual
      station addresses as HDLC does.

Control: A single byte that contains the binary sequence 00000011 (03),
      which calls for transmission of user data in an unsequenced
      frame. A connectionless link service similar to that of Logical
      Link Control (LLC) Type 1 is provided.

Protocol: Two bytes that identify the protocol encapsulated in the
      information field of the frame. The most up-to-date values of
      the protocol field are specified in the most recent Assigned
      Numbers Request For Comments (RFC).

Data: Zero or more bytes that contain the datagram for the protocol
      specified in the protocol field. The end of the information
      field is found by locating the closing flag sequence and
      allowing 2 bytes for the FCS field. The default maximum length
      of the information field is 1,500 bytes. By prior agreement,
      consenting PPP implementations can use other values for the
      maximum information field length.

Frame check sequence (FCS): Normally 16 bits (2 bytes). By prior
      agreement, consenting PPP implementations can use a 32-bit
      (4-byte) FCS for improved error detection.
*/
namespace nasaCE {

/**
	@author Tad Kollar  
*/
class PppFrame : public HdlcFrame {
public:
	enum { headerByteLen = 5 }; /*!< The length of the non-variable fields in the header. */
	enum { escSeq = 0x7D, flagSeq = 0x7E };
	
	/// Default constructor.
    PppFrame(): HdlcFrame(1, 1, 2) { }

	/// @return The header length + trailer length (data size omitted).
	unsigned nonDataLength() { return PppFrame::headerByteLen + getFCSLength() + 1; }
	
	/// Pointer to the protocol field.
	ACE_UINT8* protocolPtr() { return HdlcFrame::dataPtr(); }
	/// @return The two byte value that identifies the protocol of the data/payload.
	ACE_UINT16 protocolVal() { return to_u16_(protocolPtr()); }
	/// Change the value of the protocol field.
	void setProtocolVal(const ACE_UINT16 newVal) { from_u16_(newVal, protocolPtr()); }
	
	/// Pointer to the encapsulated data.
	ACE_UINT8* dataPtr() { return protocolPtr() + 2; }
	
	/// Set field values that are always the same for a PPP frame.
	void setStaticVals() {
		*startFlagPtr() = 0x7E;
		*addressPtr() = 0xFF;
		*controlPtr() = 0x03;
		*endFlagPtr() = 0x7E;
	}
	
	/// Determines whether the frame is an LCP or not.
	bool isLCP() { return (protocolVal() == 0xC021); }
	
	/// Determines whether the frame is an IPCP or not.
	bool isIPCP() { return (protocolVal() == 0x8021); }
	
	/// Determiens whether the frame contains an IPv4 packet.
	bool isIPv4() { return (protocolVal() == 0x0021); }
	
	/// Preface all octet values under 0x20 with 0x7D and add 0x20 to them.
	/// @p accm A reference to the Async-Control-Character-Map to use when escaping.
	void escape(std::bitset<256>&);
	
	/// Undo the effects of escape().
	void unescape();
	
	/// Construct a new PPP frame to send out.
	/// @p protocol The type of the payload.
	/// @p data The payload to integrate into the packet.
	/// @p fcsLen The length of the FCS field, either 2 or 4.
	void build(const ACE_UINT16, NetworkData*, const ACE_UINT8 fcsLen = 2);
	
	/// String representation of the contained data type.
	std::string typeStr() const { return "PPP"; }	
	
}; // class PppFrame

} // namespace nasaCE

#endif // NASAPPPFRAME_HPP
