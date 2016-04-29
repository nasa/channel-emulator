/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modEthFrame_Add_Interface.cpp
 * @author Tad Kollar  
 *
 * $Id: modEthFrame_Add_Interface.cpp 1670 2012-08-28 13:25:28Z tkollar $ 
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================


#include "modEthFrame_Add.hpp"
#include "TrafficHandler_Interface.hpp"

#include <sstream>
#include <iomanip>

namespace nEthFrame_Add {

//=============================================================================
/**
 * @class modEthFrame_Add_Interface
 * @author Tad Kollar  
 * @brief Provide an XML-RPC interface to the Ethernet Frame Add module.
 */
//=============================================================================
class modEthFrame_Add_Interface: public nasaCE::TrafficHandler_Interface<modEthFrame_Add> {
public:
	modEthFrame_Add_Interface(xmlrpc_c::registryPtr& rpcRegistry):
		nasaCE::TrafficHandler_Interface<modEthFrame_Add>(rpcRegistry, "modEthFrame_Add") {
		ACE_TRACE("modEthFrame_Add_Interface::modEthFrame_Add_Interface");
		
		register_methods(rpcRegistry);
	}

	virtual void set_frame_type(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP) {
		ACE_TRACE("modEthFrame_Add_Interface::set_frame_type");
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
		ACE_TRACE("modEthFrame_Add_Interface::get_frame_type");
		
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

	GENERATE_STRING_ACCESSORS(dst_mac, setDstMACStr, getDstMACStr);
	GENERATE_STRING_ACCESSORS(src_mac, setSrcMACStr, getSrcMACStr);
	GENERATE_INT_ACCESSORS(default_payload_type, setDefaultPayloadType, getDefaultPayloadType);
	
	/// Export specified methods via XML-RPC.
	void register_methods(xmlrpc_c::registryPtr&);

protected:	
	void get_settings_(xmlrpc_c::paramList const& paramList, modEthFrame_Add* handler,
		xstruct& settings) {
		ACE_TRACE("modEthFrame_Add_Interface::get_settings_");
		
		nasaCE::TrafficHandler_Interface<modEthFrame_Add>::get_settings_(paramList, handler, settings);
		
		// Build a map of associated settings
		settings["frameType"] = xmlrpc_c::value_string(handler->getFrameTypeStr());
		settings["dstMAC"] = xmlrpc_c::value_string(handler->getDstMACStr());
		settings["srcMAC"] = xmlrpc_c::value_string(handler->getSrcMACStr());
		
		using namespace std;
		ostringstream os;
		
		os << "0x" << uppercase << hex << setw(4) << setfill('0') << handler->getDefaultPayloadType();
		settings["defaultPayloadType"] = xmlrpc_c::value_string(os.str());
	}

};

modEthFrame_Add_Interface* modEthFrame_Add_InterfaceP;

TEMPLATE_METHOD_CLASSES(modEthFrame_Add_Interface, modEthFrame_Add_InterfaceP);
GENERATE_ACCESSOR_METHODS(frame_type, s, "the type of Ethernet Frame expected (PCap, LinuxTap, IEEE).",
	modEthFrame_Add_InterfaceP);
GENERATE_ACCESSOR_METHODS(dst_mac, s, "the destination MAC address to use in each frame.",
modEthFrame_Add_InterfaceP);
GENERATE_ACCESSOR_METHODS(src_mac, s, "the source MAC to use in each frame.",
modEthFrame_Add_InterfaceP);
GENERATE_ACCESSOR_METHODS(default_payload_type, i, "the Ethernet payload type to use for unrecognized data.",
modEthFrame_Add_InterfaceP);
	
void modEthFrame_Add_Interface::register_methods(xmlrpc_c::registryPtr& rpcRegistry) {
	ACE_TRACE("modEthFrame_Add_Interface::register_methods");

	TEMPLATE_REGISTER_METHODS(modEthFrame_Add, modEthFrame_Add_Interface);
	REGISTER_ACCESSOR_METHODS(frame_type, modEthFrame_Add, FrameType);
	REGISTER_ACCESSOR_METHODS(dst_mac, modEthFrame_Add, DstMAC);
	REGISTER_ACCESSOR_METHODS(src_mac, modEthFrame_Add, SrcMAC);
	REGISTER_ACCESSOR_METHODS(default_payload_type, modEthFrame_Add, DefaultPayloadType);

}

} // namespace nEthFrame_Add
