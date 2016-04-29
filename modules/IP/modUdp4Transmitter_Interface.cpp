/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modUdp4Transmitter_Interface.cpp
 * @author Tad Kollar  
 *
 * $Id: modUdp4Transmitter_Interface.cpp 1670 2012-08-28 13:25:28Z tkollar $ 
 * Copyright (c) 2011.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "modUdp4Transmitter.hpp"
#include "TrafficHandler_Interface.hpp"

namespace nUdp4Transmitter {

//=============================================================================
/**
 * @class modUdp4Transmitter_Interface
 * @author Tad Kollar  
 * @brief Provide an XML-RPC interface to the UDP Add module.
 */
//=============================================================================
class modUdp4Transmitter_Interface: public nasaCE::TrafficHandler_Interface<modUdp4Transmitter> {
public:
	modUdp4Transmitter_Interface(xmlrpc_c::registryPtr& rpcRegistry):
		nasaCE::TrafficHandler_Interface<modUdp4Transmitter>(rpcRegistry, "modUdp4Transmitter") {
		ACE_TRACE("modUdp4Transmitter_Interface::modUdp4Transmitter_Interface");
		
		register_methods(rpcRegistry);
	}
	
	void set_dst_addr(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP) {
		ACE_TRACE("modUdp4Transmitter_Interface::set_dst_addr");
		std::string dstIPAddrStr = (std::string) paramList.getString(2);
		
		find_handler(paramList)->setDestAddress(dstIPAddrStr);
		
		*retvalP = xmlrpc_c::value_nil();		
	}	
	
	void get_dst_addr(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP) {
		ACE_TRACE("modUdp4Transmitter_Interface::get_dst_addr");
		
		*retvalP = xmlrpc_c::value_string(find_handler(paramList)->getDestAddress());		
	}	
		
	GENERATE_INT_ACCESSORS(dst_port, setDestPort, getDestPort);

	/// Export specified methods via XML-RPC.
	void register_methods(xmlrpc_c::registryPtr&);
	
protected:
	void get_settings_(xmlrpc_c::paramList const& paramList, modUdp4Transmitter* handler,
		xstruct& settings) {
		ACE_TRACE("modUdp4Transmitter_Interface::get_settings_");
		
		nasaCE::TrafficHandler_Interface<modUdp4Transmitter>::get_settings_(paramList, handler, settings);
		
		// Build a map of associated settings
		settings["dstAddr"] = xmlrpc_c::value_string(handler->getDestAddress());
		settings["dstPort"] = xmlrpc_c::value_int(handler->getDestPort());
	}
};

modUdp4Transmitter_Interface* modUdp4Transmitter_InterfaceP;

TEMPLATE_METHOD_CLASSES(modUdp4Transmitter_Interface, modUdp4Transmitter_InterfaceP);
	
GENERATE_ACCESSOR_METHODS(dst_addr, s, "the destination IPv4 address.", modUdp4Transmitter_InterfaceP);
GENERATE_ACCESSOR_METHODS(dst_port, i, "the destination UDP port.", modUdp4Transmitter_InterfaceP);
	
void modUdp4Transmitter_Interface::register_methods(xmlrpc_c::registryPtr& rpcRegistry) {
	ACE_TRACE("modUdp4Transmitter_Interface::register_methods");

	TEMPLATE_REGISTER_METHODS(modUdp4Transmitter, modUdp4Transmitter_Interface);
	REGISTER_ACCESSOR_METHODS(dst_addr, modUdp4Transmitter, DstAddr);
	REGISTER_ACCESSOR_METHODS(dst_port, modUdp4Transmitter, DstPort);
}

} // namespace nUdp4Transmitter