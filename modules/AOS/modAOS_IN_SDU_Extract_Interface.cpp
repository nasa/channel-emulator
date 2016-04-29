/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modAOS_IN_SDU_Extract_Interface.cpp
 * @author Tad Kollar  
 *
 * $Id: modAOS_IN_SDU_Extract_Interface.cpp 2027 2013-06-04 19:28:10Z tkollar $
 * Copyright (c) 2012.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "modAOS_IN_SDU_Extract.hpp"
#include "AOS_Interface_Macros.hpp"

namespace nAOS_IN_SDU_Extract {

//=============================================================================
/**
 * @class modAOS_IN_SDU_Extract_Interface
 * @author Tad Kollar  
 * @brief Provide an XML-RPC interface to the IN_SDU Extraction module.
 */
//=============================================================================
class modAOS_IN_SDU_Extract_Interface: public nasaCE::TrafficHandler_Interface<modAOS_IN_SDU_Extract> {
public:
	modAOS_IN_SDU_Extract_Interface(xmlrpc_c::registryPtr& rpcRegistry):
		nasaCE::TrafficHandler_Interface<modAOS_IN_SDU_Extract>(rpcRegistry, "modAOS_IN_SDU_Extract") {
		ACE_TRACE("modAOS_IN_SDU_Extract_Interface::modAOS_IN_SDU_Extract_Interface");

		register_methods(rpcRegistry);
	}

	AOS_PHYSICAL_CHANNEL_DEFINE_ACCESSORS();

	// Export specified methods via XML-RPC.
	void register_methods(xmlrpc_c::registryPtr&);

protected:
	void get_counters_(xmlrpc_c::paramList const& paramList, modAOS_IN_SDU_Extract* handler,
		xstruct& counters) {
		ACE_TRACE("modAOS_IN_SDU_Extract_Interface::get_counters_");

		nasaCE::TrafficHandler_Interface<modAOS_IN_SDU_Extract>::get_counters_(paramList, handler, counters);

		GET_AOS_PHYSICAL_CHANNEL_COUNTERS();
	}

	void get_settings_(xmlrpc_c::paramList const& paramList, modAOS_IN_SDU_Extract* handler,
		xstruct& settings) {
		ACE_TRACE("modAOS_IN_SDU_Extract_Interface::get_settings_");

		nasaCE::TrafficHandler_Interface<modAOS_IN_SDU_Extract>::get_settings_(paramList, handler, settings);

		GET_AOS_PHYSICAL_CHANNEL_SETTINGS();
	}
};

modAOS_IN_SDU_Extract_Interface* modAOS_IN_SDU_Extract_InterfaceP;

TEMPLATE_METHOD_CLASSES(modAOS_IN_SDU_Extract_Interface, modAOS_IN_SDU_Extract_InterfaceP);

AOS_PHYSICAL_CHANNEL_GENERATE_METHODS(modAOS_IN_SDU_Extract_InterfaceP);

void modAOS_IN_SDU_Extract_Interface::register_methods(xmlrpc_c::registryPtr& rpcRegistry) {
	ACE_TRACE("modAOS_IN_SDU_Extract_Interface::register_methods");

	TEMPLATE_REGISTER_METHODS(modAOS_IN_SDU_Extract, modAOS_IN_SDU_Extract_Interface);

	AOS_PHYSICAL_CHANNEL_REGISTER_METHODS(modAOS_IN_SDU_Extract);
}

} // namespace nAOS_IN_SDU_Extract
