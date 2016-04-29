/* -*- C++ -*- */

//=============================================================================
/**
 * @file   CE_DLL_Interface.cpp
 * @author Tad Kollar  
 *
 * $Id: CE_DLL_Interface.cpp 1947 2013-04-10 18:35:23Z tkollar $
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "CE_DLL_Interface.hpp"
#include  <boost/property_tree/xml_parser.hpp>

using namespace std;

namespace nasaCE {
CE_DLL_Interface* CE_DLL_InterfaceP;

CE_DLL_Interface::CE_DLL_Interface(xmlrpc_c::registryPtr& rpcRegistry):
	CE_DLL("CE_DLL_Interface") {
	ACE_TRACE("CE_DLL_Interface::CE_DLL_Interface");
	register_methods(rpcRegistry);
}

CE_DLL_Interface::~CE_DLL_Interface() {
	ACE_TRACE("CE_DLL_Interface::~CE_DLL_Interface");
}

void CE_DLL_Interface::register_methods(xmlrpc_c::registryPtr& rpcRegistry) {
	ACE_TRACE("CE_DLL_Interface::register_methods");
	REGISTER_METHOD(is_loaded, "dll.isLoaded");
	REGISTER_METHOD(list_loaded, "dll.listLoaded");
	REGISTER_METHOD(load, "dll.load");
	REGISTER_METHOD(unload, "dll.unload");
	REGISTER_METHOD(is_available, "dll.isAvailable");
	REGISTER_METHOD(list_available_devs, "dll.listAvailableDevices");
	REGISTER_METHOD(list_available_macros, "dll.listAvailableMacros");
	REGISTER_METHOD(list_available_segs, "dll.listAvailableSegments");
	REGISTER_METHOD(desc_available, "dll.getDescription");
	REGISTER_METHOD(settings_available, "dll.getSettings");
	REGISTER_METHOD(propfile_available, "dll.getPropertiesXML");
	REGISTER_METHOD(refresh_available, "dll.refreshAvailable");
}

void CE_DLL_Interface::is_loaded(xmlrpc_c::paramList const& paramList,
    xmlrpc_c::value* const retvalP) {

	const string dllName = (string) paramList.getString(0);
    *retvalP = xmlrpc_c::value_boolean(dll_registry::instance()->isLoaded(dllName));
}

void CE_DLL_Interface::list_loaded(xmlrpc_c::paramList const& paramList,
    xmlrpc_c::value* const retvalP) {
	std::vector<std::string> dlls;
	dll_registry::instance()->listLoaded(dlls);
	std::vector<xmlrpc_c::value> arrayData;

	for (unsigned int i = 0; i < dlls.size(); ++i) {
		std::string mod_name = dlls[i];
		arrayData.push_back(xmlrpc_c::value_string(mod_name));
	}
	*retvalP = xmlrpc_c::value_array(arrayData);
}

void CE_DLL_Interface::load(xmlrpc_c::paramList const& paramList,
    xmlrpc_c::value* const retvalP) {
	const string dllName = paramList.getString(0);
	paramList.verifyEnd(1);

	bool is_loaded;

	try {
		is_loaded = dll_registry::instance()->loadIfNotLoaded(dllName,
			xmlrpc_server::instance()->rpcRegistry);
	}
	catch (RegistryError& rerr) {
		throw rerr;
	}

	*retvalP = xmlrpc_c::value_boolean(is_loaded);
}

void CE_DLL_Interface::unload(xmlrpc_c::paramList const& paramList,
    xmlrpc_c::value* const retvalP) {
	const string dllName(paramList.getString(0));
	paramList.verifyEnd(1);

	bool is_loaded = dll_registry::instance()->isLoaded(dllName);

	dll_registry::instance()->unloadIfLoaded(dllName, xmlrpc_server::instance()->rpcRegistry);

	*retvalP = xmlrpc_c::value_boolean(is_loaded);
}

void CE_DLL_Interface::is_available(xmlrpc_c::paramList const& paramList,
    xmlrpc_c::value* const retvalP) {

	const std::string dllName = paramList.getString(0);
	*retvalP = xmlrpc_c::value_boolean(dll_registry::instance()->isAvailable(dllName));
}

void CE_DLL_Interface::list_available_devs(xmlrpc_c::paramList const& paramList,
    xmlrpc_c::value* const retvalP) {
	std::vector<std::string> devDlls;
	dll_registry::instance()->listAvailableDeviceModules(devDlls);
	vector<xmlrpc_c::value> arrayData;

	for (unsigned int i = 0; i < devDlls.size(); ++i) {
		const std::string modName = devDlls[i];
		arrayData.push_back(xmlrpc_c::value_string(modName));
	}
	*retvalP = xmlrpc_c::value_array(arrayData);
}

void CE_DLL_Interface::list_available_macros(xmlrpc_c::paramList const& paramList,
    xmlrpc_c::value* const retvalP) {
	std::vector<std::string> devDlls;
	dll_registry::instance()->listAvailableMacroModules(devDlls);
	vector<xmlrpc_c::value> arrayData;

	for (unsigned int i = 0; i < devDlls.size(); ++i) {
		const std::string modName = devDlls[i];
		arrayData.push_back(xmlrpc_c::value_string(modName));
	}
	*retvalP = xmlrpc_c::value_array(arrayData);
}

void CE_DLL_Interface::list_available_segs(xmlrpc_c::paramList const& paramList,
    xmlrpc_c::value* const retvalP) {
	std::vector<std::string> segDlls;
	dll_registry::instance()->listAvailableSegmentModules(segDlls);
	vector<xmlrpc_c::value> arrayData;

	for (unsigned int i = 0; i < segDlls.size(); ++i) {
		const std::string modName = segDlls[i];
		arrayData.push_back(xmlrpc_c::value_string(modName));
	}
	*retvalP = xmlrpc_c::value_array(arrayData);
}

void CE_DLL_Interface::desc_available(xmlrpc_c::paramList const& paramList,
    xmlrpc_c::value* const retvalP) {
	const std::string dllName = paramList.getString(0);
	std::string desc(dll_registry::instance()->getDescription(dllName));

	*retvalP = xmlrpc_c::value_string(desc);
}

void CE_DLL_Interface::settings_available(xmlrpc_c::paramList const& paramList,
    xmlrpc_c::value* const retvalP) {
	const std::string dllName = paramList.getString(0);

	const CEModuleDesc& desc = dll_registry::instance()->getDescObj(dllName);
	const ModuleSettingsMap& settings = desc.getSettingsMap();

	std::vector<xmlrpc_c::value> settingsArray;
	ModuleSettingsMap::const_iterator pos;

	for (pos = settings.begin(); pos != settings.end(); ++pos) {
		std::map<std::string, xmlrpc_c::value> s;
		s["prompt"] = xmlrpc_c::value_string(pos->second.getPrompt());
		s["path"] = xmlrpc_c::value_string(pos->second.getPath());
		s["type"] = xmlrpc_c::value_string(pos->second.getType());
		s["desc"] = xmlrpc_c::value_string(pos->second.getDesc());
		s["default"] = xmlrpc_c::value_string(pos->second.getDefault());
		s["req"] = xmlrpc_c::value_boolean(pos->second.getRequiredFlag());
		settingsArray.push_back(xmlrpc_c::value_struct(s));
	}
	*retvalP = xmlrpc_c::value_array(settingsArray);
}

void CE_DLL_Interface::refresh_available(xmlrpc_c::paramList const& paramList,
    xmlrpc_c::value* const retvalP) {

	dll_registry::instance()->refreshAvailableDLLList();
	*retvalP = xmlrpc_c::value_nil();
}

void CE_DLL_Interface::propfile_available(xmlrpc_c::paramList const& paramList,
    xmlrpc_c::value* const retvalP) {

	const std::string dllName = paramList.getString(0);

	const CEModuleDesc& desc = dll_registry::instance()->getDescObj(dllName);

	std::ostringstream os;

	boost::property_tree::write_xml(os, desc.const_getProperties());

	*retvalP = xmlrpc_c::value_string(os.str());
}

}
