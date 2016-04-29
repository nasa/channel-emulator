/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modRSEncode_Interface.cpp
 * @author Tad Kollar  
 *
 * $Id: modRSEncode_Interface.cpp 1670 2012-08-28 13:25:28Z tkollar $ 
 * Copyright (c) 2011.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "modRSEncode.hpp"
#include "TrafficHandler_Interface.hpp"

namespace nRSEncode {

//=============================================================================
/**
 * @class modRSEncode_Interface
 * @author Tad Kollar  
 * @brief Provide an XML-RPC interface to the CCSDS Reed-Solomon Encoder module.
 */
//=============================================================================
class modRSEncode_Interface: public nasaCE::TrafficHandler_Interface<modRSEncode> {
public:

	modRSEncode_Interface(xmlrpc_c::registryPtr& rpcRegistry):
		nasaCE::TrafficHandler_Interface<modRSEncode>(rpcRegistry, "modRSEncode") {
		ACE_TRACE("modRSEncode_Interface::modRSEncode_Interface");
		
		register_methods(rpcRegistry);
	}
	
	GENERATE_INT_ACCESSORS(maxErrorsPerCodeWord, setMaxErrorsPerCodeword, getMaxErrorsPerCodeword);
	GENERATE_INT_ACCESSORS(interleavingDepth, setInterleavingDepth, getInterleavingDepth);
		
	void rebuild_encoder(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP) {
		ACE_TRACE("modRSEncode_Interface::rebuild_encoder");
			
		find_handler(paramList)->rebuildEncoder();
		
		*retvalP = xmlrpc_c::value_nil();
	}
		
	/// Export specified methods via XML-RPC.
	void register_methods(xmlrpc_c::registryPtr&);
	
protected:
	void get_settings_(xmlrpc_c::paramList const& paramList, modRSEncode* handler,
		xstruct& settings) {
		ACE_TRACE("modRSEncode_Interface::get_settings_");
		
		nasaCE::TrafficHandler_Interface<modRSEncode>::get_settings_(paramList, handler, settings);
		
		// Build a map of associated settings
		settings["maxErrorsPerCodeWord"] = xmlrpc_c::value_int(handler->getMaxErrorsPerCodeword());		
		settings["interleavingDepth"] = xmlrpc_c::value_int(handler->getInterleavingDepth());		
	}
};

modRSEncode_Interface* modRSEncode_InterfaceP;

TEMPLATE_METHOD_CLASSES(modRSEncode_Interface, modRSEncode_InterfaceP);
GENERATE_ACCESSOR_METHODS(maxErrorsPerCodeWord, i, "the Reed-Solomon error correction capability, in symbols, within a codeword.",
	modRSEncode_InterfaceP);
GENERATE_ACCESSOR_METHODS(interleavingDepth, i, "the number of codewords to use for encoding.",
	modRSEncode_InterfaceP);
METHOD_CLASS(rebuild_encoder, "n:n",
	"After settings have been changed, rebuild the encoder function.", modRSEncode_InterfaceP);
	
void modRSEncode_Interface::register_methods(xmlrpc_c::registryPtr& rpcRegistry) {
	ACE_TRACE("modRSEncode_Interface::register_methods");

	TEMPLATE_REGISTER_METHODS(modRSEncode, modRSEncode_Interface);
	REGISTER_ACCESSOR_METHODS(maxErrorsPerCodeWord, modRSEncode, MaxErrorsPerCodeWord);
	REGISTER_ACCESSOR_METHODS(interleavingDepth, modRSEncode, InterleavingDepth);	
	REGISTER_METHOD(rebuild_encoder, "modRSEncode.rebuildEncoder");
}

} // namespace nRSEncode