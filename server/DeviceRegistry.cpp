/* -*- C++ -*- */

//=============================================================================
/**
 * @file   DeviceRegistry.cpp
 * @author Tad Kollar  
 *
 * $Id: DeviceRegistry.cpp 2101 2013-06-13 19:11:25Z tkollar $
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "CE_Macros.hpp"
#include "DeviceRegistry.hpp"
#include "SettingsManager.hpp"
#include <ace/Reactor.h>
#include <xmlrpc-c/base.hpp>
#include <xmlrpc-c/xml.hpp>
#include <xmlrpc-c/girerr.hpp>

namespace nasaCE {

DeviceRegistry::DeviceRegistry() {
}

DeviceRegistry::~DeviceRegistry() {
	removeAll();
}

void DeviceRegistry::autoLoad() {
	ND_INFO("[DeviceRegistry] Loading devices specified in the configuration file.\n");

	bool wasCreated;
	Setting& devSettings = CEcfg::instance()->getOrAdd("Devices", Setting::TypeGroup, wasCreated);

	for ( int devIdx = 0 ; devIdx < devSettings.getLength() ; ++devIdx ) {
		// If there's a section with a devType setting, try to load it
		// as a device - unless there's also an autoLoad setting and it's false.
		if  ( devSettings[devIdx].isGroup() &&
		      devSettings[devIdx].exists("devType") &&
			( ! devSettings[devIdx].exists("autoLoad") ||
			( devSettings[devIdx].exists("autoLoad") &&
				static_cast<bool>(devSettings[devIdx]["autoLoad"]) == true ) ) ) {
			std::string devName = devSettings[devIdx].getName();

			ND_DEBUG("   ... loading device %s.\n", devName.c_str());

			if ( ! devices::instance()->exists(devName) ) { // make sure it's not already loaded
				Setting& devSection = devSettings[devIdx];
				std::string devType = devSection["devType"];

				dll_registry::instance()->loadIfNotLoaded("dev" + devType,
					xmlrpc_server::instance()->rpcRegistry);

				xmlrpc_c::paramList addParams;
				xmlrpc_c::value addResult;

				addParams.add(xmlrpc_c::value_string(devName));
				try {
					xmlrpc_server::instance()->doCall("dev" + devType + std::string(".add"), addParams, &addResult);
				}
				catch (girerr::error& e) {
					ND_ERROR("XML-RPC returns error: %s\n", e.what());
				}
			}
		}
	}
}

void DeviceRegistry::add(CE_Device* newDevice) {
	ND_DEBUG("[DeviceRegistry] Registering new Device named %s.\n", newDevice->getName().c_str());
	if (_devices.find(newDevice->getName()) != _devices.end())
		throw DeviceExists("Device named " + newDevice->getName() + " already exists.");

	_devices[newDevice->getName()] = newDevice;
	/*
	Setting& autoLoad = CEcfg::instance()->getOrAddArray("Devices.autoLoad");

	bool foundDevice = false;
	for ( int idx = 0; idx < autoLoad.getLength(); ++idx ) {
		std::string testName = autoLoad[idx];
		if ( testName == newDevice->getName() ) {
			foundDevice = true;
			break;
		}
	}
	if ( ! foundDevice ) autoLoad.add(Setting::TypeString) = newDevice->getName();
	*/
}

void DeviceRegistry::remove(const std::string& deviceName) {
	if ( ! exists(deviceName) ) throw UnmanagedDevice(deviceName +
		std::string(" does not exist in the managed device registry."));

	CE_Device* device = _devices[deviceName];
	_devices.erase(deviceName); // Remove the key from the map.
	delete device;
	/*
	Setting& autoLoad = CEcfg::instance()->getOrAddArray("Devices.autoLoad");
	for ( int idx = 0; idx < autoLoad.getLength(); ++idx ) {
		std::string testName = autoLoad[idx];
		if ( testName == deviceName ) {
			autoLoad.remove(idx);
			break;
		}
	}
	*/
}

void DeviceRegistry::removeAll() {
	ND_DEBUG("[DeviceRegistry] Unregistering and deleting all devices.\n");

	DeviceMap::iterator pos;
	CE_Device* device;

	while (_devices.size()) {
		pos = _devices.begin();
		device = pos->second;
		ND_DEBUG("[DeviceRegistry] Deleting device %s.\n", pos->first.c_str());
		pos->second = 0;
		_devices.erase(pos);
		delete device;
	}
}

CE_Device *DeviceRegistry::device(const std::string& deviceName) {
	if ( ! exists(deviceName) ) throw UnmanagedDevice(deviceName +
		std::string(" does not exist in the managed device registry."));

	return _devices[deviceName];
}

bool DeviceRegistry::exists(const std::string& deviceName) {
	if ( _devices.find(deviceName) == _devices.end() ) return false;
	return true;
}

void DeviceRegistry::list_names(std::vector<std::string>& device_names) {
	device_names.clear();

	DeviceMap::iterator pos;
	for ( pos = _devices.begin(); pos != _devices.end(); ++pos )
		device_names.push_back(pos->first);
}

} // namespace nasaCE
