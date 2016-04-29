/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modEthFrame_Remove.cpp
 * @author Tad Kollar  
 *
 * $Id: modEthFrame_Remove.cpp 2344 2013-08-30 20:24:16Z tkollar $
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "AOS_Transfer_Frame.hpp"
#include "modEthFrame_Remove.hpp"
#include "ArpPacket.hpp"
#include "IPv4Packet.hpp"
#include "IPv4_UDP_Datagram.hpp"

namespace nEthFrame_Remove {

modEthFrame_Remove::modEthFrame_Remove(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr):
	BaseTrafficHandler(newName, newChannelName, newDLLPtr), _frameType(EthernetFrame::PCap) {
}

modEthFrame_Remove::~modEthFrame_Remove() {
	MOD_DEBUG("Running ~modEthFrame_Remove().");
}


int modEthFrame_Remove::svc() {
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

		EthernetFrame* frame = dynamic_cast<EthernetFrame*>(queueTop.first);

		if ( ! frame ) {
			MOD_DEBUG("Received data (%s) that was not an Ethernet frame.", queueTop.first->typeStr().c_str());
			ndSafeRelease(queueTop.first);
			queueTop.first = 0;
			continue;
		}

		MOD_DEBUG("Received a %d-octet Ethernet Frame.", frame->getUnitLength());

		if ( links_[PrimaryOutputLink] ) {
			NetworkData* payload = 0;

			switch ( frame->getEtherType() ) {
				case EthernetFrame::Eth_IPv4:
					MOD_DEBUG("Frame contains an IPv4 Packet.");
					payload = frame->wrapInnerPDU<IPv4Packet>();

					if (dynamic_cast<IPv4Packet*>(payload)->getProtocol() == IPv4Packet::UDP) {
						payload->release();
						MOD_DEBUG("Packet is an IPv4 UDP datagram.");
						payload = frame->wrapInnerPDU<IPv4_UDP_Datagram>();
					}
					#ifdef DEFINE_DEBUG
					else if (dynamic_cast<IPv4Packet*>(payload)->getProtocol() == IPv4Packet::TCP) {
						MOD_DEBUG("Packet is an IPv4 TCP packet.");
					}
					else if (dynamic_cast<IPv4Packet*>(payload)->getProtocol() == IPv4Packet::ICMP) {
						MOD_DEBUG("Packet is an IPv4 ICMP packet.");
					}
					else { MOD_DEBUG("Packet is an unrecognized IPv4 type."); }
					#endif
					break;
				case EthernetFrame::Eth_ARP:
					MOD_DEBUG("Frame contains an ARP Packet.");
					payload = frame->wrapInnerPDU<ArpPacket>();
					break;
				case EthernetFrame::Eth_AOS:
					MOD_DEBUG("Frame contains an AOS Transfer Frame.");
					payload = frame->wrapInnerPDU<AOS_Transfer_Frame>();
					break;
				default:
					MOD_DEBUG("Frame contains unrecognized data type.");
					payload = frame->wrapInnerPDU<NetworkData>();
					break;
			}

			MOD_DEBUG("Sending %d octets.", payload->getUnitLength());
			links_[PrimaryOutputLink]->send(payload);
		}
		else {
			MOD_NOTICE("No output target defined yet, dropping data.");
		}

		ndSafeRelease(frame);

	}

	return svcEnd_();
}

} // namespace nEthFrame_Remove
