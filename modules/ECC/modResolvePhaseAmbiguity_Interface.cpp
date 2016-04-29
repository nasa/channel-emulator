/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modResolvePhaseAmbiguity_Interface.cpp
 * @author Tad Kollar  
 *
 * $Id: modResolvePhaseAmbiguity_Interface.cpp 1670 2012-08-28 13:25:28Z tkollar $ 
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "modResolvePhaseAmbiguity.hpp"
#include "TrafficHandler_Interface.hpp"

namespace nResolvePhaseAmbiguity {

//=============================================================================
/**
 * @class modResolvePhaseAmbiguity_Interface
 * @author Tad Kollar  
 * @brief Provide an XML-RPC interface to the Phase Ambiguity Resolution module.
 */
//=============================================================================
class modResolvePhaseAmbiguity_Interface: public nasaCE::TrafficHandler_Interface<modResolvePhaseAmbiguity> {
public:
	modResolvePhaseAmbiguity_Interface(xmlrpc_c::registryPtr& rpcRegistry):
		nasaCE::TrafficHandler_Interface<modResolvePhaseAmbiguity>(rpcRegistry, "modResolvePhaseAmbiguity") {
		ACE_TRACE("modResolvePhaseAmbiguity_Interface::modResolvePhaseAmbiguity_Interface");
		
		register_methods(rpcRegistry);
	}
	
	GENERATE_INT_ACCESSORS(left_shift_bits, setLeftShiftBits, getLeftShiftBits);
	GENERATE_BOOL_ACCESSORS(invert, setInversion, getInversion);
	GENERATE_INT_ACCESSORS(usecs_to_wait, setMaxUsecsForNewData, getMaxUsecsForNewData);
	GENERATE_BOOL_ACCESSORS(detect_shift, setDetectShift, getDetectShift);
	GENERATE_INT_ACCESSORS(allowed_bit_errors, setAllowedMarkerBitErrors, getAllowedMarkerBitErrors);
	
	void set_marker(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP) {
		ACE_TRACE("modResolvePhaseAmbiguity::set_marker");
		const std::vector<xmlrpc_c::value> xml_rpc_marker = paramList.getArray(2);
		
		std::vector<uint8_t> marker;
		
		for (unsigned i = 0; i < xml_rpc_marker.size(); i++ ) {
			marker[i] = xmlrpc_c::value_int(xml_rpc_marker[i]);
		}
		
		find_handler(paramList)->setMarker(marker);
		
		*retvalP = xmlrpc_c::value_nil();
	}
	
	void get_marker(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP) {
		ACE_TRACE("modResolvePhaseAmbiguity::get_marker");
		
		std::vector<uint8_t> marker;
		find_handler(paramList)->getMarker(marker);
		
		std::vector<xmlrpc_c::value> xml_rpc_marker;
		
		for (unsigned i = 0; i < marker.size(); i++ ) {
			xml_rpc_marker[i] = xmlrpc_c::value_int(marker[i]);
		}
		
		*retvalP = xmlrpc_c::value_array(xml_rpc_marker);
	}	

	/// Export specified methods via XML-RPC.
	void register_methods(xmlrpc_c::registryPtr&);
	
protected:
	void get_settings_(xmlrpc_c::paramList const& paramList, modResolvePhaseAmbiguity* handler,
		xstruct& settings) {
		ACE_TRACE("modResolvePhaseAmbiguity_Interface::get_settings_");
		
		nasaCE::TrafficHandler_Interface<modResolvePhaseAmbiguity>::get_settings_(paramList, handler, settings);
		
		// Build a map of associated settings
		settings["leftShiftBits"] = xmlrpc_c::value_int(handler->getLeftShiftBits());
		settings["invert"] = xmlrpc_c::value_boolean(handler->getInversion());
		settings["maxUsecsForNewData"] = xmlrpc_c::value_int(handler->getMaxUsecsForNewData());
		settings["allowedMarkerBitErrors"] = xmlrpc_c::value_int(handler->getAllowedMarkerBitErrors());
		
		using namespace std;
		
		vector<uint8_t> marker;
		handler->getMarker(marker);
		vector<uint8_t>::iterator pos;
		ostringstream os;
		
		for ( pos = marker.begin(); pos != marker.end(); ++pos )
			os << uppercase << hex << setw(2) << setfill('0') << static_cast<int>(*pos);
			
		settings["markerPattern"] = xmlrpc_c::value_string(os.str());		
	}	
};

modResolvePhaseAmbiguity_Interface* modResolvePhaseAmbiguity_InterfaceP;

TEMPLATE_METHOD_CLASSES(modResolvePhaseAmbiguity_Interface, modResolvePhaseAmbiguity_InterfaceP);
	
GENERATE_ACCESSOR_METHODS(left_shift_bits, i, "the number of bits to left shift data.",
	modResolvePhaseAmbiguity_InterfaceP);	
GENERATE_ACCESSOR_METHODS(invert, b, "whether to invert all of the data.",
	modResolvePhaseAmbiguity_InterfaceP);	
GENERATE_ACCESSOR_METHODS(usecs_to_wait, i, "the microseconds to wait before sending the carry with the current block.",
	modResolvePhaseAmbiguity_InterfaceP);
GENERATE_ACCESSOR_METHODS(detect_shift, b, "whether to auto detect phase shift/inversion.",
	modResolvePhaseAmbiguity_InterfaceP);
GENERATE_ACCESSOR_METHODS(allowed_bit_errors, i, "the number of bit errors allowed in an ASM match.",
	modResolvePhaseAmbiguity_InterfaceP);	
METHOD_CLASS(set_marker, "n:A", "Sets a new attached sync marker (ASM).", modResolvePhaseAmbiguity_InterfaceP);
METHOD_CLASS(get_marker, "A:n", "Returns the current attached sync marker (ASM).", modResolvePhaseAmbiguity_InterfaceP);
	
void modResolvePhaseAmbiguity_Interface::register_methods(xmlrpc_c::registryPtr& rpcRegistry) {
	ACE_TRACE("modResolvePhaseAmbiguity_Interface::register_methods");

	TEMPLATE_REGISTER_METHODS(modResolvePhaseAmbiguity, modResolvePhaseAmbiguity_Interface);
	REGISTER_ACCESSOR_METHODS(left_shift_bits, modResolvePhaseAmbiguity, LeftShiftBits);
	REGISTER_ACCESSOR_METHODS(invert, modResolvePhaseAmbiguity, Inversion);
	REGISTER_ACCESSOR_METHODS(usecs_to_wait, modResolvePhaseAmbiguity, MaxUsecsForNewData);
	REGISTER_ACCESSOR_METHODS(detect_shift, modResolvePhaseAmbiguity, DetectShift);	
	REGISTER_ACCESSOR_METHODS(allowed_bit_errors, modResolvePhaseAmbiguity, AllowedBitErrors);
	REGISTER_ACCESSOR_METHODS(marker, modResolvePhaseAmbiguity, ASM);	
}

} // namespace nResolvePhaseAmbiguity