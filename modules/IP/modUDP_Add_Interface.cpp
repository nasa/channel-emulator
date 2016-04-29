/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modUDP_Add_Interface.cpp
 * @author Tad Kollar  
 *
 * $Id: modUDP_Add_Interface.cpp 1670 2012-08-28 13:25:28Z tkollar $ 
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "modUDP_Add.hpp"
#include "IPv4Addr.hpp"
#include "TrafficHandler_Interface.hpp"

namespace nUDP_Add {

//=============================================================================
/**
 * @class modUDP_Add_Interface
 * @author Tad Kollar  
 * @brief Provide an XML-RPC interface to the UDP Add module.
 */
//=============================================================================
class modUDP_Add_Interface: public nasaCE::TrafficHandler_Interface<modUDP_Add> {
public:
	modUDP_Add_Interface(xmlrpc_c::registryPtr& rpcRegistry):
		nasaCE::TrafficHandler_Interface<modUDP_Add>(rpcRegistry, "modUDP_Add") {
		ACE_TRACE("modUDP_Add_Interface::modUDP_Add_Interface");
		
		register_methods(rpcRegistry);
	}
	
	void set_src_addr(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP) {
		ACE_TRACE("modUDP_Add_Interface::set_src_addr");
		std::string srcIPAddrStr = (std::string) paramList.getString(2);
			
		find_handler(paramList)->setSrcIPAddr(srcIPAddrStr);
		
		*retvalP = xmlrpc_c::value_nil();		
	}	
	
	void get_src_addr(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP) {
		ACE_TRACE("modUDP_Add_Interface::get_src_addr");

		*retvalP = xmlrpc_c::value_string(find_handler(paramList)->getSrcIPAddr().get_quad());		
	}	
	
	GENERATE_INT_ACCESSORS(src_port, setSrcPort, getSrcPort);
	
	void set_dst_addr(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP) {
		ACE_TRACE("modUDP_Add_Interface::set_dst_addr");
		std::string dstIPAddrStr = (std::string) paramList.getString(2);
		
		find_handler(paramList)->setDstIPAddr(dstIPAddrStr);
		
		*retvalP = xmlrpc_c::value_nil();		
	}	
	
	void get_dst_addr(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP) {
		ACE_TRACE("modUDP_Add_Interface::get_dst_addr");
		
		*retvalP = xmlrpc_c::value_string(find_handler(paramList)->getDstIPAddr().get_quad());		
	}	
		
	GENERATE_INT_ACCESSORS(dst_port, setDstPort, getDstPort);
	GENERATE_BOOL_ACCESSORS(crc, setComputeCRC, isComputingCRC);

	/// Export specified methods via XML-RPC.
	void register_methods(xmlrpc_c::registryPtr&);
	
protected:
	void get_settings_(xmlrpc_c::paramList const& paramList, modUDP_Add* handler,
		xstruct& settings) {
		ACE_TRACE("modUDP_Add_Interface::get_settings_");
		
		nasaCE::TrafficHandler_Interface<modUDP_Add>::get_settings_(paramList, handler, settings);
		
		// Build a map of associated settings
		settings["srcAddr"] = xmlrpc_c::value_string(handler->getSrcIPAddr().get_quad());
		settings["srcPort"] = xmlrpc_c::value_int(handler->getSrcPort());
		settings["dstAddr"] = xmlrpc_c::value_string(handler->getDstIPAddr().get_quad());
		settings["dstPort"] = xmlrpc_c::value_int(handler->getDstPort());
		settings["useUDPCRC"] = xmlrpc_c::value_string(handler->isComputingCRC()? "True" : "False");
	}
};

modUDP_Add_Interface* modUDP_Add_InterfaceP;

TEMPLATE_METHOD_CLASSES(modUDP_Add_Interface, modUDP_Add_InterfaceP);
	
GENERATE_ACCESSOR_METHODS(src_addr, s, "the source IPv4 address.", modUDP_Add_InterfaceP);
GENERATE_ACCESSOR_METHODS(src_port, i, "the source port.", modUDP_Add_InterfaceP);
GENERATE_ACCESSOR_METHODS(dst_addr, s, "the destination IPv4 address.", modUDP_Add_InterfaceP);
GENERATE_ACCESSOR_METHODS(dst_port, i, "the destination port.", modUDP_Add_InterfaceP);
GENERATE_ACCESSOR_METHODS(crc, b, "whether the UDP Checksum will be used.", modUDP_Add_InterfaceP);
	
void modUDP_Add_Interface::register_methods(xmlrpc_c::registryPtr& rpcRegistry) {
	ACE_TRACE("modUDP_Add_Interface::register_methods");

	TEMPLATE_REGISTER_METHODS(modUDP_Add, modUDP_Add_Interface);
	REGISTER_ACCESSOR_METHODS(src_addr, modUDP_Add, SrcAddr);
	REGISTER_ACCESSOR_METHODS(src_port, modUDP_Add, SrcPort);
	REGISTER_ACCESSOR_METHODS(dst_addr, modUDP_Add, DstAddr);
	REGISTER_ACCESSOR_METHODS(dst_port, modUDP_Add, DstPort);
	REGISTER_ACCESSOR_METHODS(crc, modUDP_Add, UDPCRC);
}

} // namespace nUDP_Add