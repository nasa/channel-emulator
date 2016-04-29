/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modTcp4Transmitter_Interface.cpp
 * @author Tad Kollar  
 *
 * $Id: modTcp4Transmitter_Interface.cpp 1670 2012-08-28 13:25:28Z tkollar $ 
 * Copyright (c) 2010.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "modTcp4Transmitter.hpp"
#include "TrafficHandler_Interface.hpp"

namespace nTcp4Transmitter {

//=============================================================================
/**
 * @class modTcp4Transmitter_Interface
 * @author Tad Kollar  
 * @brief Provide an XML-RPC interface to the IPv4 TCP receiver module.
 */
//=============================================================================
struct modTcp4Transmitter_Interface: public nasaCE::TrafficHandler_Interface<modTcp4Transmitter> {

	modTcp4Transmitter_Interface(xmlrpc_c::registryPtr& rpcRegistry):
		nasaCE::TrafficHandler_Interface<modTcp4Transmitter>(rpcRegistry, "modTcp4Transmitter") {
		ACE_TRACE("modTcp4Transmitter_Interface::modTcp4Transmitter_Interface");
		
		register_methods(rpcRegistry);
	}
	
	/// Export specified methods via XML-RPC.
	void register_methods(xmlrpc_c::registryPtr&);
};

modTcp4Transmitter_Interface* modTcp4Transmitter_InterfaceP;

TEMPLATE_METHOD_CLASSES(modTcp4Transmitter_Interface, modTcp4Transmitter_InterfaceP);
	
void modTcp4Transmitter_Interface::register_methods(xmlrpc_c::registryPtr& rpcRegistry) {
	ACE_TRACE("modTcp4Transmitter_Interface::register_methods");

	TEMPLATE_REGISTER_METHODS(modTcp4Transmitter, modTcp4Transmitter_Interface);
}


} // namespace nTcp4Transmitter