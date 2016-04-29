/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modAOS_IN_SDU_Insert_Interface.cpp
 * @author Tad Kollar  
 *
 * $Id: modAOS_IN_SDU_Insert_Interface.cpp 2027 2013-06-04 19:28:10Z tkollar $
 * Copyright (c) 2012.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "modAOS_IN_SDU_Insert.hpp"
#include "AOS_Interface_Macros.hpp"
#include "PeriodicHandler_Interface.hpp"

namespace nAOS_IN_SDU_Insert {

//=============================================================================
/**
 * @class modAOS_IN_SDU_Insert_Interface
 * @author Tad Kollar  
 * @brief Provide an XML-RPC interface to the Frame Secondary Header insertion module.
 */
//=============================================================================
class modAOS_IN_SDU_Insert_Interface: public nasaCE::PeriodicHandler_Interface<modAOS_IN_SDU_Insert> {
public:
	modAOS_IN_SDU_Insert_Interface(xmlrpc_c::registryPtr& rpcRegistry):
		nasaCE::PeriodicHandler_Interface<modAOS_IN_SDU_Insert>(rpcRegistry, "modAOS_IN_SDU_Insert") {
		ACE_TRACE("modAOS_IN_SDU_Insert_Interface::modAOS_IN_SDU_Insert_Interface");

		register_methods(rpcRegistry);
	}

	AOS_PHYSICAL_CHANNEL_DEFINE_ACCESSORS();
	GENERATE_I8_ACCESSORS(rcvd_in_sdu_unit_tally, setReceivedInSduUnitCount, getReceivedInSduUnitCount);
	GENERATE_I8_ACCESSORS(rcvd_in_sdu_octet_tally, setReceivedInSduOctetCount, getReceivedInSduOctetCount);
	GENERATE_I8_ACCESSORS(rcvd_frame_unit_tally, setReceivedFrameUnitCount, getReceivedFrameUnitCount);
	GENERATE_I8_ACCESSORS(rcvd_frame_octet_tally, setReceivedFrameOctetCount, getReceivedFrameOctetCount);

	// Export specified methods via XML-RPC.
	void register_methods(xmlrpc_c::registryPtr&);

protected:
	void get_counters_(xmlrpc_c::paramList const& paramList, modAOS_IN_SDU_Insert* handler,
		xstruct& counters) {
		ACE_TRACE("modAOS_IN_SDU_Insert_Interface::get_counters_");

		nasaCE::TrafficHandler_Interface<modAOS_IN_SDU_Insert>::get_counters_(paramList, handler, counters);

		GET_AOS_PHYSICAL_CHANNEL_COUNTERS();
		counters["receivedInSduUnitTally"] = xmlrpc_c::value_i8(handler->getReceivedInSduUnitCount());
		counters["receivedInSduOctetTally"] = xmlrpc_c::value_i8(handler->getReceivedInSduOctetCount());
		counters["receivedFrameUnitTally"] = xmlrpc_c::value_i8(handler->getReceivedFrameUnitCount());
		counters["receivedFrameOctetTally"] = xmlrpc_c::value_i8(handler->getReceivedFrameOctetCount());
	}

	void get_settings_(xmlrpc_c::paramList const& paramList, modAOS_IN_SDU_Insert* handler,
		xstruct& settings) {
		ACE_TRACE("modAOS_IN_SDU_Insert_Interface::get_settings_");

		nasaCE::TrafficHandler_Interface<modAOS_IN_SDU_Insert>::get_settings_(paramList, handler, settings);

		GET_AOS_PHYSICAL_CHANNEL_SETTINGS();
	}
};

modAOS_IN_SDU_Insert_Interface* modAOS_IN_SDU_Insert_InterfaceP;

PERIODIC_METHOD_CLASSES(modAOS_IN_SDU_Insert_Interface, modAOS_IN_SDU_Insert_InterfaceP);

AOS_PHYSICAL_CHANNEL_GENERATE_METHODS(modAOS_IN_SDU_Insert_InterfaceP);
GENERATE_ACCESSOR_METHODS(rcvd_in_sdu_unit_tally, i, "the total number of valid IN_SDU units received.",
	modAOS_IN_SDU_Insert_InterfaceP);
GENERATE_ACCESSOR_METHODS(rcvd_in_sdu_octet_tally, i, "the total number of octets in received IN_SDU units.",
	modAOS_IN_SDU_Insert_InterfaceP);
GENERATE_ACCESSOR_METHODS(rcvd_frame_unit_tally, i, "the total number of valid AOS Transfer Frames received.",
	modAOS_IN_SDU_Insert_InterfaceP);
GENERATE_ACCESSOR_METHODS(rcvd_frame_octet_tally, i, "the total number of octets in received valid AOS Transfer Frames.",
	modAOS_IN_SDU_Insert_InterfaceP);

void modAOS_IN_SDU_Insert_Interface::register_methods(xmlrpc_c::registryPtr& rpcRegistry) {
	ACE_TRACE("modAOS_IN_SDU_Insert_Interface::register_methods");

	PERIODIC_REGISTER_METHODS(modAOS_IN_SDU_Insert, modAOS_IN_SDU_Insert_Interface);
	AOS_PHYSICAL_CHANNEL_REGISTER_METHODS(modAOS_IN_SDU_Insert);
	REGISTER_ACCESSOR_METHODS(rcvd_in_sdu_unit_tally, modAOS_IN_SDU_Insert, ReceivedInSduUnitCount);
	REGISTER_ACCESSOR_METHODS(rcvd_in_sdu_octet_tally, modAOS_IN_SDU_Insert, ReceivedInSduOctetCount);
	REGISTER_ACCESSOR_METHODS(rcvd_frame_unit_tally, modAOS_IN_SDU_Insert, ReceivedFrameUnitCount);
	REGISTER_ACCESSOR_METHODS(rcvd_frame_octet_tally, modAOS_IN_SDU_Insert, ReceivedFrameOctetCount);
}

} // namespace nAOS_IN_SDU_Insert