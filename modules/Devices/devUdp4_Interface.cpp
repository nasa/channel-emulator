/* -*- C++ -*- */

//=============================================================================
/**
 * @file   devUdp4_Interface.cpp
 * @author Tad Kollar  
 *
 * $Id: devUdp4_Interface.cpp 2080 2013-06-12 19:03:30Z tkollar $
 * Copyright (c) 2011.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "DeviceRegistry.hpp"
#include "devUdp4_Interface.hpp"
#include "SettingsManager.hpp"

namespace nasaCE {

devUdp4_Interface::devUdp4_Interface():
	Device_Interface_Templ<devUdp4>("devUdp4_Interface") {	}

devUdp4_Interface::devUdp4_Interface(xmlrpc_c::registryPtr& rpcRegistry):
	Device_Interface_Templ<devUdp4>("devUdp4_Interface") {
	ACE_TRACE("devUdp4_Interface::devUdp4_Interface");

	register_methods(rpcRegistry);
}

devUdp4_Interface::~devUdp4_Interface() { }

void devUdp4_Interface::dev_udp_create(xmlrpc_c::paramList const& paramList,
	xmlrpc_c::value* retvalP) {

	ACE_TRACE("devUdp4_Interface::create");

	std::string devname = static_cast<std::string>(paramList.getString(0));

	if (devices::instance()->exists(devname)) {
		throw Exception("Device with that name already exists.");
	}

	std::string address = IPv4Addr::IPv4_Any;
	int port = 0;

	devUdp4* udp4_dev = new devUdp4(devname, this);
	devices::instance()->add(udp4_dev);

	if (paramList.size() > 1) {
		address = static_cast<std::string>(paramList.getString(2));
		udp4_dev->setAddress(address);
	}
	if (paramList.size() > 2) {
		port = static_cast<int>(paramList.getInt(3));
		udp4_dev->setPort(port);
	}

	*retvalP = xmlrpc_c::value_nil();
}
/*
void devUdp4_Interface::dev_udp_remove(xmlrpc_c::paramList const& paramList,
	xmlrpc_c::value* retvalP) {

	ACE_TRACE("devUdp4_Interface::remove");

	std::string devname = static_cast<std::string>(paramList.getString(0));
	devices::instance()->remove(devname);

	CEcfg::instance()->get("Devices").remove(devname);

	*retvalP = xmlrpc_c::value_nil();
}
*/
void devUdp4_Interface::dev_udp_set_address(xmlrpc_c::paramList const& paramList,
	xmlrpc_c::value* retvalP) {
	ACE_TRACE("devUdp4_Interface::set_address");

	std::string devname = static_cast<std::string>(paramList.getString(0));
	std::string address = static_cast<std::string>(paramList.getString(1));
	devUdp4* udp4_dev = dynamic_cast<devUdp4*>(devices::instance()->device(devname));

	udp4_dev->setAddress(address);
	*retvalP = xmlrpc_c::value_nil();
}

void devUdp4_Interface::dev_udp_get_address(xmlrpc_c::paramList const& paramList,
	xmlrpc_c::value* retvalP) {
	ACE_TRACE("devUdp4_Interface::get_address");

	std::string devname = static_cast<std::string>(paramList.getString(0));
	devUdp4* udp4_dev = dynamic_cast<devUdp4*>(devices::instance()->device(devname));

	std::string address = udp4_dev->getAddress();
	*retvalP = xmlrpc_c::value_string(address);
}

void devUdp4_Interface::dev_udp_set_port(xmlrpc_c::paramList const& paramList,
	xmlrpc_c::value* retvalP) {
	ACE_TRACE("devUdp4_Interface::set_port");

	std::string devname = static_cast<std::string>(paramList.getString(0));
	int port = static_cast<int>(paramList.getInt(1));
	devUdp4* udp4_dev = dynamic_cast<devUdp4*>(devices::instance()->device(devname));

	udp4_dev->setPort(port);
	*retvalP = xmlrpc_c::value_nil();
}

void devUdp4_Interface::dev_udp_get_port(xmlrpc_c::paramList const& paramList,
	xmlrpc_c::value* retvalP) {
	ACE_TRACE("devUdp4_Interface::get_port");

	std::string devname = static_cast<std::string>(paramList.getString(0));
	devUdp4* udp4_dev = dynamic_cast<devUdp4*>(devices::instance()->device(devname));

	int port = udp4_dev->getPort();
	*retvalP = xmlrpc_c::value_int(port);
}

void devUdp4_Interface::dev_udp_bind(xmlrpc_c::paramList const& paramList,
	xmlrpc_c::value* retvalP) {
	ACE_TRACE("devUdp4_Interface::get_port");

	std::string devname = static_cast<std::string>(paramList.getString(0));
	devUdp4* udp4_dev = dynamic_cast<devUdp4*>(devices::instance()->device(devname));

	udp4_dev->bindSocket();
	*retvalP = xmlrpc_c::value_nil();
}

void devUdp4_Interface::register_methods(xmlrpc_c::registryPtr& rpcRegistry) {
	ACE_TRACE("devUdp4_Interface::register_methods");

	DEV_TEMPLATE_REGISTER_METHODS(devUdp4, devUdp4_Interface);
	REGISTER_METHOD(dev_udp_create, "devUdp4.create");
//	REGISTER_METHOD(dev_udp_remove, "devUdp4.remove");
	REGISTER_METHOD(dev_udp_set_address, "devUdp4.setAddress");
	REGISTER_METHOD(dev_udp_get_address, "devUdp4.getAddress");
	REGISTER_METHOD(dev_udp_set_port, "devUdp4.setPort");
	REGISTER_METHOD(dev_udp_get_port, "devUdp4.getPort");
	REGISTER_METHOD(dev_udp_bind, "devUdp4.bind");

}


devUdp4_Interface* devUdp4_InterfaceP;

} // namespace nasaCE
