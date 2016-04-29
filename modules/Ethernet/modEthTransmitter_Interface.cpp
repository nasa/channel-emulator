/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modEthTransmitter_Interface.cpp
 * @author Tad Kollar  
 *
 * $Id: modEthTransmitter_Interface.cpp 1670 2012-08-28 13:25:28Z tkollar $ 
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "modEthTransmitter.hpp"
#include "TrafficHandler_Interface.hpp"

namespace nEthTransmitter {

//=============================================================================
/**
 * @class modEthTransmitter_Interface
 * @author Tad Kollar  
 * @brief Provide an XML-RPC interface to the Ethernet receiver module.
 */
//=============================================================================
class modEthTransmitter_Interface: public nasaCE::TrafficHandler_Interface<modEthTransmitter> {
public:
	modEthTransmitter_Interface(xmlrpc_c::registryPtr& rpcRegistry):
		nasaCE::TrafficHandler_Interface<modEthTransmitter>(rpcRegistry, "modEthTransmitter") {
		ACE_TRACE("modEthTransmitter_Interface::modEthTransmitter_Interface");
		
		register_methods(rpcRegistry);
	}
	
	GENERATE_STRING_ACCESSORS(link_check_freq, setLinkCheckFreqStr, getLinkCheckFreqStr);
	GENERATE_INT_ACCESSORS(link_check_max_iter, setLinkCheckMaxIterations, getLinkCheckMaxIterations);
	GENERATE_INT_ACCESSORS(link_check_sleep_ms, setLinkCheckSleepMSec, getLinkCheckSleepMSec);	
	
	/// Export specified methods via XML-RPC.
	void register_methods(xmlrpc_c::registryPtr&);
	
protected:
	void get_settings_(xmlrpc_c::paramList const& paramList, modEthTransmitter* handler,
		xstruct& settings) {
		ACE_TRACE("modEthTransmitter_Interface::get_settings_");
		
		nasaCE::TrafficHandler_Interface<modEthTransmitter>::get_settings_(paramList, handler, settings);
		
		// Build a map of associated settings
		settings["linkCheckFrequency"] = xmlrpc_c::value_string(handler->getLinkCheckFreqStr());
		settings["linkCheckMaxIterations"] = xmlrpc_c::value_int(handler->getLinkCheckMaxIterations());		
		settings["linkCheckSleepMSec"] = xmlrpc_c::value_int(handler->getLinkCheckSleepMSec());
	}
};

modEthTransmitter_Interface* modEthTransmitter_InterfaceP;

TEMPLATE_METHOD_CLASSES(modEthTransmitter_Interface, modEthTransmitter_InterfaceP)
GENERATE_ACCESSOR_METHODS(link_check_freq, s, "when to check for link presence (Never, Once, Always).",
	modEthTransmitter_InterfaceP);
GENERATE_ACCESSOR_METHODS(link_check_max_iter, i, "the number of failed link check attempts allowed per test.",
modEthTransmitter_InterfaceP);
GENERATE_ACCESSOR_METHODS(link_check_sleep_ms, i, "the number of milliseconds to sleep after each failed link check attempt.",
modEthTransmitter_InterfaceP);

void modEthTransmitter_Interface::register_methods(xmlrpc_c::registryPtr& rpcRegistry) {
	ACE_TRACE("modEthTransmitter_Interface::register_methods");

	TEMPLATE_REGISTER_METHODS(modEthTransmitter, modEthTransmitter_Interface);
	REGISTER_ACCESSOR_METHODS(link_check_freq, modEthTransmitter, LinkCheckFrequency);
	REGISTER_ACCESSOR_METHODS(link_check_max_iter, modEthTransmitter, LinkCheckMaxIterations);
	REGISTER_ACCESSOR_METHODS(link_check_sleep_ms, modEthTransmitter, LinkCheckSleepMSec);	
}

} // namespace nEthTransmitter