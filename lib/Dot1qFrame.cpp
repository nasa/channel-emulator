/* -*- C++ -*- */

//=============================================================================
/**
 * @file   Dot1qFrame.cpp
 * @author Tad Kollar  
 *
 * $Id: Dot1qFrame.cpp 1670 2012-08-28 13:25:28Z tkollar $ 
 * Copyright (c) 2008.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "Dot1qFrame.hpp"

namespace nasaCE {

void Dot1qFrame::build(MacAddress& dstMac, MacAddress& srcMac,
	NetworkData* newPayload, ACE_UINT16 newPayloadType,
	ACE_UINT16 new8021qTag) {
	
	ACE_TRACE("Dot1qFrame::build");
	throwIfInitialized();
	
	setTypeLen_(type8021qLen);
	
	setUnitLength(estimate_length(newPayload->getUnitLength(), preambleLength_, typeLen_));
	setInitialized_(true);
	
	if (preambleLength_ == LinuxTap) { setPreambleType(newPayloadType); }
	setDstAddr(dstMac);
	setSrcAddr(srcMac);
	
	setEtherType(Eth_8021q);
	setTag(new8021qTag);
	setEther8021qType(newPayloadType);
	setPayload(newPayload);
}

} // namespace nasaCE
