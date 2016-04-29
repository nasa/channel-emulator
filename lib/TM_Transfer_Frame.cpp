//=============================================================================
/**
* @file   TM_Transfer_Frame.cpp
* @author Tad Kollar  
*
* $Id: TM_Transfer_Frame.cpp 2322 2013-08-07 16:14:42Z tkollar $
* Copyright (c) 2012.
*      NASA Glenn Research Center.  All rights reserved.
*      See COPYING file that comes with this distribution.
*/
//=============================================================================

#include "TM_Transfer_Frame.hpp"

namespace nasaCE {

const ACE_UINT8 TM_Transfer_Frame::maxVersionNumber = 3;
const ACE_UINT16 TM_Transfer_Frame::maxSpacecraftID = 0x3ff;
const ACE_UINT8 TM_Transfer_Frame::maxVirtualChannelID = 7;
const ACE_UINT16 TM_Transfer_Frame::maxFirstHeaderPointer = 0x7ff;
const ACE_UINT8 TM_Transfer_Frame::maxMCFrameCount = 0xff;
const ACE_UINT8 TM_Transfer_Frame::maxVCFrameCount = 0xff;
const ACE_UINT8 TM_Transfer_Frame::maxFSHLen = 64;

const size_t TM_Transfer_Frame::spanPrimaryHeader = 6;
const size_t TM_Transfer_Frame::spanOperationalControlField = 4;
const size_t TM_Transfer_Frame::spanFrameErrorControlField = 2;

const ACE_UINT16 TM_Transfer_Frame::CRC_Initial = 0xffff;
const ACE_UINT16 TM_Transfer_Frame::CCITT_16_CRC_Generating_Poly = 0x1021;

TM_Transfer_Frame::TM_Transfer_Frame(
	const bool hasOperationalControl /* = false */,
	const bool hasFrameErrorControl /* = false */,
	const size_t secondaryHeaderLen /* = 0 */ ):
	NetworkData(),
	_hasOperationalControl(hasOperationalControl),
	_hasFrameErrorControl(hasFrameErrorControl),
	_secondaryHeaderLen(secondaryHeaderLen) {

}

TM_Transfer_Frame::TM_Transfer_Frame(
	const size_t bufLen,
	const ACE_UINT8* frameBuf /* = 0 */,

	const bool hasOperationalControl /* = false */,
	const bool hasFrameErrorControl /* = false */,
	const size_t secondaryHeaderLen /* = 0 */):
	NetworkData(bufLen, frameBuf),
	_hasOperationalControl(hasOperationalControl),
	_hasFrameErrorControl(hasFrameErrorControl),
	_secondaryHeaderLen(secondaryHeaderLen) {

}

TM_Transfer_Frame::TM_Transfer_Frame(const TM_Transfer_Frame* other,
	const bool doDeepCopy /* = true */,
	const size_t offset /* = 0 */,
	const size_t newLen /* = 0 */):
	NetworkData(other, doDeepCopy, offset, newLen),
	_hasOperationalControl(other->_hasOperationalControl),
	_hasFrameErrorControl(other->_hasFrameErrorControl),
	_secondaryHeaderLen(other->_secondaryHeaderLen) {
}

TM_Transfer_Frame::TM_Transfer_Frame(const NetworkData* other,
	const bool doDeepCopy /* = true */,
	const size_t offset /* = 0 */,
	const size_t newLen /* = 0 */):
	NetworkData(other, doDeepCopy, offset, newLen),
	_hasOperationalControl(false),
	_hasFrameErrorControl(false),
	_secondaryHeaderLen(0) {
}

TM_Transfer_Frame::~TM_Transfer_Frame() {
}

int TM_Transfer_Frame::isValid() const {
	// TFVN must be 00.
	if ( getTFVN() != 0 ) return -1;

	// Operational Control Field Flag must be set if there's an Operational Control Field.
	if ( hasOperationalControl() && ! getOCFF() ) return -2;

	// Operational Control Field Flag must be not set if there's no Operational Control Field.
	if ( ! hasOperationalControl() && getOCFF() ) return -3;

	// Secondary header flag must be set if there's a secondary header.
	if ( getSecondaryHeaderLen() > 0 && ! getSecondaryHeaderFlag() ) return -4;

	// Secondary header flag must not be set if there's no secondary header.
	if ( getSecondaryHeaderLen() == 0 && getSecondaryHeaderFlag() ) return -5;

	// If the Synchronization Flag is 0, the Packet Order Flag must be 0.
	if ( ! getSynchronizationFlag() && getPacketOrderFlag() ) return -6;

	// If the Synchronization Flag is 0, the Segment Length Identifier must be 11 (binary)
	if ( ! getSynchronizationFlag() && getSegmentLengthID() != 3 ) return -7;

	// The secondary header data length stored in the buffer must be one less
	// than the mission-phase secondary header length field.
	if ( hasSecondaryHeader() && getSecondaryHeaderDataLength() != getSecondaryHeaderLen() - 1 ) return -8;

	// The first header pointer can't point beyond the end of the data field.
	if ( ! noPacketStart() && ! onlyIdleData() && getFirstHeaderPointer() > getDataLength() ) return -9;

	return 1;
}

TC_Comm_Link_Control_Word* TM_Transfer_Frame::getCLCW() {
	if ( ! hasOperationalControl() && ! isType1Report() )
		throw MissingField(MissingField::msg("TM Transfer Frame",
			"Communications Link Control Word in the Operational Control"));

	return wrapInnerPDU<TC_Comm_Link_Control_Word>(spanOperationalControlField,
		ptrOperationalControl());
}

void TM_Transfer_Frame::setCLCW(TC_Comm_Link_Control_Word* newCLCW) {
	if ( ! hasOperationalControl() )
		throw MissingField(MissingField::msg("TM Transfer Frame",
			"Communications Link Control Word in the Operational Control"));

	copyUnit(ptrOperationalControl(), newCLCW);
}

void TM_Transfer_Frame::setType2ReportData(ACE_UINT8* data) {
	if ( ! hasOperationalControl() )
		throw MissingField(MissingField::msg("TM Transfer Frame",
			"Type-2-Report in the Operational Control"));

	copyUnit(ptrOperationalControl(), data, 4);

	if ( ! isType2Report() )
		throw MalformedPayload(MalformedPayload::msg("TM Transfer Frame",
			"properly set report type bit"));
}

ACE_UINT16 TM_Transfer_Frame::computeCCITT_16_CRC(ACE_UINT8* data,
	const size_t len, ACE_UINT16 crc, const ACE_UINT16 generator) {
	unsigned idx;

	for ( idx = 0; idx < len; ++idx) {
		crc  = (unsigned char)(crc >> 8) | (crc << 8);
		crc ^= *(data + idx);
		crc ^= (unsigned char)(crc & 0xff) >> 4;
		crc ^= (crc << 8) << 4;
		crc ^= ((crc & 0xff) << 4) << 1;
	}

	return crc;
}

void TM_Transfer_Frame::initialize(
	const ACE_UINT16& scid,
	const ACE_UINT8& vcid,
	const size_t& dataLen,
	const ACE_UINT8 mcFrameCount /* = 0 */,
	const ACE_UINT8 vcFrameCount /* = 0 */
) {

	size_t required_size = getNonDataLength() + dataLen;

	if ( getBufferSize() != required_size ) clearBuffer(required_size);
	else clearBuffer();

	setInitialized_(true);

	setTFVN(0);
	setSpacecraftID(scid);
	setVCID(vcid);
	setMCFrameCount(mcFrameCount);
	setVCFrameCount(vcFrameCount);
	setSynchronizationFlag(false);
	setPacketOrderFlag(false);
	setSegmentLengthID(3);

	if (hasOperationalControl()) setOCFF(1);

	if ( hasSecondaryHeader() ) {
		setSecondaryHeaderFlag(true);
		setSecondaryHeaderVersion(00);
		setSecondaryHeaderDataLength(_secondaryHeaderLen - 1);
	};
}

TM_Transfer_Frame* TM_Transfer_Frame::build(
	const ACE_UINT16& scid,
	const ACE_UINT8& vcid,
	const ACE_UINT8& mcFrameCount,
	const ACE_UINT8& vcFrameCount,
	NetworkData* data /* = 0 */,
	const bool doCRC /* = false */,
	const ACE_UINT16 firstHeaderPointer /* = 0 */,
	NetworkData* fshData /* = 0 */,
	TC_Comm_Link_Control_Word* clcw /* = 0 */,
	ACE_UINT8* ocfData /* = 0 */
) {

	initialize(scid, vcid, (data)? data->getUnitLength() : 0, mcFrameCount, vcFrameCount);

	if (data) setData(data);
	setFirstHeaderPointer(firstHeaderPointer);

	if ( hasSecondaryHeader() && fshData ) setSecondaryHeaderData(fshData);

	if ( hasOperationalControl() ) {
		 if ( clcw ) setCLCW(clcw);
		 else if ( ocfData ) { setType2ReportData(ocfData); }
	}

	if ( doCRC && hasFrameErrorControl() ) initializeCRC();

	return this;
}

} // namespace nasaCE