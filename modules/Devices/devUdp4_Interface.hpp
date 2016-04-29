/* -*- C++ -*- */

//=============================================================================
/**
 * @file   devUdp4_Interface.hpp
 * @author Tad Kollar  
 *
 * $Id: devUdp4_Interface.hpp 2080 2013-06-12 19:03:30Z tkollar $
 * Copyright (c) 2011.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _NASA_UDP4_SERVER_DEVICE_INTERFACE_HPP_
#define _NASA_UDP4_SERVER_DEVICE_INTERFACE_HPP_

#include "CE_DLL.hpp"
#include "CE_Macros.hpp"
#include "devUdp4.hpp"
#include "Device_Interface_Templ.hpp"

namespace nasaCE {

//=============================================================================
/**
 * @class devUdp4_Interface
 * @author Tad Kollar  
 */
//=============================================================================

class devUdp4_Interface : public Device_Interface_Templ<devUdp4> {
public:
	/// Default constructor.
	devUdp4_Interface();

	/// Initialize as well as registering exported methods.
	/// @param rpcRegistry XML-RPC method registry referenced by Abyss.
	devUdp4_Interface(xmlrpc_c::registryPtr& rpcRegistry);

	/// Destructor.
	~devUdp4_Interface();

	void dev_udp_create(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);

//	void dev_udp_remove(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);

	void dev_udp_set_address(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);

	void dev_udp_get_address(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);

	void dev_udp_set_port(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);

	void dev_udp_get_port(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);

	void dev_udp_bind(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);

	/// Export specified methods via XML-RPC.
	virtual void register_methods(xmlrpc_c::registryPtr&);
};

extern devUdp4_Interface* devUdp4_InterfaceP;

DEV_TEMPLATE_METHOD_CLASSES(devUdp4_Interface, devUdp4_InterfaceP);
METHOD_CLASS(dev_udp_create, "n:ssi",
	"Initialize a new IPv4 UDP Device.", devUdp4_InterfaceP);
/*
METHOD_CLASS(dev_udp_remove, "n:s",
	"Shut down an existing IPv4 UDO Device.", devUdp4_InterfaceP);
	*/
METHOD_CLASS(dev_udp_set_address, "n:ss",
	"Set the IPv4 address for the socket to receive on.", devUdp4_InterfaceP);
METHOD_CLASS(dev_udp_get_address, "s:s",
	"Retrieve the IPv4 address for the socket to receive on.", devUdp4_InterfaceP);
METHOD_CLASS(dev_udp_set_port, "n:si",
	"Set the IPv4 UDP port to receive on.", devUdp4_InterfaceP);
METHOD_CLASS(dev_udp_get_port, "i:s",
	"Get the IPv4 TCP port to receive on.", devUdp4_InterfaceP);
METHOD_CLASS(dev_udp_bind, "n:s",
	"Create (if necessary) and bind to a new socket.", devUdp4_InterfaceP);

} // namespace nasaCE

#endif // _NASA_UDP4_SERVER_DEVICE_INTERFACE_HPP_
