/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modASM_Add_Interface.cpp
 * @author Tad Kollar  
 *
 * $Id: modASM_Add_Interface.cpp 1931 2013-04-02 18:19:22Z tkollar $
 * Copyright (c) 2010.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "modASM_Add.hpp"
#include "TrafficHandler_Interface.hpp"

namespace nASM_Add {

//=============================================================================
/**
 * @class modASM_Add_Interface
 * @author Tad Kollar  
 * @brief Provide an XML-RPC interface to the ASM Add module.
 */
//=============================================================================
class modASM_Add_Interface: public nasaCE::TrafficHandler_Interface<modASM_Add> {
public:
	modASM_Add_Interface(xmlrpc_c::registryPtr& rpcRegistry):
		nasaCE::TrafficHandler_Interface<modASM_Add>(rpcRegistry, "modASM_Add") {
		ACE_TRACE("modASM_Add_Interface::modASM_Add_Interface");

		register_methods(rpcRegistry);
	}

	void set_marker(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP) {
		ACE_TRACE("modASM_Add_Interface::set_marker");
		const std::vector<xmlrpc_c::value> xml_rpc_marker = paramList.getArray(2);

		std::vector<uint8_t> marker;

		for (unsigned i = 0; i < xml_rpc_marker.size(); i++ ) {
			marker.push_back(xmlrpc_c::value_int(xml_rpc_marker[i]));
		}

		find_handler(paramList)->setMarker(marker);

		*retvalP = xmlrpc_c::value_nil();
	}

	void get_marker(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP) {
		ACE_TRACE("modASM_Add_Interface::get_marker");

		std::vector<uint8_t> marker;
		find_handler(paramList)->getMarker(marker);

		std::vector<xmlrpc_c::value> xml_rpc_marker;

		for (unsigned i = 0; i < marker.size(); i++ ) {
			xml_rpc_marker.push_back(xmlrpc_c::value_int(marker[i]));
		}

		*retvalP = xmlrpc_c::value_array(xml_rpc_marker);
	}

	/// Export specified methods via XML-RPC.
	void register_methods(xmlrpc_c::registryPtr&);

protected:
	void get_counters_(xmlrpc_c::paramList const& paramList, modASM_Add* handler,
		xstruct& counters) {
		ACE_TRACE("modASM_Add_Interface::get_counters_");

		nasaCE::TrafficHandler_Interface<modASM_Add>::get_counters_(paramList, handler, counters);

		counters["asmCount"] = xmlrpc_c::value_i8(handler->getAsmCount());
	}

	void get_settings_(xmlrpc_c::paramList const& paramList, modASM_Add* handler,
		xstruct& settings) {
		ACE_TRACE("modASM_Add_Interface::get_settings_");

		nasaCE::TrafficHandler_Interface<modASM_Add>::get_settings_(paramList, handler, settings);
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

modASM_Add_Interface* modASM_Add_InterfaceP;

TEMPLATE_METHOD_CLASSES(modASM_Add_Interface, modASM_Add_InterfaceP);
METHOD_CLASS(set_marker, "n:A", "Sets a new attached sync marker (ASM).", modASM_Add_InterfaceP);
METHOD_CLASS(get_marker, "A:n", "Returns the current attached sync marker (ASM).", modASM_Add_InterfaceP);

void modASM_Add_Interface::register_methods(xmlrpc_c::registryPtr& rpcRegistry) {
	ACE_TRACE("modASM_Add_Interface::register_methods");

	TEMPLATE_REGISTER_METHODS(modASM_Add, modASM_Add_Interface);
	REGISTER_ACCESSOR_METHODS(marker, modASM_Add, ASM);

}

} // namespace nASM_Add