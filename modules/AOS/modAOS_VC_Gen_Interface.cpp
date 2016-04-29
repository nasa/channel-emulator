/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modAOS_VC_Gen_Interface.cpp
 * @author Tad Kollar  
 *
 * $Id: modAOS_VC_Gen_Interface.cpp 2018 2013-06-04 14:59:19Z tkollar $
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "modAOS_VC_Gen.hpp"
#include "AOS_Interface_Macros.hpp"

namespace nAOS_VC_Gen {

//=============================================================================
/**
 * @class modAOS_VC_Gen_Interface
 * @author Tad Kollar  
 * @brief Provide an XML-RPC interface to the AOS Virtual Channel Generation module.
 */
//=============================================================================
class modAOS_VC_Gen_Interface: public nasaCE::TrafficHandler_Interface<modAOS_VC_Gen> {
public:
	modAOS_VC_Gen_Interface(xmlrpc_c::registryPtr& rpcRegistry):
		nasaCE::TrafficHandler_Interface<modAOS_VC_Gen>(rpcRegistry, "modAOS_VC_Gen") {
		ACE_TRACE("modAOS_VC_Gen_Interface::modAOS_VC_Gen_Interface");

		register_methods(rpcRegistry);
	}

	AOS_PHYSICAL_CHANNEL_DEFINE_ACCESSORS();
	AOS_MASTER_CHANNEL_DEFINE_ACCESSORS();
	AOS_VIRTUAL_CHANNEL_DEFINE_ACCESSORS();

	/// Export specified methods via XML-RPC.
	void register_methods(xmlrpc_c::registryPtr&);

protected:
	void get_counters_(xmlrpc_c::paramList const& paramList, modAOS_VC_Gen* handler,
		xstruct& counters) {
		ACE_TRACE("modAOS_VC_Gen_Interface::get_counters_");

		nasaCE::TrafficHandler_Interface<modAOS_VC_Gen>::get_counters_(paramList, handler, counters);

		GET_AOS_PHYSICAL_CHANNEL_COUNTERS();
		GET_AOS_MASTER_CHANNEL_COUNTERS();
		GET_AOS_VIRTUAL_CHANNEL_COUNTERS();
	}

	void get_settings_(xmlrpc_c::paramList const& paramList, modAOS_VC_Gen* handler,
		xstruct& settings) {
		ACE_TRACE("modAOS_VC_Gen_Interface::get_settings_");

		nasaCE::TrafficHandler_Interface<modAOS_VC_Gen>::get_settings_(paramList, handler, settings);

		GET_AOS_PHYSICAL_CHANNEL_SETTINGS();
		GET_AOS_MASTER_CHANNEL_SETTINGS();
		GET_AOS_VIRTUAL_CHANNEL_SETTINGS();
	}
};

modAOS_VC_Gen_Interface* modAOS_VC_Gen_InterfaceP;

TEMPLATE_METHOD_CLASSES(modAOS_VC_Gen_Interface, modAOS_VC_Gen_InterfaceP);
AOS_PHYSICAL_CHANNEL_GENERATE_METHODS(modAOS_VC_Gen_InterfaceP);
AOS_MASTER_CHANNEL_GENERATE_METHODS(modAOS_VC_Gen_InterfaceP);
AOS_VIRTUAL_CHANNEL_GENERATE_METHODS(modAOS_VC_Gen_InterfaceP);

void modAOS_VC_Gen_Interface::register_methods(xmlrpc_c::registryPtr& rpcRegistry) {
	ACE_TRACE("modAOS_VC_Gen_Interface::register_methods");

	TEMPLATE_REGISTER_METHODS(modAOS_VC_Gen, modAOS_VC_Gen_Interface);
	AOS_PHYSICAL_CHANNEL_REGISTER_METHODS(modAOS_VC_Gen);
	AOS_MASTER_CHANNEL_REGISTER_METHODS(modAOS_VC_Gen);
	AOS_VIRTUAL_CHANNEL_REGISTER_METHODS(modAOS_VC_Gen);

}

} // namespace nAOS_VC_Gen
