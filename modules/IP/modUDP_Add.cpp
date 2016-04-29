/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modUDP_Add.cpp
 * @author Tad Kollar  
 *
 * $Id: modUDP_Add.cpp 2344 2013-08-30 20:24:16Z tkollar $
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "modUDP_Add.hpp"

namespace nUDP_Add {

modUDP_Add::modUDP_Add(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr):
	BaseTrafficHandler(newName, newChannelName, newDLLPtr),
	_srcIPAddrStr(CEcfg::instance()->getOrAddString(cfgKey("srcAddr"), "0.0.0.0")),
	_srcPort(CEcfg::instance()->getOrAddInt(cfgKey("srcPort"), 0)),
	_dstIPAddrStr(CEcfg::instance()->getOrAddString(cfgKey("dstAddr"), "0.0.0.0")),
	_dstPort(CEcfg::instance()->getOrAddInt(cfgKey("dstPort"), 0)),
	_compute_CRC(CEcfg::instance()->getOrAddBool(cfgKey("useUDPCRC"), false)),
	_dgmHeader(0) {

	MOD_DEBUG("Initializing with source address %s:%d, dest address %s:%d; %susing UDP CRC.",
			  static_cast<const char*>(_srcIPAddrStr),
			  static_cast<int>(_srcPort),
			  static_cast<const char*>(_dstIPAddrStr),
			  static_cast<int>(_dstPort),
			  (static_cast<bool>(_compute_CRC))? "" : "not "
 			);

	_dgmHeader = new IPv4_UDP_Datagram();
	_updateTemplate();
}

modUDP_Add::~modUDP_Add() {
	MOD_DEBUG("Running ~modUDP_Add().");
	stopTraffic();
	ndSafeRelease(_dgmHeader);
}

int modUDP_Add::svc() {
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

		NetworkData* data = queueTop.first;

		MOD_DEBUG("Received %d bytes to make into a UDP datagram.", data->getUnitLength());

		if ( links_[PrimaryOutputLink] ) {
			IPv4_UDP_Datagram* dgm = new IPv4_UDP_Datagram(_dgmHeader);
			dgm->setData(data);
			dgm->setChecksum(dgm->computeChecksum());
			if (isComputingCRC()) dgm->setUDPChecksum(dgm->computeUDPChecksum());

			MOD_DEBUG("Sending %d bytes.", dgm->getUnitLength());
			links_[PrimaryOutputLink]->send(dgm);
		}
		else {
			MOD_NOTICE("No output target defined yet, dropping data.");
		}

		ndSafeRelease(data);
	}

	return svcEnd_();

}

void modUDP_Add::_updateTemplate() {
	_dgmHeader->build(getSrcIPAddr(), getSrcPort(), getDstIPAddr(), getDstPort());
}

}
