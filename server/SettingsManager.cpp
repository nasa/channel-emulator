/* -*- C++ -*- */

//=============================================================================
/**
 * @file   SettingsManager.cpp
 * @author Tad Kollar  
 *
 * $Id: SettingsManager.cpp 2069 2013-06-11 19:20:55Z tkollar $
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================
#include "CE_Config.hpp"
#include "CE_Macros.hpp"
#include "SettingsManager.hpp"

#include "ace/OS_main.h"
#include <sstream>

namespace nasaCE {
SettingsManager::SettingsManager():
	_cfgFilePath(globalCfg.defaultCfgFilePath),
	_cfgFileName(globalCfg.defaultCfgFileName),
	_saveFlag(true) {


}

SettingsManager::~SettingsManager() { }

void SettingsManager::initialize(const bool cmdLinePath, const bool cmdLineFile) {
	_srvCfgRoot.setAutoConvert(true); // allow ints to be floats and vice versa
	_cfgRoot.setAutoConvert(true);

	const std::string filename(globalCfg.srvCfgFile);

	ND_INFO("[SettingsManager] Loading server configuration from %s.\n", filename.c_str());

	try {
		_srvCfgRoot.readFile(filename.c_str());
	}
	catch (ParseException& e) {
		// Problem with config file syntax
		ND_ERROR("Parse Exception in line %d of server configuration file: %s\n",
			e.getLine(), e.getError());

		ACE_OS::kill(ACE_OS::getpid(), SIGTERM);
	}
	catch (FileIOException& e) {
		// File could not be read
		ND_ERROR("I/O error while attempting to read server configuration file.\n");
		ACE_OS::kill(ACE_OS::getpid(), SIGTERM);
	}

	std::string cfgPath(globalCfg.cfgFilePath), cfgFile(globalCfg.cfgFileName);

	// If initial config file paths were not specified on the command line, check the server config.
	if ( !cmdLinePath && existsSrv("cfgPath") ) {
		cfgPath = std::string(getSrv("cfgPath").c_str());
	}

	if ( !cmdLineFile && existsSrv("cfgFile") ) {
		cfgFile = std::string(getSrv("cfgFile").c_str());
	}

	loadConfig(globalCfg.cfgFilePath, globalCfg.cfgFileName);
}

void SettingsManager::_wipeCfgRoot() {
	ND_INFO("[SettingsManager] Removing %d children settings from the root configuration.\n", _cfgRoot.getRoot().getLength());
	while ( _cfgRoot.getRoot().getLength() > 0 ) _cfgRoot.getRoot().remove(static_cast<unsigned int>(0));
}

void SettingsManager::loadConfig(const std::string& configFilePath, const std::string& configFileName) {
	const std::string filename(configFilePath + "/" + configFileName);

	ND_INFO("[SettingsManager] Loading device/channel configuration from %s.\n", filename.c_str());
	_wipeCfgRoot(); // Erase the current config file from memory
	_cfgRoot.setAutoConvert(true); // allow ints to be floats and vice versa

	try {
		_cfgRoot.readFile(filename.c_str());
	}
	catch (ParseException& e) {
		// Problem with config file syntax
		ND_ERROR("Parse Exception in line %d of configuration file: %s\n",
			e.getLine(), e.getError());

		_wipeCfgRoot();
	}
	catch (FileIOException& e) {
		// File could not be read
		ND_ERROR("I/O error while attempting to read configuration file.\n");
		_wipeCfgRoot();
	}

	_cfgFilePath = configFilePath;
	_cfgFileName = configFileName;
}

void SettingsManager::shutdown() {
	if ( _saveFlag) {
		std::ostringstream os;

		os << _cfgFileName << "." << ACE_OS::time(0);

		saveConfig(os.str());
	}
}

void SettingsManager::saveConfig(const std::string& filename) {
	const std::string fullPath(_cfgFilePath + "/" + filename);

	ND_INFO("[SettingsManager] Writing out configuration file to %s.\n", fullPath.c_str());

	try {
		_cfgRoot.writeFile(fullPath.c_str());
	}
	catch (const FileIOException& e) {
		ND_ERROR("[SettingsManager] Writing configuration to %s failed: I/O Error\n", fullPath.c_str());
	}
}


std::string SettingsManager::settingType2Str(const Setting::Type settingType) {
	switch (settingType) {
		case Setting::TypeInt: return "Int";
		case Setting::TypeInt64: return "Int64";
		case Setting::TypeFloat: return "Float";
		case Setting::TypeString: return "String";
		case Setting::TypeBoolean: return "Boolean";
		case Setting::TypeArray: return "Array";
		case Setting::TypeList: return "List";
		case Setting::TypeGroup: return "Group";
		case Setting::TypeNone: return "Unknown";
		default: return "Unknown";
	}

	return "Unknown";
};

Setting& SettingsManager::getOrAdd(const std::string& path, const Setting::Type settingType, bool& wasCreated) {
	if ( path.empty() ) return _cfgRoot.getRoot();

	if ( _cfgRoot.exists(path) ) {
		wasCreated = false;
		Setting& curSetting = _cfgRoot.lookup(path);
		if ( curSetting.getType() == settingType ) return curSetting;
		else {
			std::ostringstream os;

			os	<< "Config file error, line " << curSetting.getSourceLine() << ", setting " << path
				<< ": expected type '" << settingType2Str(settingType) << "', found type '"
				<< settingType2Str(curSetting.getType()) << "'.";

			if ( curSetting.getType() == Setting::TypeString ) os << " Try removing quotes around value.";
			else if ( settingType == Setting::TypeString ) os << " Try putting quotes around value.";

			throw Exception(os.str());
		}
	}

	wasCreated = true;
	bool parentWasCreated;
	Setting& parentGroup = getOrAdd(getParentPath(path), Setting::TypeGroup, parentWasCreated);
	return parentGroup.add(getChildName(path), settingType);
}

Setting& SettingsManager::getOrAddString(const std::string& path, const std::string& defaultVal) {
	bool wasCreated = false;
	Setting& s = getOrAdd(path, Setting::TypeString, wasCreated);
	if (wasCreated) s = defaultVal;

	return s;
}

Setting& SettingsManager::getOrAddInt(const std::string& path, const int& defaultVal) {
	bool wasCreated = false;
	Setting& s = getOrAdd(path, Setting::TypeInt, wasCreated);
	if (wasCreated) s = defaultVal;

	return s;
}

Setting& SettingsManager::getOrAddInt64(const std::string& path, const int64_t& defaultVal) {
	bool wasCreated = false;
	Setting& s = getOrAdd(path, Setting::TypeInt64, wasCreated);
	if (wasCreated) s = defaultVal;

	return s;
}

Setting& SettingsManager::getOrAddBool(const std::string& path, const bool defaultVal) {
	bool wasCreated = false;
	Setting& s = getOrAdd(path, Setting::TypeBoolean, wasCreated);
	if (wasCreated) s = defaultVal;

	return s;
}

Setting& SettingsManager::getOrAddFloat(const std::string& path, const double& defaultVal) {
	bool wasCreated = false;
	Setting& s = getOrAdd(path, Setting::TypeFloat, wasCreated);
	if (wasCreated) s = defaultVal;

	return s;
}

Setting& SettingsManager::getOrAddList(const std::string& path) {
	bool wasCreated = false;
	Setting& s = getOrAdd(path, Setting::TypeList, wasCreated);
	return s;
}

Setting& SettingsManager::getOrAddArray(const std::string& path) {
	bool wasCreated = false;
	Setting& s = getOrAdd(path, Setting::TypeArray, wasCreated);
	return s;
}

Setting& SettingsManager::getOrAddGroup(const std::string& path) {
	bool wasCreated = false;
	Setting& s = getOrAdd(path, Setting::TypeGroup, wasCreated);
	return s;
}

Setting& SettingsManager::get(const std::string& path) {
	if ( path.empty() ) return _cfgRoot.getRoot();

	return _cfgRoot.lookup(path);
}

Setting& SettingsManager::getSrv(const std::string& path) {
	if ( path.empty() ) return _srvCfgRoot.getRoot();

	return _srvCfgRoot.lookup(path);
}

void SettingsManager::remove(const std::string& path) {
	if ( exists(path) ) {
		int index = get(path).getIndex();
		Setting& parent = get(path).getParent();
		parent.remove(index);
	}
}

std::string SettingsManager::getString(const std::string& path) {
	std::string strVal = get(path);
	return strVal;
}

bool SettingsManager::exists(const std::string& path) const { return _cfgRoot.exists(path); }

bool SettingsManager::existsSrv(const std::string& path) const { return _srvCfgRoot.exists(path); }

bool SettingsManager::added(const std::string& path) const {
	return ( _cfgRoot.lookup(path).getSourceLine() > 0? false : true );
}

std::string SettingsManager::getParentPath(const std::string& path) {
	std::string::size_type idx;

	idx = path.rfind(".");

	if ( idx == std::string::npos ) return "";

	return path.substr(0, idx);
}

std::string SettingsManager::getChildName(const std::string& path) {
	std::string::size_type idx;

	idx = path.rfind(".");

	if ( idx == std::string::npos ) return path;

	return path.substr(idx + 1);
}

bool addedPostBoot(Setting& setting) { return ( setting.getSourceLine() > 0? false : true ); }

void SettingsManager::configToString(std::string& configStr) const {
	char *ptr;
	size_t sizeloc;
	FILE *cfgStream = open_memstream(&ptr, &sizeloc);

	_cfgRoot.write(cfgStream);

	fclose(cfgStream);

	configStr = std::string(ptr);
	free(ptr);
}


} // namespace nasaCE
