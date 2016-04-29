/* -*- C++ -*- */

//=============================================================================
/**
 * @file   EncapsulationPacket.cpp
 * @author Tad Kollar  
 *
 * $Id: EncapsulationPacket.cpp 2433 2014-01-15 17:45:20Z tkollar $
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "EncapsulationPacket.hpp"

namespace nasaCE {

const ACE_UINT8 EncapsulationPacketBase::maxProtocolID = 0x7;
const ACE_UINT8 EncapsulationPacketBase::maxLengthOfLength = 0x3;
const ACE_UINT8 EncapsulationPacketBase::maxUserDefinedField = 0xF;
const ACE_UINT8 EncapsulationPacketBase::maxProtocolIDExtension = 0xF;

ACE_UINT8* EncapsulationPacketBase::ptrPacketLength() const {
	switch (getLengthOfLength()) {
		case 0: throw MissingField(MissingField::msg("Encapsulation Packet", "Packet Length"));
			break;
		case 1: return ptrPacketVersionNumber() + 1;
			break;
		case 2: return ptrUserDefinedField() + 1;
			break;
		case 3: return ptrCssdsDefinedField() + 2;
			break;
		default: throw LogicError("Very confused, impossible Length of Length value encountered.");
	}
}

ACE_UINT32 EncapsulationPacketBase::getPacketLength() const {
	switch (getLengthOfLength()) {
		case 0: throw MissingField(MissingField::msg("Encapsulation Packet", "Packet Length"));
			break;
		case 1: return *ptrPacketLength();
			break;
		case 2: return to_u16_(ptrPacketLength());
			break;
		case 3: return to_u32_(ptrPacketLength());
			break;
		default: throw LogicError("Very confused, impossible Length of Length value encountered.");
	}
}

void EncapsulationPacketBase::setPacketLength(const ACE_UINT32 newPacketLen) {
	switch (getLengthOfLength()) {
		case 0: throw MissingField(MissingField::msg("Encapsulation Packet", "Packet Length"));
			break;
		case 1:
			if (newPacketLen <= (1 << 8) - 1) *ptrPacketLength() = (ACE_UINT8) newPacketLen;
			else throw ValueTooLarge(ValueTooLarge::msg("Packet Length", newPacketLen, (1 << 8) - 1));
			break;
		case 2:
			if (newPacketLen <= (1 << 16) - 1) from_u16_(newPacketLen, ptrPacketLength());
			else throw ValueTooLarge(ValueTooLarge::msg("Packet Length", newPacketLen, (1 << 16) - 1));
			break;
		case 3:
			if (newPacketLen <= ULONG_MAX - 4) from_u32_(newPacketLen, ptrPacketLength());
			else throw ValueTooLarge(ValueTooLarge::msg("Packet Length", newPacketLen, ULONG_MAX - 4));
			break;
		default: throw LogicError("Very confused, impossible Length of Length value encountered.");
	}
}

unsigned char EncapsulationPacketBase::getHeaderOctets(const ACE_UINT8 lengthOfLength) {
	switch (lengthOfLength & maxLengthOfLength) {
		case 0: return 1;
		case 1: return 2;
		case 2: return 4;
		case 3: return 8;
	}

	return 0;
}

const ACE_UINT64 EncapsulationPacketWithIPE::maxIntStorableInIPE = 0xFFFFFFFFFFFFFFllu;

unsigned short EncapsulationPacketWithIPE::countIPEOctets() const {
	if (hasIPE()) {
		unsigned short count;
		for ( count = 0; ! (*(ptrIPE() + count) & 0x1); ++count ) {
			if ( getHeaderOctets() + count + 1U > getUnitLength() )
				throw nd_error("countIPEOctets() attempted to search beyond the end-of-packet, IPE set incorrectly?");
		}
		return count + 1;
	}
	return 0;
}

ACE_UINT64 EncapsulationPacketWithIPE::convertToIPE(ACE_UINT64 anyNum) {
	if (anyNum > maxIntStorableInIPE)
		throw ValueTooLarge(ValueTooLarge::msg("Internet Protocol Extension", anyNum, maxIntStorableInIPE));

	NetworkData* newIPE = new NetworkData(8);
	newIPE->clearBuffer();
	ACE_UINT8 currentOctetVal = 0x1;
	int octetIndex = 7;

	do {
		currentOctetVal |= (anyNum & 0x7F) << 1;
		anyNum >>= 7;
		*(newIPE->ptrUnit() + octetIndex) = currentOctetVal;
		--octetIndex;
		currentOctetVal = 0;
	} while (anyNum);

	return to_u64_(newIPE->ptrUnit());
}

ACE_UINT64 EncapsulationPacketWithIPE::unconvertIPE(ACE_UINT64 testIPE) {
	if ( ! properIPE(testIPE) )
		throw BadValue(BadValue::msg("Internet Protocol Extension", testIPE));

	ACE_UINT64 normalNum = 0;
	int currentBits = 0;

	do {
		normalNum |= (((testIPE & 0xFE) >> 1) << currentBits);
		testIPE >>= 8;
		currentBits += 7;
	} while (testIPE);

	return normalNum;
}

ACE_UINT64 EncapsulationPacketWithIPE::getIPE() const {
	if (hasIPE()) {
		short idx = 0;
		ACE_UINT64 aggregate = 0;

		for ( idx = 0; ! (*(ptrIPE() + idx) & 0x1); idx++ ) {
			aggregate |= *(ptrIPE() + idx);
			if ( getHeaderOctets() + idx + 1U > getUnitLength() )
				throw nd_error("getIPE() attempted to search beyond the end-of-packet, IPE set incorrectly?");
			aggregate <<= 8;
		}
		aggregate |= *(ptrIPE() + idx);

		return aggregate;
	}

	throw MissingField(MissingField::msg("Encapsulation Packet", "Internet Protocol Extension"));
}

void EncapsulationPacketWithIPE::setIPE(ACE_UINT64 newIPE) {
	if (hasIPE()) {
		if ( ! properIPE(newIPE) )
			throw BadValue(BadValue::msg("Internet Protocol Extension", newIPE));

		ACE_UINT8 currentOctetVal; // to hold the value we want to store
		unsigned short required_octets = calcIPEOctets(newIPE); // figure out how much room is needed
		unsigned short octetIndex = required_octets - 1; // set the starting position
		ACE_UINT8 growIPE[required_octets];
		// NetworkData* growIPE = new NetworkData(required_octets); // allocate space to store the converted IPE
		// growIPE->clearBuffer();

		while (newIPE) {
			currentOctetVal = newIPE & 0xFF;
			growIPE[octetIndex] = currentOctetVal;
			newIPE >>= 8;
			--octetIndex;
		}

		/*
		if ( growIPE->getUnitLength() != required_octets )
			throw LogicError("Stored data size not equal to required IPE size.");
		*/
		copyUnit(ptrIPE(), growIPE, required_octets);
		// delete growIPE;
	}
	else {
		throw MissingField(MissingField::msg("Encapsulation Packet", "Internet Protocol Extension"));
	}
}

void EncapsulationPacketWithIPE::build(
	const ACE_UINT8 protID,
	const ACE_UINT8 newLoL,
	const ACE_UINT32 pktLen,
	NetworkData* data,
	const ACE_UINT64 ipe,
	const ACE_UINT8 UDF,
	const ACE_UINT8 protIDExt,
	const ACE_UINT16 ccsdsDefinedField /* = 0 */
	) {
	size_t ipeLen = (ipe > 0)? calcIPEOctets(ipe) : 0;

	if (data) clearBuffer(getHeaderOctets(newLoL) + ipeLen + data->getUnitLength());
	else clearBuffer(getHeaderOctets(newLoL) + ipeLen);

	setPacketVersionNumber();
	setProtocolID(protID);
	setLengthOfLength(newLoL);
	if (hasUserDefinedField()) setUserDefinedField(UDF);
	if (hasProtocolIDExtension()) setProtocolIDExtension(protIDExt);
	if (hasCssdsDefinedField()) setCssdsDefinedField(ccsdsDefinedField);
	if (hasIPE() && ipe) setIPE(ipe);
	if (hasData() && data) setData(data);

	if (hasPacketLength()) {
		if (pktLen) setPacketLength(pktLen);
		else setPacketLength(getUnitLength());
	}
}

void EncapsulationPacketPreIPE::build(
	const ACE_UINT8 protID,
	const ACE_UINT8 newLoL,
	const ACE_UINT32 pktLen,
	NetworkData* data,
	const ACE_UINT8 UDF,
	const ACE_UINT8 protIDExt,
	const ACE_UINT16 ccsdsDefinedField /* = 0 */
	) {
	if (data) clearBuffer(getHeaderOctets(newLoL) + data->getUnitLength());
	else clearBuffer(getHeaderOctets(newLoL));

	setPacketVersionNumber();
	setProtocolID(protID);
	setLengthOfLength(newLoL);
	if (hasUserDefinedField()) setUserDefinedField(UDF);
	if (hasProtocolIDExtension()) setProtocolIDExtension(protIDExt);
	if (hasCssdsDefinedField()) setCssdsDefinedField(ccsdsDefinedField);
	if (hasData() && data) setData(data);

	if (hasPacketLength()) {
		if (pktLen) setPacketLength(pktLen);
		else setPacketLength(getUnitLength());
	}
}

} // namespace nasaCE
