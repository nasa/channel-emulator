/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modTM_VC_Gen_Packet_Interface.cpp
 * @author Tad Kollar  
 *
 * $Id: modTM_VC_Gen_Packet_Interface.cpp 1997 2013-05-30 19:12:48Z tkollar $
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "modTM_VC_Gen_Packet.hpp"
#include "TM_Interface_Macros.hpp"

namespace nTM_VC_Gen_Packet {

//=============================================================================
/**
 * @class modTM_VC_Gen_Packet_Interface
 * @author Tad Kollar  
 * @brief Provide an XML-RPC interface to the TM Virtual Channel Access Generation module.
 */
//=============================================================================
class modTM_VC_Gen_Packet_Interface: public nasaCE::TrafficHandler_Interface<modTM_VC_Gen_Packet> {
public:
	modTM_VC_Gen_Packet_Interface(xmlrpc_c::registryPtr& rpcRegistry):
		nasaCE::TrafficHandler_Interface<modTM_VC_Gen_Packet>(rpcRegistry, "modTM_VC_Gen_Packet") {
		ACE_TRACE("modTM_VC_Gen_Packet_Interface::modTM_VC_Gen_Packet_Interface");

		register_methods(rpcRegistry);
	}

	TM_PHYSICAL_CHANNEL_DEFINE_ACCESSORS();
	TM_MASTER_CHANNEL_DEFINE_ACCESSORS();
	TM_VIRTUAL_CHANNEL_DEFINE_ACCESSORS();
	GENERATE_INT_ACCESSORS(max_usecs, setMaxUsecsForNextPacket, getMaxUsecsForNextPacket);
	GENERATE_INT_ACCESSORS(multi_data_field, setMultiPacketDataField, getMultiPacketDataField);

	void set_fill_pattern(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP) {
		ACE_TRACE("modTM_VC_Gen_Packet_Interface::set_fill_pattern");
		const std::vector<xmlrpc_c::value> xml_rpc_pattern = paramList.getArray(2);

		std::vector<uint8_t> fillPattern;

		for (unsigned i = 0; i < xml_rpc_pattern.size(); i++ ) {
			fillPattern[i] = xmlrpc_c::value_int(xml_rpc_pattern[i]);
		}

		find_handler(paramList)->setFillPattern(fillPattern);

		*retvalP = xmlrpc_c::value_nil();
	}

	void get_fill_pattern(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP) {
		ACE_TRACE("modTM_VC_Gen_Packet_Interface::get_fill_pattern");

		std::vector<uint8_t> fillPattern;
		find_handler(paramList)->getFillPattern(fillPattern);

		std::vector<xmlrpc_c::value> xml_rpc_pattern;

		for (unsigned i = 0; i < fillPattern.size(); i++ ) {
			xml_rpc_pattern[i] = xmlrpc_c::value_int(fillPattern[i]);
		}

		*retvalP = xmlrpc_c::value_array(xml_rpc_pattern);
	}

	/// Export specified methods via XML-RPC.
	void register_methods(xmlrpc_c::registryPtr&);

protected:
	void get_counters_(xmlrpc_c::paramList const& paramList, modTM_VC_Gen_Packet* handler,
		xstruct& counters) {
		ACE_TRACE("modTM_VC_Gen_Packet_Interface::get_counters_");

		nasaCE::TrafficHandler_Interface<modTM_VC_Gen_Packet>::get_counters_(paramList, handler, counters);

		GET_TM_PHYSICAL_CHANNEL_COUNTERS();
		GET_TM_MASTER_CHANNEL_COUNTERS();
		GET_TM_VIRTUAL_CHANNEL_COUNTERS();
	}

	void get_settings_(xmlrpc_c::paramList const& paramList, modTM_VC_Gen_Packet* handler,
		xstruct& settings) {
		ACE_TRACE("modTM_VC_Gen_Packet_Interface::get_settings_");

		nasaCE::TrafficHandler_Interface<modTM_VC_Gen_Packet>::get_settings_(paramList, handler, settings);

		GET_TM_PHYSICAL_CHANNEL_SETTINGS();
		GET_TM_MASTER_CHANNEL_SETTINGS();
		GET_TM_VIRTUAL_CHANNEL_SETTINGS();
	}
};

modTM_VC_Gen_Packet_Interface* modTM_VC_Gen_Packet_InterfaceP;

TEMPLATE_METHOD_CLASSES(modTM_VC_Gen_Packet_Interface, modTM_VC_Gen_Packet_InterfaceP);
TM_PHYSICAL_CHANNEL_GENERATE_METHODS(modTM_VC_Gen_Packet_InterfaceP);
TM_MASTER_CHANNEL_GENERATE_METHODS(modTM_VC_Gen_Packet_InterfaceP);
TM_VIRTUAL_CHANNEL_GENERATE_METHODS(modTM_VC_Gen_Packet_InterfaceP);
GENERATE_ACCESSOR_METHODS(max_usecs, i, "how long to wait for an incoming packet when the current Data Field isn't full.", modTM_VC_Gen_Packet_InterfaceP);
GENERATE_ACCESSOR_METHODS(multi_data_field, b, "whether multiple packets may be found in the Data Field.",
	modTM_VC_Gen_Packet_InterfaceP);
METHOD_CLASS(set_fill_pattern, "n:A", "Sets a new fill pattern for the Data Field.", modTM_VC_Gen_Packet_InterfaceP);
METHOD_CLASS(get_fill_pattern, "A:n", "Returns the current fill pattern used in the Data Field.", modTM_VC_Gen_Packet_InterfaceP);

void modTM_VC_Gen_Packet_Interface::register_methods(xmlrpc_c::registryPtr& rpcRegistry) {
	ACE_TRACE("modTM_VC_Gen_Packet_Interface::register_methods");

	TEMPLATE_REGISTER_METHODS(modTM_VC_Gen_Packet, modTM_VC_Gen_Packet_Interface);
	TM_PHYSICAL_CHANNEL_REGISTER_METHODS(modTM_VC_Gen_Packet);
	TM_MASTER_CHANNEL_REGISTER_METHODS(modTM_VC_Gen_Packet);
	TM_VIRTUAL_CHANNEL_REGISTER_METHODS(modTM_VC_Gen_Packet);
	REGISTER_ACCESSOR_METHODS(max_usecs, modTM_VC_Gen_Packet, WaitForNextPacket);
	REGISTER_ACCESSOR_METHODS(multi_data_field, modTM_VC_Gen_Packet, MultiDataField);
	REGISTER_ACCESSOR_METHODS(fill_pattern, modTM_VC_Gen_Packet, FillPattern);
}

} // namespace nTM_VC_Gen_Packet
