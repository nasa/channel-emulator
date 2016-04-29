/* -*- C++ -*- */

//=============================================================================
/**
 * @file   devEthernet_Interface.cpp
 * @author Tad Kollar  
 *
 * $Id: devEthernet_Interface.cpp 2320 2013-08-06 16:01:20Z tkollar $
 * Copyright (c) 2008.
 *      NASA Glenn Research Center.  All rights reserved.
 */
//=============================================================================

#include "DeviceRegistry.hpp"
#include "devEthernet_Interface.hpp"
#include "SettingsManager.hpp"

namespace nasaCE {

void devEthernet_Interface::is_up(xmlrpc_c::paramList const& paramList,
	xmlrpc_c::value* retvalP) {
	ACE_TRACE("devEthernet_Interface::is_up");

	*retvalP = xmlrpc_c::value_boolean(find_device_(paramList)->isUp());
}

void devEthernet_Interface::activate_interface(xmlrpc_c::paramList const& paramList,
	xmlrpc_c::value* retvalP) {
	ACE_TRACE("devEthernet_Interface::activate_interface");

	std::string ifname = (std::string) paramList.getString(0);
	devEthernet* iface = 0;

	if (! devices::instance()->exists(ifname)) {
		iface = new devEthernet(ifname, this);
		devices::instance()->add(iface);
	}
	else {
		find_device_(paramList)->activate();
	}

	*retvalP = xmlrpc_c::value_nil();
}

void devEthernet_Interface::deactivate_interface(xmlrpc_c::paramList const& paramList,
	xmlrpc_c::value* retvalP) {
	ACE_TRACE("devEthernet_Interface::deactivate_interface");
	const std::string ifname(paramList.getString(0));

	devEthernet* iface = find_device_(paramList);

	iface->deactivate();
	CEcfg::instance()->get("Devices").remove(ifname);

	*retvalP = xmlrpc_c::value_nil();
}

void devEthernet_Interface::get_promiscuous(xmlrpc_c::paramList const& paramList,
	xmlrpc_c::value* retvalP) {
	ACE_TRACE("devEthernet_Interface::get_promiscuous");

	*retvalP = xmlrpc_c::value_boolean(find_device_(paramList)->isPromiscuous());
}

void devEthernet_Interface::set_promiscuous(xmlrpc_c::paramList const& paramList,
	xmlrpc_c::value* retvalP) {
	ACE_TRACE("devEthernet_Interface::set_promiscuous");
	if ( paramList.getBoolean(1) == true ) find_device_(paramList)->enablePromiscuous();
	else find_device_(paramList)->disablePromiscuous();

	*retvalP = xmlrpc_c::value_nil();
}

void devEthernet_Interface::get_counters(xmlrpc_c::paramList const& paramList,
	xmlrpc_c::value* retvalP) {
	ACE_TRACE("devEthernet_Interface::get_counters");

	std::map<std::string, xmlrpc_c::value> counters;
	find_device_(paramList)->loadCounters(counters);

	*retvalP = xmlrpc_c::value_struct(counters);
}

void devEthernet_Interface::set_no_arp(xmlrpc_c::paramList const& paramList,
	xmlrpc_c::value* retvalP) {
	ACE_TRACE("devEthernet_Interface::set_no_arp");

	if ( paramList.getBoolean(1) == false ) find_device_(paramList)->enableARP();
	else find_device_(paramList)->disableARP();

	*retvalP = xmlrpc_c::value_nil();
}

void devEthernet_Interface::get_no_arp(xmlrpc_c::paramList const& paramList,
	xmlrpc_c::value* retvalP) {
	ACE_TRACE("devEthernet_Interface::get_no_arp");

	*retvalP = xmlrpc_c::value_boolean(find_device_(paramList)->isNoARP());
}

void devEthernet_Interface::get_mtu(xmlrpc_c::paramList const& paramList,
	xmlrpc_c::value* retvalP) {
	ACE_TRACE("devEthernet_Interface::get_mtu");

	*retvalP = xmlrpc_c::value_int(find_device_(paramList)->getMTU());
}

void devEthernet_Interface::set_mtu(xmlrpc_c::paramList const& paramList,
	xmlrpc_c::value* retvalP) {
	ACE_TRACE("devEthernet_Interface::set_mtu");

	int newMtu = paramList.getInt(1);

	find_device_(paramList)->setMTU(newMtu);

	*retvalP = xmlrpc_c::value_nil();
}

void devEthernet_Interface::get_iface_name(xmlrpc_c::paramList const& paramList,
	xmlrpc_c::value* retvalP) {
	ACE_TRACE("devEthernet_Interface::get_iface_name");

	*retvalP = xmlrpc_c::value_string(find_device_(paramList)->getIfaceName());
}

void devEthernet_Interface::set_iface_name(xmlrpc_c::paramList const& paramList,
	xmlrpc_c::value* retvalP) {
	ACE_TRACE("devEthernet_Interface::set_iface_name");

	std::string newName = paramList.getString(1);

	find_device_(paramList)->setIfaceName(newName);

	*retvalP = xmlrpc_c::value_nil();
}

void devEthernet_Interface::get_snaplen(xmlrpc_c::paramList const& paramList,
	xmlrpc_c::value* retvalP) {
	ACE_TRACE("devEthernet_Interface::get_snaplen");

	*retvalP = xmlrpc_c::value_int(find_device_(paramList)->getSnapLen());
}

void devEthernet_Interface::set_snaplen(xmlrpc_c::paramList const& paramList,
	xmlrpc_c::value* retvalP) {
	ACE_TRACE("devEthernet_Interface::set_snaplen");

	find_device_(paramList)->setSnapLen(paramList.getInt(1));

	*retvalP = xmlrpc_c::value_nil();
}

void devEthernet_Interface::is_protected(xmlrpc_c::paramList const& paramList,
	xmlrpc_c::value* retvalP) {
	ACE_TRACE("devEthernet_Interface::is_protected");

	*retvalP = xmlrpc_c::value_boolean(find_device_(paramList)->isProtected());
}

void devEthernet_Interface::list_all_ifaces(xmlrpc_c::paramList const& paramList,
	xmlrpc_c::value* retvalP) {
	ACE_TRACE("devEthernet_Interface::list_all_ifaces");

	std::vector<xmlrpc_c::value> xmlIfaces;
	std::list<std::string> stringIfaces;

	devEthernet::listAllInterfaces(stringIfaces);

	std::list<std::string>::iterator pos;
	for (pos = stringIfaces.begin(); pos != stringIfaces.end(); ++pos) {
		xmlIfaces.push_back(xmlrpc_c::value_string(*pos));
	}

	*retvalP = xmlrpc_c::value_array(xmlIfaces);
}

void devEthernet_Interface::register_methods(xmlrpc_c::registryPtr& rpcRegistry) {
	ACE_TRACE("devEthernet_Interface::register_methods");

	DEV_TEMPLATE_REGISTER_METHODS(devEthernet, devEthernet_Interface);
	REGISTER_METHOD(is_up, "devEthernet.isUp");
	REGISTER_METHOD(activate_interface, "devEthernet.activate");
	REGISTER_METHOD(deactivate_interface, "devEthernet.deactivate");
	REGISTER_METHOD(get_promiscuous, "devEthernet.getPromiscuous");
	REGISTER_METHOD(set_promiscuous, "devEthernet.setPromiscuous");
	REGISTER_METHOD(set_no_arp, "devEthernet.setNoARP");
	REGISTER_METHOD(get_no_arp, "devEthernet.getNoARP");
	REGISTER_METHOD(get_counters, "devEthernet.getCounters");
	REGISTER_METHOD(get_mtu, "devEthernet.getMTU");
	REGISTER_METHOD(set_mtu, "devEthernet.setMTU");
	REGISTER_METHOD(get_iface_name, "devEthernet.getIfaceName");
	REGISTER_METHOD(set_iface_name, "devEthernet.setIfaceName");
	REGISTER_METHOD(get_snaplen, "devEthernet.getSnapLength");
	REGISTER_METHOD(set_snaplen, "devEthernet.setSnapLength");
	REGISTER_METHOD(is_protected, "devEthernet.isProtected");
	REGISTER_METHOD(list_all_ifaces, "devEthernet.listAllInterfaces");
}

devEthernet_Interface* devEthernet_InterfaceP;

} // namespace nasaCE
