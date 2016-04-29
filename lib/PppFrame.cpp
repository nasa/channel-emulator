/* -*- C++ -*- */

//=============================================================================
/**
 * @file   PppFrame.cpp
 * @author Tad Kollar  
 *
 * $Id: PppFrame.cpp 1670 2012-08-28 13:25:28Z tkollar $ 
 * Copyright (c) 2008.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "PppFrame.hpp"

namespace nasaCE {

void PppFrame::build(const ACE_UINT16 protocol, NetworkData* data, const ACE_UINT8 fcsLen) {
	ACE_TRACE("PppFrame::build");
	throwIfInitialized();
	
	setAddressLength_(1);
	setControlLength_(1);
	setFCSLength_(fcsLen);
	clearBuffer(nonDataLength() + data->getUnitLength());
	setInitialized_(true);

	setStaticVals();
	setProtocolVal(protocol);
	setData(data, false);
	
	switch (getFCSLength()) {
		case 2:
			setFcsVal16(0, false);
			setFcsVal16(fcs16());
			break;
		case 4:
			setFcsVal32(0, false);
			setFcsVal32(fcs32());
			break;
		default:
			throw std::runtime_error("FCS length not set to either 2 or 4.");
	}
}

void PppFrame::escape(std::bitset<256>& accm) {
	int bufIdx = 0;
	ACE_UINT8 buf[getUnitLength() * 2];
	ACE_UINT8 cur;
	
	buf[bufIdx++] = PppFrame::flagSeq; // Prepend the flag sequence
	
	for ( unsigned idx = 1; idx < getUnitLength() - 1; idx++ ) { 
		cur = *(ptrUnit() + idx);
		if ( accm[cur] ) {
			// ND_DEBUG("escaping %02X\n", cur);	
			buf[bufIdx++] = PppFrame::escSeq;
			buf[bufIdx++] = cur ^ 0x20;
		}		
		else buf[bufIdx++] = cur;
	}			

	buf[bufIdx++] = PppFrame::flagSeq; // Append the flag sequence
	replicateBuffer(buf, bufIdx);
}

void PppFrame::unescape() {
	int bufIdx = 0;
	ACE_UINT8 buf[getUnitLength() * 2];
	ACE_UINT8 cur;
	bool escaped = false;
	
	for ( unsigned idx = 0; idx < getUnitLength(); ++idx ) {
		cur = *(ptrUnit() + idx);
		
		if (escaped) {
			buf[bufIdx++] = cur ^ 0x20;		
			escaped = false;
		}
		else if (cur == PppFrame::escSeq) { escaped = true; }
		else { buf[bufIdx++] = cur; }
	}
	
	replicateBuffer(buf, bufIdx);				
}

} // namespace nasaCE
