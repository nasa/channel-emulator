/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modAOS_OCF_Extract_Interface.cpp
 * @author Tad Kollar  
 *
 * $Id: modAOS_OCF_Extract_Interface.cpp 2007 2013-05-31 20:06:00Z tkollar $
 * Copyright (c) 2012.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "modAOS_OCF_Extract.hpp"
#include "AOS_Interface_Macros.hpp"

namespace nAOS_OCF_Extract {

//=============================================================================
/**
 * @class modAOS_OCF_Extract_Interface
 * @author Tad Kollar  
 * @brief Provide an XML-RPC interface to the OCF Extraction module.
 */
//=============================================================================
class modAOS_OCF_Extract_Interface: public nasaCE::TrafficHandler_Interface<modAOS_OCF_Extract> {
public:
	modAOS_OCF_Extract_Interface(xmlrpc_c::registryPtr& rpcRegistry):
		nasaCE::TrafficHandler_Interface<modAOS_OCF_Extract>(rpcRegistry, "modAOS_OCF_Extract") {
		ACE_TRACE("modAOS_OCF_Extract_Interface::modAOS_OCF_Extract_Interface");

		register_methods(rpcRegistry);
	}

	AOS_PHYSICAL_CHANNEL_DEFINE_ACCESSORS();
	AOS_MASTER_CHANNEL_DEFINE_ACCESSORS();

	// Export specified methods via XML-RPC.
	void register_methods(xmlrpc_c::registryPtr&);

protected:
	void get_counters_(xmlrpc_c::paramList const& paramList, modAOS_OCF_Extract* handler,
		xstruct& counters) {
		ACE_TRACE("modAOS_OCF_Extract_Interface::get_counters_");

		nasaCE::TrafficHandler_Interface<modAOS_OCF_Extract>::get_counters_(paramList, handler, counters);

		GET_AOS_PHYSICAL_CHANNEL_COUNTERS();
		GET_AOS_MASTER_CHANNEL_COUNTERS();
	}

	void get_settings_(xmlrpc_c::paramList const& paramList, modAOS_OCF_Extract* handler,
		xstruct& settings) {
		ACE_TRACE("modAOS_OCF_Extract_Interface::get_settings_");

		nasaCE::TrafficHandler_Interface<modAOS_OCF_Extract>::get_settings_(paramList, handler, settings);

		GET_AOS_PHYSICAL_CHANNEL_SETTINGS();
		GET_AOS_MASTER_CHANNEL_SETTINGS();
	}
};

modAOS_OCF_Extract_Interface* modAOS_OCF_Extract_InterfaceP;

TEMPLATE_METHOD_CLASSES(modAOS_OCF_Extract_Interface, modAOS_OCF_Extract_InterfaceP);

AOS_PHYSICAL_CHANNEL_GENERATE_METHODS(modAOS_OCF_Extract_InterfaceP);
AOS_MASTER_CHANNEL_GENERATE_METHODS(modAOS_OCF_Extract_InterfaceP);

void modAOS_OCF_Extract_Interface::register_methods(xmlrpc_c::registryPtr& rpcRegistry) {
	ACE_TRACE("modAOS_OCF_Extract_Interface::register_methods");

	TEMPLATE_REGISTER_METHODS(modAOS_OCF_Extract, modAOS_OCF_Extract_Interface);

	AOS_PHYSICAL_CHANNEL_REGISTER_METHODS(modAOS_OCF_Extract);
	AOS_MASTER_CHANNEL_REGISTER_METHODS(modAOS_OCF_Extract);
}

} // namespace nAOS_OCF_Extract
