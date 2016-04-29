/* -*- C++ -*- */

//=============================================================================
/**
 * @file   SpacePacket.cpp
 * @author Tad Kollar  
 *
 * $Id: SpacePacket.cpp 1670 2012-08-28 13:25:28Z tkollar $
 * Copyright (c) 2012.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "SpacePacket.hpp"

using namespace nasaCE;
namespace nasaCE {

const ACE_UINT16 SpacePacket::idleAPIDVal = 0x7ff;
const size_t SpacePacket::maxAPID = 0x7ff;
const size_t SpacePacket::maxSeqFlags = 3;
const size_t SpacePacket::maxPktSeqCount = 0x3fff;

ACE_UINT8 SpacePacket::minBytesToDetermineLength(ACE_UINT8 firstOctet /* = 0 */) { return 6; }

SpacePacket::SpacePacket(): NetworkData(7), _secHdrLen(0), _timeCodeLen(0), _ancDataLen(0) { }

SpacePacket::SpacePacket(const ACE_UINT32& bufLen, const ACE_UINT8* frameBuf /* = 0 */,
	const size_t& secHdrLen /* = 0 */, const size_t& timeCodeLen /* = 0 */, const size_t& ancDataLen /* = 0 */):
	NetworkData(bufLen, frameBuf), _secHdrLen(secHdrLen), _timeCodeLen(timeCodeLen), _ancDataLen(ancDataLen) {

	if ( _timeCodeLen + _ancDataLen > _secHdrLen ) {
		_timeCodeLen = 0;
		_ancDataLen = 0;
	}

	if ( bufLen ) { setPktDataLen(bufLen - 7); }
}

SpacePacket::SpacePacket(const SpacePacket* other, const bool doDeepCopy /* = true */,
		const size_t& offset /* = 0 */, const size_t& newLen /* = 0 */): NetworkData(other, doDeepCopy, offset, newLen),
		_secHdrLen(other->_secHdrLen), _timeCodeLen(other->_timeCodeLen), _ancDataLen(other->_ancDataLen) {

}

SpacePacket::SpacePacket(const NetworkData* other, const bool doDeepCopy /* = true */,
		const size_t offset /* = 0 */, const size_t newLen /* = 0 */): NetworkData(other, doDeepCopy, offset, newLen),
		_secHdrLen(0), _timeCodeLen(0), _ancDataLen(0) {

}

SpacePacket::~SpacePacket() { }

ACE_UINT8* SpacePacket::ptrPacketVersionNumber() const { return ptrUnit(); }

ACE_UINT8 SpacePacket::getPacketVersionNumber() const { return *ptrPacketVersionNumber() >> 5; }

void SpacePacket::setPacketVersionNumber() { *ptrPacketVersionNumber() &= 0x1f; }

bool SpacePacket::getPacketType() const { return (*ptrUnit() >> 4) & 1; }

void SpacePacket::setPacketType(const bool newType) {
	ACE_UINT8 newVal = newType;
	*ptrPacketVersionNumber() = (*ptrUnit() & 0xef) | ( newVal << 4 );
}

bool SpacePacket::isTC() { return getPacketType(); }

bool SpacePacket::isTM() { return ! getPacketType(); }

bool SpacePacket::getSecHdrFlag() const { return (*ptrUnit() >> 3) & 1; }

void SpacePacket::setSecHdrFlag(const bool newFlag) {
	ACE_UINT8 newVal = newFlag;
	*ptrUnit() = (*ptrUnit() & 0xf7) | ( newVal << 3 );
}

ACE_UINT16 SpacePacket::getAPID() const { return to_u16_(ptrUnit()) & 0x7ff; }


void SpacePacket::setAPID(const ACE_UINT16& newVal) {
	if ( newVal > maxAPID )
		throw ValueTooLarge(ValueTooLarge::msg("APID", newVal, maxAPID));

	from_u16_((to_u16_(ptrUnit()) & 0xf800) | (newVal & 0x7ff), ptrUnit());
}

bool SpacePacket::isIdle() const { return getAPID() == idleAPIDVal; }

void SpacePacket::makeIdle(const NetworkData* fillPattern /* = 0 */) {
	setAPID(idleAPIDVal);
	setSecHdrFlag(false);

	if ( fillPattern != 0 ) {
		setDataToPattern(fillPattern);
	}
}

ACE_UINT8* SpacePacket::ptrPktSeqCtrl() const { return ptrUnit() + 2; }

ACE_UINT8 SpacePacket::getSeqFlags() const { return to_u16_(ptrPktSeqCtrl()) >> 14; }

void SpacePacket::setSeqFlags(const ACE_UINT8& newFlags) {
	if ( newFlags > maxSeqFlags )
		throw ValueTooLarge(ValueTooLarge::msg("Sequence Flags", newFlags, maxSeqFlags));

	from_u16_((to_u16_(ptrPktSeqCtrl()) & 0x3fff) | (newFlags << 14), ptrPktSeqCtrl());
}

bool SpacePacket::getContainsContinuation() const { return getSeqFlags() == 0; }

void SpacePacket::setContainsContinuation() { setSeqFlags(0); }

bool SpacePacket::getContainsFirstSegment() const { return getSeqFlags() == 1; }

void SpacePacket::setContainsFirstSegment() { setSeqFlags(1); }

bool SpacePacket::getContainsLastSegment() const { return getSeqFlags() == 2; }

void SpacePacket::setContainsLastSegment() { setSeqFlags(2); }

bool SpacePacket::getContainsUnsegmented() const { return getSeqFlags() == 3; }

void SpacePacket::setContainsUnsegmented() { setSeqFlags(3); }

ACE_UINT16 SpacePacket::getPktSeqCount() const { return to_u16_(ptrPktSeqCtrl()) & 0x3fff; }

void SpacePacket::setPktSeqCount(const ACE_UINT16& newVal) {
	if ( newVal > maxPktSeqCount )
		throw ValueTooLarge(ValueTooLarge::msg("Packet Sequence Count/Name", newVal, maxPktSeqCount));

	from_u16_((to_u16_(ptrPktSeqCtrl()) & 0xc000) | newVal, ptrPktSeqCtrl());
}

bool SpacePacket::incPktSeqCount() {
	ACE_UINT16 count = getPktSeqCount();

	if ( count == maxPktSeqCount ) {
		setPktSeqCount(0);
		return true;
	}

	setPktSeqCount(count + 1);
	return false;
}

ACE_UINT8* SpacePacket::ptrPktDataLen() const { return ptrUnit() + 4; }

ACE_UINT16 SpacePacket::getPktDataLen() const { return to_u16_(ptrPktDataLen()); }

void SpacePacket::setPktDataLen(const ACE_UINT16& newVal) {
	from_u16_(newVal, ptrPktDataLen());
}

ACE_UINT8* SpacePacket::ptrPktDataField() const { return ptrUnit() + 6; }

size_t SpacePacket::getSecHdrLen() const { return _secHdrLen; }

void SpacePacket::setSecHdrLen(const size_t& newLen) {
	if ( ! getSecHdrFlag() )
		throw MissingField(MissingField::msg("Packet Secondary Header", "Space Packet"));

	_secHdrLen = newLen;
}

ACE_UINT8* SpacePacket::ptrSecHdr() const {
	if ( ! getSecHdrFlag() )
		throw MissingField(MissingField::msg("Packet Secondary Header", "Space Packet"));

	return ptrPktDataField();
}

size_t SpacePacket::getTimeCodeLen() const { return _timeCodeLen; }

void SpacePacket::setTimeCodeLen(const size_t& newLen) { _timeCodeLen = newLen; }

ACE_UINT8* SpacePacket::ptrTimeCode() const {
	if ( ! getTimeCodeLen() )
		throw MissingField(MissingField::msg("Time Code Field", "Space Packet"));

	return ptrSecHdr();
}

NetworkData* SpacePacket::getTimeCode() {
	return wrapInnerPDU<NetworkData>(getTimeCodeLen(), ptrTimeCode());
}

void SpacePacket::setTimeCode(const ACE_UINT8* timeCodeBuf, const size_t timeCodeBufLen /* = 0 */) {
	copyUnit(ptrTimeCode(), timeCodeBuf, (timeCodeBufLen > 0)? timeCodeBufLen : _timeCodeLen);
}

size_t SpacePacket::getAncDataLen() const { return _ancDataLen; }

void SpacePacket::setAncDataLen(const size_t& newLen) { _ancDataLen = newLen; }

ACE_UINT8* SpacePacket::ptrAncData() const {
	if ( ! getAncDataLen() )
		throw MissingField(MissingField::msg("Ancillary Data Field", "Space Packet"));

	return ptrSecHdr() + getTimeCodeLen();
}

NetworkData* SpacePacket::getAncData() {
	return wrapInnerPDU<NetworkData>(getAncDataLen(), ptrAncData());
}

void SpacePacket::setAncData(const ACE_UINT8* ancDataBuf, const size_t ancDataBufLen /* = 0 */) {
	copyUnit(ptrAncData(), ancDataBuf, (ancDataBufLen > 0)? ancDataBufLen : _ancDataLen);
}

ACE_UINT8* SpacePacket::ptrData() const {
	return ptrPktDataField() + _secHdrLen;
}

size_t SpacePacket::getDataLength() const {
	return getPktDataLen() + 1 - _secHdrLen;
}

size_t SpacePacket::expectedLength() const {
	return getPktDataLen() + 7;
}


} // namespace nasaCE
