/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modEncapPkt_Remove.cpp
 * @author Tad Kollar  
 *
 * $Id: modEncapPkt_Remove.cpp 2344 2013-08-30 20:24:16Z tkollar $
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "modEncapPkt_Remove.hpp"
#include "EncapsulationPacket.hpp"
#include "IPv4_UDP_Datagram.hpp"

namespace nEncapPkt_Remove {

modEncapPkt_Remove::modEncapPkt_Remove(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr /* = 0 */):
	BaseTrafficHandler(newName, newChannelName, newDLLPtr),
	_ipeSupport(true),
	_ipeSupportSetting(CEcfg::instance()->getOrAddBool(cfgKey("supportIPE"), _ipeSupport)) {

	_ipeSupport = _ipeSupportSetting;
}

modEncapPkt_Remove::~modEncapPkt_Remove() {
	MOD_DEBUG("Running ~modEncapPkt_Remove().");
}


int modEncapPkt_Remove::svc() {
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

		EncapsulationPacketBase* encap_pkt;

		if (getIpeSupport()) {
			encap_pkt = dynamic_cast<EncapsulationPacketWithIPE*>(queueTop.first);
		}
		else {
			encap_pkt = dynamic_cast<EncapsulationPacketPreIPE*>(queueTop.first);
		}

		if ( ! encap_pkt ) {
			MOD_INFO("Received %d octets of non-Encapsulation Packet data! Dropping.",
				queueTop.first->getUnitLength());
			ndSafeRelease(queueTop.first);
			queueTop.first = 0;
			continue;
		}

		if ( encap_pkt->hasPacketLength() && encap_pkt->getUnitLength() != encap_pkt->getPacketLength() ) {
			MOD_WARNING("Encap buffer size (%d) doesn't match Packet Length value (%d)! Dropping.",
				encap_pkt->getUnitLength(), encap_pkt->getPacketLength());
			ndSafeRelease(encap_pkt);
			continue;
		}

		MOD_DEBUG("Received a %d-octet Encapsulation Packet to extract.", encap_pkt->getUnitLength());

		if ( links_[PrimaryOutputLink]  ) {

			NetworkData* data = 0;

			// If the packet contains an IPv4 Datagram, wrap the data field with an IPv4Packet.
			if ( (  getIpeSupport() && dynamic_cast<EncapsulationPacketWithIPE*>(encap_pkt)->hasIPE() &&
					dynamic_cast<EncapsulationPacketWithIPE*>(encap_pkt)->getIPE() == EncapsulationPacketWithIPE::IPv4_Datagram ) ||
				 (! getIpeSupport() && dynamic_cast<EncapsulationPacketPreIPE*>(encap_pkt)->getProtocolID() == EncapsulationPacketPreIPE::IPv4) ) {

				data = encap_pkt->wrapInnerPDU<IPv4Packet>();

				// Get more specific if the IPv4 unit is actually a UDP datagram.
				if ( dynamic_cast<IPv4Packet*>(data)->getProtocol() == IPv4Packet::UDP) {
					ndSafeRelease(data);
					MOD_DEBUG("Packet is an IPv4 UDP datagram.");
					data = encap_pkt->wrapInnerPDU<IPv4_UDP_Datagram>();
				}
			}
			// Otherwise, if there's data in there, send it on untyped.
			else if ( encap_pkt->hasData() ) {
				data = encap_pkt->wrapInnerPDU<NetworkData>();
			}
			// Nothing else to do with it but ignore.
			else {
				MOD_DEBUG("Dropping fill data.");
			}

			MOD_DEBUG("Sending %d octets.", data->getUnitLength());
			links_[PrimaryOutputLink]->send(data);

		}
		else {
			MOD_NOTICE("No output target defined yet, dropping data.");
		}

		ndSafeRelease(encap_pkt);
	}

	return svcEnd_();

}

}
