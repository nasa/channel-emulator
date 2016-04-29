/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modAOS_AF_Gen_Interface.cpp
 * @author Tad Kollar  
 *
 * $Id: modAOS_AF_Gen_Interface.cpp 1670 2012-08-28 13:25:28Z tkollar $
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================


#include "modAOS_AF_Gen.hpp"
#include "AOS_Interface_Macros.hpp"

namespace nAOS_AF_Gen {

//=============================================================================
/**
 * @class modAOS_AF_Gen_Interface
 * @author Tad Kollar  
 * @brief Provide an XML-RPC interface to the AOS All Frames Generation module.
 */
//=============================================================================
class modAOS_AF_Gen_Interface: public nasaCE::TrafficHandler_Interface<modAOS_AF_Gen> {
public:
	modAOS_AF_Gen_Interface(xmlrpc_c::registryPtr& rpcRegistry):
		nasaCE::TrafficHandler_Interface<modAOS_AF_Gen>(rpcRegistry, "modAOS_AF_Gen") {
		ACE_TRACE("modAOS_AF_Gen_Interface::modAOS_AF_Gen_Interface");

		register_methods(rpcRegistry);
	}

	AOS_PHYSICAL_CHANNEL_DEFINE_ACCESSORS();

	/// Export specified methods via XML-RPC.
	void register_methods(xmlrpc_c::registryPtr&);

protected:
	void get_counters_(xmlrpc_c::paramList const& paramList, modAOS_AF_Gen* handler,
		xstruct& counters) {
		ACE_TRACE("modAOS_AF_Gen_Interface::get_counters_");

		nasaCE::TrafficHandler_Interface<modAOS_AF_Gen>::get_counters_(paramList, handler, counters);

		GET_AOS_PHYSICAL_CHANNEL_COUNTERS();
	}

	void get_settings_(xmlrpc_c::paramList const& paramList, modAOS_AF_Gen* handler,
		xstruct& settings) {
		ACE_TRACE("modAOS_AF_Gen_Interface::get_settings_");

		nasaCE::TrafficHandler_Interface<modAOS_AF_Gen>::get_settings_(paramList, handler, settings);

		GET_AOS_PHYSICAL_CHANNEL_SETTINGS();
	}
};

modAOS_AF_Gen_Interface* modAOS_AF_Gen_InterfaceP;

TEMPLATE_METHOD_CLASSES(modAOS_AF_Gen_Interface, modAOS_AF_Gen_InterfaceP);
AOS_PHYSICAL_CHANNEL_GENERATE_METHODS(modAOS_AF_Gen_InterfaceP);

void modAOS_AF_Gen_Interface::register_methods(xmlrpc_c::registryPtr& rpcRegistry) {
	ACE_TRACE("modAOS_AF_Gen_Interface::register_methods");

	TEMPLATE_REGISTER_METHODS(modAOS_AF_Gen, modAOS_AF_Gen_Interface);
	AOS_PHYSICAL_CHANNEL_REGISTER_METHODS(modAOS_AF_Gen);
}

} // namespace nAOS_AF_Gen
