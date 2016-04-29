/* -*- C++ -*- */

//=============================================================================
/**
 * @file   DeviceRegistry.hpp
 * @author Tad Kollar  
 *
 * $Id: DeviceRegistry.hpp 2060 2013-06-11 16:16:02Z tkollar $
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _NASA_DEVICE_REGISTRY_HPP_
#define _NASA_DEVICE_REGISTRY_HPP_

#include "CE_Device.hpp"
#include <ace/Singleton.h>
#include <vector>

namespace nasaCE {

typedef std::map<std::string, CE_Device*> DeviceMap;

//=============================================================================
/**
 *	@author Tad Kollar  
 *	@class DeviceRegistry
 *	@brief Manage a map containing keys to all devices.
*/
//=============================================================================
class DeviceRegistry {

public:
	DeviceRegistry();

    ~DeviceRegistry();

	/// @class UnmanagedDevice
	/// @brief Thrown when a request is made for a device not in the map.
	struct UnmanagedDevice: public Exception {
		UnmanagedDevice(const std::string e): Exception(e) { }
	};

	/// @class DeviceExists
	/// @brief Thrown when an attempt is made to create a device that already exists.
	struct DeviceExists: public Exception {
		DeviceExists(const std::string e): Exception(e) { }
	};

	/// Add all devices specified in the Devices.autoLoad config file array.
	void autoLoad();

	/// Creates a new device name to the registry if it doesn't already exist.
	void add(CE_Device* newDevice);

	/// Removes the named device from the registry.
	/// @throw UnmanagedDevice If the device name is not found in the map.
	void remove(const std::string& deviceName);

	/// Remove all devices from the registry.
	void removeAll();

	/// An accessor for the device map.
	/// @throw UnmanagedDevice If the device name is not found in the map.
	CE_Device *operator [](const std::string& deviceName) { return device(deviceName); }

	/// An accessor for the device map.
	/// @throw UnmanagedDevice If the device name is not found in the map.
	CE_Device *device(const std::string& deviceName);

	/// Returns true if the device name is found in the map as a key.
	bool exists(const std::string& deviceName);

	/// Clears then loads a vector with device names, found as keys in the map.
	void list_names(std::vector<std::string>& device_names);

private:
	DeviceMap _devices;

}; // class DeviceRegistry

typedef ACE_Singleton<DeviceRegistry, ACE_Recursive_Thread_Mutex> devices;

} // namespace nasaCE

#endif // _NASA_DEVICE_REGISTRY_HPP_
