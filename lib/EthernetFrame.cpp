/* -*- C++ -*- */

//=============================================================================
/**
 * @file   EthernetFrame.cpp
 * @author Tad Kollar  
 *
 * $Id: EthernetFrame.cpp 1670 2012-08-28 13:25:28Z tkollar $ 
 * Copyright (c) 2008.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "EthernetFrame.hpp"
#include <ace/Log_Msg.h>

namespace nasaCE {

ACE_UINT64 EthernetFrame::getPreamble() const { 
	std::ostringstream err;
	switch (preambleLength_) {
		case LinuxTap:	return to_u16_(ptrPreamble()); break;
		case IEEE:		return to_u64_(ptrPreamble()); break;
		default:
			err << "Unrecognized preamble length (" << preambleLength_ << ") in use.";
			throw PreambleLengthError(err.str());
	}
}

void EthernetFrame::setPreamble(const ACE_UINT64 newVal /* = 0xAAAAAAAAAAAAAAABull */ ) { 
	std::ostringstream err;
	
	switch (preambleLength_) {
		case LinuxTap:	from_u16_((ACE_UINT16) newVal, ptrPreamble()); break;
		case IEEE:		from_u64_( newVal, ptrPreamble()); break;
		default:
			err << "Unrecognized preamble length (" << preambleLength_ << ") in use.";
			throw PreambleLengthError(err.str());
	}
}

ACE_UINT8* EthernetFrame::ptrDstAddr() const {
	std::ostringstream err;
	switch (preambleLength_) {
		case PCap: return ptrUnit(); break;
		case LinuxTap: return ptrPreambleType() + 2; break;
		case IEEE: return ptrPreamble() + 8; break;
		default: 
			err << "Unrecognized preamble length (" << preambleLength_ << ") in use.";
			throw PreambleLengthError(err.str());
	}
	
	return 0;
}

bool EthernetFrame::isExpectedType() const {
	ACE_TRACE("EthernetFrameT::isExpectedType");
	if ( getUnitLength() < minBytesToDetermineLength() )
		throw std::runtime_error("Not enough bytes loaded to determine packet type validity.");

	// Not much to go on here; only useful for packets coming/going through
	// Linux tap devices.
	switch (preambleLength_) {
		case LinuxTap:
			return (getPreamble() == 0) && (getPreambleType() == getEtherType());
		case IEEE:
			break;
	}
	
	// If the value of the type field is recognized, that's a good sign
	ACE_UINT16 eType = getEtherType();
	if ( eType == Eth_IPv4	|| eType == Eth_ARP	||
			eType == Eth_8021q || eType == Eth_IPv6 ) return true;
	
	return false; // We can't determine
}

unsigned EthernetFrame::setBack(ACE_UINT8* buffer, const unsigned bufLen) {
	ACE_TRACE("EthernetFrame::setBack");
	throwIfUninitialized(); // setHeader() has to be called first

	if ( getExpectedLength() != bufLen + minBytesToDetermineLength() ) {
		ND_ERROR("Expected length %d != %d + buffer size %d (type is %04X)!\n",
			getExpectedLength(), minBytesToDetermineLength(), bufLen, getEtherType());
		throw std::runtime_error("Expected Ethernet header/payload length is different than buffer.");
	}
	
	setBufferSize(getExpectedLength());
	copyUnit(ptrUnit() + minBytesToDetermineLength(), buffer, bufLen);

	return getUnitLength();
}

unsigned EthernetFrame::extractPayloadLength() const {
	ACE_TRACE("EthernetFrame::extractPayloadLength");
	
	if ( getPayloadType() <= 1500 ) return getPayloadType();
	
	unsigned len = 0; 
	
	switch ((PayloadType) getPayloadType()) {
		case Eth_IPv4:
			len = to_u16_(ptrPayload() + 2);
			break;
			
		case Eth_ARP:
			len = 46;
			break;
			
		case Eth_IPv6:
			len = 40 + to_u16_(ptrPayload() + 4);
			break;
			
		default:
			throw nd_error("EthernetFrame::extractPayloadLength() attempted on unrecognized payload.");
	}
	
	return len;
}

std::string EthernetFrame::getEtherTypeStr() {
	ACE_TRACE("EthernetFrame::getEtherTypeStr");
	
	switch (getPayloadType()) {
		case Eth_IPv4: return "IPv4";
		case Eth_ARP: return "ARP";
		case Eth_IPv6: return "IPv6";
		default: return "Unhandled type";
	}
}

void EthernetFrame::build(MacAddress& dstMac, MacAddress& srcMac, NetworkData* newPayload /* = 0 */,
	const PayloadType newPayloadType /* = Eth_MaxType */) {

	ACE_TRACE("EthernetFrame::build");
	throwIfInitialized();
	setInitialized_(true);

	if ( newPayload ) 
		setUnitLength(estimate_length(newPayload->getUnitLength(), preambleLength_, typeLen_));
	else
		setUnitLength(estimate_length(0, preambleLength_, typeLen_));
	
	if (preambleLength_ == LinuxTap) { setPreambleType(newPayloadType); }
	setDstAddr(dstMac);
	setSrcAddr(srcMac);
	setEtherType(newPayloadType);
	if ( newPayload) setPayload(newPayload);
}

void EthernetFrame::testPreambleLength_(const int length /* = -1 */ ) const {
	if ( length < 0 ) {
		if  ( preambleLength_ != LinuxTap && preambleLength_ != IEEE )
			throw PreambleLengthError("This Ethernet frame type has no preamble or is not a recognized length.");
	}
	else {
		if ( static_cast<unsigned>(length) != preambleLength_ )
			throw PreambleLengthError("The requested length of the Ethernet preamble does not match the one in use.");
	}
}	

} // namespace nasaCE
