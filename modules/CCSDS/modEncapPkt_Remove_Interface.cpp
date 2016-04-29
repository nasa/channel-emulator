/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modEncapPkt_Remove_Interface.cpp
 * @author Tad Kollar  
 *
 * $Id: modEncapPkt_Remove_Interface.cpp 1670 2012-08-28 13:25:28Z tkollar $ 
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "modEncapPkt_Remove.hpp"
#include "TrafficHandler_Interface.hpp"

namespace nEncapPkt_Remove {

//=============================================================================
/**
 * @class modEncapPkt_Remove_Interface
 * @author Tad Kollar  
 * @brief Provide an XML-RPC interface to the Encapsulation Packet Removal module.
 */
//=============================================================================
struct modEncapPkt_Remove_Interface: public nasaCE::TrafficHandler_Interface<modEncapPkt_Remove> {
	modEncapPkt_Remove_Interface(xmlrpc_c::registryPtr& rpcRegistry):
		nasaCE::TrafficHandler_Interface<modEncapPkt_Remove>(rpcRegistry, "modEncapPkt_Remove") {
		ACE_TRACE("modEncapPkt_Remove_Interface::modEncapPkt_Remove_Interface");
		
		register_methods(rpcRegistry);
	}
	
	GENERATE_BOOL_ACCESSORS(ipe_support, setIpeSupport, getIpeSupport);

	/// Export specified methods via XML-RPC.
	void register_methods(xmlrpc_c::registryPtr&);
};

modEncapPkt_Remove_Interface* modEncapPkt_Remove_InterfaceP;

TEMPLATE_METHOD_CLASSES(modEncapPkt_Remove_Interface, modEncapPkt_Remove_InterfaceP);
GENERATE_ACCESSOR_METHODS(ipe_support, i, "whether the Internet Protocol Extension is supported.",
	modEncapPkt_Remove_InterfaceP);
		
void modEncapPkt_Remove_Interface::register_methods(xmlrpc_c::registryPtr& rpcRegistry) {
	ACE_TRACE("modEncapPkt_Remove_Interface::register_methods");

	TEMPLATE_REGISTER_METHODS(modEncapPkt_Remove, modEncapPkt_Remove_Interface);
	REGISTER_ACCESSOR_METHODS(ipe_support, modEncapPkt_Remove, SupportIPE);	
}

} // namespace nEncapPkt_Remove