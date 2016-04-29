/* -*- C++ -*- */

//=============================================================================
/**
 * @file   SpacePacket.hpp
 * @author Tad Kollar  
 *
 * $Id: SpacePacket.hpp 1670 2012-08-28 13:25:28Z tkollar $
 * Copyright (c) 2012.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _NASA_SPACE_PACKET_HPP_
#define _NASA_SPACE_PACKET_HPP_

#include "NetworkData.hpp"
#include <sstream>

namespace nasaCE {

//=============================================================================
/**
 * @class SpacePacket
 * @author Tad Kollar  
 * @brief Implementation of the CCSDS Space Packet.
 * @see [1] CSSDS 133.0-B-1, "Space Packet Protcol", Blue Book
 * @see [2] CCSDS 135.0-B-4, "SPACE LINK IDENTIFIERS", Blue Book

<p><i>From [1], page 1-1:</i></p>
<p><b>1.1 PURPOSE</b></p>
<p>The purpose of this Recommendation is to specify the Space Packet Protocol. Space
missions will use this protocol to transfer space application data over a network that involves
a ground-to-space or space-to-space communications link.
</p>
<p><i>From [1], page 4-2:</i></p>
@htmlonly
<style type='text/css'>
<!--
th { font-weight: normal; }
bits { font-size: 8pt; }
-->
</style>
@endhtmlonly
<table border='1'>
<tr>
<th rowspan='2'>Packet Version Number<br><span class='bits'>3 bits</span></th>
<th colspan='3'>Packet Identification</th>
<th colspan='2'>Packet Sequence Control</th>
<th rowspan='2'>Packet Data Length<br><span class='bits'>2 octets</span></th>
</tr>
<tr>
<th>Packet Type<br><span class='bits'>1 bit</span></th>
<th>Sec. Hdr. Flag<br><span class='bits'>1 bit</span></th>
<th>Application Process Identifier<br><span class='bits'>11 bits</span></th>
<th>Sequence Flags<br><span class='bits'>2 bits</span></th>
<th>Packet Sequence Count / Packet Name<br><span class='bits'>14 bits</span></th>
</tr>
<tr>
<th class='bits' colspan='4'>2 octets</th>
<th class='bits' colspan='2'>2 octets</th>
<th class='bits'>2 octets</th>
</tr>
</table>
<p><b>Figure 4-2: Packet Primary Header</b></p>
*/
//=============================================================================
class SpacePacket: public NetworkData {
public:
	static const ACE_UINT16 idleAPIDVal;
	static const size_t maxAPID;
	static const size_t maxSeqFlags;
	static const size_t maxPktSeqCount;

	/// @brief The number of bytes required to extract the expected length from the payload.
	static ACE_UINT8 minBytesToDetermineLength(ACE_UINT8 firstOctet = 0);

	/// @brief Default constructor.
	SpacePacket();

	/// @brief Initializing constructor.
	/// @param bufLen The size to initialize the buffer to, as well as the size of frameBuf if it's non-zero.
	/// @param frameBuf Pointer to bufLen octets to copy to the new buffer.
	/// @param secHdrLen The length the Packet Secondary Header.
	/// @param timeCodeLen The length of the Time Code Field in the Packet Secondary Header.
	/// @param ancDataLen The length of the Ancillary Data Field in the Packet Secondary Header.
	SpacePacket(const ACE_UINT32& bufLen, const ACE_UINT8* frameBuf = 0,
		const size_t& secHdrLen = 0, const size_t& timeCodeLen = 0, const size_t& ancDataLen = 0);

	/// @brief Copy constructor from another SpacePacket only.
	explicit SpacePacket(const SpacePacket* other, const bool doDeepCopy = true,
			const size_t& offset = 0, const size_t& newLen = 0);

	/// @brief Copy constructor from any NetworkData object.
	SpacePacket(const NetworkData* other, const bool doDeepCopy = true,
			const size_t offset = 0, const size_t newLen = 0);

	/// @brief Destructor.
	virtual ~SpacePacket();

	/// @brief A pointer to the Packet Version Number field at the start of the packet.
	ACE_UINT8* ptrPacketVersionNumber() const;

	/// @brief The 3-bit version number field value, which should always be 000.
	ACE_UINT8 getPacketVersionNumber() const;

	/// @brief Set the Packet Version number to binary '000'.
	void setPacketVersionNumber();

	/// @brief Retrieve the Packet Type.
	/// @return If true, the packet is a telecommand (TC) Packet. If false, it's a telemetry (TM) Packet.
	bool getPacketType() const;

	/// @brief Change the Packet Type.
	/// @param newType If true, the packet is a telecommand (TC) Packet. If false, it's a telemetry (TM) Packet.
	void setPacketType(const bool newType);

	/// @brief Return true if this is a telecommand packet.
	bool isTC();

	/// @brief Return true if this is a telemetry packet.
	bool isTM();

	/// @brief Retrieve the Secondary Header Flag.
	/// @return If true, the Packet Secondary Header is present.
	bool getSecHdrFlag() const;

	/// @brief Change the Secondary Header Flag.
	/// @param newVal If true, the Packet Secondary Header is present.
	void setSecHdrFlag(const bool newFlag);

	/// @brief Retrieve the Application Process Identifier.
	ACE_UINT16 getAPID() const;

	/// @brief Change the Application Process Identifier.
	/// @param newVal An integer between 0 and 2047.
	/// @throw ValueTooLarge If newVal is greater than maxAPID.
	void setAPID(const ACE_UINT16& newVal);

	/// @brief True if the APID is all ones, i.e. 11111111111 (0x7ff).
	bool isIdle() const;

	/// @brief Designate this as an Idle Packet by setting the APID to all ones, i.e. 11111111111 (0x7ff).
	/// @param fillPattern If non-zero, the pattern to copy throughout the User Data Field.
	void makeIdle(const NetworkData* fillPattern = 0);

	/// @brief Get a pointer to the start of the Packet Sequence Control section.
	ACE_UINT8* ptrPktSeqCtrl() const;

	/// @brief Return the value (0-3) of the Sequence Flags.
	ACE_UINT8 getSeqFlags() const;

	/// @brief Change the value of the Sequence Flags.
	/// @param newFlags A new integer value in the range 0-3.
	/// @throw ValueTooLarge If newFlags is greater than 3 (maxSeqFlags).
	void setSeqFlags(const ACE_UINT8& newFlags);

	/// @brief Return true if the Space Packet contains a continuation segment of User Data.
	bool getContainsContinuation() const;

	/// @brief Indicate that the Space Packet contains a continuation segment of User Data.
	void setContainsContinuation();

	/// @brief Return true if the Space Packet contains the first segment of User Data.
	bool getContainsFirstSegment() const;

	/// @brief Indicate that the Space Packet contains the first segment of User Data.
	void setContainsFirstSegment();

	/// @brief Return true if the Space Packet contains the last segment of User Data.
	bool getContainsLastSegment() const;

	/// @brief Indicate that the Space Packet contains the last segment of User Data.
	void setContainsLastSegment();

	/// @brief Return true if the Space Packet contains unsegmented User Data.
	bool getContainsUnsegmented() const;

	/// @brief Indicate that the Space Packet contains unsegmented User Data.
	void setContainsUnsegmented();

	/// @brief Retrieve the current Packet Sequence Count or Packet Name.
	ACE_UINT16 getPktSeqCount() const;

	/// @brief Change the value of the Packet Sequence Count or Packet Name.
	/// @param newVal An integer value between 0 and 0x3fff (maxPktSeqCount).
	/// @throw ValueTooLarge If newVal is greater than maxPktSeqCount.
	void setPktSeqCount(const ACE_UINT16& newVal);

	/// @brief Increate the Packet Sequence Count by 1, resetting to 0 if maxPktSeqCount is reached.
	/// @return True if the count was reset to 0, false otherwise.
	bool incPktSeqCount();

	/// @brief Get a pointer to the start of the Packet Data Length field.
	ACE_UINT8* ptrPktDataLen() const;

	/// @brief Return the value of the Packet Data Length field (one less than the number of octets in the Data Field).
	ACE_UINT16 getPktDataLen() const;

	/// @brief Set the value of the Packet Data Length field (one less than the number of octets in the Data Field).
	void setPktDataLen(const ACE_UINT16& newVal);

	/// @brief Get a pointer to the start of the Packet Data Field.
	ACE_UINT8* ptrPktDataField() const;

	/// @brief Read-only accessor to _secHdrLen.
	size_t getSecHdrLen() const;

	/// @brief Write-only accessor to _secHdrLen.
	/// @throw MissingField If the Secondary Header Flag is unset.
	void setSecHdrLen(const size_t& newLen);

	/// @brief Get a pointer to the start of the Packet Secondary Header.
	/// @throw MissingField If the Secondary Header Flag is unset.
	ACE_UINT8* ptrSecHdr() const;

	/// @brief Read-only accessor to _timeCodeLen.
	size_t getTimeCodeLen() const;

	/// @brief Write-only accessor to _timeCodeLen.
	void setTimeCodeLen(const size_t& newLen);

	/// @brief Get a pointer to the Time Code Field.
	ACE_UINT8* ptrTimeCode() const;

	/// @brief Return the contents of the Time Code Field as a NetworkData wrapper.
	NetworkData* getTimeCode();

	/// @brief Copy the supplied buffer into the Time Code Field.
	/// @param timeCodeBuf Pointer to the buffer to copy.
	/// @param timeCodeBufLen Length of the buffer to copy. If 0, use _timeCodeLen.
	void setTimeCode(const ACE_UINT8* timeCodeBuf, const size_t timeCodeBufLen = 0);

	/// @brief Read-only accessor to _ancDataLen.
	size_t getAncDataLen() const;

	/// @brief Write-only accessor to _ancDataLen.
	void setAncDataLen(const size_t& newLen);

	/// @brief Get a pointer to the Ancillary Data Field.
	ACE_UINT8* ptrAncData() const;

	/// @brief Return the contents of the Ancillary Data Field as a NetworkData wrapper.
	NetworkData* getAncData();

	/// @brief Copy the supplied buffer into the Ancillary Data Field.
	/// @param ancDataBuf Pointer to the buffer to copy.
	/// @param ancDataBufLen Length of the buffer to copy. If 0, use _ancDataLen.
	void setAncData(const ACE_UINT8* ancDataBuf, const size_t ancDataBufLen = 0);

	/// @brief Get a pointer to the User Data Field.
	ACE_UINT8* ptrData() const;

	/// @brief Return the true size of the User Data Field.
	size_t getDataLength() const;

	/// Extract the intended length of the packet from header fields. The entire buffer may not yet be allocated.
	size_t expectedLength() const;

private:

	/// The size of the Packet Secondary Header. This is not stored in the Packet.
	size_t _secHdrLen;

	/// The size of the Time Code Field in the Packet Secondary Header.
	size_t _timeCodeLen;

	/// The size ofthe Ancillary Data Field in the Packet Secondary Header.
	size_t _ancDataLen;

};


} // namespace nasaCE

#endif // _NASA_SPACE_PACKET_HPP_
