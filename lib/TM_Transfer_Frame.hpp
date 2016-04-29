/* -*- C++ -*- */

//=============================================================================
/**
* @file   TM_Transfer_Frame.cpp
* @author Tad Kollar  
*
* $Id: TM_Transfer_Frame.hpp 2322 2013-08-07 16:14:42Z tkollar $
* Copyright (c) 2012.
*      NASA Glenn Research Center.  All rights reserved.
*      See COPYING file that comes with this distribution.
*/
//=============================================================================

#ifndef _NASA_TM_TRANSFER_FRAME_HPP_
#define _NASA_TM_TRANSFER_FRAME_HPP_

#include "NetworkData.hpp"
#include "TC_Comm_Link_Control_Word.hpp"

namespace nasaCE {

//=============================================================================
/**
 * @class AOS_Transfer_Frame
 * @author Tad Kollar  
 * @brief The PDU for the CCSDS Telemetry (TM) Space Data Link Protocol.
 */
//=============================================================================
class TM_Transfer_Frame: public NetworkData {
public:
	/// Field value limits:
	static const ACE_UINT8 maxVersionNumber;
	static const ACE_UINT16 maxSpacecraftID;
	static const ACE_UINT8 maxVirtualChannelID;
	static const ACE_UINT16 maxFirstHeaderPointer;
	static const ACE_UINT8 maxMCFrameCount;
	static const ACE_UINT8 maxVCFrameCount;
	static const ACE_UINT8 maxFSHLen;

	/// Field length constants (octets unless otherwise specified):
	static const size_t spanPrimaryHeader;
	static const size_t spanOperationalControlField;
	static const size_t spanFrameErrorControlField;

	/// For frame error control:
	static const ACE_UINT16 CRC_Initial;
	static const ACE_UINT16 CCITT_16_CRC_Generating_Poly;

	/// Default constructor.
	/// @param hasOperationalControl True if the frame uses an operational control field.
	/// @param hasFrameErrorControl True when a CRC value is to be generated for the entire frame.
	/// @param secondaryHeaderLen The length of the secondary header, if the frame needs one.
	TM_Transfer_Frame(
		const bool hasOperationalControl = false,
		const bool hasFrameErrorControl = false,
		const size_t secondaryHeaderLen = 0
	);

	/// Constructor for copying an existing buffer.
	/// @param bufLen Amount of the supplied buffer to copy.
	/// @param frameBuf The buffer to copy.
	/// @param hasOperationalControl True if the frame uses an operational control field.
	/// @param hasFrameErrorControl True when a CRC value is to be generated for the entire frame.
	/// @param secondaryHeaderLen The length of the secondary header, if the frame needs one.
	TM_Transfer_Frame(
		const size_t bufLen,
		const ACE_UINT8* frameBuf = 0,
		const bool hasOperationalControl = false,
		const bool hasFrameErrorControl = false,
		const size_t secondaryHeaderLen = 0
	);

	/// Copy constructor.
	explicit TM_Transfer_Frame(
		const TM_Transfer_Frame* other,
		const bool doDeepCopy = true,
		const size_t offset = 0,
		const size_t newLen = 0
	);

	/// Copy constructor.
	TM_Transfer_Frame(
		const NetworkData* other,
		const bool doDeepCopy = true,
		const size_t offset = 0,
		const size_t newLen = 0
	);

	/// Destructor.
	~TM_Transfer_Frame();

	// ----- Accessors to intra-buffer fields -----

	/// Get a pointer to the primary header.
	inline ACE_UINT8* ptrPrimaryHeader() const { return ptrUnit(); }

	/// Get a pointer to the Master Channel ID, the first field in the frame/header.
	inline ACE_UINT8* ptrMCID() const { return ptrUnit(); }

	/// @brief Read-only accessor to the entire Master Channel Identifier.
	/// The Transfer Frame Version Number + Spacecraft Identifier; the first 12 bits of the frame.
	ACE_UINT16 getMCID() const { return to_u16_(ptrMCID()) >> 4; }

	/// @brief Write-only accessor to the entire MCID value.
	void setMCID(const ACE_UINT16& newVal) {
		from_u16_((to_u16_(ptrMCID()) & 0xf) | (newVal << 4), ptrMCID());
	}

	/// @brief Return the MCID combined with the VCID.
	ACE_UINT16 getGVCID() const { return to_u16_(ptrMCID()) >> 1; }

	/// @brief Read-only accessor to the Transfer Frame Version Number field.
	/// This 2-bit field shall identify the data unit as a Transfer Frame defined by this
	/// Recommendation; it shall be set to ‘01’.
	/// @return The value of the transfer frame version number field.
	ACE_UINT8 getTFVN() const { return *ptrMCID() >> 6; }

	/// @brief Set the value of the Transfer Frame Version Number field.
	/// @param newVer The value to set the field to (only the first two bits are used)
	void setTFVN(const ACE_UINT8& newVer) { *ptrMCID() = (*ptrMCID() & 0x3f) | (newVer << 6); }

	/// @brief Read-only accessor to the Spacecraft Identifier.
	/// Bits 2–11 of the Transfer Frame Primary Header shall contain the Spacecraft
	/// Identifier (SCID).
	/// The Spacecraft Identifier shall provide the identification of the spacecraft which
	/// is associated with the data contained in the Transfer Frame.
	/// The Spacecraft Identifier shall be static throughout all Mission Phases.
	/// @return The value of the spacecraft ID field.
	ACE_UINT16 getSpacecraftID() const { return getMCID() & 0x3ff; }

	/// @brief Set the value of the spacecraft ID field and perform range checking.
	/// @param newSCID The new spacecraft ID (8 bits).
	/// @throw ValueTooLarge If the ID is larger than maxSpacecraftID.
	void setSpacecraftID(const ACE_UINT16& newSCID) {
		if ( newSCID > maxSpacecraftID )
			throw ValueTooLarge(ValueTooLarge::msg("Spacecraft ID", newSCID, maxSpacecraftID));

		setMCID((getMCID() & 0xc00) | newSCID);
	}

	/// @brief Read-only accessor to the virtual channel identifier.
	ACE_UINT8 getVCID() const { return (*(ptrMCID() + 1) & 0xe) >> 1; }

	/// @brief Set the value of the virtual channel identifier field and perform range checking.
	/// @param newVCID The virtual channel ID (3 bits).
	/// @throw ValueTooLarge If the provided VCID is larger than maxVirtualChannelID.
	void setVCID(const ACE_UINT8& newVCID) {
		if ( newVCID > maxVirtualChannelID )
			throw ValueTooLarge(ValueTooLarge::msg("VCID", newVCID, maxVirtualChannelID));

		*(ptrMCID() + 1) = (*(ptrMCID() + 1) & 0xf1) | (newVCID << 1);
	}

	/// @brief Read-only accessor to the Operational Control Field Flag
	/// The Operational Control Field Flag shall indicate the presence or absence of the
	/// Operational Control Field. It shall be ‘1’ if the Operational Control Field is present; it shall
	/// be ‘0’ if the Operational Control Field is not present.
	bool getOCFF() const { return *(ptrMCID() + 1) & 1; }

	/// @brief Write-only accessor to the Operational Control Field Flag
	void setOCFF(const bool newVal) {
		*(ptrMCID() + 1) = (*(ptrMCID() + 1) & 0xfe) | newVal;
	}

	/// @brief Pointer to the Master Channel Frame Count field.
	/// This 8-bit field shall contain a sequential binary count (modulo-256) of each
	/// Transfer Frame transmitted within a specific Master Channel.
	/// A re-setting of the Master Channel Frame Count before reaching 255 shall not
	/// take place unless it is unavoidable.
	inline ACE_UINT8* ptrMCFrameCount() const { return ptrUnit() + 2; }

	/// @brief Read-only accessor to the master channel frame count field.
	ACE_UINT8 getMCFrameCount() const { return *ptrMCFrameCount(); }

	/// @brief Write-only accessor to the master channel frame count field.
	void setMCFrameCount(const ACE_UINT8& newVal) { *ptrMCFrameCount() = newVal; }

	/// @brief Pointer to the Virtual Channel Frame Count field.
	/// This 8-bit field shall contain a sequential binary count (modulo-256) of each
	/// Transfer Frame transmitted within a specific Virtual Channel.
	/// A re-setting of the Virtual Channel Frame Count before reaching 255 shall not
	/// take place unless it is unavoidable.
	/// @return Pointer to the virtual channel frame count field.
	inline ACE_UINT8* ptrVCFrameCount() const { return ptrUnit() + 3; }

	/// @brief Read-only accessor to the virtual channel frame count field.
	ACE_UINT8 getVCFrameCount() const { return *ptrVCFrameCount(); }

	/// @brief Write-only accessor to the virtual channel frame count field.
	void setVCFrameCount(const ACE_UINT8& newVal) { *ptrVCFrameCount() = newVal; }

	/// @brief Pointer to the Transfer Frame Data Field Status
	/// This 16-bit field shall be sub-divided into five sub-fields, as follows:
	/// a) Transfer Frame Secondary Header Flag (1 bit, mandatory);
	/// b) Synchronization Flag (1 bit, mandatory);
	/// c) Packet Order Flag (1 bit, mandatory);
	/// d) Segment Length Identifier (2 bits, mandatory);
	/// e) First Header Pointer (11 bits, mandatory).
	inline ACE_UINT8* ptrDataFieldStat() const { return ptrUnit() + 4; }

	/// @brief Retrieve all 16 bits of the Data Field Status Field.
	ACE_UINT16 getDataFieldStat() const { return to_u16_(ptrDataFieldStat()); }

	/// @brief Simultaneously set all 16 bits of the Data Field Status Field.
	void setDataFieldStat(const ACE_UINT16& newVal) { from_u16_(newVal, ptrDataFieldStat()); }

	/// @brief Read-only accessor to the Transfer Frame Secondary Header Flag.
	/// The Transfer Frame Secondary Header Flag shall signal the presence or absence
	/// of the Transfer Frame Secondary Header. It shall be ‘1’ if a Transfer Frame Secondary
	/// Header is present; it shall be ‘0’ if a Transfer Frame Secondary Header is not present.
	/// The Transfer Frame Secondary Header Flag shall be static within the associated
	/// Master or Virtual Channel throughout a Mission Phase.
	bool getSecondaryHeaderFlag() const { return (*ptrDataFieldStat() >> 7) & 1; }

	/// @brief Write-only accessor to the Transfer Frame Secondary Header Flag.
	void setSecondaryHeaderFlag(const bool newVal) {
		*ptrDataFieldStat() = (*ptrDataFieldStat() & 0x7f) | (newVal << 7);
	}

	/// @brief Read-only accessor to the Synchronization Flag.
	/// The Synchronization Flag shall signal the type of data which are inserted into
	/// the Transfer Frame Data Field. It shall be ‘0’ if octet-synchronized and forward-ordered
	/// Packets or Idle Data are inserted; it shall be ‘1’ if a VCA_SDU is inserted.
	/// The Synchronization Flag shall be static within a specific Virtual Channel
	/// throughout a Mission Phase.
	bool getSynchronizationFlag() const { return (*ptrDataFieldStat() >> 6) & 1; }

	/// @brief Write-only accessor to the Synchronization Flag.
	void setSynchronizationFlag(const bool newVal) {
		*ptrDataFieldStat() = (*ptrDataFieldStat() & 0xbf) | (newVal << 6);
	}

	/// @brief Read-only accessor to the Packet Order Flag.
	/// If the Synchronization Flag is set to ‘0’, the Packet Order Flag is reserved for
	/// future use by the CCSDS and shall be set to ‘0’. If the Synchronization Flag is
	/// set to ‘1’, the use of the Packet Order Flag is undefined.
	bool getPacketOrderFlag() const { return (*ptrDataFieldStat() >> 5) & 1; }

	/// @brief Write-only accessor to the Packet Order Flag.
	void setPacketOrderFlag(const bool newVal) {
		*ptrDataFieldStat() = (*ptrDataFieldStat() & 0xdf) | (newVal << 5);
	}

	/// @brief Read-only accessor to the Segment Length Identifier
	/// If the Synchronization Flag is set to ‘0’, the Segment Length Identifier shall be set to ‘11’.
	/// This Identifier was required for earlier versions of this Recommendation to allow for
	/// the use of Source Packet Segments, which are no longer defined. Its value has been
	/// set to the value used to denote non-use of Source Packet Segments in previous versions.
	/// If the Synchronization Flag is set to ‘1’, then the Segment Length Identifier is undefined.
	ACE_UINT8 getSegmentLengthID() const { return (*ptrDataFieldStat() >> 3) & 3; }

	/// @brief Write-only accessor to the Segment Length Identifier
	void setSegmentLengthID(const ACE_UINT8& newVal) {
		*ptrDataFieldStat() = (*ptrDataFieldStat() & 0xe7) | ((newVal & 3) << 3);
	}

	/// @brief Read-only accessor to the First Header Pointer.
	ACE_UINT16 getFirstHeaderPointer() const { return getDataFieldStat() & 0x7ff; }

	/// @brief Write-only accessor to the First Header Pointer.
	///	If the Synchronization Flag is set to ‘0’, the First Header Pointer shall contain
	/// the position of the first octet of the first Packet that starts in the Transfer Frame Data Field.
	/// If the Synchronization Flag is set to ‘1’, then the First Header Pointer is undefined.
	/// The locations of the octets in the Transfer Frame Data Field shall be numbered
	/// in ascending order. The first octet in this Field is assigned the number 0. The First Header
	/// Pointer shall contain the binary representation of the location of the first octet of the first
	/// Packet that starts in the Transfer Frame Data Field.
	/// @throw ValueTooLarge If newVal is larger than maxFirstHeaderPointer.
	void setFirstHeaderPointer(const ACE_UINT16& newVal) {
		if ( newVal > maxFirstHeaderPointer )
			throw ValueTooLarge(ValueTooLarge::msg("First Header Pointer", newVal, maxFirstHeaderPointer));

		setDataFieldStat((getDataFieldStat() & 0xf800) | (newVal & maxFirstHeaderPointer));
	}

	/// @brief Return true if the First Header Pointer is 0x7ff (all ones).
	/// If no Packet starts in the Transfer Frame Data Field, the First Header Pointer
	/// shall be set to ‘11111111111’ (0x7ff).
	/// The above situation may occur if a long Packet extends across more than one Transfer Frame.
	bool noPacketStart() const { return ( getFirstHeaderPointer() == maxFirstHeaderPointer ); }

	/// @brief Indicate that the frame has no packet starting in the data field.
	void setNoPacketStart() { setFirstHeaderPointer(maxFirstHeaderPointer); }

	/// @brief Return true if the First Header Pointer is 0x7fe (all ones minus one).
	/// If a Transfer Frame contains only Idle Data in its Transfer Frame Data Field, the
	/// First Header Pointer shall be set to ‘11111111110’.
	/// A Transfer Frame with its First Header Pointer set to ‘11111111110’ is called an
	/// OID Transfer Frame, meaning that it has Only Idle Data in its Data Field.
	bool onlyIdleData() const { return ( getFirstHeaderPointer() == maxFirstHeaderPointer - 1); }

	/// @brief Indicate that the frame data field only contains idle data.
	void setOnlyIdleData() { setFirstHeaderPointer(maxFirstHeaderPointer - 1); }

	/// @name TM Transfer Frame Secondary Header Functions
	//@{

	/// @brief Get a pointer to the start of the Transfer Frame Secondary Header, if it exists.
	/// @throw MissingField If the Transfer Frame Secondary Header is unused.
	ACE_UINT8* ptrSecondaryHeader() const {
		if ( ! hasSecondaryHeader() )
			throw MissingField(MissingField::msg("TM Transfer Frame", "Transfer Frame Secondary Header"));

		return ptrUnit() + spanPrimaryHeader;
	}

	/// @brief Retrieve the version number from the Secondary Header Version Number Field.
	/// @return Should always be 00.
	ACE_UINT8 getSecondaryHeaderVersion() const { return *ptrSecondaryHeader() >> 6; }

	/// Set the version number from the Secondary Header Version Number Field.
	/// @param newVer Should always be 00 per this release of the standard.
	void setSecondaryHeaderVersion(const ACE_UINT8 newVer = 00) {
		*ptrSecondaryHeader() = (*ptrSecondaryHeader() & 0x3f) | ((newVer & 0x3) << 6);
	}

	/// @brief Retrieve the length of the Secondary Header Data Field.
	/// This is different from getSecondaryHeaderLength() because that function returns
	/// the entire length of field as known to an TM_Transfer_Frame object, rather than the
	/// value stored within the field itself.
	ACE_UINT8 getSecondaryHeaderDataLength() const { return *ptrSecondaryHeader() & 0x3f; };

	/// @brief Set the length of the Secondary Header Data Field.
	/// This is different from getSecondaryHeaderLength() because that function sets
	/// the entire length of field as known to an TM_Transfer_Frame object, rather than the
	/// value within the field itself.
	void setSecondaryHeaderDataLength(const ACE_UINT8 newLen) const {
		*ptrSecondaryHeader() = (*ptrSecondaryHeader() & 0xc0) | (newLen & 0x3f);
	}

	/// @brief Pointer to the Secondary Header Data Field.
	ACE_UINT8* ptrSecondaryHeaderData() const { return ptrSecondaryHeader() + 1; }

	/// @brief Copy the supplied buffer into the Secondary Header Data Field and set the Header Length field.
	/// @param data Pointer to the buffer to copy.
	/// @param length Size of the buffer to copy.
	/// @throw BufferOverflow If length is greater than _secondaryHeaderLen.
	void setSecondaryHeaderData(ACE_UINT8* data, const size_t length) {
		if ( length < _secondaryHeaderLen ) { // Data length is 1 less than _secondaryHeaderLen.
			copyUnit(ptrSecondaryHeaderData(), data, length);

		}
		else throw BufferOverflow(BufferOverflow::msg("Transfer Frame Secondary Header", length,
			_secondaryHeaderLen));
	}

	/// @brief Copy the NetworkData buffer into the Secondary Header Data Field and set the Header Length.
	/// @param data The NetworkData object with buffer to duplicate.
	void setSecondaryHeaderData(NetworkData* data) {
		setSecondaryHeaderData(data->ptrUnit(), data->getUnitLength());
	}

	//@}
	/// @name Data Field Functions
	//@{
	/// @brief Pointer to the start of the data field.
	ACE_UINT8* ptrData() const { return ptrDataFieldStat() + 2 + getSecondaryHeaderLen(); }

	/// This and the FSH are the only fields in the frame whose length can vary.
	/// @return The length of the data field, computed by subtracting all other field
	/// lengths from the total frame length.
	size_t getDataLength() const { return getUnitLength() - getNonDataLength(); }

	//@}
	/// @name Operational Control Field Functions
	//@{
	/// @brief Get a pointer to the Operational Control Field.
	/// @throw MissingField If the field is unused.
	ACE_UINT8* ptrOperationalControl() const {
		if ( ! _hasOperationalControl )
			throw MissingField(MissingField::msg("TM Transfer Frame", "Operational Control"));
		return ptrData() + getDataLength();
	}

	/// @brief A Type-1-Report contains a Communications Link Control Word.
	/// @return Either 1 or 2, for a Type-1-Report or a Type-2-Report, respectively.
	ACE_UINT8 getReportType() const { return ( *ptrOperationalControl() & 0x80 ) ? 2 : 1; }

	/// @brief If true, the OCF contains a CLCW.
	bool isType1Report() const { return ( getReportType() == 1 ); }

	/// @brief If true, the OCF contains a project-defined value.
	bool isType2Report() const { return ( getReportType() == 2 ); }

	/// @brief If true, the OCF contains a Type-2-Report and the contents are reserved for future use.
	bool isReserved() const { return isType2Report() && ! (*ptrOperationalControl() & 0x40); }

	/// @brief If true, the OCF contains a Type-2-Report and the contents are project-specific.
	bool isProjectSpecific() const { return isType2Report() && (*ptrOperationalControl() & 0x40); }

	/// @brief Get a pointer to the TC_Comm_Link_Control_Word object if it exists.
	///
	/// This will only be accessible if the Operational Control Field is in use, and is a
	/// Type-1-Report. The buffer managed by this object consists of the four bytes in the OCF.
	TC_Comm_Link_Control_Word* getCLCW();

	/// @brief Copy a Comm Link Control Word into the Operational Control Field.
	/// @param newCLCW A pointer to the CLCW to copy.
	/// @throw MissingField If no OCF has been allocated.
	///
	/// This will only be accessible if the Operational Control Field is in use. The
	/// Report Type is automatically changed to Type-1.
	void setCLCW(TC_Comm_Link_Control_Word* newCLCW);

	/// @brief Copy a buffer into the Operational Control Field.
	/// @param data A 4-octet buffer with the first two bits set correctly.
	/// @throw MissingField If no OCF has been allocated.
	/// @throw MalformedPayload If the first bit of data does not indicate a Type-2-Report.
	void setType2ReportData(ACE_UINT8* data);

	//@}

	/// @name Methods Related to Error Control
	//@{

	/// @brief Get a pointer to the Frame Error Control Field.
	ACE_UINT8* ptrFrameErrorControl() const {
		if ( ! _hasFrameErrorControl )
			throw MissingField(MissingField::msg("TM Transfer Frame", "Frame Error Control"));

		return _hasOperationalControl * spanOperationalControlField +
			 ptrData() + getDataLength();
	}

	/// @brief Get the value of the Frame Error Control Field.
	ACE_UINT16 getFrameErrorControl() const { return to_u16_(ptrFrameErrorControl()); }

	/// @brief Set the value of the Frame Error Control Field.
	/// @param newECVal The new value of the fFrame Error Control Field.
	void setFrameErrorControl(const ACE_UINT16& newECVal) {
		from_u16_(newECVal, ptrFrameErrorControl());
	}

	/// @brief Compute the CRC for a 16-bit Frame Error Control Field.
	/// @param crc Initialize CRC to 0xFFFF if computing CRC, or current value if verifying.
	ACE_UINT16 getFrameCRC16(ACE_UINT16 crc = CRC_Initial) {
		return computeCCITT_16_CRC(ptrUnit(),
			getUnitLength() - spanFrameErrorControlField, crc, CCITT_16_CRC_Generating_Poly);
	}

	/// @brief Update the supplied CRC using the CCITT 16-bit algorithm (X^16 + X^12 + X^5 + 1).
	/// @param data Start of buffer to compute for.
	/// @param len Length of buffer.
	/// @param crc Initialize CRC to 0xFFFF if computing CRC, or current value if verifying.
	/// @param generator Generating polynomial
	static ACE_UINT16 computeCCITT_16_CRC(ACE_UINT8* data, const size_t len,
		ACE_UINT16 crc = CRC_Initial, const ACE_UINT16 generator = CCITT_16_CRC_Generating_Poly);

	/// @brief Calculate and set the frame error control CRC value.
	/// @return The 16-bit calculated value.
	ACE_UINT16 initializeCRC() {
		if ( _hasFrameErrorControl) setFrameErrorControl(getFrameCRC16());

		return getFrameErrorControl();
	}

	/// @brief Return whether the checksum is valid for this frame.
	///
	/// If the checksum calculated over a frame with an appended CRC comes back zero, the frame is valid.
	bool hasValidCRC(ACE_UINT16 crc = CRC_Initial) {
		return ( computeCCITT_16_CRC(ptrUnit(), getUnitLength(), crc, CCITT_16_CRC_Generating_Poly) == 0 );
	}

	//@}


	/// @brief Determine whether the buffer contains a valid TM Transfer Frame according to the CCSDS Blue Book.
	/// @return 1 if the frame is valid, or < 0 if not.
	int isValid() const;

	// ----- Accessors to non-buffer data members -----

	/// @brief Returns true if _secondaryHeaderLen is non-zero.
	bool hasSecondaryHeader() const { return ( _secondaryHeaderLen > 0); }

	/// @brief Read-only accessor to _secondaryHeaderLen.
	size_t getSecondaryHeaderLen() const { return _secondaryHeaderLen; }

	/// @brief Write-only accessor to _secondaryHeaderLen.
	/// @throw ValueTooLarge If newVal is larger than maxFSHLen.
	void setSecondaryHeaderLen(const size_t& newVal) {
		if ( newVal > maxFSHLen )
			throw ValueTooLarge(ValueTooLarge::msg("Frame Secondary Header Length", newVal, maxFSHLen));
		_secondaryHeaderLen = newVal;

	}

	/// @brief Read-only accessor to _hasOperationalControl.
	bool hasOperationalControl() const { return _hasOperationalControl; }

	/// @brief Read-write access to _hasOperationalControl.
	bool useOperationalControl(const bool newSetting = true) {
		return ( _hasOperationalControl = newSetting );
	}

	/// @brief Read-only accessor to _hasFrameErrorControl.
	bool hasFrameErrorControl() const { return _hasFrameErrorControl; }

	/// @brief Read-write access to _hasFrameErrorControl.
	bool useFrameErrorControl(const bool newSetting = true) {
		return ( _hasFrameErrorControl = newSetting );
	}

	/// @brief Return the primary header length + secondary header length + trailer length
	/// (i.e. size of everything with the data field omitted).
	static size_t getNonDataLength(const bool withOperationalControl,
		const bool withFrameErrorControl,
		const size_t secondaryHeaderLen = 0) {
		return spanPrimaryHeader + secondaryHeaderLen +
			spanOperationalControlField * withOperationalControl +
			spanFrameErrorControlField * withFrameErrorControl;
	}

	/// @brief Return the combined length of all non-data fields in this object.
	size_t getNonDataLength() const {
		return getNonDataLength(_hasOperationalControl, _hasFrameErrorControl, _secondaryHeaderLen);
	}

	/// @brief Return a short string describing the data type (usually just the name).
	std::string typeStr() const { return "TM Transfer Frame"; }

	/// @brief Construct an TM Transfer Frame with the specified parameters and empty data field.
	/// @param scid Specify the 10-bit spacecraft ID value.
	/// @param vcid Specify the 3-bit virtual channel value.
	/// @param dataLen The size of the data field.
	/// @param mcFrameCount Current 8-bit master channel frame count.
	/// @param vcFrameCount Current 8-bit virtual channel frame count.
	void initialize(
		const ACE_UINT16& scid,
		const ACE_UINT8& vcid,
		const size_t& dataLen,
		const ACE_UINT8 mcFrameCount = 0,
		const ACE_UINT8 vcFrameCount = 0
	);

	/// @brief Construct an TM Transfer Frame with the specified parameters and possibly filled data fields.
	/// @param scid Specify the 10-bit spacecraft ID value.
	/// @param vcid Specify the 3-bit virtual channel value.
	/// @param mcFrameCount Current 8-bit master channel frame count.
	/// @param vcFrameCount Current 8-bit virtual channel frame count.
	/// @param data The information to copy to the data field.
	/// @param firstHeaderPointer Index to the start of the first packet header in the data field.
	/// @param fshData Data to go into the Frame Secondary Header data field.
	/// @param clcw Comm Link Control Word to copy into the Operational Control Field.
	/// @param ocfData Pointer to a 4-octet buffer (not a CLCW) to copy to the OCF.
	TM_Transfer_Frame* build(
		const ACE_UINT16& scid,
		const ACE_UINT8& vcid,
		const ACE_UINT8& mcFrameCount,
		const ACE_UINT8& vcFrameCount,
		NetworkData* data = 0,
		const bool doCRC = false,
		const ACE_UINT16 firstHeaderPointer = 0,
		NetworkData* fshData = 0,
		TC_Comm_Link_Control_Word* clcw = 0,
		ACE_UINT8* ocfData = 0
	);

	/// Convert the frame to Idle.
	/// @param scid The Spacecraft Identifier to use in the idle frame.
	/// @param vcid The Virtual Channel Identifier to use in the idle frame.
	/// @param idlePattern An optional object pointer containing the project-specified 'idle' pattern.
	/// @throw nd_error If the frame has not been initialized yet.
	void makeIdle(const ACE_UINT8 scid, const ACE_UINT8 vcid, NetworkData *idlePattern = 0) {
		throwIfUninitialized();
		setTFVN(0);
		setSpacecraftID(scid);
		setVCID(vcid);
		setOnlyIdleData();
		if (idlePattern) setDataToPattern(idlePattern);
	}

private:

	/// True if the Operational Control Field is used.
	bool _hasOperationalControl;

	/// True if space should be reserved for a frame checksum.
	bool _hasFrameErrorControl;

	/// Length of the secondary header if used, otherwise 0.
	size_t _secondaryHeaderLen;
};

}; // namespace nasaCE

#endif // _NASA_TM_TRANSFER_FRAME_HPP_
