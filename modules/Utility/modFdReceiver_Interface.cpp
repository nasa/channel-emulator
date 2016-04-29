/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modFdReceiver_Interface.cpp
 * @author Tad Kollar  
 *
 * $Id: modFdReceiver_Interface.cpp 1670 2012-08-28 13:25:28Z tkollar $ 
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "modFdReceiver.hpp"
#include "TrafficHandler_Interface.hpp"

namespace nFdReceiver {

//=============================================================================
/**
 * @class modFdReceiver_Interface
 * @author Tad Kollar  
 * @brief Provide an XML-RPC interface to the FD receiver module.
 */
//=============================================================================
class modFdReceiver_Interface: public nasaCE::TrafficHandler_Interface<modFdReceiver> {

public:
	modFdReceiver_Interface(xmlrpc_c::registryPtr& rpcRegistry):
		nasaCE::TrafficHandler_Interface<modFdReceiver>(rpcRegistry, "modFdReceiver") {
		ACE_TRACE("modFdReceiver_Interface::modFdReceiver_Interface");
		
		register_methods(rpcRegistry);
	}
	
	void get_max_read(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP) {
		ACE_TRACE("modFdReceiver_Interface::get_max_read");
		
		*retvalP = xmlrpc_c::value_int(find_handler(paramList)->getMaxRead());		
	}	

	void set_max_read(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP) {
		ACE_TRACE("modFdReceiver_Interface::set_max_read");
		
		modFdReceiver* handler = find_handler(paramList);
		
		const unsigned newMaxRead = (int) paramList.getInt(2);
		
		handler->setMaxRead(newMaxRead);
		
		*retvalP = xmlrpc_c::value_nil();		
	}
	
	void get_repeat_count(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP) {
		ACE_TRACE("modFdReceiver_Interface::get_repeat_count");

		modFdReceiver* handler = find_handler(paramList);

		*retvalP = xmlrpc_c::value_int(handler->getRepeatCount());
	}
	
	void get_repeat_max(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP) {
		ACE_TRACE("modFdReceiver_Interface::get_repeat_max");

		modFdReceiver* handler = find_handler(paramList);

		*retvalP = xmlrpc_c::value_int(handler->getRepeatMax());
	}
	
	void set_repeat_max(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP) {
		ACE_TRACE("modFdReceiver_Interface:set_repeat_max");

		modFdReceiver* handler = find_handler(paramList);
		const unsigned newRepeatMax = (int) paramList.getInt(2);
		
		handler->setRepeatMax(newRepeatMax);

		*retvalP = xmlrpc_c::value_nil();
	}
	
	/// Export specified methods via XML-RPC.
	void register_methods(xmlrpc_c::registryPtr&);
	
protected:
	void get_counters_(xmlrpc_c::paramList const& paramList, modFdReceiver* handler,
		xstruct& counters) {
		ACE_TRACE("modFdReceiver_Interface::get_counters_");
	
		nasaCE::TrafficHandler_Interface<modFdReceiver>::get_counters_(paramList, handler, counters);

		// Build a map of all associated counters
		counters["repeatCount"] = xmlrpc_c::value_int(handler->getRepeatCount());
	}
	
	void get_settings_(xmlrpc_c::paramList const& paramList, modFdReceiver* handler,
		xstruct& settings) {
		ACE_TRACE("modFdReceiver_Interface::get_settings_");
		
		nasaCE::TrafficHandler_Interface<modFdReceiver>::get_settings_(paramList, handler, settings);
		
		// Build a map of associated settings
		settings["repeatMax"] = xmlrpc_c::value_int(handler->getRepeatMax());
	}
};

modFdReceiver_Interface* modFdReceiver_InterfaceP;

TEMPLATE_METHOD_CLASSES(modFdReceiver_Interface, modFdReceiver_InterfaceP);

METHOD_CLASS(get_max_read, "i:ss",
	"Gets the maximum block read value.", modFdReceiver_InterfaceP);
METHOD_CLASS(set_max_read, "n:ssi",
	"Sets the maximum block read value.", modFdReceiver_InterfaceP);
METHOD_CLASS(get_repeat_count, "i:ss",
	"Return the number of times the regular file has been read/sent.", modFdReceiver_InterfaceP);	
METHOD_CLASS(get_repeat_max, "i:ss",
	"Return the number of times the regular file is to be read/sent.", modFdReceiver_InterfaceP);
METHOD_CLASS(set_repeat_max, "n:ssi",
	"Set the number of times to read/send a regular file.", modFdReceiver_InterfaceP);
	
void modFdReceiver_Interface::register_methods(xmlrpc_c::registryPtr& rpcRegistry) {
	ACE_TRACE("modFdReceiver_Interface::register_methods");

	TEMPLATE_REGISTER_METHODS(modFdReceiver, modFdReceiver_Interface);
	REGISTER_METHOD(get_max_read, "modFdReceiver.getMaxRead");
	REGISTER_METHOD(set_max_read, "modFdReceiver.setMaxRead");
	REGISTER_METHOD(get_repeat_count, "modFdReceiver.getRepeatCount");
	REGISTER_METHOD(get_repeat_max, "modFdReceiver.getRepeatMax");
	REGISTER_METHOD(set_repeat_max, "modFdReceiver.setRepeatMax");
}

} // namespace nFdReceiver