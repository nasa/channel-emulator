/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modTM_FSH_Insert_Interface.cpp
 * @author Tad Kollar  
 *
 * $Id: modTM_FSH_Insert_Interface.cpp 1670 2012-08-28 13:25:28Z tkollar $
 * Copyright (c) 2012.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "modTM_FSH_Insert.hpp"
#include "TM_Interface_Macros.hpp"
#include "PeriodicHandler_Interface.hpp"

namespace nTM_FSH_Insert {

//=============================================================================
/**
 * @class modTM_FSH_Insert_Interface
 * @author Tad Kollar  
 * @brief Provide an XML-RPC interface to the Frame Secondary Header insertion module.
 */
//=============================================================================
class modTM_FSH_Insert_Interface: public nasaCE::PeriodicHandler_Interface<modTM_FSH_Insert> {
public:
	modTM_FSH_Insert_Interface(xmlrpc_c::registryPtr& rpcRegistry):
		nasaCE::PeriodicHandler_Interface<modTM_FSH_Insert>(rpcRegistry, "modTM_FSH_Insert") {
		ACE_TRACE("modTM_FSH_Insert_Interface::modTM_FSH_Insert_Interface");

		register_methods(rpcRegistry);
	}

	TM_PHYSICAL_CHANNEL_DEFINE_ACCESSORS();
	TM_MASTER_CHANNEL_DEFINE_ACCESSORS();
	GENERATE_I8_ACCESSORS(rcvd_fsh_unit_tally, setReceivedFshUnitCount, getReceivedFshUnitCount);
	GENERATE_I8_ACCESSORS(rcvd_fsh_octet_tally, setReceivedFshOctetCount, getReceivedFshOctetCount);
	GENERATE_I8_ACCESSORS(rcvd_frame_unit_tally, setReceivedFrameUnitCount, getReceivedFrameUnitCount);
	GENERATE_I8_ACCESSORS(rcvd_frame_octet_tally, setReceivedFrameOctetCount, getReceivedFrameOctetCount);

	// Export specified methods via XML-RPC.
	void register_methods(xmlrpc_c::registryPtr&);

protected:
	void get_counters_(xmlrpc_c::paramList const& paramList, modTM_FSH_Insert* handler,
		xstruct& counters) {
		ACE_TRACE("modTM_FSH_Insert_Interface::get_counters_");

		nasaCE::TrafficHandler_Interface<modTM_FSH_Insert>::get_counters_(paramList, handler, counters);

		GET_TM_PHYSICAL_CHANNEL_COUNTERS();
		GET_TM_MASTER_CHANNEL_COUNTERS();
		counters["receivedFSHUnitTally"] = xmlrpc_c::value_i8(handler->getReceivedFshUnitCount());
		counters["receivedFSHOctetTally"] = xmlrpc_c::value_i8(handler->getReceivedFshOctetCount());
		counters["receivedFrameUnitTally"] = xmlrpc_c::value_i8(handler->getReceivedFrameUnitCount());
		counters["receivedFrameOctetTally"] = xmlrpc_c::value_i8(handler->getReceivedFrameOctetCount());
	}

	void get_settings_(xmlrpc_c::paramList const& paramList, modTM_FSH_Insert* handler,
		xstruct& settings) {
		ACE_TRACE("modTM_FSH_Insert_Interface::get_settings_");

		nasaCE::TrafficHandler_Interface<modTM_FSH_Insert>::get_settings_(paramList, handler, settings);

		GET_TM_PHYSICAL_CHANNEL_SETTINGS();
		GET_TM_MASTER_CHANNEL_SETTINGS();
	}
};

modTM_FSH_Insert_Interface* modTM_FSH_Insert_InterfaceP;

PERIODIC_METHOD_CLASSES(modTM_FSH_Insert_Interface, modTM_FSH_Insert_InterfaceP);

TM_PHYSICAL_CHANNEL_GENERATE_METHODS(modTM_FSH_Insert_InterfaceP);
TM_MASTER_CHANNEL_GENERATE_METHODS(modTM_FSH_Insert_InterfaceP);
GENERATE_ACCESSOR_METHODS(rcvd_fsh_unit_tally, i, "the total number of valid FSH units received.",
	modTM_FSH_Insert_InterfaceP);
GENERATE_ACCESSOR_METHODS(rcvd_fsh_octet_tally, i, "the total number of octets in received FSH units.",
	modTM_FSH_Insert_InterfaceP);
GENERATE_ACCESSOR_METHODS(rcvd_frame_unit_tally, i, "the total number of valid TM Transfer Frames received.",
	modTM_FSH_Insert_InterfaceP);
GENERATE_ACCESSOR_METHODS(rcvd_frame_octet_tally, i, "the total number of octets in received valid TM Transfer Frames.",
	modTM_FSH_Insert_InterfaceP);

void modTM_FSH_Insert_Interface::register_methods(xmlrpc_c::registryPtr& rpcRegistry) {
	ACE_TRACE("modTM_FSH_Insert_Interface::register_methods");

	PERIODIC_REGISTER_METHODS(modTM_FSH_Insert, modTM_FSH_Insert_Interface);
	TM_PHYSICAL_CHANNEL_REGISTER_METHODS(modTM_FSH_Insert);
	TM_MASTER_CHANNEL_REGISTER_METHODS(modTM_FSH_Insert);
	REGISTER_ACCESSOR_METHODS(rcvd_fsh_unit_tally, modTM_FSH_Insert, ReceivedFshUnitCount);
	REGISTER_ACCESSOR_METHODS(rcvd_fsh_octet_tally, modTM_FSH_Insert, ReceivedFshOctetCount);
	REGISTER_ACCESSOR_METHODS(rcvd_frame_unit_tally, modTM_FSH_Insert, ReceivedFrameUnitCount);
	REGISTER_ACCESSOR_METHODS(rcvd_frame_octet_tally, modTM_FSH_Insert, ReceivedFrameOctetCount);
}

} // namespace nTM_FSH_Insert