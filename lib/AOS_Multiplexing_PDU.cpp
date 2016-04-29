/* -*- C++ -*- */

//=============================================================================
/**
 * @file   AOS_Multiplexing_PDU.cpp
 * @author Tad Kollar  
 *
 * $Id: AOS_Multiplexing_PDU.cpp 1670 2012-08-28 13:25:28Z tkollar $ 
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "AOS_Multiplexing_PDU.hpp"

namespace nasaCE {

const unsigned char AOS_Multiplexing_PDU::spanHeader = 2;
const ACE_UINT16 AOS_Multiplexing_PDU::HasIdleVal = 0x7FE;
const ACE_UINT16 AOS_Multiplexing_PDU::HasHeaderlessVal = 0x7FF;

void AOS_Multiplexing_PDU::build(
	const ACE_UINT16 firstHeaderIdx,
	NetworkData* data /* = 0 */,
	const size_t packetZoneSize /* = 0 */,
	const NetworkData* fill /* = 0 */ ) {
	
	setInitialized_();
	
	if (data) {
		if (packetZoneSize) {
			if ( packetZoneSize < data->getUnitLength())
				throw BufferOverflow(BufferOverflow::msg("Packet Zone", data->getUnitLength(), packetZoneSize));
			else
				clearUnit(getHeaderLength() + packetZoneSize);
		}
		else { clearUnit(getHeaderLength() + data->getUnitLength()); }
	}
	else {
		clearUnit(packetZoneSize? getHeaderLength() + packetZoneSize : getHeaderLength());
	}
	
	setRsvdSpare(0);
	setFirstHeaderIndex(firstHeaderIdx);
	if (data) { 
		setData(data);
		if (fill) addFill(data->getUnitLength(), fill);
	}
}

size_t AOS_Multiplexing_PDU::addFill(const size_t fillIdx, const NetworkData* fill ) {

	size_t fillCount = getDataLength() - fillIdx;
	size_t copySize;
	
	for ( size_t i = 0; i < fillCount; i += fill->getUnitLength() ) {
		copySize = ( fillCount - i >= fill->getUnitLength() ) ? fill->getUnitLength() : fillCount - i;
		ACE_OS::memcpy(ptrData() + fillIdx + i, fill->ptrUnit(), copySize);
	}
	
	return 0;
}

} // namespace nasaCE
