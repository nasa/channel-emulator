/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modPseudoRandomize_Interface.cpp
 * @author Tad Kollar  
 *
 * $Id: modPseudoRandomize_Interface.cpp 2313 2013-08-01 13:53:59Z tkollar $
 * Copyright (c) 2008-2013.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "modPseudoRandomize.hpp"
#include "TrafficHandler_Interface.hpp"

namespace nPseudoRandomize {

//=============================================================================
/**
 * @class modPseudoRandomize_Interface
 * @author Tad Kollar  
 * @brief Provide an XML-RPC interface to the PseudoRandomize module.
 */
//=============================================================================
struct modPseudoRandomize_Interface: public nasaCE::TrafficHandler_Interface<modPseudoRandomize> {
	modPseudoRandomize_Interface(xmlrpc_c::registryPtr& rpcRegistry):
		nasaCE::TrafficHandler_Interface<modPseudoRandomize>(rpcRegistry, "modPseudoRandomize") {
		ACE_TRACE("modPseudoRandomize_Interface::modPseudoRandomize_Interface");

		register_methods(rpcRegistry);
	}

	/// Export specified methods via XML-RPC.
	void register_methods(xmlrpc_c::registryPtr&);
};

modPseudoRandomize_Interface* modPseudoRandomize_InterfaceP;

TEMPLATE_METHOD_CLASSES(modPseudoRandomize_Interface, modPseudoRandomize_InterfaceP);

void modPseudoRandomize_Interface::register_methods(xmlrpc_c::registryPtr& rpcRegistry) {
	ACE_TRACE("modPseudoRandomize_Interface::register_methods");

	TEMPLATE_REGISTER_METHODS(modPseudoRandomize, modPseudoRandomize_Interface);
}

} // namespace nPseudoRandomize