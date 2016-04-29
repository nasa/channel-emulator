/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modASM_Remove_Interface.cpp
 * @author Tad Kollar  
 *
 * $Id: modASM_Remove_Interface.cpp 2482 2014-02-13 15:10:19Z tkollar $
 * Copyright (c) 2010.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "modASM_Remove.hpp"
#include "TrafficHandler_Interface.hpp"

namespace nASM_Remove {

//=============================================================================
/**
 * @class modASM_Remove_Interface
 * @author Tad Kollar  
 * @brief Provide an XML-RPC interface to the ASM Remove module.
 */
//=============================================================================
class modASM_Remove_Interface: public nasaCE::TrafficHandler_Interface<modASM_Remove> {
public:
	modASM_Remove_Interface(xmlrpc_c::registryPtr& rpcRegistry):
		nasaCE::TrafficHandler_Interface<modASM_Remove>(rpcRegistry, "modASM_Remove") {
		ACE_TRACE("modASM_Remove_Interface::modASM_Remove_Interface");

		register_methods(rpcRegistry);
	}

	void set_marker(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP) {
		ACE_TRACE("modASM_Remove_Interface::set_marker");
		const std::vector<xmlrpc_c::value> xml_rpc_marker = paramList.getArray(2);

		std::vector<uint8_t> marker;

		for (unsigned i = 0; i < xml_rpc_marker.size(); i++ ) {
			marker.push_back(xmlrpc_c::value_int(xml_rpc_marker[i]));
		}

		find_handler(paramList)->setMarker(marker);

		*retvalP = xmlrpc_c::value_nil();
	}

	void get_marker(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP) {
		ACE_TRACE("modASM_Remove_Interface::get_marker");

		std::vector<uint8_t> marker;
		find_handler(paramList)->getMarker(marker);

		std::vector<xmlrpc_c::value> xml_rpc_marker;

		for (unsigned i = 0; i < marker.size(); i++ ) {
			xml_rpc_marker.push_back(xmlrpc_c::value_int(marker[i]));
		}

		*retvalP = xmlrpc_c::value_array(xml_rpc_marker);
	}

	GENERATE_INT_ACCESSORS(expected_unit_length, setExpectedUnitLength, getExpectedUnitLength);
	GENERATE_INT_ACCESSORS(allowed_bit_errors, setAllowedMarkerBitErrors, getAllowedMarkerBitErrors);
	GENERATE_I8_ACCESSORS(asm_count, setAsmCount, getAsmCount);
	GENERATE_I8_ACCESSORS(asm_valid_count, setAsmValidCount, getAsmValidCount);
	GENERATE_I8_ACCESSORS(asm_missed, setAsmMissedCount, getAsmMissedCount);
	GENERATE_I8_ACCESSORS(asm_searches, setSearchCount, getSearchCount);
	GENERATE_I8_ACCESSORS(asm_allowed_bit_errs, setAsmBitErrorsAllowed, getAsmBitErrorsAllowed);
	GENERATE_I8_ACCESSORS(asm_allowed_with_errs, setAsmAllowedWithBitErrorsCount, getAsmAllowedWithBitErrorsCount);
	GENERATE_I8_ACCESSORS(asm_rejected_bit_errs, setAsmBitErrorsRejected, getAsmBitErrorsRejected);
	GENERATE_I8_ACCESSORS(asm_rejected_with_errs, setAsmRejectedWithBitErrorsCount, getAsmRejectedWithBitErrorsCount);
	GENERATE_I8_ACCESSORS(asm_partial_errs, setPartialMismatchCount, getPartialMismatchCount);
	GENERATE_I8_ACCESSORS(asm_discovered_count, setAsmDiscoveredCount, getAsmDiscoveredCount);

	/// Export specified methods via XML-RPC.
	void register_methods(xmlrpc_c::registryPtr&);

protected:
	void get_counters_(xmlrpc_c::paramList const& paramList, modASM_Remove* handler,
		xstruct& counters) {
		ACE_TRACE("modASM_Remove_Interface::get_counters_");

		nasaCE::TrafficHandler_Interface<modASM_Remove>::get_counters_(paramList, handler, counters);

		counters["asmCount"] = xmlrpc_c::value_i8(handler->getAsmCount());
		counters["asmValidCount"] = xmlrpc_c::value_i8(handler->getAsmValidCount());
		counters["asmMissedCount"] = xmlrpc_c::value_i8(handler->getAsmMissedCount());
		counters["asmSearchCount"] = xmlrpc_c::value_i8(handler->getSearchCount());
		counters["asmAllowedBitErrorCount"] = xmlrpc_c::value_i8(handler->getAsmBitErrorsAllowed());
		counters["asmAllowedUnitErrorCount"] = xmlrpc_c::value_i8(handler->getAsmAllowedWithBitErrorsCount());
		counters["asmRejectedBitErrorCount"] = xmlrpc_c::value_i8(handler->getAsmBitErrorsRejected());
		counters["asmRejectedUnitErrorCount"] = xmlrpc_c::value_i8(handler->getAsmRejectedWithBitErrorsCount());
		counters["asmPartialCount"] = xmlrpc_c::value_i8(handler->getPartialMismatchCount());
		counters["asmDiscoveredCount"] = xmlrpc_c::value_i8(handler->getAsmDiscoveredCount());
	}

	void get_settings_(xmlrpc_c::paramList const& paramList, modASM_Remove* handler,
		xstruct& settings) {
		ACE_TRACE("modASM_Remove_Interface::get_settings_");

		nasaCE::TrafficHandler_Interface<modASM_Remove>::get_settings_(paramList, handler, settings);
		using namespace std;

		vector<uint8_t> marker;
		handler->getMarker(marker);
		vector<uint8_t>::iterator pos;
		ostringstream os;

		for ( pos = marker.begin(); pos != marker.end(); ++pos )
			os << uppercase << hex << setw(2) << setfill('0') << static_cast<int>(*pos);

		settings["markerPattern"] = xmlrpc_c::value_string(os.str());
		settings["expectedUnitLength"] = xmlrpc_c::value_int(handler->getExpectedUnitLength());
		settings["allowedMarkerBitErrors"] = xmlrpc_c::value_int(handler->getAllowedMarkerBitErrors());
	}
};

modASM_Remove_Interface* modASM_Remove_InterfaceP;

TEMPLATE_METHOD_CLASSES(modASM_Remove_Interface, modASM_Remove_InterfaceP);
METHOD_CLASS(set_marker, "n:A", "Sets a new attached sync marker (ASM).", modASM_Remove_InterfaceP);
METHOD_CLASS(get_marker, "A:n", "Returns the current attached sync marker (ASM).", modASM_Remove_InterfaceP);
GENERATE_ACCESSOR_METHODS(expected_unit_length, i, "the length of all expected data units.",
	modASM_Remove_InterfaceP);
GENERATE_ACCESSOR_METHODS(allowed_bit_errors, i, "the number of bit errors allowed in an ASM match.",
	modASM_Remove_InterfaceP);
GENERATE_ACCESSOR_METHODS(asm_count, i, "the grand total of ASMs that were located.",
	modASM_Remove_InterfaceP);
GENERATE_ACCESSOR_METHODS(asm_valid_count, i, "the tally of ASMs found exactly where expected.",
	modASM_Remove_InterfaceP);
GENERATE_ACCESSOR_METHODS(asm_missed, i, "the tally of ASMs that were not where they were supposed to be.",
	modASM_Remove_InterfaceP);
GENERATE_ACCESSOR_METHODS(asm_searches, i, "the tally of ASM searches started.",
	modASM_Remove_InterfaceP);
GENERATE_ACCESSOR_METHODS(asm_allowed_bit_errs, i, "the tally of bit errors accepted in ASMs.",
	modASM_Remove_InterfaceP);
GENERATE_ACCESSOR_METHODS(asm_allowed_with_errs, i, "the tally of ASM units accepted containing errors.",
	modASM_Remove_InterfaceP);
GENERATE_ACCESSOR_METHODS(asm_rejected_bit_errs, i, "the tally of bit errors rejected in ASMs.",
	modASM_Remove_InterfaceP);
GENERATE_ACCESSOR_METHODS(asm_rejected_with_errs, i, "the tally of ASM units rejected containing errors.",
	modASM_Remove_InterfaceP);
GENERATE_ACCESSOR_METHODS(asm_partial_errs, i, "the tally of ASMs with a correct head but not tail.",
	modASM_Remove_InterfaceP);
GENERATE_ACCESSOR_METHODS(asm_discovered_count, i, "the tally of ASMs found only after a search.",
	modASM_Remove_InterfaceP);

void modASM_Remove_Interface::register_methods(xmlrpc_c::registryPtr& rpcRegistry) {
	ACE_TRACE("modASM_Remove_Interface::register_methods");

	TEMPLATE_REGISTER_METHODS(modASM_Remove, modASM_Remove_Interface);
	REGISTER_ACCESSOR_METHODS(marker, modASM_Remove, ASM);
	REGISTER_ACCESSOR_METHODS(expected_unit_length, modASM_Remove, ExpectedUnitLength);
	REGISTER_ACCESSOR_METHODS(allowed_bit_errors, modASM_Remove, AllowedMarkerBitErrors);
	REGISTER_ACCESSOR_METHODS(asm_count, modASM_Remove, ASMCount);
	REGISTER_ACCESSOR_METHODS(asm_valid_count, modASM_Remove, ASMValidCount);
	REGISTER_ACCESSOR_METHODS(asm_missed, modASM_Remove, ASMMissedCount);
	REGISTER_ACCESSOR_METHODS(asm_searches, modASM_Remove, ASMSearches);
	REGISTER_ACCESSOR_METHODS(asm_allowed_bit_errs, modASM_Remove, ASMAllowedBitErrors);
	REGISTER_ACCESSOR_METHODS(asm_allowed_with_errs, modASM_Remove, ASMAllowedUnitErrors);
	REGISTER_ACCESSOR_METHODS(asm_rejected_bit_errs, modASM_Remove, ASMRejectedBitErrors);
	REGISTER_ACCESSOR_METHODS(asm_rejected_with_errs, modASM_Remove, ASMRejectedUnitErrors);
	REGISTER_ACCESSOR_METHODS(asm_partial_errs, modASM_Remove, ASMPartialErrors);
	REGISTER_ACCESSOR_METHODS(asm_discovered_count, modASM_Remove, ASMDiscoveredCount);
}

} // namespace nASM_Remove
