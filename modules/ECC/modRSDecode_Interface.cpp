/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modRSDecode_Interface.cpp
 * @author Tad Kollar  
 *
 * $Id: modRSDecode_Interface.cpp 2004 2013-05-31 19:07:32Z tkollar $
 * Copyright (c) 2011.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "modRSDecode.hpp"
#include "TrafficHandler_Interface.hpp"

namespace nRSDecode {

//=============================================================================
/**
 * @class modRSDecode_Interface
 * @author Tad Kollar  
 * @brief Provide an XML-RPC interface to the CCSDS Reed-Solomon Decoder module.
 */
//=============================================================================
class modRSDecode_Interface: public nasaCE::TrafficHandler_Interface<modRSDecode> {
public:

	modRSDecode_Interface(xmlrpc_c::registryPtr& rpcRegistry):
		nasaCE::TrafficHandler_Interface<modRSDecode>(rpcRegistry, "modRSDecode") {
		ACE_TRACE("modRSDecode_Interface::modRSDecode_Interface");

		register_methods(rpcRegistry);
	}

	GENERATE_INT_ACCESSORS(maxErrorsPerCodeWord, setMaxErrorsPerCodeword, getMaxErrorsPerCodeword);
	GENERATE_INT_ACCESSORS(interleavingDepth, setInterleavingDepth, getInterleavingDepth);
	GENERATE_I8_ACCESSORS(correctedErrorCount, setCorrectedErrorCount, getCorrectedErrorCount);
	GENERATE_I8_ACCESSORS(uncorrected_error_count, setUncorrectedErrorCount, getUncorrectedErrorCount);
	GENERATE_I8_ACCESSORS(errorless_unit_count, setErrorlessUnitCount, getErrorlessUnitCount);
	GENERATE_I8_ACCESSORS(corrected_unit_count, setCorrectedUnitCount, getCorrectedUnitCount);
	GENERATE_I8_ACCESSORS(set_uncorrected_unit_count, setUncorrectedUnitCount, getUncorrectedUnitCount);

	void rebuild_decoder(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP) {
		ACE_TRACE("modRSDecode_Interface::rebuild_decoder");

		find_handler(paramList)->rebuildDecoder();

		*retvalP = xmlrpc_c::value_nil();
	}

	/// Export specified methods via XML-RPC.
	void register_methods(xmlrpc_c::registryPtr&);

protected:
	void get_counters_(xmlrpc_c::paramList const& paramList, modRSDecode* handler,
		xstruct& counters) {
		ACE_TRACE("modRSDecode_Interface::get_counters_");

		nasaCE::TrafficHandler_Interface<modRSDecode>::get_counters_(paramList, handler, counters);

		counters["correctedErrors"] = xmlrpc_c::value_i8(handler->getCorrectedErrorCount());
		counters["uncorrectedErrors"] = xmlrpc_c::value_i8(handler->getUncorrectedErrorCount());
		counters["errorlessUnits"] = xmlrpc_c::value_i8(handler->getErrorlessUnitCount());
		counters["correctedUnits"] = xmlrpc_c::value_i8(handler->getCorrectedUnitCount());
		counters["uncorrectedUnits"] = xmlrpc_c::value_i8(handler->getUncorrectedUnitCount());
	}

	void get_settings_(xmlrpc_c::paramList const& paramList, modRSDecode* handler,
		xstruct& settings) {
		ACE_TRACE("modRSDecode_Interface::get_settings_");

		nasaCE::TrafficHandler_Interface<modRSDecode>::get_settings_(paramList, handler, settings);

		// Build a map of associated settings
		settings["maxErrorsPerCodeWord"] = xmlrpc_c::value_int(handler->getMaxErrorsPerCodeword());
		settings["interleavingDepth"] = xmlrpc_c::value_int(handler->getInterleavingDepth());
	}
};

modRSDecode_Interface* modRSDecode_InterfaceP;

TEMPLATE_METHOD_CLASSES(modRSDecode_Interface, modRSDecode_InterfaceP);
GENERATE_ACCESSOR_METHODS(maxErrorsPerCodeWord, i, "the Reed-Solomon error correction capability, in symbols, within a codeword.",
	modRSDecode_InterfaceP);
GENERATE_ACCESSOR_METHODS(interleavingDepth, i, "the number of codewords to expect for decoding.",
	modRSDecode_InterfaceP);
GENERATE_ACCESSOR_METHODS(correctedErrorCount, i, "the tally of errors corrected.",
	modRSDecode_InterfaceP);
GENERATE_ACCESSOR_METHODS(uncorrected_error_count, i, "the tally of uncorrectable errors.", modRSDecode_InterfaceP);
GENERATE_ACCESSOR_METHODS(errorless_unit_count, i, "the tally of units with no errors.", modRSDecode_InterfaceP);
GENERATE_ACCESSOR_METHODS(corrected_unit_count, i, "the tally of units with correctable errors.", modRSDecode_InterfaceP);
GENERATE_ACCESSOR_METHODS(set_uncorrected_unit_count, i, "the tally of units with uncorrectable errors.", modRSDecode_InterfaceP);
METHOD_CLASS(rebuild_decoder, "n:n",
	"After settings have been changed, rebuild the decoder function.", modRSDecode_InterfaceP);

void modRSDecode_Interface::register_methods(xmlrpc_c::registryPtr& rpcRegistry) {
	ACE_TRACE("modRSDecode_Interface::register_methods");

	TEMPLATE_REGISTER_METHODS(modRSDecode, modRSDecode_Interface);
	REGISTER_ACCESSOR_METHODS(maxErrorsPerCodeWord, modRSDecode, MaxErrorsPerCodeWord);
	REGISTER_ACCESSOR_METHODS(interleavingDepth, modRSDecode, InterleavingDepth);
	REGISTER_ACCESSOR_METHODS(correctedErrorCount, modRSDecode, CorrectedErrorCount);
	REGISTER_ACCESSOR_METHODS(uncorrected_error_count, modRSDecode, UncorrectedErrorCount);
	REGISTER_ACCESSOR_METHODS(errorless_unit_count, modRSDecode, ErrorlessUnitCount);
	REGISTER_ACCESSOR_METHODS(corrected_unit_count, modRSDecode, CorrectedUnitCount);
	REGISTER_ACCESSOR_METHODS(set_uncorrected_unit_count, modRSDecode, UncorrectedUnitCount);
	REGISTER_METHOD(rebuild_decoder, "modRSDecode.rebuildDecoder");
}

} // namespace nRSDecode