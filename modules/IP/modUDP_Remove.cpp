/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modUDP_Remove.cpp
 * @author Tad Kollar  
 *
 * $Id: modUDP_Remove.cpp 2344 2013-08-30 20:24:16Z tkollar $
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "modUDP_Remove.hpp"
#include "IPv4_UDP_Datagram.hpp"

namespace nUDP_Remove {

modUDP_Remove::modUDP_Remove(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr):
	BaseTrafficHandler(newName, newChannelName, newDLLPtr),
	_check_CRC(CEcfg::instance()->getOrAddBool(cfgKey("useUDPCRC"), false)),
	_badIPCRCCount(0),
	_badUDPCRCCount(0) {

	MOD_DEBUG("Using check-CRC flag (%s).", (static_cast<bool>(_check_CRC))? "true" : "false");
}

modUDP_Remove::~modUDP_Remove() {
	MOD_DEBUG("Running ~modUDP_Remove().");
}

bool modUDP_Remove::_goodCRC(IPv4_UDP_Datagram* dgm) {
	if (isCheckingCRC()) {
		if ( ! dgm->hasValidChecksum() ) {
			MOD_INFO("Dropping datagram with bad IPv4 checksum.");
			incBadIPCRCCount();
			return false;
		}
		else if ( ! dgm->hasValidUDPChecksum() ) {
			MOD_INFO("Dropping datagram with bad UDP checksum.");
			incBadUDPCRCCount();
			return false;
		}
	}

	return true;
}

int modUDP_Remove::svc() {
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

		IPv4_UDP_Datagram* dgm = dynamic_cast<IPv4_UDP_Datagram*>(queueTop.first);

		if ( ! dgm ) {
			MOD_INFO("Received %d octets of non-UDP Datagram data! Dropping.",
				queueTop.first->getUnitLength());
			ndSafeRelease(queueTop.first);
			queueTop.first = 0;
			continue;
		}

		MOD_DEBUG("Received a %d byte UDP datagram to extract.", dgm->getUnitLength());

		if ( links_[PrimaryOutputLink] ) {
			if (_goodCRC(dgm)) {

				NetworkData* data = dgm->wrapInnerPDU<NetworkData>();

				MOD_DEBUG("Sending %d octets.", data->getUnitLength());
				links_[PrimaryOutputLink]->send(data);
			}
		}
		else {
			MOD_NOTICE("No output target defined yet, dropping data.");
		}

		ndSafeRelease(dgm);

	}

	return svcEnd_();

}

}
