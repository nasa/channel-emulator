/* -*- C++ -*- */

//=============================================================================
/**
 * @file   devTcp4Client_Interface.hpp
 * @author Tad Kollar  
 *
 * $Id: devTcp4Client_Interface.hpp 2111 2013-06-14 19:27:27Z tkollar $
 * Copyright (c) 2010.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _NASA_TCP4_CLIENT_DEVICE_INTERFACE_HPP_
#define _NASA_TCP4_CLIENT_DEVICE_INTERFACE_HPP_

#include "CE_DLL.hpp"
#include "CE_Macros.hpp"
#include "devTcp4Client.hpp"
#include "Device_Interface_Templ.hpp"

namespace nasaCE {

//=============================================================================
/**
 * @class devTcp4Client_Interface
 * @author Tad Kollar  
 */
//=============================================================================

class devTcp4Client_Interface : public Device_Interface_Templ<devTcp4Client> {
public:
	/// Default constructor.
	devTcp4Client_Interface();

	/// Initialize as well as registering exported methods.
	/// @param rpcRegistry XML-RPC method registry referenced by Abyss.
	devTcp4Client_Interface(xmlrpc_c::registryPtr& rpcRegistry);

	/// Destructor.
	~devTcp4Client_Interface();

	void client_create(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);

	// void client_remove(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);

	void client_is_connected(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);

	void client_set_address(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);
	void client_get_address(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);

	void client_set_port(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);
	void client_get_port(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);

	void client_set_ms_between_attempts(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);
	void client_get_ms_between_attempts(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);

	void client_open_connection(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);

	/// Export specified methods via XML-RPC.
	virtual void register_methods(xmlrpc_c::registryPtr&);
};

extern devTcp4Client_Interface* devTcp4Client_InterfaceP;

DEV_TEMPLATE_METHOD_CLASSES(devTcp4Client_Interface, devTcp4Client_InterfaceP);
METHOD_CLASS(client_create, "n:ssi",
	"Initialize a new IPv4 TCP Device.", devTcp4Client_InterfaceP);
/*
METHOD_CLASS(client_remove, "n:s",
	"Shut down an existing IPv4 TCP Device.", devTcp4Client_InterfaceP);
	*/
METHOD_CLASS(client_is_connected, "b:s",
	"Determines if the device currently has a connection.", devTcp4Client_InterfaceP);
METHOD_CLASS(client_set_address, "n:ss",
	"Set the IPv4 address to either listen on or connect to.", devTcp4Client_InterfaceP);
METHOD_CLASS(client_get_address, "s:s",
	"Retrieve the IPv4 address to either listen on or connect to.", devTcp4Client_InterfaceP);
METHOD_CLASS(client_set_port, "n:si",
	"Set the IPv4 TCP port to either listen on or connect to.", devTcp4Client_InterfaceP);
METHOD_CLASS(client_get_port, "i:s",
	"Get the IPv4 TCP port to either listen on or connect to.", devTcp4Client_InterfaceP);
METHOD_CLASS(client_set_ms_between_attempts, "n:si",
	"Set the number of milliseconds to wait between failed connection attempts.", devTcp4Client_InterfaceP);
METHOD_CLASS(client_get_ms_between_attempts, "i:s",
	"Get the number of milliseconds to wait between failed connection attempts.", devTcp4Client_InterfaceP);
METHOD_CLASS(client_open_connection, "n:s",
	"Connect to a remote TCP server.", devTcp4Client_InterfaceP);

} // namespace nasaCE

#endif // _NASA_TCP4_CLIENT_DEVICE_INTERFACE_HPP_
