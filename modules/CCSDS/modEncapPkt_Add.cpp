/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modEncapPkt_Add.cpp
 * @author Tad Kollar  
 *
 * $Id: modEncapPkt_Add.cpp 2344 2013-08-30 20:24:16Z tkollar $
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "modEncapPkt_Add.hpp"
#include "EncapsulationPacket.hpp"

namespace nEncapPkt_Add {

// The IPE can't be an even value, so if set to 2 we'll know to ignore the override.
const ACE_UINT64 modEncapPkt_Add::unsetIpeVal = 2UL;

// Protocol must be 0 - 8, so if it's higher we know to ignore it.
const ACE_UINT8 modEncapPkt_Add::unsetProtocolVal = 0xFF;

modEncapPkt_Add::modEncapPkt_Add(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr /* = 0 */):
	BaseTrafficHandler(newName, newChannelName, newDLLPtr),

	_ipeSupport(true),
	_ipeSupportSetting(CEcfg::instance()->getOrAddBool(cfgKey("supportIPE"), _ipeSupport)),

	_ipe(unsetIpeVal),
	_protocol(unsetProtocolVal),

	_lengthOfLength(2),
	_lengthOfLengthSetting(CEcfg::instance()->getOrAddInt(cfgKey("lengthOfLength"), _lengthOfLength)),

	_userDefinedField(0),
	_userDefinedFieldSetting(CEcfg::instance()->getOrAddInt(cfgKey("userDefinedField"), _userDefinedField)),

	_protocolIdExt(0),
	_protocolIdExtSetting(CEcfg::instance()->getOrAddInt(cfgKey("protocolIDExtension"), _protocolIdExt)),

	_ccsdsDefinedField(0),
	_ccsdsDefinedFieldSetting(CEcfg::instance()->getOrAddInt(cfgKey("CCSDSDefinedField"), _ccsdsDefinedField))
	 {

	setIpeSupport(_ipeSupportSetting);
	if ( CEcfg::instance()->exists(cfgKey("IPE")) )
		setIpe(static_cast<long long>(CEcfg::instance()->get(cfgKey("IPE"))));

	if ( CEcfg::instance()->exists(cfgKey("protocol")) )
		setProtocol(static_cast<int>(CEcfg::instance()->get(cfgKey("protocol"))));

	setLengthOfLength(static_cast<int>(_lengthOfLengthSetting));
	setUserDefinedField(static_cast<int>(_userDefinedFieldSetting));
	setProtocolIdExt(static_cast<int>(_protocolIdExtSetting));
	setCcsdsDefinedField(static_cast<int>(_ccsdsDefinedFieldSetting));
}

modEncapPkt_Add::~modEncapPkt_Add() {
	MOD_DEBUG("Running ~modEncapPkt_Add().");
}

int modEncapPkt_Add::svc() {
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

		MOD_DEBUG("Received %d bytes to put into an Encapsulation Packet.", data->getUnitLength());

		ACE_UINT8 protocol = unsetProtocolVal;

		if ( links_[PrimaryOutputLink] ) {

			EncapsulationPacketBase* encap_pkt;

			if (getIpeSupport()) {
				ACE_UINT64 ipe = 0;

				if ( getIpe() != unsetIpeVal ) {
					protocol = _pickProtocol(EncapsulationPacketWithIPE::InternetProtocolExtenstion);
					ipe = getIpe();
				}
				else if ( data->getIPE_Header_Val() == -1 ) {
					protocol = _pickProtocol(EncapsulationPacketWithIPE::ArbitraryAggregationsOfOctets);
					MOD_DEBUG("Received unknown type (%s), treating as an Arbitrary Aggregations of Octets.", data->typeStr().c_str());
				}
				else {
					protocol = _pickProtocol(EncapsulationPacketWithIPE::InternetProtocolExtenstion);
					ipe = data->getIPE_Header_Val();
				}

				encap_pkt = new EncapsulationPacketWithIPE();

				dynamic_cast<EncapsulationPacketWithIPE*>(encap_pkt)->build(
					protocol, // Protocol Identifier
					getLengthOfLength(), // Length of Length
					0, // Packet Length (0 causes build() to calculate)
					data,
					ipe,
					getUserDefinedField(),
					getProtocolIdExt(),
					getCcsdsDefinedField()
				);
			}
			else {
				if ( data->getIPE_Header_Val() == -1 ) {
					protocol = _pickProtocol(EncapsulationPacketPreIPE::ArbitraryAggregationsOfOctets);
					MOD_DEBUG("Received unknown type (%s), treating as an Arbitrary Aggregations of Octets.", data->typeStr().c_str());
				}
				else {
					// Even though the IPE won't be inserted in this case, it
					// can still be used to ID the protocol.
					switch (data->getIPE_Header_Val()) {
						case EncapsulationPacketWithIPE::IPv4_Datagram:
							protocol = _pickProtocol(EncapsulationPacketPreIPE::IPv4);
							break;
						case EncapsulationPacketWithIPE::IPv6_Datagram:
							protocol = _pickProtocol(EncapsulationPacketPreIPE::IPv6);
							break;
						default:
							break;
					};
				}

				encap_pkt = new EncapsulationPacketPreIPE();

				dynamic_cast<EncapsulationPacketPreIPE*>(encap_pkt)->build(
					protocol, // Protocol Identifier
					getLengthOfLength(), // Length of Length
					0, // Packet Length (0 causes build() to calculate)
					data,
					getUserDefinedField(),
					getProtocolIdExt(),
					getCcsdsDefinedField()
				);
			}

			dumpUnit(encap_pkt);
			MOD_DEBUG("Sending %d octets.", encap_pkt->getUnitLength());

			links_[PrimaryOutputLink]->send(encap_pkt);
		}
		else {
			MOD_NOTICE("No output target defined yet, dropping data.");
		}

		ndSafeRelease(data);

	}

	return svcEnd_();

}

void modEncapPkt_Add::setIpe(const ACE_UINT64& newVal) {
	if ( newVal != unsetIpeVal && ! EncapsulationPacketWithIPE::properIPE(newVal) )
		throw BadValue(BadValue::msg("IPE", newVal));

	MOD_DEBUG("IPE setting %Q is either unset (%d) or valid.", unsetIpeVal, newVal);

	_ipe = newVal;
	if ( newVal != unsetIpeVal ) CEcfg::instance()->getOrAddInt64(cfgKey("IPE"), _ipe);
}

}
