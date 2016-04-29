/* -*- C++ -*- */

//=============================================================================
/**
 * @file   Device_Interface_Templ.hpp
 * @author Tad Kollar  
 *
 * $Id: Device_Interface_Templ.hpp 1980 2013-05-03 20:20:55Z tkollar $
 * Copyright (c) 2010.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _DEVICE_INTERFACE_TEMPLATE_HPP_
#define _DEVICE_INTERFACE_TEMPLATE_HPP_

#include "BaseTrafficHandler.hpp"
#include "CE_Config.hpp"
#include "CE_Device.hpp"
#include "CE_Macros.hpp"
#include "DeviceRegistry.hpp"
#include "InterfaceMacros.hpp"

namespace nasaCE {

//=============================================================================
/**
 * @class Device_Interface_Templ
 * @author Tad Kollar  
 * @brief Generates an XML-RPC interface for subclasses of CE_Device.
*/
//=============================================================================
template<class DeviceType>
class Device_Interface_Templ: public CE_DLL {

public:
    Device_Interface_Templ(const std::string& dllName):
    	nasaCE::CE_DLL(dllName) {
		ACE_TRACE("Device_Interface_Templ::Device_Interface_Templ");
	}

	virtual ~Device_Interface_Templ() {
		ACE_TRACE("Device_Interface_Templ::~Device_Interface_Templ");
	}

	virtual void addInstance(const std::string& segName, const std::string& channelName = "") { }

	virtual void add(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP) {
		ACE_TRACE("Device_Interface_Templ::add");

		const std::string newName(paramList.getString(0));

		CE_Device* dev = dynamic_cast<CE_Device*>(new DeviceType(newName, this));

		try {
			devices::instance()->add(dev);
		}
		catch (const Exception &e) {
			delete dev;
			throw nd_error("Unable to add new device " + newName + ": " + e.what());
		}

		ND_DEBUG("Added device %s.\n", dev->getName().c_str());

		*retvalP = xmlrpc_c::value_string(dev->getName());
	}

	virtual void remove(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP) {
		ACE_TRACE("Device_Interface_Templ::remove");

		const std::string devName(paramList.getString(0));

		devices::instance()->remove(devName);
		CEcfg::instance()->remove("Devices." + devName);

		ND_DEBUG("Removed device named %s.\n", devName.c_str());

		*retvalP = xmlrpc_c::value_nil();
	}

	virtual void get_reader(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP) {
		ACE_TRACE("Device_Interface_Templ::get_reader");
		DeviceType* device = find_device_(paramList);

		xstruct readerInfo;
		std::string channelName("");
		std::string segmentName("");

		BaseTrafficHandler* ptrReader = (BaseTrafficHandler*) device->getReader();

		if (ptrReader) {
			channelName = ptrReader->getChannelName();
			segmentName = ptrReader->getName();
		}

		readerInfo["channel"] = xmlrpc_c::value_string(channelName);
		readerInfo["name"] = xmlrpc_c::value_string(segmentName);

		*retvalP = xmlrpc_c::value_struct(readerInfo);
	}

	virtual void get_writer(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP) {
		ACE_TRACE("Device_Interface_Templ::get_writer");
		DeviceType* device = find_device_(paramList);

		xstruct writerInfo;
		std::string channelName("");
		std::string segmentName("");

		BaseTrafficHandler* ptrWriter = (BaseTrafficHandler*) device->getWriter();

		if (ptrWriter) {
			channelName = ptrWriter->getChannelName();
			segmentName = ptrWriter->getName();
		}

		writerInfo["channel"] = xmlrpc_c::value_string(channelName);
		writerInfo["name"] = xmlrpc_c::value_string(segmentName);

		*retvalP = xmlrpc_c::value_struct(writerInfo);
	}

	virtual void is_initialized(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP) {
		ACE_TRACE("Device_Interface_Templ::is_initialized");

		*retvalP = xmlrpc_c::value_boolean(find_device_(paramList)->isInitialized());
	}

	virtual void is_left_side(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP) {
		ACE_TRACE("Device_Interface_Templ::is_left_side");
		DeviceType* device = find_device_(paramList);

		Setting& sideSetting = CEcfg::instance()->get(device->cfgKey("leftSide"));

		*retvalP = xmlrpc_c::value_boolean(static_cast<bool>(sideSetting));
	}

	virtual void set_left_side(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP) {
		ACE_TRACE("Device_Interface_Templ::set_left_side");
		const bool leftSide = paramList.getBoolean(1);

		DeviceType* device = find_device_(paramList);

		Setting& sideSetting = CEcfg::instance()->getOrAddBool(device->cfgKey("leftSide"), leftSide);

		sideSetting = leftSide;

		*retvalP = xmlrpc_c::value_nil();
	}

	#ifdef DEFINE_DEBUG

	virtual void get_debug(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP) {
		ACE_TRACE("Device_Interface_Templ::get_debug");
		DeviceType* device = find_device_(paramList);

		*retvalP = xmlrpc_c::value_boolean(device->getDebugging());
	}

	virtual void set_debug(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP) {
		ACE_TRACE("Device_Interface_Templ::set_debug");
		bool debugVal(paramList.getBoolean(1));

		DeviceType* device = find_device_(paramList);

		device->setDebugging(debugVal);

		*retvalP = xmlrpc_c::value_nil();
	}

	#endif

protected:
	DeviceType* find_device_(xmlrpc_c::paramList const& paramList) {
		const std::string deviceName(paramList.getString(0));

		DeviceType* device = dynamic_cast<DeviceType*>(devices::instance()->device(deviceName));

		// Make sure module X didn't search for a name used by a module Y.
		if (typeid(*device) != typeid(DeviceType))
			throw nd_error("Device of wrong type found stored with supplied name.");

		// Make sure module X didn't search for a name used by a module Y.
		if (!device)
			throw nd_error(std::string("No device found with supplied name: ") + deviceName );

		return device;
	}
};

} // namespace nasaCE

//=============================================================================
/// @def DEV_TEMPLATE_METHOD_CLASSES
/// @brief Generate classes to access the Device_Interface_Templ template methods.
//=============================================================================
#define DEV_TEMPLATE_METHOD_CLASSES_BASE(derived_type, pointer_name) \
\
METHOD_CLASS2(derived_type, add , "s:s",\
	"Add an instance of the device module.",\
	pointer_name) \
\
METHOD_CLASS2(derived_type, remove , "n:s",\
	"Remove the specified instance of the device module.",\
	pointer_name) \
\
METHOD_CLASS2(derived_type, get_reader , "A:s",\
	"Get the channel and name of a device's reader segment if it exists.",\
	pointer_name) \
\
METHOD_CLASS2(derived_type, get_writer , "A:s",\
	"Get the channel and name of a device's writer segment if it exists.",\
	pointer_name) \
\
METHOD_CLASS2(derived_type, is_initialized , "b:s",\
	"Determine whether a device has been properly configured yet.",\
	pointer_name) \
\
METHOD_CLASS2(derived_type, is_left_side , "b:s",\
	"Return whether the device should be visualized on the left (true) or right (false).",\
	pointer_name) \
\
METHOD_CLASS2(derived_type, set_left_side , "n:sb",\
	"Set whether the device should be visualized on the left (true) or right (false).",\
	pointer_name)

#define DEV_ACTIVATE_DEACTIVATE(derived_type, pointer_name) \
extern "C" nasaCE::CE_DLL* activate(xmlrpc_c::registryPtr& rpcRegistry) {\
	ND_DEBUG("Activating %s\n", #derived_type);\
	pointer_name = new derived_type(rpcRegistry);\
	return pointer_name;\
}\
\
extern "C" void deactivate() {\
	ND_DEBUG("Deactivating %s\n", #derived_type);\
	delete pointer_name;\
	pointer_name = 0;\
}

#ifdef DEFINE_DEBUG
	#define DEV_TEMPLATE_METHOD_CLASSES(derived_type, pointer_name) \
		DEV_TEMPLATE_METHOD_CLASSES_BASE(derived_type, pointer_name) \
		METHOD_CLASS2(derived_type, get_debug , "b:s",\
			"Get whether the device is printing debug info.",\
			pointer_name) \
		\
		METHOD_CLASS2(derived_type, set_debug , "n:sb",\
			"Set whether the device is printing debug info.",\
			pointer_name) \
		\
		DEV_ACTIVATE_DEACTIVATE(derived_type, pointer_name)
#else
	#define DEV_TEMPLATE_METHOD_CLASSES(derived_type, pointer_name) \
		DEV_TEMPLATE_METHOD_CLASSES_BASE(derived_type, pointer_name) \
		DEV_ACTIVATE_DEACTIVATE(derived_type, pointer_name)
#endif

//=============================================================================
/// @def DEV_TEMPLATE_REGISTER_METHODS
/// @brief Generate code to put into register_methods() of a Device_Interface_Templ subclass.
//=============================================================================
#define DEV_TEMPLATE_REGISTER_METHODS_BASE(prefix, derived_type) \
	REGISTER_METHOD(derived_type##_add ,	#prefix ".add");\
	REGISTER_METHOD(derived_type##_remove ,	#prefix ".remove");\
	REGISTER_METHOD(derived_type##_get_reader ,	#prefix ".getReaderInfo");\
	REGISTER_METHOD(derived_type##_get_writer ,	#prefix ".getWriterInfo");\
	REGISTER_METHOD(derived_type##_is_initialized  ,	#prefix ".isInitialized");\
	REGISTER_METHOD(derived_type##_is_left_side  ,			#prefix ".isLeftSide");\
	REGISTER_METHOD(derived_type##_set_left_side  ,			#prefix ".setLeftSide");

#ifdef DEFINE_DEBUG
	#define DEV_TEMPLATE_REGISTER_METHODS(prefix, derived_type) \
		DEV_TEMPLATE_REGISTER_METHODS_BASE(prefix, derived_type) \
		REGISTER_METHOD(derived_type##_get_debug  ,			#prefix ".getDebugging"); \
		REGISTER_METHOD(derived_type##_set_debug  ,			#prefix ".setDebugging");
#else
	#define DEV_TEMPLATE_REGISTER_METHODS(prefix, derived_type) \
		DEV_TEMPLATE_REGISTER_METHODS_BASE(prefix, derived_type)
#endif

//=============================================================================
/// @def GENERATE_DEVICE_ACCESSOR_METHODS
/// @brief Create XML-RPC method classes for the named accessors.
/// @param fn_base_name The name to append to set and get for the newly generated methods.
/// @param xmlrpc_type An XML-RPC signature type, like "i", "s", "b".
/// @param short_desc Will be appended to "Sets " and "Returns " in the respective method descriptions.
/// @param ptr_name The name of the handler pointer.
//=============================================================================
#define GENERATE_DEVICE_ACCESSOR_METHODS(fn_base_name, xmlrpc_type, short_desc, ptr_name)\
METHOD_CLASS(set_##fn_base_name, "n:s" #xmlrpc_type,\
	"Sets " short_desc, ptr_name)\
METHOD_CLASS(get_##fn_base_name, #xmlrpc_type ":s", \
	"Returns " short_desc, ptr_name)

#endif