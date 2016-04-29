/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modEthFrame_Remove_Interface.cpp
 * @author Tad Kollar  
 *
 * $Id: modEthFrame_Remove_Interface.cpp 1670 2012-08-28 13:25:28Z tkollar $ 
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "modEthFrame_Remove.hpp"
#include "TrafficHandler_Interface.hpp"

#include <sstream>
#include <iomanip>

namespace nEthFrame_Remove {

//=============================================================================
/**
 * @class modEthFrame_Remove_Interface
 * @author Tad Kollar  
 * @brief Provide an XML-RPC interface to the Ethernet Frame Removal module.
 */
//=============================================================================
class modEthFrame_Remove_Interface: public nasaCE::TrafficHandler_Interface<modEthFrame_Remove> {
public:
	modEthFrame_Remove_Interface(xmlrpc_c::registryPtr& rpcRegistry):
		nasaCE::TrafficHandler_Interface<modEthFrame_Remove>(rpcRegistry, "modEthFrame_Remove") {
		ACE_TRACE("modEthFrame_Remove_Interface::modEthFrame_Remove_Interface");
		
		register_methods(rpcRegistry);
	}

	virtual void set_frame_type(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP) {
		ACE_TRACE("modEthFrame_Remove_Interface::set_frame_type");
		std::string frameTypeStr = (std::string) paramList.getString(2);
		std::ostringstream os;
		
		os << std::uppercase << frameTypeStr;
		
		EthernetFrame::SupportedFrameTypes newType;
		
		if ( os.str() == "PCAP" ) newType = EthernetFrame::PCap;
		else if ( os.str() == "TAP" || os.str() == "LINUXTAP" ) newType = EthernetFrame::LinuxTap;
		else if ( os.str() == "IEEE" ) newType = EthernetFrame::IEEE;
		else throw nd_error(std::string("Unsupported Ethernet frame type '") + frameTypeStr +
			std::string("' specified."));
		
		find_handler(paramList)->setFrameType(newType);
		
		*retvalP = xmlrpc_c::value_nil();		
	}

	virtual void get_frame_type(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP) {
		ACE_TRACE("modEthFrame_Remove_Interface::get_frame_type");
		
		std::string frameTypeStr;
		
		switch (find_handler(paramList)->getFrameType()) {
			case EthernetFrame::PCap:
				frameTypeStr = "PCap";
				break;
			case EthernetFrame::LinuxTap:
				frameTypeStr = "LinuxTap";
				break;
			case EthernetFrame::IEEE:
				frameTypeStr = "IEEE";
				break;
			default:
				frameTypeStr = "Unknown";			
		}
		
		*retvalP = xmlrpc_c::value_string(frameTypeStr);		
	}

	/// Export specified methods via XML-RPC.
	void register_methods(xmlrpc_c::registryPtr&);

protected:
	void get_settings_(xmlrpc_c::paramList const& paramList, modEthFrame_Remove* handler,
		xstruct& settings) {
		ACE_TRACE("modEthFrame_Remove_Interface::get_settings_");
		
		nasaCE::TrafficHandler_Interface<modEthFrame_Remove>::get_settings_(paramList, handler, settings);
		
		settings["frameType"] = xmlrpc_c::value_string(handler->getFrameTypeStr());
	}
};

modEthFrame_Remove_Interface* modEthFrame_Remove_InterfaceP;

TEMPLATE_METHOD_CLASSES(modEthFrame_Remove_Interface, modEthFrame_Remove_InterfaceP)
GENERATE_ACCESSOR_METHODS(frame_type, s, "the type of Ethernet Frame expected (PCap, LinuxTap, IEEE).",
	modEthFrame_Remove_InterfaceP);
	
void modEthFrame_Remove_Interface::register_methods(xmlrpc_c::registryPtr& rpcRegistry) {
	ACE_TRACE("modEthFrame_Remove_Interface::register_methods");

	TEMPLATE_REGISTER_METHODS(modEthFrame_Remove, modEthFrame_Remove_Interface);
	REGISTER_ACCESSOR_METHODS(frame_type, modEthFrame_Remove, FrameType);
}

} // namespace nEthFrame_Remove
