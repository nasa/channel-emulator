/* -*- C++ -*- */

//=============================================================================
/**
 * @file   devTcp4Server_Interface.cpp
 * @author Tad Kollar  
 *
 * $Id: devTcp4Server_Interface.cpp 2080 2013-06-12 19:03:30Z tkollar $
 * Copyright (c) 2010.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "DeviceRegistry.hpp"
#include "devTcp4Server_Interface.hpp"
#include "SettingsManager.hpp"

namespace nasaCE {

devTcp4Server_Interface::devTcp4Server_Interface():
	Device_Interface_Templ<devTcp4Server>("devTcp4Server_Interface") {	}

devTcp4Server_Interface::devTcp4Server_Interface(xmlrpc_c::registryPtr& rpcRegistry):
	Device_Interface_Templ<devTcp4Server>("devTcp4Server_Interface") {
	ACE_TRACE("devTcp4Server_Interface::devTcp4Server_Interface");

	register_methods(rpcRegistry);
}

devTcp4Server_Interface::~devTcp4Server_Interface() { }

void devTcp4Server_Interface::server_create(xmlrpc_c::paramList const& paramList,
	xmlrpc_c::value* retvalP) {

	ACE_TRACE("devTcp4Server_Interface::create");

	std::string devname = static_cast<std::string>(paramList.getString(0));

	if (devices::instance()->exists(devname)) {
		throw Exception("Device with that name already exists.");
	}

	std::string address = IPv4Addr::IPv4_Any;
	int port = 0;

	devTcp4Server* tcp4_dev = new devTcp4Server(devname, this);
	devices::instance()->add(tcp4_dev);

	if (paramList.size() > 1) {
		address = static_cast<std::string>(paramList.getString(2));
		tcp4_dev->setAddress(address);
	}
	if (paramList.size() > 2) {
		port = static_cast<int>(paramList.getInt(3));
		tcp4_dev->setPort(port);
		if (port > 0 && ! tcp4_dev->isServing()) tcp4_dev->startListening();
	}

	*retvalP = xmlrpc_c::value_nil();
}

void devTcp4Server_Interface::server_is_serving(xmlrpc_c::paramList const& paramList,
	xmlrpc_c::value* retvalP) {
	ACE_TRACE("devTcp4Server_Interface::is_serving");

	std::string devname = static_cast<std::string>(paramList.getString(0));
	devTcp4Server* tcp4_dev = dynamic_cast<devTcp4Server*>(devices::instance()->device(devname));

	bool isServing = tcp4_dev->isServing();
	*retvalP = xmlrpc_c::value_boolean(isServing);
}

void devTcp4Server_Interface::server_is_connected(xmlrpc_c::paramList const& paramList,
	xmlrpc_c::value* retvalP) {
	ACE_TRACE("devTcp4Server_Interface::is_connected");

	std::string devname = static_cast<std::string>(paramList.getString(0));
	devTcp4Server* tcp4_dev = dynamic_cast<devTcp4Server*>(devices::instance()->device(devname));

	bool isConnected = tcp4_dev->isConnected();
	*retvalP = xmlrpc_c::value_boolean(isConnected);
}

void devTcp4Server_Interface::server_set_address(xmlrpc_c::paramList const& paramList,
	xmlrpc_c::value* retvalP) {
	ACE_TRACE("devTcp4Server_Interface::set_address");

	std::string devname = static_cast<std::string>(paramList.getString(0));
	std::string address = static_cast<std::string>(paramList.getString(1));
	devTcp4Server* tcp4_dev = dynamic_cast<devTcp4Server*>(devices::instance()->device(devname));

	tcp4_dev->setAddress(address);
	*retvalP = xmlrpc_c::value_nil();
}

void devTcp4Server_Interface::server_get_address(xmlrpc_c::paramList const& paramList,
	xmlrpc_c::value* retvalP) {
	ACE_TRACE("devTcp4Server_Interface::get_address");

	std::string devname = static_cast<std::string>(paramList.getString(0));
	devTcp4Server* tcp4_dev = dynamic_cast<devTcp4Server*>(devices::instance()->device(devname));

	std::string address = tcp4_dev->getAddress();
	*retvalP = xmlrpc_c::value_string(address);
}

void devTcp4Server_Interface::server_set_port(xmlrpc_c::paramList const& paramList,
	xmlrpc_c::value* retvalP) {
	ACE_TRACE("devTcp4Server_Interface::set_port");

	std::string devname = static_cast<std::string>(paramList.getString(0));
	int port = static_cast<int>(paramList.getInt(1));
	devTcp4Server* tcp4_dev = dynamic_cast<devTcp4Server*>(devices::instance()->device(devname));

	tcp4_dev->setPort(port);
	*retvalP = xmlrpc_c::value_nil();
}

void devTcp4Server_Interface::server_get_port(xmlrpc_c::paramList const& paramList,
	xmlrpc_c::value* retvalP) {
	ACE_TRACE("devTcp4Server_Interface::get_port");

	std::string devname = static_cast<std::string>(paramList.getString(0));
	devTcp4Server* tcp4_dev = dynamic_cast<devTcp4Server*>(devices::instance()->device(devname));

	int port = tcp4_dev->getPort();
	*retvalP = xmlrpc_c::value_int(port);
}

void devTcp4Server_Interface::server_start_listening(xmlrpc_c::paramList const& paramList,
	xmlrpc_c::value* retvalP) {
	ACE_TRACE("devTcp4Server_Interface::start_listening");

	std::string devname = static_cast<std::string>(paramList.getString(0));
	devTcp4Server* tcp4_dev = dynamic_cast<devTcp4Server*>(devices::instance()->device(devname));

	int success = tcp4_dev->startListening();
	*retvalP = xmlrpc_c::value_boolean(success);
}

void devTcp4Server_Interface::register_methods(xmlrpc_c::registryPtr& rpcRegistry) {
	ACE_TRACE("devTcp4Server_Interface::register_methods");

	DEV_TEMPLATE_REGISTER_METHODS(devTcp4Server, devTcp4Server_Interface);
	REGISTER_METHOD(server_create, "devTcp4Server.create");
	REGISTER_METHOD(server_is_serving, "devTcp4Server.isServing");
	REGISTER_METHOD(server_is_connected, "devTcp4Server.isConnected");
	REGISTER_METHOD(server_set_address, "devTcp4Server.setAddress");
	REGISTER_METHOD(server_get_address, "devTcp4Server.getAddress");
	REGISTER_METHOD(server_set_port, "devTcp4Server.setPort");
	REGISTER_METHOD(server_get_port, "devTcp4Server.getPort");
	REGISTER_METHOD(server_start_listening, "devTcp4Server.startListening");
}


devTcp4Server_Interface* devTcp4Server_InterfaceP;

} // namespace nasaCE
