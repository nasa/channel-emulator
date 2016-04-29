/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modAOS_B_PDU_Remove_Interface.cpp
 * @author Tad Kollar  
 *
 * $Id: modAOS_B_PDU_Remove_Interface.cpp 1670 2012-08-28 13:25:28Z tkollar $ 
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "modAOS_B_PDU_Remove.hpp"
#include "TrafficHandler_Interface.hpp"

namespace nAOS_B_PDU_Remove {

//=============================================================================
/**
 * @class modAOS_B_PDU_Remove_Interface
 * @author Tad Kollar  
 * @brief Provide an XML-RPC interface to the Encapsulation Packet Removal module.
 */
//=============================================================================
struct modAOS_B_PDU_Remove_Interface: public nasaCE::TrafficHandler_Interface<modAOS_B_PDU_Remove> {
	modAOS_B_PDU_Remove_Interface(xmlrpc_c::registryPtr& rpcRegistry):
		nasaCE::TrafficHandler_Interface<modAOS_B_PDU_Remove>(rpcRegistry, "modAOS_B_PDU_Remove") {
		ACE_TRACE("modAOS_B_PDU_Remove_Interface::modAOS_B_PDU_Remove_Interface");
		
		register_methods(rpcRegistry);
	}

	/// Export specified methods via XML-RPC.
	void register_methods(xmlrpc_c::registryPtr&);
};

modAOS_B_PDU_Remove_Interface* modAOS_B_PDU_Remove_InterfaceP;

TEMPLATE_METHOD_CLASSES(modAOS_B_PDU_Remove_Interface, modAOS_B_PDU_Remove_InterfaceP);
	
void modAOS_B_PDU_Remove_Interface::register_methods(xmlrpc_c::registryPtr& rpcRegistry) {
	ACE_TRACE("modAOS_B_PDU_Remove_Interface::register_methods");

	TEMPLATE_REGISTER_METHODS(modAOS_B_PDU_Remove, modAOS_B_PDU_Remove_Interface);
}

} // namespace nAOS_B_PDU_Remove