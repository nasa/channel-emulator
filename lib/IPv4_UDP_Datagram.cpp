/* -*- C++ -*- */

//=============================================================================
/**
 * @file   IPv4_UDP_Datagram.cpp
 * @author Tad Kollar  
 *
 * $Id: IPv4_UDP_Datagram.cpp 1670 2012-08-28 13:25:28Z tkollar $ 
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "IPv4_UDP_Datagram.hpp"

namespace nasaCE {

const size_t IPv4_UDP_Datagram::spanUDPSrcPort = 2;
const size_t IPv4_UDP_Datagram::spanUDPDstPort = 2;
const size_t IPv4_UDP_Datagram::spanUDPLength = 2;
const size_t IPv4_UDP_Datagram::spanUDPChecksum = 2;

void IPv4_UDP_Datagram::setData(NetworkData* data) {
	if ( getUnitLength() != data->getUnitLength() + getHeaderLength() )
		setUnitLength(getHeaderLength() + data->getUnitLength());
	NetworkData::setData(data);
	setTotalLength(getUnitLength());
	setUDPLength(getUnitLength() - IPv4Packet::getHeaderLength());
}

void IPv4_UDP_Datagram::build(const IPv4Addr& srcIPAddr, const ACE_UINT16 srcPort,
	const IPv4Addr& dstIPAddr, const ACE_UINT16 dstPort, NetworkData* data /* = 0 */,
	const ACE_UINT8 ttl /* = 63 */) {
	this->IPv4Packet::build(srcIPAddr, dstIPAddr, 0, ttl, IPv4Packet::UDP);
	
	size_t newSize = (data)? getHeaderLength() + data->getUnitLength() : getHeaderLength();
	
	setUnitLength(newSize);
	ACE_OS::memset(ptrSrcPort(), 0, newSize - IPv4Packet::getHeaderLength());
	
	setSrcPort(srcPort);
	setDstPort(dstPort);
	if (data) setData(data);
	
	// setChecksum(computeChecksum());
}

ACE_UINT16 IPv4_UDP_Datagram::computeUDPChecksum(const ACE_UINT16 protocol) const {
	ACE_UINT16 word = 0;
	ACE_UINT32 sum = 0;

	// ** Calculate the pseudo-header checksum first **
	// Add in the source address
	sum += to_u16_(ptrSrc());
	sum += to_u16_(ptrSrc() + 2);
	
	// Add in the dest address
	sum += to_u16_(ptrDst());
	sum += to_u16_(ptrDst() + 2);		
		
	sum += protocol + to_u16_(ptrUDPLength());
	// ** Now add in the actual datagram **
	ACE_UINT8* ptr = ptrSrcPort();
	unsigned idx;
	
	if ( getUnitLength() % 2 ) { // Odd
		for ( idx = 0; idx < getUDPLength() - 1U ; idx += 2U ) {
			word = (unsigned long)to_u16_(ptr + idx);
			sum += word;		
		}
		
		// Pretend there's one more byte and it's zero.
		word = *(ptr + idx);
		word <<= 8;
		sum += word;
	}
	else { // Even
		for ( idx = 0; idx < getUDPLength(); idx += 2 ) {
			word = to_u16_(ptr + idx);
			sum += word;
		}
	}

	// keep only the last 16 bits of the 32 bit calculated sum and add the carries
	while (sum >> 16) sum = (sum & 0xFFFF) + (sum >> 16);

	// One's complement
	sum = ~sum;
	
	// If the calculation results in 0, return 0xFFFF so that
	// it's known that the CRC is in use.
	if ( sum == 0 ) sum = 0xFFFF;
				
	return ((ACE_UINT16) sum);
}

void IPv4_UDP_Datagram::throwIfInvalid(const bool checkIPv4 /* = true */) const {
	if ( checkIPv4) IPv4Packet::throwIfInvalid();
	if ( getUnitLength() < minimumLength() )
		throw IPv4_UDP_Datagram::UnderMinLength(UnderMinLength::msg(typeStr(), minimumLength(), getUnitLength()));
		
	if ( getUDPLength() + IPv4Packet::getHeaderLength() != getUnitLength() )
		throw IPv4_UDP_Datagram::WrongSize(WrongSize::msg(typeStr(), getUDPLength(), getUnitLength()));		
		
	if ( ! hasValidUDPChecksum() )
		throw IPv4_UDP_Datagram::InvalidChecksum(InvalidChecksum::msg(typeStr(), "UDP CRC"));

}

} // namespace nasaCE
