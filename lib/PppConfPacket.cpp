/* -*- C++ -*- */

//=============================================================================
/**
 * @file   PppConfPacket.cpp
 * @author Tad Kollar  
 *
 * $Id: PppConfPacket.cpp 1670 2012-08-28 13:25:28Z tkollar $ 
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "PppConfPacket.hpp"

namespace nasaCE {

std::string PppConfPacket::codeTypeStr[ControlCodeCount] = {
	"Vendor_Specific",
	"Configure_Request", 
	"Configure_Ack",
	"Configure_Nak",
	"Configure_Reject",
	"Terminate_Request",
	"Terminate_Ack",
	"Code_Reject",
	"Protocol_Reject",
	"Echo_Request",
	"Echo_Reply",
	"Discard_Request",
	"Identification",
	"Time_Remaining"
};

PppConfOption* PppConfPacket::optionAt (const int optIdx) {
	ACE_TRACE("PppConfPacket::optionAt");
	
	int optNum = 0;
	
	if (codeVal() != Configure_Request &&
		codeVal() != Configure_Ack &&
		codeVal() != Configure_Nak)
			throw std::runtime_error("No options in this type of packet.");
	
	ACE_UINT8 optType, bufLen;
	ACE_UINT8* optPtr;
	int optPos = 0;
	
	do {
		optType = *(dataPtr() + optPos);
		bufLen = *(dataPtr() + optPos + 1) - 2;
		optPtr = dataPtr() + optPos + 2;
		optPos += bufLen + 2;
	} while ( optPos < dataLength() && optNum++ <= optIdx );
	
	if ( optNum != optIdx )
		throw std::runtime_error("Could not reach specified option index.");
		
	// TODO: use an auto_ptr here so there's no worry about deleting later
	return new PppConfOption(optType, bufLen, optPtr);
}

unsigned PppConfPacket::countOpts() {
	unsigned optNum = 0;
	for (int optPos = 0; optPos < dataLength(); optNum++ ) {
		ACE_UINT8 bufLen = *(dataPtr() + optPos + 1) - 2;
		optPos += bufLen + 2;
	}
			
	return optNum;
}

void PppConfPacket::appendOption(PppConfOption* opt) {
	unsigned newLen = getUnitLength() + opt->bufLen + 2;
	setUnitLength(newLen);
	setLengthVal(newLen); // Change the length within the packet.
	
	// Point to the new empty space at the end of the buffer.
	// Can't get this before changing the size in case the buffer is moved.
	ACE_UINT8* ptr = ptrUnit() + getUnitLength() - (opt->bufLen + 2); 
	
	*ptr = opt->optType;
	++ptr;
	*ptr = opt->bufLen + 2;
	++ptr;
	copyBuffer(ptr, opt->optBuf, opt->bufLen);
}

void PppConfPacket::build(const ACE_UINT8 code, const ACE_UINT8 identifier, 
	std::vector<PppConfOption*>& options) {
	
	setUnitLength(nonDataLength());
	setInitialized_(true);
	
	setCodeVal(code);
	setIdentVal(identifier);
	setLengthVal(nonDataLength());
	
	std::vector<PppConfOption*>::iterator pos;
	for (pos = options.begin(); pos != options.end(); ++pos) {
		appendOption(*pos);
	}
}

} // namespace nasaCE
