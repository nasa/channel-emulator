/* -*- C++ -*- */

//=============================================================================
/**
 * @file   AOS_Transfer_Frame.hpp
 * @author Tad Kollar  
 *
 * $Id: AOS_Transfer_Frame.hpp 2007 2013-05-31 20:06:00Z tkollar $
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _NASA_AOS_TRANSFER_FRAME_HPP_
#define _NASA_AOS_TRANSFER_FRAME_HPP_

#include "NetworkData.hpp"
#include "TC_Comm_Link_Control_Word.hpp"
#include "EthernetFrame.hpp" // for EtherType
#include <sstream>

namespace nasaCE {

//=============================================================================
/**
 * @class AOS_Transfer_Frame
 * @author Tad Kollar  
 * @brief The basic container for the AOS Space Data Link Protocol.
 * @see [1] CCSDS 732.0-B-2, "AOS SPACE DATA LINK PROTOCOL", Blue Book
 * @see [2] CCSDS 232.0-B-1, "TC SPACE DATA LINK PROTOCOL", Blue Book
 *

<p><b>AOS TRANSFER FRAME STRUCTURAL COMPONENTS</b>
<table border="1">
<tr>
<th>Field</th>
<th>Transfer<br/>Frame<br/>Primary<br/>Header</th>
<th>Transfer<br/>Frame<br/>Header<br/>Error<br/>Control*</th>
<th>Insert<br/>Zone*</th>
<th>Transfer<br/>Frame<br/>Data<br/>Field</th>
<th>Operational<br/>Control<br/>Field*</th>
<th>Frame<br/>Error<br/>Control<br/>Field*</th>
</tr>
<tr>
<th>Octets</th>
<td>6</td>
<td>2</td>
<td>Varies</td>
<td>Varies</td>
<td>4</td>
<td>2</td>
</tr>
</table>
<i>* Optional</i>
</p>
<p>
<b>4.1.1.1</b> An AOS Transfer Frame shall encompass the major fields, positioned contiguously,
in the following sequence:</p>
<ol type='a'>
<li>Transfer Frame Primary Header (6 or 8 octets; mandatory);</li>
<li>Master Channel Insert Zone (integral number of octets; optional);</li>
<li>Transfer Frame Data Field (integral number of octets; mandatory);</li>
<li>Operational Control Field (4 octets; optional);</li>
<li>Frame Error Control Field (2 octets, optional).</li>
</ol>
<p><b>TRANSFER FRAME PRIMARY HEADER</b>
<table border="1">
<tr><th rowspan='2'>Field</th><th colspan='2'>MCID</th><th rowspan='2'>VCID</th><th rowspan='2'>VC Frame Count</th>
<th colspan='4'>Signaling Field</th><th rowspan='2'>Frame Header Error Control</th></tr>
<tr><th>Version Number</th><th>Spacecraft ID</th><th>Replay Flag</th><th>VC Frame Count Usage Flag</th>
<th>Rsvd. Spare</th><th>VC Frame Count Cycle</th></tr>
<tr><th>Bits</th><td>2</td><td>8</td><td>6</td><td>24</td><td>1</td><td>1</td><td>2</td><td>4</td><td>16</td></tr>
<tr><th>Octets</th><td colspan='3'>2</td><td>3</td><td colspan='4'>1</td><td>2</td></tr>
</table>
<i>* Optional</i>
</p>
<p><b>4.1.2.1 General</b></p>
<p>The Transfer Frame Primary Header is mandatory and shall consist of five fields, positioned
contiguously, in the following sequence:</p>
<ol type='a'>
<li>Master Channel Identifier (10 bits; mandatory);</li>
<li>Virtual Channel Identifier (6 bits; mandatory);</li>
<li>Virtual Channel Frame Count (3 octets; mandatory);</li>
<li>Signaling Field (1 octet; mandatory);</li>
<li>Frame Header Error Control (2 octets, optional).</li>
</ol>
 */
//=============================================================================

class AOS_Transfer_Frame: public NetworkData {

public:

	static const size_t spanGVCIDField;
	static const size_t spanSignalingField;
	static const size_t spanVCFrameCountField;
	static const size_t spanRequiredHeader;

	static const size_t spanFrameHeaderErrorControl;
	static const size_t spanOperationalControlField;
	static const size_t spanFrameErrorControlField;

	// For frame error control:
	static const ACE_UINT16 CRC_Initial;
	static const ACE_UINT16 GeneratingPoly;

	// Field value limits:
	static const ACE_UINT8 maxVersionNumber;
	static const ACE_UINT8 maxSpacecraftID;
	static const ACE_UINT8 maxVirtualChannelID;
	static const ACE_UINT32 maxVCFrameCount;
	static const ACE_UINT8 maxVCFrameCountCycle;

	static const ACE_UINT8 idleVirtualChannelID;

	/// Default constructor.
	/// @param hasFrameHeaderErrorControl True if the frame will have an error control header field.
	/// @param inSduLen The length of the insert zone, if the frame needs one.
	/// @param hasOperationalControl True if the frame uses an operational control field.
	/// @param hasFrameErrorControl True when a CRC value is to be generated for the entire frame.
	AOS_Transfer_Frame(
		const bool hasHeaderErrorControl = false,
		const unsigned inSduLen = 0,
		const bool hasOperationalControl = false,
		const bool hasFrameErrorControl = false
	);

	/// Constructor for copying an existing buffer.
	/// @param bufLen Amount of the supplied buffer to copy.
	/// @param frameBuf The buffer to copy.
	/// @param hasHeaderErrorControl True if the frame will have an error control header field.
	/// @param inSduLen The length of the insert zone, if the frame needs one.
	/// @param hasOperationalControl True if the frame uses an operational control field.
	/// @param hasFrameErrorControl True when a CRC value is to be generated for the entire frame.
	/// @param secondaryHeaderLen The length of the secondary header, if the frame needs one.
	AOS_Transfer_Frame(
		const size_t bufLen,
		const ACE_UINT8* frameBuf = 0,
		const bool hasHeaderErrorControl = false,
		const unsigned inSduLen = 0,
		const bool hasOperationalControl = false,
		const bool hasFrameErrorControl = false
	);

	/// Copy constructor.
	explicit AOS_Transfer_Frame(const AOS_Transfer_Frame* other, const bool doDeepCopy = true,
    	const size_t offset = 0, const size_t newLen = 0);

	/// Copy constructor.
	AOS_Transfer_Frame(const NetworkData* other, const bool doDeepCopy = true,
    	const size_t offset = 0, const size_t newLen = 0);

	/// Destructor.
	~AOS_Transfer_Frame() { }

	/// Return the calculated bytes in the primary header, depends on whether the header FEC is present.
	static size_t getHeaderByteLength(const bool withFrameHeaderErrorControl) {
		return spanRequiredHeader + spanFrameHeaderErrorControl * withFrameHeaderErrorControl;
	}

	/// Return the primary header length + secondary header length + trailer length
	/// (data field, insert zone length omitted).
	static size_t getNonDataLength(const bool withFrameHeaderErrorControl,
		const bool withOperationalControl,
		const bool withFrameErrorControl) {
		return getHeaderByteLength(withFrameHeaderErrorControl) +
			spanOperationalControlField * withOperationalControl +
			spanFrameErrorControlField * withFrameErrorControl;
	}

	/// The number of bytes in the primary header of this object.
	virtual size_t getHeaderByteLength() const {
		return getHeaderByteLength(_hasHeaderErrorControl);
	}

	/// Total size of all non-data fields in this object.
	virtual size_t getNonDataLength() const {
		return getNonDataLength(_hasHeaderErrorControl, _hasOperationalControl,
			_hasFrameErrorControl);
	}

	/// Read-only accessor to _hasHeaderErrorControl.
	bool hasHeaderErrorControl() const { return _hasHeaderErrorControl; }

	/// Read-write access to _hasHeaderErrorControl.
	bool useHeaderErrorControl(const bool newSetting = true) {
		return ( _hasHeaderErrorControl = newSetting );
	}

	/// Read-only accessor to _hasOperationalControl.
	bool hasOperationalControl() const { return _hasOperationalControl; }

	/// Read-write access to _hasOperationalControl.
	bool useOperationalControl(const bool newSetting = true) {
		return ( _hasOperationalControl = newSetting );
	}

	/// Read-only accessor to _hasFrameErrorControl.
	bool hasFrameErrorControl() const { return _hasFrameErrorControl; }

	/// Read-write access to _hasFrameErrorControl.
	bool useFrameErrorControl(const bool newSetting = true) {
		return ( _hasFrameErrorControl = newSetting );
	}

	/// Get a pointer to the Master Channel ID, the first field in the frame/header.
	ACE_UINT8* ptrMCID() const { return ptrUnit(); }

	/// The Transfer Frame Version Number + Spacecraft Identifier; the first 10 bits of the frame.
	ACE_UINT16 getMCID() const { return (to_u16_(ptrMCID()) >> 6); }

	/// This 2-bit field shall identify the data unit as a Transfer Frame defined by this
	/// Recommendation; it shall be set to ‘01’.
	/// @return The value of the transfer frame version number field.
	ACE_UINT8 getTFVN() const { return (_front2OctetVal() & 0xC000) >> 14; }

	/// Set the value of the transfer frame version number field.
	/// @param newVer The value to set the field to (only the first two bits are used)
	void setTFVN(const ACE_UINT8 newVer) {
		_front2OctetSet(newVer, getSpacecraftID(), getVirtualChannelID()); }

	/// The Spacecraft Identifier is assigned by CCSDS and shall provide the
	/// identification of the spacecraft which is associated with the data
	/// contained in the Transfer Frame.
	/// The Spacecraft Identifier shall be static throughout all Mission Phases.
	/// @return The value of the spacecraft ID field.
	ACE_UINT8 getSpacecraftID() const { return (_front2OctetVal() & 0x3FC0) >> 6; }

	/// Set the value of the spacecraft ID field and perform range checking.
	/// @param newSCID The new spacecraft ID (8 bits).
	/// @throw ValueTooLarge If the ID is larger than maxSpacecraftID.
	void setSpacecraftID(const ACE_UINT8 newSCID) {
		if ( newSCID <= maxSpacecraftID ) {
			_front2OctetSet(getTFVN(), newSCID, getVirtualChannelID());
		}
		else throw ValueTooLarge(ValueTooLarge::msg("Spacecraft ID", newSCID, maxSpacecraftID));
	}

	/// If only one Virtual Channel is used, these bits are set permanently to value all zeros’.
	/// A Virtual Channel used for transmission of Idle Transfer Frames (see 4.1.4) is
	/// indicated by setting these bits to the reserved value of ‘all ones’.
	/// There are no restrictions on the selection of Virtual Channel Identifiers except the
	/// rules described above. In particular, Virtual Channels are not required to be
	/// numbered consecutively.
	/// A Transfer Frame on the ‘Idle’ Virtual Channel may not contain any valid user data
	/// within its Transfer Frame Data Field, but it must contain the Insert Zone if the Insert
	/// Service is supported.
	/// @return The value of the virtual channel ID field.
	ACE_UINT8 getVirtualChannelID() const { return _front2OctetVal() & 0x3F; }

	/// Set the value of the virtual channel ID field.
	/// @param newVcId The new virtual channel ID value.
	/// @throw ValueTooLarge If newVcId is larger than maxVirtualChannelID.
	void setVirtualChannelID(const ACE_UINT8 newVcId) {
		if ( newVcId <= maxVirtualChannelID)
			_front2OctetSet(getTFVN(), getSpacecraftID(), newVcId);
		else throw ValueTooLarge(ValueTooLarge::msg("Virtual Channel ID", newVcId, maxVirtualChannelID));
	}

	/// Return the Global Virtual Channel Identifier, consisting of the TFVN + SCID + VCID,
	/// in other words, the first two octets in the frame.
	ACE_UINT16 getGVCID() { return to_u16_(ptrMCID()); }

	/// Set the value of the first two octets in the frame, a.k.a. the GVCID (TFVN + SCID + VCID).
	void setGVCID(const ACE_UINT16 newGVCID) { from_u16_(newGVCID, ptrMCID()); }

	/// This 24-bit field shall contain a sequential binary count (modulo-16,777,216) of
	/// each Transfer Frame transmitted within a specific Virtual Channel.
	/// A resetting of the Virtual Channel Frame Count before reaching 16,777,215 shall
	/// not take place unless it is unavoidable.
	/// @return Pointer to the virtual channel frame count field.
	ACE_UINT8* ptrVCFrameCount() const { return ptrMCID() + spanGVCIDField; }

	/// @return The value of the virtual channel frame count field.
	ACE_UINT32 getVCFrameCount() const { return to_u32_(ptrVCFrameCount()) >> 8; }

	/// Set a new value for the virtual frame count.
	/// @param newCount The value of the current virtual frame count.
	/// @throw ValueTooLarge If newCount is greater than maxVCFrameCount.
	void setVCFrameCount(const ACE_UINT32 newCount) {
		if ( newCount <= maxVCFrameCount) {
			ACE_UINT8 numBuf[4];
			from_u32_(newCount, numBuf);
			copyUnit(ptrVCFrameCount(), numBuf + 1, 3);
		}
		else throw ValueTooLarge(ValueTooLarge::msg("Virtual Frame Count", newCount, maxVCFrameCount));
	}

	/// Increase the virtual frame count by one.
	void incVCFrameCount() {
		if ( getVCFrameCount() == maxVCFrameCount ) {
			setVCFrameCount(0);
			incVCFrameCountCycle();
		}
		else setVCFrameCount(getVCFrameCount() + 1);
	}

	/// @name Signaling Field Functions
	//@{

	/// @brief Get a pointer to the start of the Signaling Field.
	///
	/// The Signaling Field shall be used to alert the receiver of the Transfer Frames
	/// with respect to functions that: (a) may change more rapidly than can be handled by
	/// management, or; (b) provide a significant cross-check against manual or automated setups
	/// for fault detection and isolation purposes.
	/// @return A pointer to the signaling field.
	ACE_UINT8* ptrSignalingField() const { return ptrVCFrameCount() + spanVCFrameCountField; }

	/// Obtain the integral value of the entire Signaling Field.
	ACE_UINT8 getSignalingField() const { return (ACE_UINT8) *ptrSignalingField(); }

	/// @brief Retrieve the value of the Replay Flag.
	///
	/// Recognizing the need to store Transfer Frames during periods when the space
    /// link is unavailable, and to retrieve them for subsequent replay when the link is restored, this
    /// flag shall alert the receiver of the Transfer Frames with respect to its realtime’or replay’
    /// status. Its main purpose is to discriminate between realtime and replay Transfer Frames
    /// when they both may use the same Virtual Channel.
	/// @return The value of the replay flag.
	bool getReplayFlag() const { return (bool) (*ptrSignalingField() >> 7); }

	/// Set the value of the replay flag.
	/// @param flagVal The new flag value.
	void setReplayFlag(const bool flagVal) {
		*ptrSignalingField() = (*ptrSignalingField() & 0x7F) + (flagVal << 7);
	}

	/// @brief Get the VC Frame Count Cycle Use Flag
	///
	/// This one-bit field shall indicate whether the VC Frame Count Cycle field is
	/// used; its value shall be interpreted as follows:
    /// a) ‘0’ = VC Frame Count Cycle field is not used and shall be ignored by the receiver;
    /// b) ‘1’ = VC Frame Count Cycle field is used and shall be interpreted by the receiver.
	/// @return The value of the virtual channel frame count usage flag.
	bool getVCFrameCountCycleUsageFlag() const { return (bool) ((*ptrSignalingField() & 0x40) >> 6); }

	/// Set the virtual channel frame count usage flag.
	/// @param flagVal The new flag value.
	void setVCFrameCountCycleUsageFlag(const bool flagVal) {
		*ptrSignalingField() = (*ptrSignalingField() & 0xBF) | (flagVal << 6);
	}

	/// @brief Get the value of the Reserved Spare field.
	///
	/// This 1-bit field is reserved for future definition by CCSDS and shall be set to 0.
	/// @return The value of the reserved spare field.
	ACE_UINT8 getRsvdSpare() const { return ( *ptrSignalingField() & 0x10) >> 4; }

	/// Set the value of the reserved spare field.
	/// @param newVal The value to set the field to.
	void setRsvdSpare(const ACE_UINT8 newVal) {
		*ptrSignalingField() = (*ptrSignalingField() & 0xEF) | ((newVal & 0x1) << 4);
	}

	/// @brief Get the current Virtual Channel (VC) Frame Count Cycle
	///
	/// Each time the Virtual Channel Frame Count returns to zero, the VC Frame
    /// Count Cycle shall be incremented.
    /// NOTE The VC Frame Count Cycle effectively extends the Virtual Channel Frame Count
    /// from 24 to 28 bits.
	/// If not used, bits 44 through 47 of the Transfer Frame Primary Header shall be
    /// set to ‘all zeros.
	/// @return The value of the virtual channel frame count cycle.
	ACE_UINT8 getVCFrameCountCycle() const { return *ptrSignalingField() & 0xF; }

	/// Set the virtual channel frame count cycle. Also set the virtual frame count
	/// usage flag if newVal is greater than zero, otherwise negate it.
	/// @param newVal The value to set the field to.
	/// @throw ValueTooLarge If newVal is larger than maxVCFrameCountCycle.
	void setVCFrameCountCycle(const ACE_UINT8 newVal) {
		if ( newVal <= maxVCFrameCountCycle ) {
			if (newVal) setVCFrameCountCycleUsageFlag(true);
			else setVCFrameCountCycleUsageFlag(false);
			*ptrSignalingField() = (*ptrSignalingField() & 0xF0) | (newVal & 0xF);
		}
		else throw ValueTooLarge(ValueTooLarge::msg("Virtual Frame Count Cycle", newVal,
			maxVCFrameCountCycle));
	}

	/// Increment the Virtual Channel Frame Count by one.
	void incVCFrameCountCycle() { setVCFrameCountCycle(getVCFrameCountCycle() + 1); }

	/// Return a value up to 2^28 - 1; see the note for vcFrameCountCycle().
	ACE_UINT32 getVCFrameCountPlusCycle() const {
		return (getVCFrameCountCycle() << 24) + getVCFrameCount();
	}

	//@}

	/// The 10-bit Master Channel Identifier, the 6-bit Virtual Channel Identifier, and the
	/// 8-bit Signaling Field may all be protected by an optional error detecting and
	/// correcting code, whose check symbols are contained within this 16-bit field.
	/// @return Pointer to the frame header error control field, if it exists.
	ACE_UINT8* ptrHeaderErrorControl() const {
		if ( ! _hasHeaderErrorControl )
			throw MissingField(MissingField::msg("AOS Transfer Frame", "Header Error Control"));
		return ptrSignalingField() + spanSignalingField;
	}

	/// Get the value of the frame header error control field.
	ACE_UINT16 getHeaderErrorControl() const { return to_u16_(ptrHeaderErrorControl()); }

	/// Set the value of the frame header error control field.
	/// @param newECVal The new value of the frame header error control field.
	void setHeaderErrorControl(const ACE_UINT16 newECVal) {
		from_u16_(newECVal, ptrHeaderErrorControl());
	}

	/// Get a pointer to the start of the data field.
	ACE_UINT8* ptrData() const {
		return ptrSignalingField() + spanSignalingField + _inSduLen +
			_hasHeaderErrorControl * spanFrameHeaderErrorControl;
	}

	/// This and the FSH are the only fields in the frame whose length can vary.
	/// @return The length of the data field, computed by subtracting all other field
	/// lengths from the total frame length.
	size_t getDataLength() const {	return getUnitLength() - getInSduLen() - getNonDataLength(); }

	/// Get a pointer to the Frame Error Control Field.
	ACE_UINT8* ptrFrameErrorControl() const {
		if ( ! _hasFrameErrorControl )
			throw MissingField(MissingField::msg("AOS Transfer Frame", "Frame Error Control"));

		return _hasOperationalControl * spanOperationalControlField +
			 ptrData() + getDataLength();
	}

	/// Get the value of the Frame Error Control Field.
	ACE_UINT16 getFrameErrorControl() const { return to_u16_(ptrFrameErrorControl()); }

	/// Set the value of the Frame Error Control Field.
	/// @param newECVal The new value of the fFrame Error Control Field.
	void setFrameErrorControl(const ACE_UINT16 newECVal) {
		from_u16_(newECVal, ptrFrameErrorControl());
	}

	/// Compute the CRC for a 16-bit Frame Error Control Field.
	/// @param crc Initialize CRC to 0xFFFF if computing CRC, or current value if verifying.
	ACE_UINT16 getFrameCRC16(ACE_UINT16 crc = CRC_Initial) {
		return computeCCITT_16_CRC(ptrUnit(),
			getUnitLength() - spanFrameErrorControlField, crc, GeneratingPoly);
	}

	/// Update the supplied CRC using the CCITT 16bit algorithm (X^16 + X^12 + X^5 + 1).
	///
	/// Sourced from http://www.eagleairaust.com.au/code/crc16.htm . Results compared
	/// against CRC16() in the JPL ENCAP_AOS sample software.
	/// @param data Start of buffer to compute for.
	/// @param len Length of buffer.
	/// @param crc Initialize CRC to 0xFFFF if computing CRC, or current value if verifying.
	/// @param generator Generating polynomial
	static ACE_UINT16 computeCCITT_16_CRC(ACE_UINT8* data, const size_t len,
		ACE_UINT16 crc = CRC_Initial, const ACE_UINT16 generator = GeneratingPoly);

	/// Calculate and set the frame error control CRC value.
	/// @return The 16-bit calculated value.
	ACE_UINT16 initializeCRC() {
		if ( _hasFrameErrorControl) setFrameErrorControl(getFrameCRC16());

		return getFrameErrorControl();
	}

	/// If the checksum calculated over a frame with an appended CRC comes back zero, the frame is valid.
	bool hasValidCRC(ACE_UINT16 crc = CRC_Initial) {
		return ( computeCCITT_16_CRC(ptrUnit(), getUnitLength(), crc, GeneratingPoly) == 0 );
	}

	/// True when the Virtual Channel ID is "all ones".
	bool isIdle() const { return getVirtualChannelID() == idleVirtualChannelID; }

	/// Create a frame containing Idle Data.
	/// @param idlePattern An optional object pointer containing the project-specified 'idle' pattern.
	/// @throw nd_error If the frame has not been initialized yet.
	void makeIdle(const ACE_UINT8 spacecraftID, NetworkData *idlePattern = 0) {
		throwIfUninitialized();
		setTFVN(01);
		setSpacecraftID(spacecraftID);
		setVirtualChannelID(idleVirtualChannelID);
		if (idlePattern) setDataToPattern(idlePattern);
	}

	/// When encapsulated in an Ethernet frame, identify with this type.
	ACE_UINT16 enetType() const { return EthernetFrame::Eth_AOS; }

	/// A short string describing the data type (usually just the name).
	std::string typeStr() const { return "AOS Transfer Frame"; }

	/// Construct an AOS Transfer Frame with the specified parameters and empty insert/data zones.
	/// @param spacecraft_id Specify the 8-bit spacecraft ID value.
	/// @param virtual_channel_id Specify the 6-bit virtual channel value.
	/// @param dataLen The size of the data field.
	/// @param vcFrameCount Current virtual channel frame count.
	/// @param vcFrameCountCycleUsageFlag Whether the receiver should look at the VC frame cycle field.
	/// @param vcFrameCountCycle The number of times the frame count has flipped to zero.
	void initialize(
		const ACE_UINT8 spacecraft_id,
		const ACE_UINT8 virtual_channel_id,
		const ACE_UINT32 dataLen = 0,
		const ACE_INT32 vcFrameCount = -1,
		const bool vcFrameCountCycleUsageFlag = true,
		const ACE_INT8 vcFrameCountCycle = -1
	);

	/// Construct an AOS Transfer Frame with the specified parameters and possibly filled data field.
	/// @param SpacecraftID Specify the 8-bit spacecraft ID value.
	/// @param VirtualChannelID Specify the 6-bit virtual channel value.
	/// @param vcFrameCount Current virtual channel frame count.
	/// @param ReplayFlag Whether this is "real-time" or recorded data
	/// @param vcFrameCountCycleUsageFlag Whether the receiver should look at the VC frame cycle field.
	/// @param vcFrameCountCycle The number of times the frame count has flipped to zero.
	/// @param reportType If the OCF is in use, whether it contains a Type-1-Report or a Type-2-Report
	/// @param data The information to copy to the data field.
	AOS_Transfer_Frame* build(
		const ACE_UINT8		SpacecraftID,
		const ACE_UINT8		VirtualChannelID,
		const ACE_UINT32     VCFrameCount,
		const bool			ReplayFlag,
		const bool			VCFrameCountCycleUsageFlag,
		const ACE_UINT8		VCFrameCountCycle,
		const ACE_UINT8		reportType,
		NetworkData* 		data = 0
	);

	/// @name Master Channel Insert Zone Functions
	//@{

	/// Get the length of the Insert Service Data Unit (IN_SDU).
	unsigned getInSduLen() const { return _inSduLen; }

	/// Set the length of the Insert Service Data Unit (IN_SDU).
	/// @param newLen The new length value.
	void setInSduLen(const unsigned newLen) { _inSduLen = newLen; }

	/// Get a pointer to the start of the Insert Zone, if it exists.
	/// @throw MissingField If the Insert Zone is unused.
	ACE_UINT8* ptrInsertZone() const {
		if ( ! _inSduLen )
			throw MissingField(MissingField::msg("AOS Transfer Frame", "Master Channel Insert Zone"));

		return ptrSignalingField() + spanSignalingField +
			_hasHeaderErrorControl * spanFrameHeaderErrorControl;
	}

	/// Copy the supplied buffer into the start of the Insert Zone.
	/// @param data Pointer to the buffer to copy.
	/// @param length Size of the buffer to copy.
	/// @throw BufferOverflow If length is greater than _inSduLen.
	void setInsertZone(ACE_UINT8* data, size_t length) {
		if ( length <= _inSduLen ) copyUnit(ptrInsertZone(), data, length);
		else throw BufferOverflow(BufferOverflow::msg("Master Channel Insert Zone", length, _inSduLen));
	}

	/// Copy the entire contents of the supplied NetworkData object into the start of the insert zone.
	/// @param data The NetworkData object to insert.
	void setInsertZone(NetworkData* data) {
		setInsertZone(data->ptrUnit(), data->getUnitLength());
	}

	/// @name Operational Control Field Functions
	//@{

	/// Get a pointer to the Operational Control Field.
	/// @throw MissingField If the field is unused.
	ACE_UINT8* ptrOperationalControl() const {
		if ( ! _hasOperationalControl )
			throw MissingField(MissingField::msg("AOS Transfer Frame", "Operational Control"));
		return ptrData() + getDataLength();
	}

	/// A Type-1-Report contains a Communications Link Control Word.
	/// @return Either 1 or 2, for a Type-1-Report or a Type-2-Report, respectively.
	ACE_UINT8 getReportType() const {
		return ( *ptrOperationalControl() & 0x80 ) ? 2 : 1;
	}

	/// Sets the type of report.
	/// @param newReportType Must be 1 or 2.
	/// @throw ValueOutOfRange If newReportVal is not 1 or 2.
	void setReportType(const ACE_UINT8 newReportVal);

	/// If true, the OCF contains a CLCW.
	bool isType1Report() const { return ( getReportType() == 1 ); }

	/// If true, the OCF contains a project-defined value.
	bool isType2Report() const { return ( getReportType() == 2 ); }

	bool isReserved() const { return isType2Report() && ! (*ptrOperationalControl() & 0x40); }
	bool isProjectSpecific() const { return isType2Report() && (*ptrOperationalControl() & 0x40); }

	/// @brief Get a pointer to the TC_Comm_Link_Control_Word object if it exists.
	///
	/// This will only be accessible if the Operational Control Field is in use, and is a
	/// Type-1-Report. The buffer managed by this object consists of the four bytes in the OCF.
	TC_Comm_Link_Control_Word* getCLCW();

	/// @brief Copy a Comm Link Control Word into the Operational Control Field.
	/// @param newCLCW A pointer to the CLCW to copy.
	/// This will only be accessible if the Operational Control Field is in use. The
	/// Report Type is automatically changed to Type-1.
	void setCLCW(TC_Comm_Link_Control_Word* newCLCW);

	/// @brief Copy a buffer into the Operational Control Field.
	/// @param data A 4-octet buffer with the first two bits set correctly.
	/// @throw MissingField If no OCF has been allocated.
	/// @throw MalformedPayload If the first bit of data does not indicate a Type-2-Report.
	void setType2ReportData(ACE_UINT8* data);

	//@}

private:
 	/// @return The unsigned 16-bit integer combined value of the first two octets in the frame.
 	ACE_UINT16 _front2OctetVal() const { return to_u16_(ptrMCID()); }

 	/// Set the combined value of the first two octets in the frame.
 	/// @param newVal The new unsigned 16-bit integer value to store.
 	void _front2OctetSet(const ACE_UINT16 newVal) { from_u16_(newVal, ptrMCID()); }

 	/// Set the combined value of the first two octets in the frame.
 	/// @param ver_num Store the first two bits of this as bits 0-1 in the 16-bit value.
 	/// @param spacecraft_id Store these 8 bits as bits 2-9 in the 16-bit value.
 	/// @param vcid Store the first 6 bits of this as bits 10-15 in the 16-bit value.
 	void _front2OctetSet(const ACE_UINT16 ver_num, const ACE_UINT16 spacecraft_id,
 		const ACE_UINT16 vcid) {
 			_front2OctetSet( (ver_num << 14) | (spacecraft_id << 6) | vcid );
 	}

 	unsigned _inSduLen; /// Length of one Insert Service Data Unit if used, otherwise 0.
	bool _hasHeaderErrorControl;
	bool _hasOperationalControl;
	bool _hasFrameErrorControl;
 };

} // namespace nasaCE

#endif // _NASA_AOS_TRANSFER_FRAME_HPP_
