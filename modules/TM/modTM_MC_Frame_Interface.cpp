/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modTM_MC_Frame_Interface.cpp
 * @author Tad Kollar  
 *
 * $Id: modTM_MC_Frame_Interface.cpp 1670 2012-08-28 13:25:28Z tkollar $
 * Copyright (c) 2012.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "modTM_MC_Frame.hpp"
#include "TM_Interface_Macros.hpp"
#include "PeriodicHandler_Interface.hpp"

namespace nTM_MC_Frame {

//=============================================================================
/**
 * @class modTM_MC_Frame_Interface
 * @author Tad Kollar  
 * @brief An XML-RPC interface to the TM Virtual Channel Frame service module.
 */
//=============================================================================
class modTM_MC_Frame_Interface: public nasaCE::PeriodicHandler_Interface<modTM_MC_Frame> {
public:
	modTM_MC_Frame_Interface(xmlrpc_c::registryPtr& rpcRegistry):
		nasaCE::PeriodicHandler_Interface<modTM_MC_Frame>(rpcRegistry, "modTM_MC_Frame") {
		ACE_TRACE("modTM_MC_Frame_Interface::modTM_MC_Frame_Interface");

		register_methods(rpcRegistry);
	}

	TM_PHYSICAL_CHANNEL_DEFINE_ACCESSORS();
	TM_MASTER_CHANNEL_DEFINE_ACCESSORS();
	TM_VIRTUAL_CHANNEL_DEFINE_ACCESSORS();

	/// Export specified methods via XML-RPC.
	void register_methods(xmlrpc_c::registryPtr&);

protected:
	void get_counters_(xmlrpc_c::paramList const& paramList, modTM_MC_Frame* handler,
		xstruct& counters) {
		ACE_TRACE("modTM_MC_Frame_Interface::get_counters_");

		nasaCE::TrafficHandler_Interface<modTM_MC_Frame>::get_counters_(paramList, handler, counters);

		GET_TM_PHYSICAL_CHANNEL_COUNTERS();
		GET_TM_MASTER_CHANNEL_COUNTERS();
		GET_TM_VIRTUAL_CHANNEL_COUNTERS();
	}

	void get_settings_(xmlrpc_c::paramList const& paramList, modTM_MC_Frame* handler,
		xstruct& settings) {
		ACE_TRACE("modTM_MC_Frame_Interface::get_settings_");

		nasaCE::TrafficHandler_Interface<modTM_MC_Frame>::get_settings_(paramList, handler, settings);

		GET_TM_PHYSICAL_CHANNEL_SETTINGS();
		GET_TM_MASTER_CHANNEL_SETTINGS();
		GET_TM_VIRTUAL_CHANNEL_SETTINGS();
	}
};

modTM_MC_Frame_Interface* modTM_MC_Frame_InterfaceP;

PERIODIC_METHOD_CLASSES(modTM_MC_Frame_Interface, modTM_MC_Frame_InterfaceP);
TM_PHYSICAL_CHANNEL_GENERATE_METHODS(modTM_MC_Frame_InterfaceP);
TM_MASTER_CHANNEL_GENERATE_METHODS(modTM_MC_Frame_InterfaceP);
TM_VIRTUAL_CHANNEL_GENERATE_METHODS(modTM_MC_Frame_InterfaceP);

void modTM_MC_Frame_Interface::register_methods(xmlrpc_c::registryPtr& rpcRegistry) {
	ACE_TRACE("modTM_MC_Frame_Interface::register_methods");

	PERIODIC_REGISTER_METHODS(modTM_MC_Frame, modTM_MC_Frame_Interface);
	TM_PHYSICAL_CHANNEL_REGISTER_METHODS(modTM_MC_Frame);
	TM_MASTER_CHANNEL_REGISTER_METHODS(modTM_MC_Frame);
	TM_VIRTUAL_CHANNEL_REGISTER_METHODS(modTM_MC_Frame);
}

} // namespace nTM_MC_Frame
