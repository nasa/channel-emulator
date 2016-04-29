/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modFdTransmitter_Interface.cpp
 * @author Tad Kollar  
 *
 * $Id: modFdTransmitter_Interface.cpp 1670 2012-08-28 13:25:28Z tkollar $ 
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "modFdTransmitter.hpp"
#include "TrafficHandler_Interface.hpp"

namespace nFdTransmitter {

//=============================================================================
/**
 * @class modFdTransmitter_Interface
 * @author Tad Kollar  
 * @brief Provide an XML-RPC interface to the FD receiver module.
 */
//=============================================================================
struct modFdTransmitter_Interface: public nasaCE::TrafficHandler_Interface<modFdTransmitter> {
	modFdTransmitter_Interface(xmlrpc_c::registryPtr& rpcRegistry):
		nasaCE::TrafficHandler_Interface<modFdTransmitter>(rpcRegistry, "modFdTransmitter") {
		ACE_TRACE("modFdTransmitter_Interface::modFdTransmitter_Interface");
		
		register_methods(rpcRegistry);
	}
	
	/// Export specified methods via XML-RPC.
	void register_methods(xmlrpc_c::registryPtr&);
};

modFdTransmitter_Interface* modFdTransmitter_InterfaceP;

TEMPLATE_METHOD_CLASSES(modFdTransmitter_Interface, modFdTransmitter_InterfaceP);
	
void modFdTransmitter_Interface::register_methods(xmlrpc_c::registryPtr& rpcRegistry) {
	ACE_TRACE("modFdTransmitter_Interface::register_methods");

	TEMPLATE_REGISTER_METHODS(modFdTransmitter, modFdTransmitter_Interface);
}

} // namespace nFdTransmitter