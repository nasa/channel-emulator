/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modAOS_OCF_Insert_Interface.cpp
 * @author Tad Kollar  
 *
 * $Id: modAOS_OCF_Insert_Interface.cpp 2006 2013-05-31 19:59:14Z tkollar $
 * Copyright (c) 2012.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "modAOS_OCF_Insert.hpp"
#include "AOS_Interface_Macros.hpp"
#include "PeriodicHandler_Interface.hpp"

namespace nAOS_OCF_Insert {

//=============================================================================
/**
 * @class modAOS_OCF_Insert_Interface
 * @author Tad Kollar  
 * @brief Provide an XML-RPC interface to the OCF insertion module.
 */
//=============================================================================
class modAOS_OCF_Insert_Interface: public nasaCE::PeriodicHandler_Interface<modAOS_OCF_Insert> {
public:
	modAOS_OCF_Insert_Interface(xmlrpc_c::registryPtr& rpcRegistry):
		nasaCE::PeriodicHandler_Interface<modAOS_OCF_Insert>(rpcRegistry, "modAOS_OCF_Insert") {
		ACE_TRACE("modAOS_OCF_Insert_Interface::modAOS_OCF_Insert_Interface");

		register_methods(rpcRegistry);
	}

	AOS_PHYSICAL_CHANNEL_DEFINE_ACCESSORS();
	AOS_MASTER_CHANNEL_DEFINE_ACCESSORS();
	GENERATE_I8_ACCESSORS(rcvd_ocf_unit_tally, setReceivedReportUnitCount, getReceivedReportUnitCount);
	GENERATE_I8_ACCESSORS(rcvd_ocf_octet_tally, setReceivedReportOctetCount, getReceivedReportOctetCount);
	GENERATE_I8_ACCESSORS(rcvd_frame_unit_tally, setReceivedFrameUnitCount, getReceivedFrameUnitCount);
	GENERATE_I8_ACCESSORS(rcvd_frame_octet_tally, setReceivedFrameOctetCount, getReceivedFrameOctetCount);

	// Export specified methods via XML-RPC.
	void register_methods(xmlrpc_c::registryPtr&);

protected:
	void get_counters_(xmlrpc_c::paramList const& paramList, modAOS_OCF_Insert* handler,
		xstruct& counters) {
		ACE_TRACE("modAOS_OCF_Insert_Interface::get_counters_");

		nasaCE::TrafficHandler_Interface<modAOS_OCF_Insert>::get_counters_(paramList, handler, counters);

		GET_AOS_PHYSICAL_CHANNEL_COUNTERS();
		GET_AOS_MASTER_CHANNEL_COUNTERS();
		counters["receivedReportUnitTally"] = xmlrpc_c::value_i8(handler->getReceivedReportUnitCount());
		counters["receivedReportOctetTally"] = xmlrpc_c::value_i8(handler->getReceivedReportOctetCount());
		counters["receivedFrameUnitTally"] = xmlrpc_c::value_i8(handler->getReceivedFrameUnitCount());
		counters["receivedFrameOctetTally"] = xmlrpc_c::value_i8(handler->getReceivedFrameOctetCount());
	}

	void get_settings_(xmlrpc_c::paramList const& paramList, modAOS_OCF_Insert* handler,
		xstruct& settings) {
		ACE_TRACE("modAOS_OCF_Insert_Interface::get_settings_");

		nasaCE::TrafficHandler_Interface<modAOS_OCF_Insert>::get_settings_(paramList, handler, settings);

		GET_AOS_PHYSICAL_CHANNEL_SETTINGS();
		GET_AOS_MASTER_CHANNEL_SETTINGS();
	}
};

modAOS_OCF_Insert_Interface* modAOS_OCF_Insert_InterfaceP;

PERIODIC_METHOD_CLASSES(modAOS_OCF_Insert_Interface, modAOS_OCF_Insert_InterfaceP);

AOS_PHYSICAL_CHANNEL_GENERATE_METHODS(modAOS_OCF_Insert_InterfaceP);
AOS_MASTER_CHANNEL_GENERATE_METHODS(modAOS_OCF_Insert_InterfaceP);
GENERATE_ACCESSOR_METHODS(rcvd_ocf_unit_tally, i, "the total number of valid Reports received.",
	modAOS_OCF_Insert_InterfaceP);
GENERATE_ACCESSOR_METHODS(rcvd_ocf_octet_tally, i, "the total number of octets in received Reports.",
	modAOS_OCF_Insert_InterfaceP);
GENERATE_ACCESSOR_METHODS(rcvd_frame_unit_tally, i, "the total number of valid AOS Transfer Frames received.",
	modAOS_OCF_Insert_InterfaceP);
GENERATE_ACCESSOR_METHODS(rcvd_frame_octet_tally, i, "the total number of octets in received valid AOS Transfer Frames.",
	modAOS_OCF_Insert_InterfaceP);

void modAOS_OCF_Insert_Interface::register_methods(xmlrpc_c::registryPtr& rpcRegistry) {
	ACE_TRACE("modAOS_OCF_Insert_Interface::register_methods");

	PERIODIC_REGISTER_METHODS(modAOS_OCF_Insert, modAOS_OCF_Insert_Interface);
	AOS_PHYSICAL_CHANNEL_REGISTER_METHODS(modAOS_OCF_Insert);
	AOS_MASTER_CHANNEL_REGISTER_METHODS(modAOS_OCF_Insert);
	REGISTER_ACCESSOR_METHODS(rcvd_ocf_unit_tally, modAOS_OCF_Insert, ReceivedReportUnitCount);
	REGISTER_ACCESSOR_METHODS(rcvd_ocf_octet_tally, modAOS_OCF_Insert, ReceivedReportOctetCount);
	REGISTER_ACCESSOR_METHODS(rcvd_frame_unit_tally, modAOS_OCF_Insert, ReceivedFrameUnitCount);
	REGISTER_ACCESSOR_METHODS(rcvd_frame_octet_tally, modAOS_OCF_Insert, ReceivedFrameOctetCount);
}

} // namespace nAOS_OCF_Insert