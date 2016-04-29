/* -*- C++ -*- */

//=============================================================================
/**
* @file   AOS_Transfer_Frame.cpp
* @author Tad Kollar  
*
* $Id: AOS_Transfer_Frame.cpp 2006 2013-05-31 19:59:14Z tkollar $
* Copyright (c) 2009.
*      NASA Glenn Research Center.  All rights reserved.
*      See COPYING file that comes with this distribution.
*/
//=============================================================================

#include "AOS_Transfer_Frame.hpp"

namespace nasaCE {

const size_t AOS_Transfer_Frame::spanGVCIDField = 2;
const size_t AOS_Transfer_Frame::spanSignalingField = 1;
const size_t AOS_Transfer_Frame::spanVCFrameCountField = 3;
const size_t AOS_Transfer_Frame::spanRequiredHeader = AOS_Transfer_Frame::spanGVCIDField +
	AOS_Transfer_Frame::spanSignalingField + AOS_Transfer_Frame::spanVCFrameCountField;

const size_t AOS_Transfer_Frame::spanFrameHeaderErrorControl = 2;
const size_t AOS_Transfer_Frame::spanOperationalControlField = 4;
const size_t AOS_Transfer_Frame::spanFrameErrorControlField = 2;


// For frame error control:
const ACE_UINT16 AOS_Transfer_Frame::CRC_Initial = 0xFFFF;
const ACE_UINT16 AOS_Transfer_Frame::GeneratingPoly = 0x1021;

// Field value limits:
const ACE_UINT8 AOS_Transfer_Frame::maxVersionNumber = 3;
const ACE_UINT8 AOS_Transfer_Frame::maxSpacecraftID = 0xFF;
const ACE_UINT8 AOS_Transfer_Frame::maxVirtualChannelID = 63;
const ACE_UINT32 AOS_Transfer_Frame::maxVCFrameCount = 16777215;
const ACE_UINT8 AOS_Transfer_Frame::maxVCFrameCountCycle = 15;

const ACE_UINT8 AOS_Transfer_Frame::idleVirtualChannelID = AOS_Transfer_Frame::maxVirtualChannelID;

AOS_Transfer_Frame::AOS_Transfer_Frame(
	const bool hasHeaderErrorControl /* = false */,
	const unsigned inSduLen /* = 0 */,
	const bool hasOperationalControl /* = false */,
	const bool hasFrameErrorControl /* = false */):
	NetworkData(),
	_inSduLen(inSduLen),
	_hasHeaderErrorControl(hasHeaderErrorControl),
	_hasOperationalControl(hasOperationalControl),
	_hasFrameErrorControl(hasFrameErrorControl) {}

AOS_Transfer_Frame::AOS_Transfer_Frame(
	const size_t bufLen,
	const ACE_UINT8* frameBuf /* = 0 */,
	const bool hasHeaderErrorControl /* = false */,
	const unsigned inSduLen /* = 0 */,
	const bool hasOperationalControl /* = false */,
	const bool hasFrameErrorControl /* = false */):
	NetworkData(bufLen, frameBuf),
	_inSduLen(inSduLen),
	_hasHeaderErrorControl(hasHeaderErrorControl),
	_hasOperationalControl(hasOperationalControl),
	_hasFrameErrorControl(hasFrameErrorControl) {}

AOS_Transfer_Frame::AOS_Transfer_Frame(const AOS_Transfer_Frame* other,
	const bool doDeepCopy /* = true */,
	const size_t offset /* = 0 */,
	const size_t newLen /* = 0 */):
	NetworkData(other, doDeepCopy, offset, newLen),
	_inSduLen(other->_inSduLen),
	_hasHeaderErrorControl(other->_hasHeaderErrorControl),
	_hasOperationalControl(other->_hasOperationalControl),
	_hasFrameErrorControl(other->_hasFrameErrorControl) {
}

AOS_Transfer_Frame::AOS_Transfer_Frame(const NetworkData* other,
	const bool doDeepCopy /* = true */,
	const size_t offset /* = 0 */,
	const size_t newLen /* = 0 */):
	NetworkData(other, doDeepCopy, offset, newLen),
	_inSduLen(0),
	_hasHeaderErrorControl(false),
	_hasOperationalControl(false),
	_hasFrameErrorControl(false) {
}

void AOS_Transfer_Frame::initialize(
	const ACE_UINT8 spacecraft_id,
	const ACE_UINT8 virtual_channel_id,
	const ACE_UINT32 dataLen,
	const ACE_INT32 vcFrameCount,
	const bool vcFrameCountCycleUsageFlag,
	const ACE_INT8 vcFrameCountCycle
) {

	size_t required_size = getNonDataLength() + dataLen + getInSduLen();

	if ( getBufferSize() != required_size ) clearBuffer(required_size);
	else clearBuffer();

	setInitialized_(true);

	setTFVN(01);
	setSpacecraftID(spacecraft_id);
	setVirtualChannelID(virtual_channel_id);
	if ( vcFrameCount > -1 ) setVCFrameCount(vcFrameCount);
	setVCFrameCountCycleUsageFlag(vcFrameCountCycleUsageFlag);
	if ( vcFrameCountCycle > -1) setVCFrameCountCycle(vcFrameCountCycle);
	setRsvdSpare(0);

}

ACE_UINT16 AOS_Transfer_Frame::computeCCITT_16_CRC(ACE_UINT8* data,
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

AOS_Transfer_Frame* AOS_Transfer_Frame::build(
	const ACE_UINT8 SpacecraftID,
	const ACE_UINT8 VirtualChannelID,
	const ACE_UINT32 VCFrameCount,
	const bool ReplayFlag,
	const bool VCFrameCountCycleUsageFlag,
	const ACE_UINT8 VCFrameCountCycle,
	const ACE_UINT8 reportType,
	NetworkData* data
) {

	initialize(SpacecraftID, VirtualChannelID, (data)? data->getUnitLength() : 0,
		VCFrameCount, VCFrameCountCycleUsageFlag, VCFrameCountCycle);

	if (_hasOperationalControl) {
		setReplayFlag(ReplayFlag);
		setReportType(reportType);
	}

	if (data) {
		setData(data);
		if (_hasFrameErrorControl) setFrameErrorControl(getFrameCRC16());
	}

	return this;
}

void AOS_Transfer_Frame::setReportType(const ACE_UINT8 newReportVal) {
	if ( newReportVal != 1 && newReportVal != 2 )
		throw ValueOutOfRange(ValueOutOfRange::msg("Report Type", newReportVal, 1, 2));
	*ptrOperationalControl() = (newReportVal == 1)?
		*ptrOperationalControl() & 0x7f :
		*ptrOperationalControl() | 0x80;
}

TC_Comm_Link_Control_Word* AOS_Transfer_Frame::getCLCW() {
	if ( ! _hasOperationalControl && ! isType1Report() )
		throw MissingField(MissingField::msg("AOS Transfer Frame",
			"Communications Link Control Word in the Operational Control"));

	return wrapInnerPDU<TC_Comm_Link_Control_Word>(spanOperationalControlField,
		ptrOperationalControl());
}

void AOS_Transfer_Frame::setCLCW(TC_Comm_Link_Control_Word* newCLCW) {
	if ( ! _hasOperationalControl )
		throw MissingField(MissingField::msg("AOS Transfer Frame",
			"Communications Link Control Word in the Operational Control"));

	copyUnit(ptrOperationalControl(), newCLCW);

	if (isType2Report()) setReportType(1);
}

void AOS_Transfer_Frame::setType2ReportData(ACE_UINT8* data) {
	if ( ! hasOperationalControl() )
		throw MissingField(MissingField::msg("AOS Transfer Frame",
			"Type-2-Report in the Operational Control"));

	copyUnit(ptrOperationalControl(), data, 4);

	if ( ! isType2Report() )
		throw MalformedPayload(MalformedPayload::msg("AOS Transfer Frame",
			"properly set report type bit"));
}

} // namespace nasaCE
