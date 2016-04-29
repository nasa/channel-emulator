/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modSplitter_Interface.cpp
 * @author Tad Kollar  
 *
 * $Id: modSplitter_Interface.cpp 1670 2012-08-28 13:25:28Z tkollar $ 
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "modSplitter.hpp"
#include "TrafficHandler_Interface.hpp"

namespace nSplitter {

//=============================================================================
/**
 * @class modSplitter_Interface
 * @author Tad Kollar  
 * @brief Provide an XML-RPC interface to the Splitter module.
 */
//=============================================================================
struct modSplitter_Interface: public nasaCE::TrafficHandler_Interface<modSplitter> {
	modSplitter_Interface(xmlrpc_c::registryPtr& rpcRegistry):
		nasaCE::TrafficHandler_Interface<modSplitter>(rpcRegistry, "modSplitter") {
		ACE_TRACE("modSplitter_Interface::modSplitter_Interface");
		
		register_methods(rpcRegistry);
	}
	
	/// Export specified methods via XML-RPC.
	void register_methods(xmlrpc_c::registryPtr&);
};

modSplitter_Interface* modSplitter_InterfaceP;

TEMPLATE_METHOD_CLASSES(modSplitter_Interface, modSplitter_InterfaceP);
	
void modSplitter_Interface::register_methods(xmlrpc_c::registryPtr& rpcRegistry) {
	ACE_TRACE("modSplitter_Interface::register_methods");

	TEMPLATE_REGISTER_METHODS(modSplitter, modSplitter_Interface);
}

} // namespace nSplitter