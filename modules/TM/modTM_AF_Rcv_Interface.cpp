/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modTM_AF_Rcv_Interface.cpp
 * @author Tad Kollar  
 *
 * $Id: modTM_AF_Rcv_Interface.cpp 1670 2012-08-28 13:25:28Z tkollar $
 * Copyright (c) 2012.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "modTM_AF_Rcv.hpp"
#include "TM_Interface_Macros.hpp"

namespace nTM_AF_Rcv {

//=============================================================================
/**
 * @class modTM_AF_Rcv_Interface
 * @author Tad Kollar  
 * @brief Provide an XML-RPC interface to the Virtual Channel Receive module.
 */
//=============================================================================
class modTM_AF_Rcv_Interface: public nasaCE::TrafficHandler_Interface<modTM_AF_Rcv> {
public:
	modTM_AF_Rcv_Interface(xmlrpc_c::registryPtr& rpcRegistry):
		nasaCE::TrafficHandler_Interface<modTM_AF_Rcv>(rpcRegistry, "modTM_AF_Rcv") {
		ACE_TRACE("modTM_AF_Rcv_Interface::modTM_AF_Rcv_Interface");

		register_methods(rpcRegistry);
	}

	TM_PHYSICAL_CHANNEL_DEFINE_ACCESSORS();

	// Export specified methods via XML-RPC.
	void register_methods(xmlrpc_c::registryPtr&);

protected:
	void get_counters_(xmlrpc_c::paramList const& paramList, modTM_AF_Rcv* handler,
		xstruct& counters) {
		ACE_TRACE("modTM_AF_Rcv_Interface::get_counters_");

		nasaCE::TrafficHandler_Interface<modTM_AF_Rcv>::get_counters_(paramList, handler, counters);

		GET_TM_PHYSICAL_CHANNEL_COUNTERS();
	}

	void get_settings_(xmlrpc_c::paramList const& paramList, modTM_AF_Rcv* handler,
		xstruct& settings) {
		ACE_TRACE("modTM_AF_Rcv_Interface::get_settings_");

		nasaCE::TrafficHandler_Interface<modTM_AF_Rcv>::get_settings_(paramList, handler, settings);

		GET_TM_PHYSICAL_CHANNEL_SETTINGS();
	}
};

modTM_AF_Rcv_Interface* modTM_AF_Rcv_InterfaceP;

TEMPLATE_METHOD_CLASSES(modTM_AF_Rcv_Interface, modTM_AF_Rcv_InterfaceP);

TM_PHYSICAL_CHANNEL_GENERATE_METHODS(modTM_AF_Rcv_InterfaceP);

void modTM_AF_Rcv_Interface::register_methods(xmlrpc_c::registryPtr& rpcRegistry) {
	ACE_TRACE("modTM_AF_Rcv_Interface::register_methods");

	TEMPLATE_REGISTER_METHODS(modTM_AF_Rcv, modTM_AF_Rcv_Interface);

	TM_PHYSICAL_CHANNEL_REGISTER_METHODS(modTM_AF_Rcv);
}

} // namespace nTM_AF_Rcv
