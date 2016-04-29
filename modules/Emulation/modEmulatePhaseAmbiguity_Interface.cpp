/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modEmulatePhaseAmbiguity_Interface.cpp
 * @author Tad Kollar  
 *
 * $Id: modEmulatePhaseAmbiguity_Interface.cpp 1670 2012-08-28 13:25:28Z tkollar $ 
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "modEmulatePhaseAmbiguity.hpp"
#include "TrafficHandler_Interface.hpp"

namespace nEmulatePhaseAmbiguity {

//=============================================================================
/**
 * @class modEmulatePhaseAmbiguity_Interface
 * @author Tad Kollar  
 * @brief Provide an XML-RPC interface to the Phase Ambiguity Emulation module.
 */
//=============================================================================
class modEmulatePhaseAmbiguity_Interface: public nasaCE::TrafficHandler_Interface<modEmulatePhaseAmbiguity> {
public:
	modEmulatePhaseAmbiguity_Interface(xmlrpc_c::registryPtr& rpcRegistry):
		nasaCE::TrafficHandler_Interface<modEmulatePhaseAmbiguity>(rpcRegistry, "modEmulatePhaseAmbiguity") {
		ACE_TRACE("modEmulatePhaseAmbiguity_Interface::modEmulatePhaseAmbiguity_Interface");
		
		register_methods(rpcRegistry);
	}
	
	GENERATE_INT_ACCESSORS(left_shift_bits, setLeftShiftBits, getLeftShiftBits);
	GENERATE_BOOL_ACCESSORS(invert, setInversion, getInversion);
	GENERATE_INT_ACCESSORS(usecs_to_wait, setMaxUsecsForNewData, getMaxUsecsForNewData);

	/// Export specified methods via XML-RPC.
	void register_methods(xmlrpc_c::registryPtr&);
	
protected:
	void get_settings_(xmlrpc_c::paramList const& paramList, modEmulatePhaseAmbiguity* handler,
		xstruct& settings) {
		ACE_TRACE("modEmulatePhaseAmbiguity_Interface::get_settings_");
		
		nasaCE::TrafficHandler_Interface<modEmulatePhaseAmbiguity>::get_settings_(paramList, handler, settings);
		
		// Build a map of associated settings
		settings["leftShiftBits"] = xmlrpc_c::value_int(handler->getLeftShiftBits());
		settings["invert"] = xmlrpc_c::value_boolean(handler->getInversion());
		settings["maxUsecsForNewData"] = xmlrpc_c::value_int(handler->getMaxUsecsForNewData());
	}	
};

modEmulatePhaseAmbiguity_Interface* modEmulatePhaseAmbiguity_InterfaceP;

TEMPLATE_METHOD_CLASSES(modEmulatePhaseAmbiguity_Interface, modEmulatePhaseAmbiguity_InterfaceP);
	
GENERATE_ACCESSOR_METHODS(left_shift_bits, i, "the number of bits to left shift data.",
	modEmulatePhaseAmbiguity_InterfaceP);	
GENERATE_ACCESSOR_METHODS(invert, b, "whether to invert all of the data.",
	modEmulatePhaseAmbiguity_InterfaceP);	
GENERATE_ACCESSOR_METHODS(usecs_to_wait, i, "the microseconds to wait before sending the carry with the current block.",
	modEmulatePhaseAmbiguity_InterfaceP);	
	
void modEmulatePhaseAmbiguity_Interface::register_methods(xmlrpc_c::registryPtr& rpcRegistry) {
	ACE_TRACE("modEmulatePhaseAmbiguity_Interface::register_methods");

	TEMPLATE_REGISTER_METHODS(modEmulatePhaseAmbiguity, modEmulatePhaseAmbiguity_Interface);
	REGISTER_ACCESSOR_METHODS(left_shift_bits, modEmulatePhaseAmbiguity, LeftShiftBits);
	REGISTER_ACCESSOR_METHODS(invert, modEmulatePhaseAmbiguity, Inversion);
	REGISTER_ACCESSOR_METHODS(usecs_to_wait, modEmulatePhaseAmbiguity, MaxUsecsForNewData);
}

} // namespace nEmulatePhaseAmbiguity