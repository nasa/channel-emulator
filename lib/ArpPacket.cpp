/* -*- C++ -*- */

//=============================================================================
/**
 * @file   ArpPacket.cpp
 * @author Tad Kollar  
 *
 * $Id: ArpPacket.cpp 1670 2012-08-28 13:25:28Z tkollar $ 
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "ArpPacket.hpp"

namespace nasaCE {

// TODO: This is probably only going to work with IPv4
void ArpPacket::buildReply(ArpPacket* request, MacAddress& requestedMAC) {
	ACE_TRACE("ArpPacket::buildReply");

	throwIfInitialized();
	
	IPv4Addr requestedIP(request->targetIP());
	
	clearBuffer(DefaultSize);
	setInitialized_(true);
	
	ACE_UINT8 senderMAC[6];
	ACE_OS::memcpy(senderMAC, requestedMAC.address(), 6);

	ACE_UINT8 manyVals[8] = { 0x00, 0x01, 0x08, 0x00, 0x06, 0x04, 0x00, Reply };
	copyUnit(ptrUnit(), manyVals, 8);
	
	// Set the sender MAC in the ARP payload
	copyUnit(senderMACPtr(), senderMAC, 6);
	
	// Set the sender IP
	copyUnit(senderIPPtr(), request->targetIPPtr(), 4);
	
	// Set the target MAC
	copyUnit(targetMACPtr(), request->senderMACPtr(), 6);
	
	// Set the target IP
	copyUnit(targetIPPtr(), request->senderIPPtr(), 4);
}

void ArpPacket::buildRequest(const IPv4Addr& requestedIP, MacAddress& myMac, const IPv4Addr& myIp) {	
	ACE_TRACE("ArpPacket::buildRequest");
	throwIfInitialized();
			
	clearBuffer(DefaultSize);
	setInitialized_(true);

	ACE_UINT8 manyVals[8] = { 0x00, 0x01, 0x08, 0x00, 0x06, 0x04, 0x00, Request };
	copyUnit(ptrUnit(), manyVals, 8);
	
	// Set the sender MAC in the ARP payload
	copyUnit(senderMACPtr(), myMac.address(), 6);
	
	// Set the sender IP
	*((ACE_UINT32*) senderIPPtr()) = myIp.get();
	
	// Don't bother setting the target MAC since it's what we're asking for, so
	// it needs to be 00:00:00:00:00:00, and we already calloc'ed the space.
	
	// Set the target IP
	*((ACE_UINT32*) targetIPPtr()) = requestedIP.get();
}

} // namespace nasaCE
