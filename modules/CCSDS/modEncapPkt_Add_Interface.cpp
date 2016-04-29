/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modEncapPkt_Add_Interface.cpp
 * @author Tad Kollar  
 *
 * $Id: modEncapPkt_Add_Interface.cpp 1856 2013-01-30 14:16:24Z tkollar $ 
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "modEncapPkt_Add.hpp"
#include "TrafficHandler_Interface.hpp"

namespace nEncapPkt_Add {

//=============================================================================
/**
 * @class modEncapPkt_Add_Interface
 * @author Tad Kollar  
 * @brief Provide an XML-RPC interface to the Encapsulation Packet Addition module.
 */
//=============================================================================
struct modEncapPkt_Add_Interface: public nasaCE::TrafficHandler_Interface<modEncapPkt_Add> {
	modEncapPkt_Add_Interface(xmlrpc_c::registryPtr& rpcRegistry):
		nasaCE::TrafficHandler_Interface<modEncapPkt_Add>(rpcRegistry, "modEncapPkt_Add") {
		ACE_TRACE("modEncapPkt_Add_Interface::modEncapPkt_Add_Interface");
		
		register_methods(rpcRegistry);
	}
	
	GENERATE_BOOL_ACCESSORS(ipe_support, setIpeSupport, getIpeSupport);
	GENERATE_I8_ACCESSORS(ipe, setIpe, getIpe);
	
	void clear_ipe(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP) {
		ACE_TRACE("modEncapPkt_Add_Interface::clear_ipe");
		
		modEncapPkt_Add* handler = find_handler(paramList);
				
		handler->clearIpe();
		
		*retvalP = xmlrpc_c::value_nil();		
	}

	GENERATE_INT_ACCESSORS(protocol, setProtocol, getProtocol);
	
	void clear_protocol(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP) {
		ACE_TRACE("modEncapPkt_Add_Interface::clear_protocol");
		
		modEncapPkt_Add* handler = find_handler(paramList);
				
		handler->clearProtocol();
		
		*retvalP = xmlrpc_c::value_nil();		
	}
	
	GENERATE_INT_ACCESSORS(length_of_length, setLengthOfLength, getLengthOfLength);
	GENERATE_INT_ACCESSORS(user_defined_field, setUserDefinedField, getUserDefinedField);
	GENERATE_INT_ACCESSORS(prot_id_ext, setProtocolIdExt, getProtocolIdExt);
	GENERATE_INT_ACCESSORS(ccsds_def_field, setCcsdsDefinedField, getCcsdsDefinedField);

	/// Export specified methods via XML-RPC.
	void register_methods(xmlrpc_c::registryPtr&);
};

modEncapPkt_Add_Interface* modEncapPkt_Add_InterfaceP;

TEMPLATE_METHOD_CLASSES(modEncapPkt_Add_Interface, modEncapPkt_Add_InterfaceP);
GENERATE_ACCESSOR_METHODS(ipe_support, i, "whether the Internet Protocol Extension is supported.",
	modEncapPkt_Add_InterfaceP);
GENERATE_ACCESSOR_METHODS(ipe, i, "the value to use for the Internet Protocol Extension instead of autodetection.",
	modEncapPkt_Add_InterfaceP);
METHOD_CLASS(clear_ipe, "n:ss",
	"Stop using a custom IPE and attempt to autodetect instead.", modEncapPkt_Add_InterfaceP);	
GENERATE_ACCESSOR_METHODS(protocol, i, "the value to use for the Protocol ID instead of autodetection.",
	modEncapPkt_Add_InterfaceP);
METHOD_CLASS(clear_protocol, "n:ss",
	"Stop using a custom protocol value and attempt to autodetect instead.", modEncapPkt_Add_InterfaceP);
GENERATE_ACCESSOR_METHODS(length_of_length, i, "the value to use for the Length of Length field.",
	modEncapPkt_Add_InterfaceP);
GENERATE_ACCESSOR_METHODS(user_defined_field, i, "the value to use for the User Defined Field.",
	modEncapPkt_Add_InterfaceP);
GENERATE_ACCESSOR_METHODS(prot_id_ext, i, "the value to use for the Protocol ID Extension field (not the same as IPE!).",
	modEncapPkt_Add_InterfaceP);
GENERATE_ACCESSOR_METHODS(ccsds_def_field, i, "the value to use for the CCSDS Defined Field.",
	modEncapPkt_Add_InterfaceP);
	
void modEncapPkt_Add_Interface::register_methods(xmlrpc_c::registryPtr& rpcRegistry) {
	ACE_TRACE("modEncapPkt_Add_Interface::register_methods");

	TEMPLATE_REGISTER_METHODS(modEncapPkt_Add, modEncapPkt_Add_Interface);
	REGISTER_ACCESSOR_METHODS(ipe_support, modEncapPkt_Add, SupportIPE);
	REGISTER_ACCESSOR_METHODS(ipe, modEncapPkt_Add, IPE);
	REGISTER_METHOD(clear_ipe, "modEncapPkt_Add.unsetIPE");
	REGISTER_ACCESSOR_METHODS(protocol, modEncapPkt_Add, Protocol);
	REGISTER_METHOD(clear_protocol, "modEncapPkt_Add.unsetProtocol");
	REGISTER_ACCESSOR_METHODS(length_of_length, modEncapPkt_Add, LengthOfLength);
	REGISTER_ACCESSOR_METHODS(user_defined_field, modEncapPkt_Add, UserDefinedField);
	REGISTER_ACCESSOR_METHODS(prot_id_ext, modEncapPkt_Add, ProtocolIDExtension);
	REGISTER_ACCESSOR_METHODS(ccsds_def_field, modEncapPkt_Add, CCSDSDefinedField);
}

} // namespace nEncapPkt_Add