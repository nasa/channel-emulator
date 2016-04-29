/* -*- C++ -*- */

//=============================================================================
/**
 * @file   IPv4Packet.cpp
 * @author Tad Kollar  
 *
 * $Id: IPv4Packet.cpp 1670 2012-08-28 13:25:28Z tkollar $ 
 * Copyright (c) 2008.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "IPv4Packet.hpp"

// Note: IPv4Packet:: scope is explicit here because many referenced
// functions will be overloaded again by derived class. When that
// happens without the scope being specified, the checksum is computed
// incorrectly and fragmentation doesn't work right because the wrong
// payload and header lengths are returned.

namespace nasaCE {

void IPv4Packet::defragment(ACE_UINT8* newDataBuf, const bool freeBuf) {
	size_t bufLen = getOffset() * 8 + IPv4Packet::getDataLength();
	size_t newTotalLen = IPv4Packet::getHeaderLength() + bufLen; 

	// Resize the packet buffer
	setUnitLength(newTotalLen);
	
	// No more fragment offset or multiple fragments flag
	setFlags(getFlags() & ~(IP_OFFMASK | IP_MF));
	
	// Copy the new data
	ACE_OS::memcpy(IPv4Packet::ptrData(), newDataBuf, bufLen);
	
	// Get rid of the old buffer if requested
	if ( freeBuf ) free(newDataBuf);
	
	// Set a new total length
	setTotalLength(getUnitLength());
	
	// No more fragment identity
	setIdent(0);
	
	// Recompute the checksum.
	setChecksum(computeChecksum());
};

ACE_UINT16 IPv4Packet::computeChecksum() const {
	ACE_UINT32 checksum = 0;

	// Add up everything in the header.
	for (ACE_UINT8* ptr = ptrUnit(); ptr < IPv4Packet::ptrData(); ptr += 2)
		if ( ptr != ptrChecksum() ) // Ignore the current checksum
			checksum += to_u16_((ACE_UINT8*) ptr);

	// Take only 16 bits out of the 32 bit sum and add up the carries
	while (checksum >> 16)
		checksum = (checksum & 0xFFFF) + (checksum >> 16);

	// One's complement the result
	checksum = ~checksum;
		
	return (ACE_UINT16) checksum;
}

IPv4Packet* IPv4Packet::_make_one_fragment(const ACE_UINT16 offset,
	const ACE_UINT16 dataLen, const ACE_UINT16 id, const bool isLast) {
	ACE_UINT16 totalLen = IPv4Packet::getHeaderLength() + dataLen;
	
	IPv4Packet *ipFrag = new IPv4Packet(totalLen);
	
	// Copy our own header to the new packet
	ipFrag->copyUnit(ipFrag->ptrUnit(), ptrUnit(), IPv4Packet::getHeaderLength());
	
	// Copy the specified amount of data
	ipFrag->copyUnit(ipFrag->ptrData(), ptrData() + offset, dataLen);
	
	// Make some changes specific to the fragment
	ipFrag->setIdent(id);
	
	if (isLast) ipFrag->setFlags((getFlags() | (offset/8)) & ~IP_MF);
	else ipFrag->setFlags(getFlags() | IP_MF | (offset/8));
	
	ipFrag->setTotalLength(totalLen);
	ipFrag->setChecksum(ipFrag->computeChecksum());
	return ipFrag;	
}

void IPv4Packet::fragment(std::vector<IPv4Packet*>& frags, const ACE_UINT16 maxLen) {
	// Fragment offsets must be divisible by 8 except for the last.
	ACE_UINT16 maxData = ((maxLen - IPv4Packet::getHeaderLength()) / 8) * 8;
	ACE_UINT16 p;
	
	// Generate a random value for the ID field
	srand48(ACE_OS::time(0));
	ACE_UINT16 common_id = lrand48() % USHRT_MAX;
	
	for ( p = 0; p + maxData < IPv4Packet::getDataLength(); p += maxData ) {
		frags.push_back(_make_one_fragment(p, maxData, common_id));
	}
	
	frags.push_back(_make_one_fragment(p, IPv4Packet::getDataLength() - p, common_id, true));
}

void IPv4Packet::build(const IPv4Addr& srcIPAddr, const IPv4Addr& dstIPAddr, NetworkData* data = 0,
	const ACE_UINT8 ttl, const ACE_UINT8 prot) {
	const unsigned headerLen = 20;
	
	ACE_UINT8 initHeader[headerLen] = {
		0x45, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x40, 0x00,
		ttl,  prot, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, // Source IP Address
		0x00, 0x00, 0x00, 0x00  // Destination IP Address
	};
	
	if (data) this->clearUnit(headerLen + data->getUnitLength());
	else this->clearUnit(headerLen);
	
	copyUnit(ptrUnit(), initHeader, headerLen);
	
	setSrc(srcIPAddr);
	setDst(dstIPAddr);
	if (data) IPv4Packet::setData(data);
	// setChecksum(computeChecksum());
}

void IPv4Packet::throwIfInvalid() const {
	if ( getUnitLength() < minimumLength() )
		throw IPv4Packet::UnderMinLength(UnderMinLength::msg(IPv4Packet::typeStr(), minimumLength(), getUnitLength()));

	if ( ! hasValidChecksum() )
		throw IPv4Packet::InvalidChecksum(InvalidChecksum::msg(IPv4Packet::typeStr(), "IPv4 Header CRC"));
		
	if ( getTotalLength() != getUnitLength() )
		throw IPv4Packet::WrongSize(WrongSize::msg(IPv4Packet::typeStr(), getTotalLength(), getUnitLength()));
}

} // namespace nasaCE
