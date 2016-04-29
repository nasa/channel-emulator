/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modAOS_M_PDU_Add_Interface.cpp
 * @author Tad Kollar  
 *
 * $Id: modAOS_M_PDU_Add_Interface.cpp 2018 2013-06-04 14:59:19Z tkollar $ 
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "modAOS_M_PDU_Add.hpp"
#include "TrafficHandler_Interface.hpp"

namespace nAOS_M_PDU_Add {

//=============================================================================
/**
 * @class modAOS_M_PDU_Add_Interface
 * @author Tad Kollar  
 * @brief Provide an XML-RPC interface to the AOS Multiplexing PDU Addition module.
 */
//=============================================================================
class modAOS_M_PDU_Add_Interface: public nasaCE::TrafficHandler_Interface<modAOS_M_PDU_Add> {
public:
	modAOS_M_PDU_Add_Interface(xmlrpc_c::registryPtr& rpcRegistry):
		nasaCE::TrafficHandler_Interface<modAOS_M_PDU_Add>(rpcRegistry, "modAOS_M_PDU_Add") {
		ACE_TRACE("modAOS_M_PDU_Add_Interface::modAOS_M_PDU_Add_Interface");
		
		register_methods(rpcRegistry);
	}
	
	GENERATE_INT_ACCESSORS(length, set_M_PDU_Length, get_M_PDU_Length);
	GENERATE_INT_ACCESSORS(max_usecs, setMaxUsecsForNextPacket, getMaxUsecsForNextPacket);
	GENERATE_BOOL_ACCESSORS(immediate_send, setSendImmediately, isSendingImmediately);
	GENERATE_BOOL_ACCESSORS(multi_packet_zone, setMultiPacketZone, getMultiPacketZone);
	
	void set_fill_pattern(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP) {
		ACE_TRACE("modAOS_M_PDU_Add_Interface::set_fill_pattern");
		const std::vector<xmlrpc_c::value> xml_rpc_pattern = paramList.getArray(2);
		
		std::vector<uint8_t> fillPattern;
		
		for (unsigned i = 0; i < xml_rpc_pattern.size(); i++ ) {
			fillPattern[i] = xmlrpc_c::value_int(xml_rpc_pattern[i]);
		}
		
		find_handler(paramList)->setFillPattern(fillPattern);
		
		*retvalP = xmlrpc_c::value_nil();
	}
	
	void get_fill_pattern(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP) {
		ACE_TRACE("modAOS_M_PDU_Add_Interface::get_fill_pattern");
		
		std::vector<uint8_t> fillPattern;
		find_handler(paramList)->getFillPattern(fillPattern);
		
		std::vector<xmlrpc_c::value> xml_rpc_pattern;
		
		for (unsigned i = 0; i < fillPattern.size(); i++ ) {
			xml_rpc_pattern[i] = xmlrpc_c::value_int(fillPattern[i]);
		}
		
		*retvalP = xmlrpc_c::value_array(xml_rpc_pattern);
	}
	
	/// Export specified methods via XML-RPC.
	void register_methods(xmlrpc_c::registryPtr&);
	
protected:
	
	void get_settings_(xmlrpc_c::paramList const& paramList, modAOS_M_PDU_Add* handler,
		xstruct& settings) {
		ACE_TRACE("modAOS_M_PDU_Add_Interface::get_settings_");
		
		nasaCE::TrafficHandler_Interface<modAOS_M_PDU_Add>::get_settings_(paramList, handler, settings);
		
		// Build a map of associated settings
		settings["immediateSend"] = xmlrpc_c::value_string(handler->isSendingImmediately()? "True" : "False");
		settings["maxUsecsForNextPacket"] = xmlrpc_c::value_int(handler->getMaxUsecsForNextPacket());
		
		using namespace std;
		vector<uint8_t> fillPattern;
		handler->getFillPattern(fillPattern);
		vector<uint8_t>::iterator pos;
		ostringstream os;
		
		for ( pos = fillPattern.begin(); pos != fillPattern.end(); ++pos ) 
			os << uppercase << hex << setw(2) << setfill('0') << static_cast<int>(*pos);
		settings["fillPattern"] = xmlrpc_c::value_string(os.str());
	}
};

modAOS_M_PDU_Add_Interface* modAOS_M_PDU_Add_InterfaceP;

TEMPLATE_METHOD_CLASSES(modAOS_M_PDU_Add_Interface, modAOS_M_PDU_Add_InterfaceP);

GENERATE_ACCESSOR_METHODS(length, i, "the length of generated AOS Multiplexing PDUs.", modAOS_M_PDU_Add_InterfaceP);
GENERATE_ACCESSOR_METHODS(max_usecs, i, "how long to wait for an incoming packet when the current M_PDU isn't full.", modAOS_M_PDU_Add_InterfaceP);
GENERATE_ACCESSOR_METHODS(immediate_send, b, "whether each M_PDU is sent as soon as it's filled.", modAOS_M_PDU_Add_InterfaceP);
GENERATE_ACCESSOR_METHODS(multi_packet_zone, b, "whether multiple packets may be found in the Packet Zone.",
	modAOS_M_PDU_Add_InterfaceP);
METHOD_CLASS(set_fill_pattern, "n:A", "Sets a new fill pattern for the Packet Zone.", modAOS_M_PDU_Add_InterfaceP);
METHOD_CLASS(get_fill_pattern, "A:n", "Returns the current fill pattern used in the Packet Zone.", modAOS_M_PDU_Add_InterfaceP);

void modAOS_M_PDU_Add_Interface::register_methods(xmlrpc_c::registryPtr& rpcRegistry) {
	ACE_TRACE("modAOS_M_PDU_Add_Interface::register_methods");

	TEMPLATE_REGISTER_METHODS(modAOS_M_PDU_Add, modAOS_M_PDU_Add_Interface);

	REGISTER_ACCESSOR_METHODS(length, modAOS_M_PDU_Add, Length);
	REGISTER_ACCESSOR_METHODS(max_usecs, modAOS_M_PDU_Add, WaitForNextPacket);
	REGISTER_ACCESSOR_METHODS(immediate_send, modAOS_M_PDU_Add, ImmediateSend);
	REGISTER_ACCESSOR_METHODS(multi_packet_zone, modAOS_M_PDU_Add, MultiPacketZone);	
	REGISTER_ACCESSOR_METHODS(fill_pattern, modAOS_M_PDU_Add, FillPattern);	
}

} // namespace nAOS_M_PDU_Add