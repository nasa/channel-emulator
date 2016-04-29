/* -*- C++ -*- */

//=============================================================================
/**
 * @file   devTcp4Client_Interface.cpp
 * @author Tad Kollar  
 *
 * $Id: devTcp4Client_Interface.cpp 2111 2013-06-14 19:27:27Z tkollar $
 * Copyright (c) 2010.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "DeviceRegistry.hpp"
#include "devTcp4Client_Interface.hpp"
#include "SettingsManager.hpp"

namespace nasaCE {

devTcp4Client_Interface::devTcp4Client_Interface():
	Device_Interface_Templ<devTcp4Client>("devTcp4Client_Interface") {	}

devTcp4Client_Interface::devTcp4Client_Interface(xmlrpc_c::registryPtr& rpcRegistry):
	Device_Interface_Templ<devTcp4Client>("devTcp4Client_Interface") {
	ACE_TRACE("devTcp4Client_Interface::devTcp4Client_Interface");

	register_methods(rpcRegistry);
}

devTcp4Client_Interface::~devTcp4Client_Interface() { }

void devTcp4Client_Interface::client_create(xmlrpc_c::paramList const& paramList,
	xmlrpc_c::value* retvalP) {

	ACE_TRACE("devTcp4Client_Interface::create");

	std::string devname = static_cast<std::string>(paramList.getString(0));

	if (devices::instance()->exists(devname)) {
		throw Exception("Device with that name already exists.");
	}

	std::string address = IPv4Addr::IPv4_Any;
	int port = 0;

	devTcp4Client* tcp4_dev = new devTcp4Client(devname, this);
	devices::instance()->add(tcp4_dev);

	if (paramList.size() > 1) {
		address = static_cast<std::string>(paramList.getString(2));
		tcp4_dev->setAddress(address);
	}
	if (paramList.size() > 2) {
		port = static_cast<int>(paramList.getInt(3));
		tcp4_dev->setPort(port);
		if (port > 0) tcp4_dev->openConnection();
	}

	*retvalP = xmlrpc_c::value_nil();
}
/*
void devTcp4Client_Interface::client_remove(xmlrpc_c::paramList const& paramList,
	xmlrpc_c::value* retvalP) {

	ACE_TRACE("devTcp4Client_Interface::remove");

	std::string devname = static_cast<std::string>(paramList.getString(0));
	devices::instance()->remove(devname);

	CEcfg::instance()->get("Devices").remove(devname);

	*retvalP = xmlrpc_c::value_nil();
}
*/
void devTcp4Client_Interface::client_is_connected(xmlrpc_c::paramList const& paramList,
	xmlrpc_c::value* retvalP) {
	ACE_TRACE("devTcp4Client_Interface::is_connected");

	std::string devname = static_cast<std::string>(paramList.getString(0));
	devTcp4Client* tcp4_dev = dynamic_cast<devTcp4Client*>(devices::instance()->device(devname));

	bool isConnected = tcp4_dev->isConnected();
	*retvalP = xmlrpc_c::value_boolean(isConnected);
}

void devTcp4Client_Interface::client_set_address(xmlrpc_c::paramList const& paramList,
	xmlrpc_c::value* retvalP) {
	ACE_TRACE("devTcp4Client_Interface::set_address");

	std::string devname = static_cast<std::string>(paramList.getString(0));
	std::string address = static_cast<std::string>(paramList.getString(1));
	devTcp4Client* tcp4_dev = dynamic_cast<devTcp4Client*>(devices::instance()->device(devname));

	tcp4_dev->setAddress(address);
	*retvalP = xmlrpc_c::value_nil();
}

void devTcp4Client_Interface::client_get_address(xmlrpc_c::paramList const& paramList,
	xmlrpc_c::value* retvalP) {
	ACE_TRACE("devTcp4Client_Interface::get_address");

	std::string devname = static_cast<std::string>(paramList.getString(0));
	devTcp4Client* tcp4_dev = dynamic_cast<devTcp4Client*>(devices::instance()->device(devname));

	std::string address = tcp4_dev->getAddress();
	*retvalP = xmlrpc_c::value_string(address);
}

void devTcp4Client_Interface::client_set_port(xmlrpc_c::paramList const& paramList,
	xmlrpc_c::value* retvalP) {
	ACE_TRACE("devTcp4Client_Interface::set_port");

	std::string devname = static_cast<std::string>(paramList.getString(0));
	int port = static_cast<int>(paramList.getInt(1));
	devTcp4Client* tcp4_dev = dynamic_cast<devTcp4Client*>(devices::instance()->device(devname));

	tcp4_dev->setPort(port);
	*retvalP = xmlrpc_c::value_nil();
}

void devTcp4Client_Interface::client_get_port(xmlrpc_c::paramList const& paramList,
	xmlrpc_c::value* retvalP) {
	ACE_TRACE("devTcp4Client_Interface::get_port");

	std::string devname = static_cast<std::string>(paramList.getString(0));
	devTcp4Client* tcp4_dev = dynamic_cast<devTcp4Client*>(devices::instance()->device(devname));

	int port = tcp4_dev->getPort();
	*retvalP = xmlrpc_c::value_int(port);
}

void devTcp4Client_Interface::client_open_connection(xmlrpc_c::paramList const& paramList,
	xmlrpc_c::value* retvalP) {
	ACE_TRACE("devTcp4Client_Interface::openConnection");

	std::string devname = static_cast<std::string>(paramList.getString(0));
	devTcp4Client* tcp4_dev = dynamic_cast<devTcp4Client*>(devices::instance()->device(devname));

	tcp4_dev->openConnection();
	*retvalP = xmlrpc_c::value_nil();
}

void devTcp4Client_Interface::client_set_ms_between_attempts(xmlrpc_c::paramList const& paramList,
	xmlrpc_c::value* retvalP) {
	ACE_TRACE("devTcp4Client_Interface::client_set_ms_between_attempts");

	std::string devname = static_cast<std::string>(paramList.getString(0));
	int ms = static_cast<int>(paramList.getInt(1));
	devTcp4Client* tcp4_dev = dynamic_cast<devTcp4Client*>(devices::instance()->device(devname));

	tcp4_dev->setMsBetweenAttempts(ms);
	*retvalP = xmlrpc_c::value_nil();
}

void devTcp4Client_Interface::client_get_ms_between_attempts(xmlrpc_c::paramList const& paramList,
	xmlrpc_c::value* retvalP) {
	ACE_TRACE("devTcp4Client_Interface::client_get_ms_between_attempts");

	std::string devname = static_cast<std::string>(paramList.getString(0));
	devTcp4Client* tcp4_dev = dynamic_cast<devTcp4Client*>(devices::instance()->device(devname));

	int ms = tcp4_dev->getMsBetweenAttempts();
	*retvalP = xmlrpc_c::value_int(ms);
}

void devTcp4Client_Interface::register_methods(xmlrpc_c::registryPtr& rpcRegistry) {
	ACE_TRACE("devTcp4Client_Interface::register_methods");

	DEV_TEMPLATE_REGISTER_METHODS(devTcp4Client, devTcp4Client_Interface);
	REGISTER_METHOD(client_create, "devTcp4Client.create");
	// REGISTER_METHOD(client_remove, "devTcp4Client.remove");
	REGISTER_METHOD(client_is_connected, "devTcp4Client.isConnected");
	REGISTER_METHOD(client_set_address, "devTcp4Client.setAddress");
	REGISTER_METHOD(client_get_address, "devTcp4Client.getAddress");
	REGISTER_METHOD(client_set_port, "devTcp4Client.setPort");
	REGISTER_METHOD(client_get_port, "devTcp4Client.getPort");
	REGISTER_METHOD(client_set_ms_between_attempts, "devTcp4Client.setMsBetweenAttempts");
	REGISTER_METHOD(client_get_ms_between_attempts, "devTcp4Client.getMsBetweenAttempts");
	REGISTER_METHOD(client_open_connection, "devTcp4Client.openConnection");
}

devTcp4Client_Interface* devTcp4Client_InterfaceP;

} // namespace nasaCE