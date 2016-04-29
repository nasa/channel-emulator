/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modEthFrame_Add.cpp
 * @author Tad Kollar  
 *
 * $Id: modEthFrame_Add.cpp 2404 2013-12-12 19:29:45Z tkollar $
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "modEthFrame_Add.hpp"
#include "IPv4Packet.hpp"
#include "IPv4_UDP_Datagram.hpp"

using namespace nasaCE;

namespace nEthFrame_Add {

modEthFrame_Add::modEthFrame_Add(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr ):
	BaseTrafficHandler(newName, newChannelName, newDLLPtr),
	_frameType(CEcfg::instance()->getOrAddString(cfgKey("frameType"))),
	_dstMAC(CEcfg::instance()->getOrAddString(cfgKey("dstMAC"))),
	_srcMAC(CEcfg::instance()->getOrAddString(cfgKey("srcMAC"))),
	_defaultPayloadType(CEcfg::instance()->getOrAddInt(cfgKey("defaultPayloadType"), 0)),
	_frameHeader(0),
	_frameHeaderCreation(newName.c_str()) {

	if ( strcmp(_frameType.c_str(), "") == 0) _frameType = "PCap";

	if ( strcmp(_dstMAC.c_str(), "") == 0) {
		MacAddress tmpMAC;
		tmpMAC.generateFake(0xFA, false);
		_dstMAC = tmpMAC.addr2str();
	}

	if ( strcmp(_srcMAC.c_str(), "") == 0) {
		MacAddress tmpMAC;
		tmpMAC.generateFake();
		_srcMAC = tmpMAC.addr2str();
	}

	_updateTemplate();

	MOD_DEBUG("Added modEthFrame_Add instance. Src MAC = %s, Dst Mac = %s.", static_cast<const char*>(_srcMAC),
		static_cast<const char*>(_dstMAC) );

}

modEthFrame_Add::~modEthFrame_Add() {
	MOD_DEBUG("Running ~modEthFrame_Add().");
	stopTraffic();
	if (_frameHeader) _frameHeader->release();
	_frameHeader = 0;
}

int modEthFrame_Add::svc() {
	svcStart_();

	NetworkData* payload = 0;

	while ( continueService() ) {
		ndSafeRelease(payload);
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

		payload = queueTop.first;

		if ( msg_queue()->deactivated() ) break;

		MOD_DEBUG("Received a %d-octet %s payload to encapsulate with Ethernet.",
			payload->getUnitLength(), payload->typeStr().c_str());

		if ( links_[PrimaryOutputLink] ) {
			if ( payload->getUnitLength() >= EthernetFrame::Eth_MinPayload ) {
				if ( ! _frameHeader ) {
					MOD_ERROR("Ethernet frame header template does not exist, closing service loop.");
					break;
				}
				_frameHeaderCreation.acquire();

				// Allocate configured frame type.
				EthernetFrame* frame = new EthernetFrame(_frameHeader->getUnitLength() + payload->getUnitLength(),
					getFrameType(), EthernetFrame::typeNormalLen);

				frame->copyUnit(frame->ptrUnit(), _frameHeader);

				_frameHeaderCreation.release();

				frame->setPayload(payload);

				// If defaultPayloadType setting > 0, use that instead of the length parameter
				if ( frame->getEtherType() <= 1500 && getDefaultPayloadType() )
					frame->setEtherType(getDefaultPayloadType());

				// TODO: compute CRC on IEEE frames

				// Send
				MOD_DEBUG("Sending %d-octet Ethernet frame.", frame->getUnitLength());
				links_[PrimaryOutputLink]->send(frame);
				frame = 0;
			}
			else {
				MOD_NOTICE("Payload length %d was less than the minimum %d, not sending.",
						   payload->getUnitLength(),
						   EthernetFrame::Eth_MinPayload);
			}
		}
		else {
			MOD_NOTICE("No output target defined yet, dropping data.");
		}
	}
	svcStart_();
	return svcEnd_();

}

void modEthFrame_Add::_updateTemplate() {
	_frameHeaderCreation.acquire();
	if ( _frameHeader) _frameHeader->release();
	_frameHeader = new EthernetFrame(getFrameType());

	if ( ! _frameHeader ) {
		MOD_ERROR("Failed to allocate new Ethernet frame header template!");
	}
	else {
		MacAddress dst(_dstMAC), src(_srcMAC);
		_frameHeader->build(dst, src);
	}
	_frameHeaderCreation.release();
}

} // namespace nEthFrame_Add
