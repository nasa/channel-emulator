/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modTM_VC_Rcv_Packet_Interface.cpp
 * @author Tad Kollar  
 *
 * $Id: modTM_VC_Rcv_Packet_Interface.cpp 2400 2013-12-11 17:34:44Z tkollar $
 * Copyright (c) 2012.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "modTM_VC_Rcv_Packet.hpp"
#include "TM_Interface_Macros.hpp"

namespace nTM_VC_Rcv_Packet {

using namespace nasaCE;

//=============================================================================
/**
 * @class modTM_VC_Rcv_Packet_Interface
 * @author Tad Kollar  
 * @brief Provide an XML-RPC interface to the Virtual Channel Receive module.
 */
//=============================================================================
class modTM_VC_Rcv_Packet_Interface: public nasaCE::TrafficHandler_Interface<modTM_VC_Rcv_Packet> {
public:
	modTM_VC_Rcv_Packet_Interface(xmlrpc_c::registryPtr& rpcRegistry):
		nasaCE::TrafficHandler_Interface<modTM_VC_Rcv_Packet>(rpcRegistry, "modTM_VC_Rcv_Packet") {
		ACE_TRACE("modTM_VC_Rcv_Packet_Interface::modTM_VC_Rcv_Packet_Interface");

		register_methods(rpcRegistry);
	}

	void connect_output(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP) {
		ACE_TRACE("modTM_VC_Rcv_Packet_Interface::connect_output");
		std::string channelName(paramList.getString(0));
		std::string handlerName(paramList.getString(1));
		ACE_UINT8 PVN = paramList.getInt(2);
		std::string targetName(paramList.getString(3));

		HandlerLink::OutputRank outputRank = HandlerLink::PrimaryOutput;
		if (paramList.size() > 4) {
			string_nocase outputRankStr(paramList.getString(4).c_str());
			if (outputRankStr.substr(0,3) == "pri" ) outputRank = HandlerLink::PrimaryOutput;
			else if (outputRankStr.substr(0,3) == "aux") outputRank = HandlerLink::AuxOutput;
			else throw BadValue("Output rank can only be 'Primary' or 'Auxiliary'.");
		}

		HandlerLink::InputRank inputRank = HandlerLink::PrimaryInput;
		if (paramList.size() > 5) {
			string_nocase inputRankStr(paramList.getString(5).c_str());
			if (inputRankStr.substr(0,3) == "pri" ) inputRank = HandlerLink::PrimaryInput;
			else if (inputRankStr.substr(0,3) == "aux" ) inputRank = HandlerLink::AuxInput;
			else throw BadValue("Input rank can only be 'Primary' or 'Auxiliary'.");
		}

		Channel* channel = channels::instance()->channel(channelName);

		modTM_VC_Rcv_Packet* handler = dynamic_cast<modTM_VC_Rcv_Packet*>(channel->find(handlerName));
		BaseTrafficHandler* targetHandler = channel->find(targetName);

		if ( outputRank == HandlerLink::PrimaryOutput )
			handler->connectPrimaryOutput(PVN, targetHandler, inputRank);
		else
			handler->connectOutput(targetHandler, outputRank, inputRank);

		*retvalP = xmlrpc_c::value_nil();
	}

	// Another way of using connect_output more conistent with the config file layout.
	void connect_primary_output(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP) {
		ACE_TRACE("modTM_VC_Rcv_Packet_Interface::connect_primary_output");

		std::vector<xmlrpc_c::value> targetArray(paramList.getArray(2));
		xmlrpc_c::paramList revisedParams(5);

		revisedParams.addx(paramList[0])
			.addx(paramList[1])
			.addx(targetArray[2])
			.addx(targetArray[0])
			.addx(xmlrpc_c::value_string("Primary"))
			.addx(targetArray[1]);

		connect_output(revisedParams, retvalP);
	}

	void get_output_links(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP) {
		ACE_TRACE("modTM_VC_Rcv_Packet_Interface::get_output_links");

		modTM_VC_Rcv_Packet* handler = dynamic_cast<modTM_VC_Rcv_Packet*>(find_handler(paramList));

		std::list<ACE_UINT8> idList;
		handler->getIdentifiers(idList);
		std::map<std::string, xmlrpc_c::value> linkDescs;

		std::list<ACE_UINT8>::iterator pos;

		for ( pos = idList.begin(); pos != idList.end(); ++pos ) {
			HandlerLink* primaryOutputLink = handler->getLink(BaseTrafficHandler::PrimaryOutputLink, *pos);

			if (primaryOutputLink) {
				linkDescs[primaryOutputLink->getTarget()->getName()] = xmlrpc_c::value_string("Primary");
			}
		}

		HandlerLink* auxOutputLink = handler->getLink(BaseTrafficHandler::AuxOutputLink);
		if (auxOutputLink)
			linkDescs[auxOutputLink->getTarget()->getName()] = xmlrpc_c::value_string("Aux");

		*retvalP = xmlrpc_c::value_struct(linkDescs);
	}

	TM_PHYSICAL_CHANNEL_DEFINE_ACCESSORS();
	TM_MASTER_CHANNEL_DEFINE_ACCESSORS();
	TM_VIRTUAL_CHANNEL_DEFINE_ACCESSORS();
	GENERATE_BOOL_ACCESSORS(ipe_support, setIpeSupport, getIpeSupport);
	GENERATE_BOOL_ACCESSORS(multi_data_field, setMultiDataField, getMultiDataField);
	GENERATE_BOOL_ACCESSORS(allow_packets_after_fill, setAllowPacketsAfterFill, getAllowPacketsAfterFill);
	GENERATE_I8_ACCESSORS(bad_packet_tally, setBadPacketTally, getBadPacketTally);


	// Export specified methods via XML-RPC.
	void register_methods(xmlrpc_c::registryPtr&);

protected:
	void get_counters_(xmlrpc_c::paramList const& paramList, modTM_VC_Rcv_Packet* handler,
		xstruct& counters) {
		ACE_TRACE("modTM_VC_Rcv_Packet_Interface::get_counters_");

		nasaCE::TrafficHandler_Interface<modTM_VC_Rcv_Packet>::get_counters_(paramList, handler, counters);

		GET_TM_PHYSICAL_CHANNEL_COUNTERS();
		GET_TM_MASTER_CHANNEL_COUNTERS();
		GET_TM_VIRTUAL_CHANNEL_COUNTERS();
		counters["badPacketTally"] = xmlrpc_c::value_i8(handler->getBadPacketTally());

	}

	void get_settings_(xmlrpc_c::paramList const& paramList, modTM_VC_Rcv_Packet* handler,
		xstruct& settings) {
		ACE_TRACE("modTM_VC_Rcv_Packet_Interface::get_settings_");

		nasaCE::TrafficHandler_Interface<modTM_VC_Rcv_Packet>::get_settings_(paramList, handler, settings);

		GET_TM_PHYSICAL_CHANNEL_SETTINGS();
		GET_TM_MASTER_CHANNEL_SETTINGS();
		GET_TM_VIRTUAL_CHANNEL_SETTINGS();
		settings["ipeSupport"] = xmlrpc_c::value_boolean(handler->getIpeSupport());
		settings["multiDataField"] = xmlrpc_c::value_boolean(handler->getMultiDataField());
		settings["allowPacketsAfterFill"] = xmlrpc_c::value_boolean(handler->getAllowPacketsAfterFill());
	}
};

modTM_VC_Rcv_Packet_Interface* modTM_VC_Rcv_Packet_InterfaceP;

TEMPLATE_METHOD_CLASSES(modTM_VC_Rcv_Packet_Interface, modTM_VC_Rcv_Packet_InterfaceP);

TM_PHYSICAL_CHANNEL_GENERATE_METHODS(modTM_VC_Rcv_Packet_InterfaceP);
TM_MASTER_CHANNEL_GENERATE_METHODS(modTM_VC_Rcv_Packet_InterfaceP);
TM_VIRTUAL_CHANNEL_GENERATE_METHODS(modTM_VC_Rcv_Packet_InterfaceP);
GENERATE_ACCESSOR_METHODS(ipe_support, b, "whether the Internet Protocol Extension is supported in assembled Encapsulation Packets.",
	modTM_VC_Rcv_Packet_InterfaceP);
GENERATE_ACCESSOR_METHODS(multi_data_field, b, "whether multiple packets may be found in the Data Field.",
	modTM_VC_Rcv_Packet_InterfaceP);
GENERATE_ACCESSOR_METHODS(allow_packets_after_fill, b, "whether non-fill packets may be found after fill Encapsulation Packets.",
	modTM_VC_Rcv_Packet_InterfaceP);
GENERATE_ACCESSOR_METHODS(bad_packet_tally, i, "the count of malformed/unrecoverable Packets.",
	modTM_VC_Rcv_Packet_InterfaceP);

void modTM_VC_Rcv_Packet_Interface::register_methods(xmlrpc_c::registryPtr& rpcRegistry) {
	ACE_TRACE("modTM_VC_Rcv_Packet_Interface::register_methods");

	TEMPLATE_REGISTER_METHODS(modTM_VC_Rcv_Packet, modTM_VC_Rcv_Packet_Interface);

	TM_PHYSICAL_CHANNEL_REGISTER_METHODS(modTM_VC_Rcv_Packet);
	TM_MASTER_CHANNEL_REGISTER_METHODS(modTM_VC_Rcv_Packet);
	TM_VIRTUAL_CHANNEL_REGISTER_METHODS(modTM_VC_Rcv_Packet);
	REGISTER_ACCESSOR_METHODS(ipe_support, modTM_VC_Rcv_Packet, SupportIPE);
	REGISTER_ACCESSOR_METHODS(multi_data_field, modTM_VC_Rcv_Packet, MultiDataField);
	REGISTER_ACCESSOR_METHODS(allow_packets_after_fill, modTM_VC_Rcv_Packet, AllowPacketsAfterFill);
	REGISTER_ACCESSOR_METHODS(bad_packet_tally, modTM_VC_Rcv_Packet, BadPacketTally);
}

} // namespace nTM_VC_Rcv_Packet
