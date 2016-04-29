/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modUDP_Remove_Interface.cpp
 * @author Tad Kollar  
 *
 * $Id: modUDP_Remove_Interface.cpp 1670 2012-08-28 13:25:28Z tkollar $ 
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "modUDP_Remove.hpp"
#include "TrafficHandler_Interface.hpp"

namespace nUDP_Remove {

//=============================================================================
/**
 * @class modUDP_Remove_Interface
 * @author Tad Kollar  
 * @brief Provide an XML-RPC interface to the UDP Add module.
 */
//=============================================================================
class modUDP_Remove_Interface: public nasaCE::TrafficHandler_Interface<modUDP_Remove> {
public:
	modUDP_Remove_Interface(xmlrpc_c::registryPtr& rpcRegistry):
		nasaCE::TrafficHandler_Interface<modUDP_Remove>(rpcRegistry, "modUDP_Remove") {
		ACE_TRACE("modUDP_Remove_Interface::modUDP_Remove_Interface");
		
		register_methods(rpcRegistry);
	}
	
	GENERATE_BOOL_ACCESSORS(crc_checking, setCheckCRC, isCheckingCRC);
	GENERATE_INT_ACCESSORS(bad_ip_crc_count, setBadIPCRCCount, getBadIPCRCCount);
	GENERATE_INT_ACCESSORS(bad_udp_crc_count, setBadUDPCRCCount, getBadUDPCRCCount);
	
	/// Export specified methods via XML-RPC.
	void register_methods(xmlrpc_c::registryPtr&);

protected:
	void get_counters_(xmlrpc_c::paramList const& paramList, modUDP_Remove* handler,
		xstruct& counters) {
		ACE_TRACE("modUDP_Remove_Interface::get_counters_");
	
		nasaCE::TrafficHandler_Interface<modUDP_Remove>::get_counters_(paramList, handler, counters);

		// Build a map of all associated counters
		counters["badIPv4CRCCount"] = xmlrpc_c::value_int(handler->getBadIPCRCCount());		
		counters["badUDPCRCCount"] = xmlrpc_c::value_int(handler->getBadUDPCRCCount());		
	}
	
	void get_settings_(xmlrpc_c::paramList const& paramList, modUDP_Remove* handler,
		xstruct& settings) {
		ACE_TRACE("modUDP_Remove_Interface::get_settings_");
		
		nasaCE::TrafficHandler_Interface<modUDP_Remove>::get_settings_(paramList, handler, settings);
		
		// Build a map of associated settings
		settings["useUDPCRC"] = xmlrpc_c::value_string(handler->isCheckingCRC()? "True" : "False");
	}
};

modUDP_Remove_Interface* modUDP_Remove_InterfaceP;

TEMPLATE_METHOD_CLASSES(modUDP_Remove_Interface, modUDP_Remove_InterfaceP);
GENERATE_ACCESSOR_METHODS(crc_checking, b, "whether the IPv4/UDP checksums will be tested.",
	modUDP_Remove_InterfaceP);
GENERATE_ACCESSOR_METHODS(bad_ip_crc_count, i, "the tally of IPv4 CRC errors.",
	modUDP_Remove_InterfaceP);
GENERATE_ACCESSOR_METHODS(bad_udp_crc_count, i, "the tally of UDP CRC errors.",
	modUDP_Remove_InterfaceP);		
	
void modUDP_Remove_Interface::register_methods(xmlrpc_c::registryPtr& rpcRegistry) {
	ACE_TRACE("modUDP_Remove_Interface::register_methods");

	TEMPLATE_REGISTER_METHODS(modUDP_Remove, modUDP_Remove_Interface);
	REGISTER_ACCESSOR_METHODS(crc_checking, modUDP_Remove, CRCCheck);
	REGISTER_ACCESSOR_METHODS(bad_ip_crc_count, modUDP_Remove, BadIPCRCCount);
	REGISTER_ACCESSOR_METHODS(bad_udp_crc_count, modUDP_Remove, BadUDPCRCCount);		
}

} // namespace nUDP_Remove