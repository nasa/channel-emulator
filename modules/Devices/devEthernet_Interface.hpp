/* -*- C++ -*- */

//=============================================================================
/**
 * @file   devEthernet_Interface.hpp
 * @author Tad Kollar  
 *
 * $Id: devEthernet_Interface.hpp 2320 2013-08-06 16:01:20Z tkollar $
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _ETHERNET_DEVICE_INTERFACE_HPP_
#define _ETHERNET_DEVICE_INTERFACE_HPP_

#include "devEthernet.hpp"
#include "Device_Interface_Templ.hpp"

namespace nasaCE {

//=============================================================================
/**
 * @class devEthernet_Interface
 * @brief The modular interface to network devices.
 *
 * devEthernet_Interface maintains a map of network interface names ->
 * devEthernet objects and exports its methods with XML-RPC.
 */
//=============================================================================
class devEthernet_Interface: public Device_Interface_Templ<devEthernet> {
public:
	/// Default constructor.
	devEthernet_Interface(): Device_Interface_Templ<devEthernet>("devEthernet_Interface") {	}

	/// Initialize as well as registering exported methods.
	/// @param rpcRegistry XML-RPC method registry referenced by Abyss.
	devEthernet_Interface(xmlrpc_c::registryPtr& rpcRegistry):
		Device_Interface_Templ<devEthernet>("devEthernet_Interface") {
		ACE_TRACE("devEthernet_Interface::devEthernet_Interface");

		register_methods(rpcRegistry);
	}

	void is_up(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);

	void activate_interface(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);

	void deactivate_interface(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);

	void get_promiscuous(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);

	void set_promiscuous(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);

	void get_counters(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);

	void set_no_arp(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);

	void get_no_arp(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);

	void get_mtu(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);

	void set_mtu(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);

	void get_iface_name(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);

	void set_iface_name(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);

	void get_snaplen(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);

	void set_snaplen(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);

	void is_protected(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);

	void list_all_ifaces(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);

	/// Export specified methods via XML-RPC.
	virtual void register_methods(xmlrpc_c::registryPtr&);

};

extern devEthernet_Interface* devEthernet_InterfaceP;

DEV_TEMPLATE_METHOD_CLASSES(devEthernet_Interface, devEthernet_InterfaceP);
METHOD_CLASS(is_up, "b:s",
	"Returns if the interface is activated/deactivated.", devEthernet_InterfaceP);
METHOD_CLASS(activate_interface, "n:s",
	"Sets the state of the managed interface to 'up'.", devEthernet_InterfaceP);
METHOD_CLASS(deactivate_interface, "n:s",
	"Sets the state of the managed interface to 'down'.", devEthernet_InterfaceP);
METHOD_CLASS(get_promiscuous, "b:s",
	"Returns if the interface is promiscuous or not.", devEthernet_InterfaceP);
METHOD_CLASS(set_promiscuous, "n:sb",
	"Puts the interface into or out of promiscuous mode.", devEthernet_InterfaceP);
METHOD_CLASS(set_no_arp, "n:sb",
	"Allows/disallows the interface to process ARP packets.", devEthernet_InterfaceP);
METHOD_CLASS(get_no_arp, "b:s",
	"Returns whether the interface is processing ARP packets.", devEthernet_InterfaceP);
METHOD_CLASS(get_counters, "S:s",
	"Loads and returns statistics from sysfs.", devEthernet_InterfaceP);
METHOD_CLASS(get_mtu, "i:s",
	"Gets the MTU of the specified network interface.", devEthernet_InterfaceP);
METHOD_CLASS(set_mtu, "n:si",
	"Sets the MTU of the specified network interface.", devEthernet_InterfaceP);
METHOD_CLASS(get_iface_name, "s:s",
	"Gets the name of the interface as known to the OS.", devEthernet_InterfaceP);
METHOD_CLASS(set_iface_name, "n:ss",
	"Sets the name of the interface as known to the OS.", devEthernet_InterfaceP);
METHOD_CLASS(get_snaplen, "i:s",
	"Gets the snap length used with the specified network interface.", devEthernet_InterfaceP);
METHOD_CLASS(set_snaplen, "n:si",
	"Sets the snap length used with the specified network interface.", devEthernet_InterfaceP);
METHOD_CLASS(is_protected, "b:s",
	"Returns whether the specified device is protected.", devEthernet_InterfaceP);
METHOD_CLASS(list_all_ifaces, "A:n",
	"Returns an array of all unprotected Ethernet devices on the host.", devEthernet_InterfaceP);
} // namespace nasaCE

#endif // _ETHERNET_DEVICE_INTERFACE_HPP_
