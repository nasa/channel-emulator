/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modEthReceiver_Interface.cpp
 * @author Tad Kollar  
 *
 * $Id: modEthReceiver_Interface.cpp 1670 2012-08-28 13:25:28Z tkollar $ 
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "modEthReceiver.hpp"
#include "TrafficHandler_Interface.hpp"

namespace nEthReceiver {

//=============================================================================
/**
 * @class modEthReceiver_Interface
 * @author Tad Kollar  
 * @brief Provide an XML-RPC interface to the Ethernet receiver module.
 */
//=============================================================================
struct modEthReceiver_Interface: public nasaCE::TrafficHandler_Interface<modEthReceiver> {
	modEthReceiver_Interface(xmlrpc_c::registryPtr& rpcRegistry):
		nasaCE::TrafficHandler_Interface<modEthReceiver>(rpcRegistry, "modEthReceiver") {
		ACE_TRACE("modEthReceiver_Interface::modEthReceiver_Interface");
		
		register_methods(rpcRegistry);
	}

	
	/// Export specified methods via XML-RPC.
	void register_methods(xmlrpc_c::registryPtr&);
};

modEthReceiver_Interface* modEthReceiver_InterfaceP;

TEMPLATE_METHOD_CLASSES(modEthReceiver_Interface, modEthReceiver_InterfaceP)

void modEthReceiver_Interface::register_methods(xmlrpc_c::registryPtr& rpcRegistry) {
	ACE_TRACE("modEthReceiver_Interface::register_methods");

	TEMPLATE_REGISTER_METHODS(modEthReceiver, modEthReceiver_Interface);
}

} // namespace nEthReceiver