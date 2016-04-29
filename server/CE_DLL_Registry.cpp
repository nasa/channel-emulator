/* -*- C++ -*- */

//=============================================================================
/**
 * @file   CE_DLL_Registry.cpp
 * @author Tad Kollar  
 *
 * $Id: CE_DLL_Registry.cpp 2060 2013-06-11 16:16:02Z tkollar $
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "CE_Config.hpp"
#include "CE_DLL_Registry.hpp"
#include "SettingsManager.hpp"
#include "XML_RPC_Server.hpp"
#include "nd_error.hpp"
#include "string_nocase.hpp"
#include <iostream>
#include <fstream>
#include <ace/OS_main.h>
#include <ace/Dirent.h>
#include <boost/regex.hpp>
#include <set>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>

namespace nasaCE {

CESettingDesc::CESettingDesc(const std::string newPrompt /* = "" */, const std::string newPath /* = "" */,
	const std::string newType /* = "" */, const std::string newDesc /* = "" */, const bool isReq /* = false */):
	_prompt(newPrompt), _path(newPath), _type(newType), _desc(newDesc), _isRequired(isReq) { }

std::string CESettingDesc::getPrompt() const { return _prompt; }

void CESettingDesc::setPrompt(const std::string& newPrompt) { _prompt = newPrompt; }

std::string CESettingDesc::getPath(const std::string& instanceName /* = "" */) const {
	// TODO: instanceName subst
	return _path;
}

void CESettingDesc::setPath(const std::string& newPath) { _path = newPath; }

std::string CESettingDesc::getType() const { return _type; }

void CESettingDesc::setType(const std::string& newType) { _type = newType; }

std::string CESettingDesc::getDesc() const { return _desc; }

void CESettingDesc::setDesc(const std::string& newDesc) { _desc = newDesc; }

std::string CESettingDesc::getDefault() const { return _default; }

void CESettingDesc::setDefault(const std::string& newDefault) { _default = newDefault; }

bool CESettingDesc::getRequiredFlag() const { return _isRequired; }

void CESettingDesc::setRequiredFlag(const bool isReq) { _isRequired = isReq; }

bool CESettingDesc::operator == (const CESettingDesc& other) const { return (_path == other._path); }


CESettingDesc NO_SETTING("NO_SETTING", "NO_SETTING", "NONE", "NO_SETTING");

CEModuleDesc::CEModuleDesc(const std::string newName /* = "" */, const std::string newCat /* = "" */,
				const std::string newPath /* = "" */, const std::string newDesc /* = ""*/ ):
	_name(newName), _category(newCat), _path(newPath), _desc(newDesc) { }

std::string CEModuleDesc::getName() const { return _name; }

void CEModuleDesc::setName(const std::string& newName) { _name = newName; }

std::string CEModuleDesc::getCategory() const { return _category; }

void CEModuleDesc::setCategory(const std::string& newCat) { _category = newCat; }

bool CEModuleDesc::isDevice() const { return string_nocase(_category.c_str()) == "device"; }

bool CEModuleDesc::isMacro() const { return string_nocase(_category.c_str()) == "macro"; }

std::string CEModuleDesc::getPath() const { return _path; }

void CEModuleDesc::setPath(const std::string& newPath) { _path = newPath; }

std::string CEModuleDesc::getDesc() const { return _desc; }

void CEModuleDesc::setDesc(const std::string& newDesc) { _desc = newDesc; }

void CEModuleDesc::addDependency(const std::string& newDep) { _dependencies.insert(newDep); }

void CEModuleDesc::clearDependencies() { _dependencies.clear(); }

void CEModuleDesc::getDependencies(std::set<std::string>& deps) { deps = _dependencies; }

void CEModuleDesc::addReverseDependency(const std::string& newDep) { _reverseDependencies.insert(newDep); }

void CEModuleDesc::clearReverseDependencies() { _reverseDependencies.clear(); }

void CEModuleDesc::getReverseDependencies(std::set<std::string>& deps) { deps = _reverseDependencies; }

void CEModuleDesc::addSetting(const CESettingDesc& newSetting) { _settings[newSetting.getPath()] = newSetting; }

void CEModuleDesc::delSettingByPath(const std::string& path) { _settings.erase(path); }

boost::property_tree::ptree& CEModuleDesc::getProperties() { return _rawProperties; }

const boost::property_tree::ptree& CEModuleDesc::const_getProperties() const { return _rawProperties; }

const CESettingDesc& CEModuleDesc::getSettingByPath(const std::string& path) {
	if ( _settings.find(path) != _settings.end() ) { return _settings[path]; }
	return NO_SETTING;
}

const ModuleSettingsMap& CEModuleDesc::getSettingsMap() const { return _settings; }

const std::string CE_DLL_Registry::propertyFileExt = ".dsc";

CE_DLL_Registry::CE_DLL_Registry() {
	refreshAvailableDLLList();
}

CE_DLL_Registry::~CE_DLL_Registry() {
	ACE_TRACE("~CE_DLL_Registry");
	unloadAll();
}

void CE_DLL_Registry::_loadPropertyFile(CEModuleDesc& modDesc, const std::string& path, const std::string& fname) {
	const std::string filename = path + "/" + fname;
	using boost::property_tree::ptree;
    ptree& pt = modDesc.getProperties();

	ND_DEBUG("Loading property file for %s from %s.\n", modDesc.getName().c_str(), filename.c_str());

	try {
		boost::property_tree::read_xml(filename, pt, boost::property_tree::xml_parser::no_comments |
			boost::property_tree::xml_parser::trim_whitespace);
	}
	catch (...) {
		ND_WARNING("Unable to read module description file %s.\n", filename.c_str());
		return;
	}

	if (pt.get_child_optional("modinfo.name")) modDesc.setName(pt.get<std::string>("modinfo.name"));
	if (pt.get_child_optional("modinfo.desc")) modDesc.setDesc(pt.get<std::string>("modinfo.desc"));
	if (pt.get_child_optional("modinfo.cat")) modDesc.setCategory(pt.get<std::string>("modinfo.cat"));

	if ( pt.get_child_optional("modinfo.depends") ) {
		BOOST_FOREACH(ptree::value_type &v, pt.get_child("modinfo.depends")) {
			modDesc.addDependency(v.second.data());
		}
	}

	if ( pt.get_child_optional("modinfo.config") ) {
		ptree& configTree = pt.get_child("modinfo.config");

		// Find any ref entries and add their contents to this property tree.
		boost::property_tree::ptree::assoc_iterator pos;
		while ( ( pos = configTree.find("ref")) != configTree.not_found() ) {
			ptree::value_type& ref = *pos;
			ptree& refTree = ref.second;
			if (refTree.get_child_optional("id")) {
				const std::string refStr = refTree.get<std::string>("id");
				ND_DEBUG("Referencing common properties file %s%s.\n", refStr.c_str(), propertyFileExt.c_str());

				std::set<std::string> ignoreSet;

				BOOST_FOREACH(ptree::value_type &i, refTree) {
					if ( std::string(i.first.data()) == "ignore" ) {
						std::string ignorePath(i.second.data());
						ND_DEBUG("Found setting path to ignore: %s\n", ignorePath.c_str());
						ignoreSet.insert(ignorePath);
					}
				}
				ND_DEBUG("Ignoring %d settings from %s%s.\n", ignoreSet.size(), refStr.c_str(), propertyFileExt.c_str());

				// If it's not already loaded, load it:
				if ( _commonModuleSettings.find(refStr) == _commonModuleSettings.end()) {
					ND_DEBUG("Loading new common settings reference %s.", refStr.c_str());
					_loadPropertyFile(_commonModuleSettings[refStr], path, refStr + propertyFileExt);
				}

				BOOST_FOREACH(ptree::value_type &c, _commonModuleSettings[refStr].getProperties().get_child("modinfo.config")) {
					if ( std::string(c.first.data()) == "setting" ) {
						std::string settingPath(c.second.get<std::string>("path"));
						ND_DEBUG("Considering setting path %s.\n", settingPath.c_str());

						// Only add the setting if it's not in the ignore list.
						if ( ignoreSet.find(settingPath) == ignoreSet.end() ) {
							configTree.push_back(c);
						}
						else {
							ND_DEBUG("Ignoring setting path %s.\n", settingPath.c_str());
						}
					}
				}
			}

			configTree.erase(configTree.to_iterator(pos));
		}

		// Now pull out the individual settings.
		BOOST_FOREACH(ptree::value_type &c, configTree) {
			const std::string propertyName(c.first.data());
			if (propertyName == "setting" ) {
				CESettingDesc newSetting;
				ptree& settingTree = c.second;

				if (settingTree.get_child_optional("prompt")) newSetting.setPrompt(settingTree.get<std::string>("prompt"));
				if (settingTree.get_child_optional("path")) newSetting.setPath(settingTree.get<std::string>("path"));
				if (settingTree.get_child_optional("type")) newSetting.setType(settingTree.get<std::string>("type"));
				if (settingTree.get_child_optional("desc")) newSetting.setDesc(settingTree.get<std::string>("desc"));
				if (settingTree.get_child_optional("default")) newSetting.setDefault(settingTree.get<std::string>("default"));
				if (settingTree.get_child_optional("required")) newSetting.setRequiredFlag(true);

				modDesc.addSetting(newSetting);
			}
		}
	}
/*
	if ( pt.get_child_optional("modinfo.config") ) {
		BOOST_FOREACH(ptree::value_type &c, pt.get_child("modinfo.config")) {
			const std::string propertyName(c.first.data());

			if (propertyName == "ref" ) {
				// Found a reference to a common settings file.
				const std::string refStr(c.second.data());

				// If it's not already loaded, load it:
				if ( _commonModuleSettings.find(refStr) == _commonModuleSettings.end()) {
					ND_DEBUG("Loading new common settings reference %s.", refStr.c_str());
					_loadPropertyFile(_commonModuleSettings[refStr], path, refStr + propertyFileExt);
				}

				// Iterate over each setting it contains and add them to the current module.
				const ModuleSettingsMap& commonSettings = _commonModuleSettings[refStr].getSettingsMap();
				ModuleSettingsMap::const_iterator pos;
				for (pos = commonSettings.begin(); pos != commonSettings.end(); ++pos) {
					modDesc.addSetting(pos->second);
				}
			}
			else if (propertyName == "setting" ) {
				CESettingDesc newSetting;
				ptree& settingTree = c.second;

				if (settingTree.get_child_optional("prompt")) newSetting.setPrompt(settingTree.get<std::string>("prompt"));
				if (settingTree.get_child_optional("path")) newSetting.setPath(settingTree.get<std::string>("path"));
				if (settingTree.get_child_optional("type")) newSetting.setType(settingTree.get<std::string>("type"));
				if (settingTree.get_child_optional("desc")) newSetting.setDesc(settingTree.get<std::string>("desc"));
				if (settingTree.get_child_optional("default")) newSetting.setDefault(settingTree.get<std::string>("default"));
				if (settingTree.get_child_optional("required")) newSetting.setRequiredFlag(true);

				modDesc.addSetting(newSetting);
			}
		}
	}
	*/
}

void CE_DLL_Registry::refreshAvailableDLLList() {
	std::string modulePath = "/usr/libexec"; // Initialize in case it's not in the config file

	if (CEcfg::instance()->existsSrv("dllPath"))
		modulePath = CEcfg::instance()->getSrv("dllPath").c_str();

	ACE_stat dirStats;

	if ( ACE_OS::stat(modulePath.c_str(), &dirStats) != 0 ) {
		int statErr = errno;
		switch (statErr) {
			case EACCES:
				ND_ERROR("No permission to access Server.dllPath %s.\n", modulePath.c_str());
				break;
			case ENOENT:
				ND_ERROR("Server.dllPath %s does not exist.\n", modulePath.c_str());
				break;
			default:
				ND_ERROR("Unable to stat Server.dllPath %s.\n", modulePath.c_str());
		}
	}
	else if ( ! S_ISDIR(dirStats.st_mode ) ) {
		ND_ERROR("Server.dllPath %s is not a directory.\n", modulePath.c_str());
	}
	else {
		using namespace boost;

		_availableModules.clear();
		ACE_Dirent dllDir(modulePath.c_str());
		dirent* entry;

		std::set<std::string> dirContents;

		while ( (entry = dllDir.read()) ) {
			const std::string filename(entry->d_name);
			dirContents.insert(filename);
		}

		dllDir.close();

		regex dll_re("^lib(dev|mod)(\\w+)\\.so\\.([\\d\\.]+)$");
		match_results<std::string::const_iterator> what;
		std::string modType, modName, descName, descPath;

		std::set<std::string>::iterator pos;

		for ( pos = dirContents.begin(); pos != dirContents.end(); ++pos ) {
			if (regex_match(*pos, what, dll_re)) {
				modType = what[1];
				modName = what[2];
				if ( modType == "dev" ) {
					ND_DEBUG("Found device module %s.\n", modName.c_str());
					CEModuleDesc newDevDesc(modType + modName, "device", modulePath + "/" + *pos, "Discovered device module.");
					_availableModules[modType + modName] = newDevDesc;
				}
				else if ( modType == "mod" ) {
					ND_DEBUG("Found segment module %s.\n",  modName.c_str());
					CEModuleDesc newSegDesc(modType + modName, "module", modulePath + "/" + *pos, "Discovered segment module.");
					_availableModules[modType + modName] = newSegDesc;
				}

				descName = modType + modName + propertyFileExt;

				if (dirContents.find(descName) != dirContents.end()) {
					_loadPropertyFile(_availableModules[modType + modName], modulePath, descName);
				}
			}
		}
	}
}

bool CE_DLL_Registry::loadIfNotLoaded(const std::string& dllName,
	xmlrpc_c::registryPtr& rpcRegistry, std::string _loading /* = "" */) {
	ND_DEBUG("Attempting to load module %s.\n", dllName.c_str());

	// Prevent infinite recursion. Get out of here if we're already trying to load it right now.
	if (_loading.find(dllName) != std::string::npos ) {
		ND_DEBUG("Already trying to load %s, returning.\n", dllName.c_str());
		return true;
	}

	_loading += dllName;

	// Return immediately if it's already loaded
	if ( _dllRegister.find(dllName) != _dllRegister.end()) return true;

	// Make sure it can be found
	if ( _availableModules.find(dllName) == _availableModules.end() ) {
		ND_ERROR("Request to load module %s which is unknown.\n", dllName.c_str());
		return false;
	}

	// Load dependencies
	std::set<std::string> deps;
	std::set<std::string>::iterator pos;
	_availableModules[dllName].getDependencies(deps);
	for ( pos = deps.begin(); pos != deps.end(); ++pos ) {
		ND_DEBUG("Ensuring dependency %s is loaded.\n", pos->c_str());
		if ( ! loadIfNotLoaded(*pos, rpcRegistry, _loading) ) return false;
		_availableModules[*pos].addReverseDependency(dllName);
	}

	CE_DLL_Ops* newDLL = new CE_DLL_Ops;
	try {
		newDLL->load(dllName, _availableModules[dllName].getPath(), rpcRegistry);
	}
	catch (...) {
		return false;
	}
	_dllRegister[dllName] = newDLL;

	return true;
}

bool CE_DLL_Registry::unloadIfLoaded(const std::string& dllName,
	xmlrpc_c::registryPtr& rpcRegistry, std::string _unloading /* = "" */) {

	// Prevent infinite recursion. Get out of here if we're already trying to unload it right now.
	if ( _unloading.find(dllName) != std::string::npos ) {
		ND_DEBUG("Already trying to unload %s, returning.\n", dllName.c_str());
		return true;
	}

	_unloading += dllName;

	RegisterMap::iterator pos;
	pos = _dllRegister.find(dllName);

	// Return if it not loaded
	if ( pos == _dllRegister.end()) { return true; }

	CE_DLL_Ops* loadedDLL = _dllRegister[dllName];

	if (loadedDLL->ptrDLL()->getReferenceCount() > 0 ) {
		ND_ERROR("Cannot unload DLL %s, still has %d objects referencing it.\n",
			dllName.c_str(), loadedDLL->ptrDLL()->getReferenceCount());
		return false;
	}

	// Unload reverse dependencies
	std::set<std::string> rdeps;
	std::set<std::string>::iterator rpos;
	_availableModules[dllName].getReverseDependencies(rdeps);
	for ( rpos = rdeps.begin(); rpos != rdeps.end(); ++rpos ) {
		if ( ! unloadIfLoaded(*rpos, rpcRegistry, _unloading) ) return false;
	}

	_dllRegister.erase(dllName);

	ND_NOTICE("Nuking XML-RPC method registry to prep for rebuild.\n");
	rpcRegistry.unpoint();
	delete loadedDLL;

	rebuild();
	return true;
}

void CE_DLL_Registry::unloadAll(bool performRebuild /* = false */) {
	if ( ! _dllRegister.size() ) {
		ND_DEBUG("No user-managed DLLs to unload.\n");
		return;
	}

	ND_NOTICE("Unloading all %d user-managed DLL(s).\n", _dllRegister.size());

	while (_dllRegister.size()) {
		unloadIfLoaded(_dllRegister.begin()->first, xmlrpc_server::instance()->rpcRegistry);
	}

	if (performRebuild) rebuild();
}

CE_DLL_Ops *CE_DLL_Registry::operator[] (const std::string& dllName) {

	if ( isLoaded(dllName) ) {
		return _dllRegister[dllName];
	}

	throw DLLNotLoaded("Attempted reference to unloaded DLL \"" +
		dllName + "\"");
}

CE_DLL_Ops *CE_DLL_Registry::dll (const std::string& dllName) {

	if ( isLoaded(dllName) ) {
		return _dllRegister[dllName];
	}

	throw DLLNotLoaded("Attempted reference to unloaded DLL \"" +
		dllName + "\"");
}

bool CE_DLL_Registry::isLoaded(const std::string& dllName) {
	RegisterMap::iterator pos;
	pos = _dllRegister.find(dllName);

	if ( pos != _dllRegister.end()) { return true; }
	return false;
}

void CE_DLL_Registry::listLoaded(std::vector<std::string>& modList) {
	RegisterMap::iterator pos;

	for (pos = _dllRegister.begin();  pos != _dllRegister.end(); ++pos) {
		modList.push_back(pos->first);
	}
}

void CE_DLL_Registry::rebuild() {
	ND_NOTICE("Performing required registry rebuild.\n");
	xmlrpc_c::registryPtr& registry = xmlrpc_server::instance()->rpcRegistry;

	registry.unpoint();
	registry = new xmlrpc_c::registry;

	xmlrpc_server::instance()->registerBuiltins();

	RegisterMap::iterator pos;


	for (pos = _dllRegister.begin();  pos != _dllRegister.end(); ++pos) {
		pos->second->register_methods(registry);
	}
}

bool CE_DLL_Registry::isAvailable(const std::string dllName) {
	bool found = false;

	if ( _availableModules.find(dllName) != _availableModules.end() ) found = true;
    return found;
}

void CE_DLL_Registry::listAvailableDeviceModules(std::vector<std::string>& modList) {
	modList.clear();

	ModuleDescMap::iterator pos;

	for ( pos = _availableModules.begin(); pos != _availableModules.end(); ++pos ) {
		if ( pos->second.isDevice() ) {
			modList.push_back(xmlrpc_c::value_string(pos->first));
		}
	}
}

void CE_DLL_Registry::listAvailableMacroModules(std::vector<std::string>& modList) {
	modList.clear();

	ModuleDescMap::iterator pos;

	for ( pos = _availableModules.begin(); pos != _availableModules.end(); ++pos ) {
		if ( pos->second.isMacro() ) {
			modList.push_back(xmlrpc_c::value_string(pos->first));
		}
	}
}

void CE_DLL_Registry::listAvailableSegmentModules(std::vector<std::string>& modList) {
	modList.clear();

	ModuleDescMap::iterator pos;

	for ( pos = _availableModules.begin(); pos != _availableModules.end(); ++pos ) {
		if ( ! ( pos->second.isDevice() || pos->second.isMacro() ) ) {
			modList.push_back(xmlrpc_c::value_string(pos->first));
		}
	}
}

std::string CE_DLL_Registry::getDescription(const std::string& dllName) {
	std::string desc = "";

	if ( _availableModules.find(dllName) != _availableModules.end() ) {
		desc = _availableModules[dllName].getDesc();
	}

	return desc;
}

const CEModuleDesc& CE_DLL_Registry::getDescObj(const std::string& dllName) {
	if ( _availableModules.find(dllName) == _availableModules.end() ) {
		throw DoesNotExist(DoesNotExist::msg("module", dllName));
	}

	return _availableModules[dllName];
}

};
