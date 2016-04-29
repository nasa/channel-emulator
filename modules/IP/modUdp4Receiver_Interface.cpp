/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modUdp4Receiver_Interface.cpp
 * @author Tad Kollar  
 *
 * $Id: modUdp4Receiver_Interface.cpp 1670 2012-08-28 13:25:28Z tkollar $ 
 * Copyright (c) 2011.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "modUdp4Receiver.hpp"
#include "TrafficHandler_Interface.hpp"

namespace nUdp4Receiver {

//=============================================================================
/**
 * @class modUdp4Receiver_Interface
 * @author Tad Kollar  
 * @brief Provide an XML-RPC interface to the IPv4 UDP receiver module.
 */
//=============================================================================
class modUdp4Receiver_Interface: public nasaCE::TrafficHandler_Interface<modUdp4Receiver> {
public:

	modUdp4Receiver_Interface(xmlrpc_c::registryPtr& rpcRegistry):
		nasaCE::TrafficHandler_Interface<modUdp4Receiver>(rpcRegistry, "modUdp4Receiver") {
		ACE_TRACE("modUdp4Receiver_Interface::modUdp4Receiver_Interface");
		
		register_methods(rpcRegistry);
	}
	
	GENERATE_INT_ACCESSORS(bufferSize, setBufferSize, getBufferSize);
	GENERATE_BOOL_ACCESSORS(receiveMax, setReceiveMax, getReceiveMax);
	
	/// Export specified methods via XML-RPC.
	void register_methods(xmlrpc_c::registryPtr&);
	
protected:
	void get_settings_(xmlrpc_c::paramList const& paramList, modUdp4Receiver* handler,
		xstruct& settings) {
		ACE_TRACE("modUdp4Receiver_Interface::get_settings_");
		
		nasaCE::TrafficHandler_Interface<modUdp4Receiver>::get_settings_(paramList, handler, settings);
		
		// Build a map of associated settings
		settings["receiveMax"] = xmlrpc_c::value_string(handler->getReceiveMax()? "True" : "False");		
	}
};

modUdp4Receiver_Interface* modUdp4Receiver_InterfaceP;

TEMPLATE_METHOD_CLASSES(modUdp4Receiver_Interface, modUdp4Receiver_InterfaceP);
GENERATE_ACCESSOR_METHODS(bufferSize, i, "the size of the buffer to hold data received from the socket.",
	modUdp4Receiver_InterfaceP);
GENERATE_ACCESSOR_METHODS(receiveMax, b, "whether to wait for the full MRU with each recv call.",
	modUdp4Receiver_InterfaceP);	
	
void modUdp4Receiver_Interface::register_methods(xmlrpc_c::registryPtr& rpcRegistry) {
	ACE_TRACE("modUdp4Receiver_Interface::register_methods");

	TEMPLATE_REGISTER_METHODS(modUdp4Receiver, modUdp4Receiver_Interface);
	REGISTER_ACCESSOR_METHODS(bufferSize, modUdp4Receiver, BufferSize);
	REGISTER_ACCESSOR_METHODS(receiveMax, modUdp4Receiver, ReceiveMax);	
	
}

} // namespace nUdp4Receiver