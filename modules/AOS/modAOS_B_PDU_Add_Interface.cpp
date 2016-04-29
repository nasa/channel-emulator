/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modAOS_B_PDU_Add_Interface.cpp
 * @author Tad Kollar  
 *
 * $Id: modAOS_B_PDU_Add_Interface.cpp 1670 2012-08-28 13:25:28Z tkollar $ 
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "modAOS_B_PDU_Add.hpp"
#include "PeriodicHandler_Interface.hpp"

namespace nAOS_B_PDU_Add {

//=============================================================================
/**
 * @class modAOS_B_PDU_Add_Interface
 * @author Tad Kollar  
 * @brief Provide an XML-RPC interface to the AOS Bitstream PDU Addition module.
 */
//=============================================================================
class modAOS_B_PDU_Add_Interface: public nasaCE::PeriodicHandler_Interface<modAOS_B_PDU_Add> {
public:
	modAOS_B_PDU_Add_Interface(xmlrpc_c::registryPtr& rpcRegistry):
		nasaCE::PeriodicHandler_Interface<modAOS_B_PDU_Add>(rpcRegistry, "modAOS_B_PDU_Add") {
		ACE_TRACE("modAOS_B_PDU_Add_Interface::modAOS_B_PDU_Add_Interface");
		
		register_methods(rpcRegistry);
	}
	
	GENERATE_INT_ACCESSORS(length, set_B_PDU_Length, get_B_PDU_Length);
	GENERATE_BOOL_ACCESSORS(immediate_send, setSendImmediately, getSendImmediately);
	
	/// Export specified methods via XML-RPC.
	void register_methods(xmlrpc_c::registryPtr&);
	
protected:
	
	void get_settings_(xmlrpc_c::paramList const& paramList, modAOS_B_PDU_Add* handler,
		xstruct& settings) {
		ACE_TRACE("modAOS_B_PDU_Add_Interface::get_settings_");
		
		nasaCE::TrafficHandler_Interface<modAOS_B_PDU_Add>::get_settings_(paramList, handler, settings);
		
		// Build a map of associated settings
		settings["immediateSend"] = xmlrpc_c::value_string(handler->getSendImmediately()? "True" : "False");		
	}	
};

modAOS_B_PDU_Add_Interface* modAOS_B_PDU_Add_InterfaceP;

PERIODIC_METHOD_CLASSES(modAOS_B_PDU_Add_Interface, modAOS_B_PDU_Add_InterfaceP);

GENERATE_ACCESSOR_METHODS(length, i, "the length of generated AOS Bitstream PDUs.", modAOS_B_PDU_Add_InterfaceP);
GENERATE_ACCESSOR_METHODS(immediate_send, b, "whether each B_PDU is sent as soon as it's filled.", modAOS_B_PDU_Add_InterfaceP);

void modAOS_B_PDU_Add_Interface::register_methods(xmlrpc_c::registryPtr& rpcRegistry) {
	ACE_TRACE("modAOS_B_PDU_Add_Interface::register_methods");

	PERIODIC_REGISTER_METHODS(modAOS_B_PDU_Add, modAOS_B_PDU_Add_Interface);

	REGISTER_ACCESSOR_METHODS(length, modAOS_B_PDU_Add, Length);
	REGISTER_ACCESSOR_METHODS(immediate_send, modAOS_B_PDU_Add, ImmediateSend);
}

} // namespace nAOS_B_PDU_Add