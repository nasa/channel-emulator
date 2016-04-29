/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modExtractor_Interface.cpp
 * @author Tad Kollar  
 *
 * $Id: modExtractor_Interface.cpp 1670 2012-08-28 13:25:28Z tkollar $ 
 * Copyright (c) 2010.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================


#include "modExtractor.hpp"
#include "TrafficHandler_Interface.hpp"

namespace nExtractor {

//=============================================================================
/**
 * @class modExtractor_Interface
 * @author Tad Kollar  
 * @brief Provide an XML-RPC interface to the Extractor module.
 */
//=============================================================================
class modExtractor_Interface: public nasaCE::TrafficHandler_Interface<modExtractor> {
public:
	modExtractor_Interface(xmlrpc_c::registryPtr& rpcRegistry):
		nasaCE::TrafficHandler_Interface<modExtractor>(rpcRegistry, "modExtractor") {
		ACE_TRACE("modExtractor_Interface::modExtractor_Interface");
		
		register_methods(rpcRegistry);
	}
	
	GENERATE_INT_ACCESSORS(header_length, setHeaderLength, getHeaderLength);
	GENERATE_INT_ACCESSORS(trailer_length, setTrailerLength, getTrailerLength);
	GENERATE_INT_ACCESSORS(header_octet_count, setHeaderOctetCount, getHeaderOctetCount);
	GENERATE_INT_ACCESSORS(trailer_octet_count, setTrailerOctetCount, getTrailerOctetCount);
	GENERATE_INT_ACCESSORS(stub_count, setStubCount, getStubCount);
	
	/// Export specified methods via XML-RPC.
	void register_methods(xmlrpc_c::registryPtr&);

protected:
	void get_counters_(xmlrpc_c::paramList const& paramList, modExtractor* handler,
		xstruct& counters) {
		ACE_TRACE("modExtractor_Interface::get_counters_");
	
		nasaCE::TrafficHandler_Interface<modExtractor>::get_counters_(paramList, handler, counters);

		// Build a map of all associated counters
		counters["headerOctetCount"] = xmlrpc_c::value_int(handler->getHeaderOctetCount());
		counters["trailerOctetCount"] = xmlrpc_c::value_int(handler->getTrailerOctetCount());
		counters["stubCount"] = xmlrpc_c::value_int(handler->getStubCount());
	}
	
	void get_settings_(xmlrpc_c::paramList const& paramList, modExtractor* handler,
		xstruct& settings) {
		ACE_TRACE("modExtractor_Interface::get_settings_");
		
		nasaCE::TrafficHandler_Interface<modExtractor>::get_settings_(paramList, handler, settings);
		
		// Build a map of associated settings
		settings["headerLength"] = xmlrpc_c::value_int(handler->getHeaderLength());
		settings["trailerLength"] = xmlrpc_c::value_int(handler->getTrailerLength());
	}
};

modExtractor_Interface* modExtractor_InterfaceP;

TEMPLATE_METHOD_CLASSES(modExtractor_Interface, modExtractor_InterfaceP);
GENERATE_ACCESSOR_METHODS(header_length, i, "the length of the header to remove.",
	modExtractor_InterfaceP);
GENERATE_ACCESSOR_METHODS(trailer_length, i, "the length of the trailer to remove.",
	modExtractor_InterfaceP);
GENERATE_ACCESSOR_METHODS(header_octet_count, i, "the tally of octets removed from all headers.",
	modExtractor_InterfaceP);
GENERATE_ACCESSOR_METHODS(trailer_octet_count, i, "the tally of octets removed from all trailers.",
	modExtractor_InterfaceP);
GENERATE_ACCESSOR_METHODS(stub_count, i, "the tally of units too short to extract.",
	modExtractor_InterfaceP);	
	
void modExtractor_Interface::register_methods(xmlrpc_c::registryPtr& rpcRegistry) {
	ACE_TRACE("modExtractor_Interface::register_methods");

	TEMPLATE_REGISTER_METHODS(modExtractor, modExtractor_Interface);
	REGISTER_ACCESSOR_METHODS(header_length, modExtractor, HeaderLength);
	REGISTER_ACCESSOR_METHODS(trailer_length, modExtractor, TrailerLength);
	REGISTER_ACCESSOR_METHODS(header_octet_count, modExtractor, HeaderOctetCount);
	REGISTER_ACCESSOR_METHODS(trailer_octet_count, modExtractor, TrailerOctetCount);
	REGISTER_ACCESSOR_METHODS(stub_count, modExtractor, StubCount);
}

} // namespace Extractor