/* -*- C++ -*- */

//=============================================================================
/**
 * @file   devTcp4Server_Interface.hpp
 * @author Tad Kollar  
 *
 * $Id: devTcp4Server_Interface.hpp 2080 2013-06-12 19:03:30Z tkollar $
 * Copyright (c) 2010.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _NASA_TCP4_SERVER_DEVICE_INTERFACE_HPP_
#define _NASA_TCP4_SERVER_DEVICE_INTERFACE_HPP_

#include "CE_DLL.hpp"
#include "CE_Macros.hpp"
#include "devTcp4Server.hpp"
#include "Device_Interface_Templ.hpp"

namespace nasaCE {

//=============================================================================
/**
 * @class devTcp4Server_Interface
 * @author Tad Kollar  
 */
//=============================================================================

class devTcp4Server_Interface : public Device_Interface_Templ<devTcp4Server> {
public:
	/// Default constructor.
	devTcp4Server_Interface();

	/// Initialize as well as registering exported methods.
	/// @param rpcRegistry XML-RPC method registry referenced by Abyss.
	devTcp4Server_Interface(xmlrpc_c::registryPtr& rpcRegistry);

	/// Destructor.
	~devTcp4Server_Interface();

	void server_create(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);

	void server_is_serving(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);

	void server_is_connected(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);

	void server_set_address(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);

	void server_get_address(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);

	void server_set_port(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);

	void server_get_port(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);

	void server_start_listening(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);

	/// Export specified methods via XML-RPC.
	virtual void register_methods(xmlrpc_c::registryPtr&);
};

extern devTcp4Server_Interface* devTcp4Server_InterfaceP;

DEV_TEMPLATE_METHOD_CLASSES(devTcp4Server_Interface, devTcp4Server_InterfaceP);
METHOD_CLASS(server_create, "n:ssi",
	"Initialize a new IPv4 TCP Device.", devTcp4Server_InterfaceP);
METHOD_CLASS(server_is_serving, "b:s",
	"Determines if the device is able to accept connections.", devTcp4Server_InterfaceP);
METHOD_CLASS(server_is_connected, "b:s",
	"Determines if the device currently has a connection.", devTcp4Server_InterfaceP);
METHOD_CLASS(server_set_address, "n:ss",
	"Set the IPv4 address to either listen on or connect to.", devTcp4Server_InterfaceP);
METHOD_CLASS(server_get_address, "s:s",
	"Retrieve the IPv4 address to either listen on or connect to.", devTcp4Server_InterfaceP);
METHOD_CLASS(server_set_port, "n:si",
	"Set the IPv4 TCP port to either listen on or connect to.", devTcp4Server_InterfaceP);
METHOD_CLASS(server_get_port, "i:s",
	"Get the IPv4 TCP port to either listen on or connect to.", devTcp4Server_InterfaceP);
METHOD_CLASS(server_start_listening, "b:s",
	"If configured as a server, begin waiting for connections.", devTcp4Server_InterfaceP);

} // namespace nasaCE

#endif // _NASA_TCP4_SERVER_DEVICE_INTERFACE_HPP_
