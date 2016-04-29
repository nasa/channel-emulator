/* -*- C++ -*- */

//=============================================================================
/**
 * @file   AOS_Bitstream_PDU.cpp
 * @author Tad Kollar  
 *
 * $Id: AOS_Bitstream_PDU.cpp 1670 2012-08-28 13:25:28Z tkollar $ 
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "AOS_Bitstream_PDU.hpp"

namespace nasaCE {
const ACE_UINT16 AOS_Bitstream_PDU::AllOnes = 0x3FFF;
const ACE_UINT16 AOS_Bitstream_PDU::AllOnesMinusOne = 0x3FFE;

const size_t AOS_Bitstream_PDU::spanHeader = 2;

size_t AOS_Bitstream_PDU::addFill(const size_t fillIdx, const ACE_UINT8 fillVal /* = 0xE0 */) {
	long fillCount = getDataLength() - fillIdx;
	if ( fillCount > 0 ) {
		ACE_OS::memset(ptrData() + fillIdx, fillVal, fillCount);
		return fillCount;
	}
	return 0;		
}

} // namespace nasaCE
