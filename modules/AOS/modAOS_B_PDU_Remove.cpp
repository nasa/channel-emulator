/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modAOS_B_PDU_Remove.cpp
 * @author Tad Kollar  
 *
 * $Id: modAOS_B_PDU_Remove.cpp 2344 2013-08-30 20:24:16Z tkollar $
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "modAOS_B_PDU_Remove.hpp"
#include "IPv4_UDP_Datagram.hpp"

namespace nAOS_B_PDU_Remove {

modAOS_B_PDU_Remove::modAOS_B_PDU_Remove(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr):
	BaseTrafficHandler(newName, newChannelName, newDLLPtr) {
}

modAOS_B_PDU_Remove::~modAOS_B_PDU_Remove() {
	MOD_DEBUG("Running ~modAOS_B_PDU_Remove().");
}

int modAOS_B_PDU_Remove::svc() {
	svcStart_();

	while ( continueService() ) {
		std::pair<NetworkData*, int> queueTop = getData_();
		if ( msg_queue()->deactivated() ) break;

		if ( queueTop.second < 0 ) {
			MOD_ERROR("getData_() call failed.");
			continue;
		}
		else if ( ! queueTop.first ) {
			MOD_ERROR("getData_() returned with null data.");
			continue;
		}

		AOS_Bitstream_PDU* b_pdu = dynamic_cast<AOS_Bitstream_PDU*>(queueTop.first);

		if ( ! b_pdu ) {
			MOD_WARNING("Received data (%s) that was not a Bitstream B_PDU.", queueTop.first->typeStr().c_str());
			ndSafeRelease(queueTop.first);
			queueTop.first = 0;
			continue;
		}

		MOD_DEBUG("Received a %d-octet Bitstream B_PDU to extract.", b_pdu->getUnitLength());

		if (b_pdu->isAllIdle()) {
			MOD_DEBUG("Encountered B_PDU with 100% idle data.");
		}
		else if ( links_[PrimaryOutputLink] ) {
			size_t dataLen = 0;

			if ( b_pdu->isAllData() ) dataLen = b_pdu->getDataLength();
			else dataLen = b_pdu->getBitstreamDataEnd() + 1;

			if ( dataLen > b_pdu->getDataLength() ) {
				MOD_WARNING("Bitstream data pointer (%d) exceeds Data Zone length (%d), dropping.", dataLen, b_pdu->getDataLength());
			}
			else if ( ! dataLen ) {
				MOD_WARNING("Got data length of zero in B_PDU not marked idle.");
			}
			else {
				NetworkData* outgoingData = b_pdu->wrapInnerPDU<NetworkData>(dataLen);
				MOD_DEBUG("Sending %d octets.", outgoingData->getUnitLength());

				// Don't dereference data after this point, it may be deleted.
				links_[PrimaryOutputLink]->send(outgoingData);
			}
		}
		else {
			MOD_NOTICE("No output target defined yet, dropping data.");
		}

		ndSafeRelease(b_pdu);
	}

	return svcEnd_();

}

}
