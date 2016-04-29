/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modAOS_M_PDU_Remove_Interface.cpp
 * @author Tad Kollar  
 *
 * $Id: modAOS_M_PDU_Remove_Interface.cpp 2018 2013-06-04 14:59:19Z tkollar $ 
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "modAOS_M_PDU_Remove.hpp"
#include "TrafficHandler_Interface.hpp"

namespace nAOS_M_PDU_Remove {

//=============================================================================
/**
 * @class modAOS_M_PDU_Remove_Interface
 * @author Tad Kollar  
 * @brief Provide an XML-RPC interface to the AOS Multiplexing PDU Addition module.
 */
//=============================================================================
class modAOS_M_PDU_Remove_Interface: public nasaCE::TrafficHandler_Interface<modAOS_M_PDU_Remove> {
public:
	modAOS_M_PDU_Remove_Interface(xmlrpc_c::registryPtr& rpcRegistry):
		nasaCE::TrafficHandler_Interface<modAOS_M_PDU_Remove>(rpcRegistry, "modAOS_M_PDU_Remove") {
		ACE_TRACE("modAOS_M_PDU_Remove_Interface::modAOS_M_PDU_Remove_Interface");
		
		register_methods(rpcRegistry);
	}
	
	GENERATE_STRING_ACCESSORS(contained_type, setContainedType, getContainedTypeStr);
	GENERATE_BOOL_ACCESSORS(ipe_support, setIpeSupport, getIpeSupport);
	GENERATE_BOOL_ACCESSORS(multi_packet_zone, setMultiPacketZone, getMultiPacketZone);
	GENERATE_BOOL_ACCESSORS(allow_packets_after_fill, setAllowPacketsAfterFill, getAllowPacketsAfterFill);
	GENERATE_I8_ACCESSORS(bad_encap_tally, setBadEncapTally, getBadEncapTally);
		
	/// Export specified methods via XML-RPC.
	void register_methods(xmlrpc_c::registryPtr&);
	
protected:
	
	void get_counters_(xmlrpc_c::paramList const& paramList, modAOS_M_PDU_Remove* handler,
		xstruct& counters) {
		ACE_TRACE("modTM_VC_Rcv_Packet_Interface::get_counters_");

		nasaCE::TrafficHandler_Interface<modAOS_M_PDU_Remove>::get_counters_(paramList, handler, counters);

		counters["badEncapTally"] = xmlrpc_c::value_i8(handler->getBadEncapTally());
	}
	
	void get_settings_(xmlrpc_c::paramList const& paramList, modAOS_M_PDU_Remove* handler,
		xstruct& settings) {
		ACE_TRACE("modAOS_M_PDU_Remove_Interface::get_settings_");
		
		nasaCE::TrafficHandler_Interface<modAOS_M_PDU_Remove>::get_settings_(paramList, handler, settings);
		
		// Build a map of associated settings
		settings["containedType"] = xmlrpc_c::value_string(handler->getContainedTypeStr());
	}
};

modAOS_M_PDU_Remove_Interface* modAOS_M_PDU_Remove_InterfaceP;

TEMPLATE_METHOD_CLASSES(modAOS_M_PDU_Remove_Interface, modAOS_M_PDU_Remove_InterfaceP);
GENERATE_ACCESSOR_METHODS(contained_type, s, "the type that incoming M_PDUs should contain.",
	modAOS_M_PDU_Remove_InterfaceP);
GENERATE_ACCESSOR_METHODS(ipe_support, b, "whether the Internet Protocol Extension is supported in assembled Encap Packets.",
	modAOS_M_PDU_Remove_InterfaceP);
GENERATE_ACCESSOR_METHODS(multi_packet_zone, b, "whether multiple packets may be found in the Packet Zone.",
	modAOS_M_PDU_Remove_InterfaceP);
GENERATE_ACCESSOR_METHODS(allow_packets_after_fill, b, "whether non-fill packets may be found after fill Encap Packets.",
	modAOS_M_PDU_Remove_InterfaceP);
GENERATE_ACCESSOR_METHODS(bad_encap_tally, i, "the tally of malformed Encap Packets.",
	modAOS_M_PDU_Remove_InterfaceP);
		
void modAOS_M_PDU_Remove_Interface::register_methods(xmlrpc_c::registryPtr& rpcRegistry) {
	ACE_TRACE("modAOS_M_PDU_Remove_Interface::register_methods");

	TEMPLATE_REGISTER_METHODS(modAOS_M_PDU_Remove, modAOS_M_PDU_Remove_Interface);
	REGISTER_ACCESSOR_METHODS(contained_type, modAOS_M_PDU_Remove, ContainedType);
	REGISTER_ACCESSOR_METHODS(ipe_support, modAOS_M_PDU_Remove, SupportIPE);
	REGISTER_ACCESSOR_METHODS(multi_packet_zone, modAOS_M_PDU_Remove, MultiPacketZone);
	REGISTER_ACCESSOR_METHODS(allow_packets_after_fill, modAOS_M_PDU_Remove, AllowPacketsAfterFill);
	REGISTER_ACCESSOR_METHODS(bad_encap_tally, modAOS_M_PDU_Remove, BadEncapTally);
}

} // namespace nAOS_M_PDU_Remove
