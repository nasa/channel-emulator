/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modAOS_AF_Rcv_Interface.cpp
 * @author Tad Kollar  
 *
 * $Id: modAOS_AF_Rcv_Interface.cpp 2004 2013-05-31 19:07:32Z tkollar $
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================


#include "modAOS_AF_Rcv.hpp"
#include "AOS_Interface_Macros.hpp"

namespace nAOS_AF_Rcv {

//=============================================================================
/**
 * @class modAOS_AF_Rcv_Interface
 * @author Tad Kollar  
 * @brief Provide an XML-RPC interface to the Virtual Channel Receive module.
 */
//=============================================================================
class modAOS_AF_Rcv_Interface: public nasaCE::TrafficHandler_Interface<modAOS_AF_Rcv> {
public:
	modAOS_AF_Rcv_Interface(xmlrpc_c::registryPtr& rpcRegistry):
		nasaCE::TrafficHandler_Interface<modAOS_AF_Rcv>(rpcRegistry, "modAOS_AF_Rcv") {
		ACE_TRACE("modAOS_AF_Rcv_Interface::modAOS_AF_Rcv_Interface");

		register_methods(rpcRegistry);
	}

	AOS_PHYSICAL_CHANNEL_DEFINE_ACCESSORS();
	GENERATE_I8_ACCESSORS(bad_frame_crc_count, setBadFrameChecksumCount, getBadFrameChecksumCount);
	GENERATE_I8_ACCESSORS(corrected_header_errors, setCorrectedHeaderErrorCount, getCorrectedHeaderErrorCount);
	GENERATE_I8_ACCESSORS(uncorrected_header_error_count, setUncorrectedHeaderErrorCount, getUncorrectedHeaderErrorCount);
	GENERATE_I8_ACCESSORS(errorless_header_unit_count, setErrorlessHeaderUnitCount, getErrorlessHeaderUnitCount);
	GENERATE_I8_ACCESSORS(corrected_header_unit_count, setCorrectedHeaderUnitCount, getCorrectedHeaderUnitCount);
	GENERATE_I8_ACCESSORS(set_uncorrected_header_unit_count, setUncorrectedHeaderUnitCount, getUncorrectedHeaderUnitCount);
	// Export specified methods via XML-RPC.
	void register_methods(xmlrpc_c::registryPtr&);

protected:
	void get_counters_(xmlrpc_c::paramList const& paramList, modAOS_AF_Rcv* handler,
		xstruct& counters) {
		ACE_TRACE("modAOS_AF_Rcv_Interface::get_counters_");

		nasaCE::TrafficHandler_Interface<modAOS_AF_Rcv>::get_counters_(paramList, handler, counters);

		GET_AOS_PHYSICAL_CHANNEL_COUNTERS();
		counters["badFrameChecksumCount"] = xmlrpc_c::value_i8(handler->getBadFrameChecksumCount());
		counters["correctedHeaderErrors"] = xmlrpc_c::value_i8(handler->getCorrectedHeaderErrorCount());
		counters["uncorrectedHeaderErrors"] = xmlrpc_c::value_i8(handler->getUncorrectedHeaderErrorCount());
		counters["errorlessHeaderUnits"] = xmlrpc_c::value_i8(handler->getErrorlessHeaderUnitCount());
		counters["correctedHeaderUnits"] = xmlrpc_c::value_i8(handler->getCorrectedHeaderUnitCount());
		counters["uncorrectedHeaderUnits"] = xmlrpc_c::value_i8(handler->getUncorrectedHeaderUnitCount());
	}

	void get_settings_(xmlrpc_c::paramList const& paramList, modAOS_AF_Rcv* handler,
		xstruct& settings) {
		ACE_TRACE("modAOS_AF_Rcv_Interface::get_settings_");

		nasaCE::TrafficHandler_Interface<modAOS_AF_Rcv>::get_settings_(paramList, handler, settings);

		GET_AOS_PHYSICAL_CHANNEL_SETTINGS();
	}
};

modAOS_AF_Rcv_Interface* modAOS_AF_Rcv_InterfaceP;

TEMPLATE_METHOD_CLASSES(modAOS_AF_Rcv_Interface, modAOS_AF_Rcv_InterfaceP);

AOS_PHYSICAL_CHANNEL_GENERATE_METHODS(modAOS_AF_Rcv_InterfaceP);
GENERATE_ACCESSOR_METHODS(bad_frame_crc_count, i, "the tally of frames with failed CRC checks.", modAOS_AF_Rcv_InterfaceP);
GENERATE_ACCESSOR_METHODS(corrected_header_errors, i, "the tally of correctable header errors.", modAOS_AF_Rcv_InterfaceP);
GENERATE_ACCESSOR_METHODS(uncorrected_header_error_count, i, "the tally of uncorrectable header errors.", modAOS_AF_Rcv_InterfaceP);
GENERATE_ACCESSOR_METHODS(errorless_header_unit_count, i, "the tally of frames with no header errors.", modAOS_AF_Rcv_InterfaceP);
GENERATE_ACCESSOR_METHODS(corrected_header_unit_count, i, "the tally of frames with correctable header errors.", modAOS_AF_Rcv_InterfaceP);
GENERATE_ACCESSOR_METHODS(set_uncorrected_header_unit_count, i, "the tally of frames with uncorrectable header errors.", modAOS_AF_Rcv_InterfaceP);

void modAOS_AF_Rcv_Interface::register_methods(xmlrpc_c::registryPtr& rpcRegistry) {
	ACE_TRACE("modAOS_AF_Rcv_Interface::register_methods");

	TEMPLATE_REGISTER_METHODS(modAOS_AF_Rcv, modAOS_AF_Rcv_Interface);

	AOS_PHYSICAL_CHANNEL_REGISTER_METHODS(modAOS_AF_Rcv);
	REGISTER_ACCESSOR_METHODS(bad_frame_crc_count, modAOS_AF_Rcv, BadFrameCRCCount);
	REGISTER_ACCESSOR_METHODS(corrected_header_errors, modAOS_AF_Rcv, CorrectedHeaderErrorCount);
	REGISTER_ACCESSOR_METHODS(uncorrected_header_error_count, modAOS_AF_Rcv, UncorrectedHeaderErrorCount);
	REGISTER_ACCESSOR_METHODS(errorless_header_unit_count, modAOS_AF_Rcv, ErrorlessHeaderUnitCount);
	REGISTER_ACCESSOR_METHODS(corrected_header_unit_count, modAOS_AF_Rcv, CorrectedHeaderUnitCount);
	REGISTER_ACCESSOR_METHODS(set_uncorrected_header_unit_count, modAOS_AF_Rcv, UncorrectedHeaderUnitCount);
}

} // namespace nAOS_AF_Rcv
