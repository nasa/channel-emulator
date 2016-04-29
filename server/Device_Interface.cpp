/* -*- C++ -*- */

//=============================================================================
/**
 * @file   Device_Interface.cpp
 * @author Tad Kollar  
 *
 * $Id: Device_Interface.cpp 2318 2013-08-05 18:32:49Z tkollar $
 * Copyright (c) 2010.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "Device_Interface.hpp"
#include "nd_error.hpp"
#include <vector>

using namespace std;

namespace nasaCE {

Device_Interface* Device_InterfaceP;

Device_Interface::Device_Interface(xmlrpc_c::registryPtr& rpcRegistry):
	CE_DLL("Device_Interface") {
	ACE_TRACE("Device_Interface::Device_Interface");
	register_methods(rpcRegistry);

}

void Device_Interface::register_methods(xmlrpc_c::registryPtr& rpcRegistry) {
	ACE_TRACE("Device_Interface::register_methods");
	REGISTER_METHOD(list_devices, "device.getList");
	REGISTER_METHOD(get_device_type, "device.getType");
	REGISTER_METHOD(exists, "device.exists");
}

void Device_Interface::list_devices(xmlrpc_c::paramList const& paramList,
	xmlrpc_c::value* retvalP) {
	ACE_TRACE("Device_Interface::list_devices");

	vector<xmlrpc_c::value> xml_devices;
	vector<string> string_devices;

	devices::instance()->list_names(string_devices);

	vector<string>::iterator pos;
	for (pos = string_devices.begin(); pos != string_devices.end(); ++pos) {
		xml_devices.push_back(xmlrpc_c::value_string(*pos));
	}

	*retvalP = xmlrpc_c::value_array(xml_devices);
}

void Device_Interface::get_device_type(xmlrpc_c::paramList const& paramList,
	xmlrpc_c::value* retvalP) {
	ACE_TRACE("Device_Interface::get_device_type");
	string deviceName = paramList.getString(0);

	if ( ! devices::instance()->exists(deviceName) )
		throw nd_error("Specified device '" + deviceName + "' does not exist.");

	*retvalP = xmlrpc_c::value_string(devices::instance()->device(deviceName)->getType());
}

void Device_Interface::exists(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP) {
	ACE_TRACE("Device_Interface::exists");
	string deviceName = paramList.getString(0);

	*retvalP = xmlrpc_c::value_boolean(devices::instance()->exists(deviceName));
}


}
