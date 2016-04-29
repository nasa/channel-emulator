/* -*- C++ -*- */

//=============================================================================
/**
 * @file   Settings_Interface.cpp
 * @author Tad Kollar  
 *
 * $Id: Settings_Interface.cpp 2082 2013-06-13 13:22:28Z tkollar $
 * Copyright (c) 2012.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "Settings_Interface.hpp"
#include "ChannelRegistry.hpp"
#include "DeviceRegistry.hpp"
#include "string_nocase.hpp"
#include <xmlrpc-c/girerr.hpp>
#include <ace/Dirent.h>

namespace nasaCE {

Settings_Interface* Settings_InterfaceP;

Settings_Interface::Settings_Interface(xmlrpc_c::registryPtr& rpcRegistry):
	CE_DLL("Settings_Interface") {
	ACE_TRACE("Settings_Interface::Settings_Interface");
	register_methods(rpcRegistry);

}

void Settings_Interface::register_methods(xmlrpc_c::registryPtr& rpcRegistry) {
	ACE_TRACE("Settings_Interface::register_methods");
	REGISTER_METHOD(remove, "setting.remove");
	REGISTER_METHOD(setting_exists, "setting.exists");
	REGISTER_METHOD(get_type, "setting.getType");

	REGISTER_METHOD(set_bool, "setting.setBool");
	REGISTER_METHOD(get_bool, "setting.getBool");
	REGISTER_METHOD(set_string, "setting.setString");
	REGISTER_METHOD(get_string, "setting.getString");
	REGISTER_METHOD(set_int, "setting.setInt");
	REGISTER_METHOD(get_int, "setting.getInt");
	REGISTER_METHOD(set_int64, "setting.setInt64");
	REGISTER_METHOD(get_int64, "setting.getInt64");
	REGISTER_METHOD(set_double, "setting.setDouble");
	REGISTER_METHOD(get_double, "setting.getDouble");
	REGISTER_METHOD(set_array, "setting.setArray");
	REGISTER_METHOD(get_array, "setting.getArray");
	REGISTER_METHOD(set_list, "setting.setList");
	REGISTER_METHOD(get_list, "setting.getList");
	REGISTER_METHOD(create_group, "setting.createGroup");
	REGISTER_METHOD(get_group, "setting.getGroup");
	REGISTER_METHOD(set_auto, "setting.setAutoType");
	REGISTER_METHOD(get_auto, "setting.getAutoType");

	REGISTER_METHOD(get_config_file, "server.getConfigFile");
	REGISTER_METHOD(list_config_files, "server.listConfigFiles");
	REGISTER_METHOD(list_data_files, "server.listDataFiles");
	REGISTER_METHOD(load_config_file, "server.loadConfigFile");
	REGISTER_METHOD(save_config_file, "server.saveConfigFile");
}

Setting::Type Settings_Interface::xArray2cArrayOrListType(const xmlrpc_c::value_array& xarray) {
	const std::vector<xmlrpc_c::value> arrayData = xarray.vectorValueValue();

	xmlrpc_c::value::type_t firstType = arrayData[0].type();

	for ( size_t i = 0; i < arrayData.size(); ++i ) {
		if ( arrayData[i].type() != firstType || // multiple types
			arrayData[i].type() == xmlrpc_c::value::TYPE_ARRAY || // non-scalar type
			arrayData[i].type() == xmlrpc_c::value::TYPE_STRUCT ) // non-scalar type
		{
			return Setting::TypeList;
		}
	}

	return Setting::TypeArray;
}

Setting::Type Settings_Interface::x2cType(const xmlrpc_c::value::type_t xtype) {
	switch (xtype) {
		case xmlrpc_c::value::TYPE_INT: return Setting::TypeInt;
		case xmlrpc_c::value::TYPE_I8: return Setting::TypeInt64;
		case xmlrpc_c::value::TYPE_BOOLEAN: return Setting::TypeBoolean;
		case xmlrpc_c::value::TYPE_STRING: return Setting::TypeString;
		case xmlrpc_c::value::TYPE_DOUBLE: return Setting::TypeFloat;
		case xmlrpc_c::value::TYPE_STRUCT: return Setting::TypeList;
		case xmlrpc_c::value::TYPE_ARRAY: return Setting::TypeArray;
		case xmlrpc_c::value::TYPE_DATETIME:
		case xmlrpc_c::value::TYPE_BYTESTRING:
		case xmlrpc_c::value::TYPE_C_PTR:
		case xmlrpc_c::value::TYPE_NIL:
		case xmlrpc_c::value::TYPE_DEAD:
			return Setting::TypeNone;
	}

	return Setting::TypeNone;
}

Setting::Type Settings_Interface::x2cType(const xmlrpc_c::value& xval) {
	return (xval.type() == xmlrpc_c::value::TYPE_ARRAY)?
		xArray2cArrayOrListType(xval) : x2cType(xval.type());
}

xmlrpc_c::value::type_t Settings_Interface::c2xType(const Setting::Type ctype) {
	switch (ctype) {
		case Setting::TypeInt: return xmlrpc_c::value::TYPE_INT;
		case Setting::TypeInt64: return xmlrpc_c::value::TYPE_I8;
		case Setting::TypeBoolean: return xmlrpc_c::value::TYPE_BOOLEAN;
		case Setting::TypeString: return xmlrpc_c::value::TYPE_STRING;
		case Setting::TypeFloat: return xmlrpc_c::value::TYPE_DOUBLE;
		case Setting::TypeList: return xmlrpc_c::value::TYPE_STRUCT;
		case Setting::TypeArray: return xmlrpc_c::value::TYPE_ARRAY;
		case Setting::TypeNone:
		case Setting::TypeGroup:
			return xmlrpc_c::value::TYPE_DEAD;
	}

	return xmlrpc_c::value::TYPE_DEAD;
}

void Settings_Interface::c2xArray(const Setting& carray, xmlrpc_c::value& xval) {
	std::vector<xmlrpc_c::value> xmlrpcArray;

	for ( int i = 0; i < carray.getLength(); ++i ) {
		xmlrpc_c::value newVal;
		c2xVal(carray[i], newVal);
		xmlrpcArray.push_back(newVal);
	}

	xval = xmlrpc_c::value_array(xmlrpcArray);
}

void Settings_Interface::x2cArray(const xmlrpc_c::value& xval, Setting& carray) {
	const std::vector<xmlrpc_c::value> xmlrpcArray = xmlrpc_c::value_array(xval).vectorValueValue();

	for ( std::size_t i = 0; i < xmlrpcArray.size(); ++i ) {
		Setting& newElement = carray.add(x2cType(xmlrpcArray[i]));
		x2cVal(xmlrpcArray[i], newElement);
	}
}

void Settings_Interface::c2xStruct(const Setting& cgroup, xmlrpc_c::value& xval) {
	xstruct xmlrpcStruct;

	for ( int i = 0 ; i < cgroup.getLength(); ++i ) {
		xmlrpc_c::value newVal;
		c2xVal(cgroup[i], newVal);
		std::pair<std::string, xmlrpc_c::value> newPair(cgroup[i].getPath(), newVal);
		xmlrpcStruct.insert(newPair);
	}

	xval = xmlrpc_c::value_struct(xmlrpcStruct);
}

void Settings_Interface::x2cVal(const xmlrpc_c::value& xval, Setting& cval) {

	switch (xval.type()) {
		case xmlrpc_c::value::TYPE_INT:     cval = static_cast<int>(xmlrpc_c::value_int(xval)); break;
		case xmlrpc_c::value::TYPE_I8:      cval = static_cast<long long>(xmlrpc_c::value_i8(xval)); break;
		case xmlrpc_c::value::TYPE_BOOLEAN: cval = static_cast<bool>(xmlrpc_c::value_boolean(xval)); break;
		case xmlrpc_c::value::TYPE_STRING:  cval = static_cast<std::string>(xmlrpc_c::value_string(xval)); break;
		case xmlrpc_c::value::TYPE_DOUBLE:  cval = static_cast<double>(xmlrpc_c::value_double(xval)); break;
		case xmlrpc_c::value::TYPE_ARRAY:   x2cArray(xval, cval); break;
		case xmlrpc_c::value::TYPE_STRUCT:  x2cArray(xval, cval); break;
		case xmlrpc_c::value::TYPE_DATETIME:
		case xmlrpc_c::value::TYPE_BYTESTRING:
		case xmlrpc_c::value::TYPE_C_PTR:
		case xmlrpc_c::value::TYPE_NIL:
		case xmlrpc_c::value::TYPE_DEAD:
			break;
	}
}

void Settings_Interface::c2xVal(const Setting& cval, xmlrpc_c::value& xval) {
	switch (cval.getType()) {
		case Setting::TypeInt:     xval = xmlrpc_c::value_int(static_cast<int>(cval)); break;
		case Setting::TypeInt64:   xval = xmlrpc_c::value_i8(static_cast<long long>(cval)); break;
		case Setting::TypeBoolean: xval = xmlrpc_c::value_boolean(static_cast<bool>(cval)); break;
		case Setting::TypeString:  xval = xmlrpc_c::value_string(cval); break;
		case Setting::TypeFloat:   xval = xmlrpc_c::value_double(static_cast<double>(cval)); break;
		case Setting::TypeArray:   c2xArray(cval, xval); break;
		case Setting::TypeList:    c2xArray(cval, xval); break;
		case Setting::TypeGroup:   c2xStruct(cval, xval); break;
		case Setting::TypeNone:
			break;
	}
}

void Settings_Interface::remove(xmlrpc_c::paramList const& paramList,
	xmlrpc_c::value* retvalP) {
	ACE_TRACE("Settings_Interface::remove");
	const std::string settingPath = paramList.getString(0);

	ND_DEBUG("Received an XML-RPC request to remove setting %s.\n", settingPath.c_str());

	CEcfg::instance()->remove(settingPath);

	*retvalP = xmlrpc_c::value_nil();
}

void Settings_Interface::setting_exists(xmlrpc_c::paramList const& paramList,
	xmlrpc_c::value* retvalP) {
	ACE_TRACE("Settings_Interface::exists");
	const std::string settingPath = paramList.getString(0);

	ND_DEBUG("Received an XML-RPC request to test existance of setting %s.\n", settingPath.c_str());

	*retvalP = xmlrpc_c::value_boolean(CEcfg::instance()->exists(settingPath));
}

void Settings_Interface::get_type(xmlrpc_c::paramList const& paramList,
	xmlrpc_c::value* retvalP) {
	ACE_TRACE("Settings_Interface::get_type");
	const std::string settingPath = paramList.getString(0);

	ND_DEBUG("Received an XML-RPC request to get the type of setting %s.\n", settingPath.c_str());

	*retvalP = xmlrpc_c::value_string(CEcfg::instance()->settingType2Str(CEcfg::instance()->get(settingPath).getType()));
}

void Settings_Interface::set_bool(xmlrpc_c::paramList const& paramList,
	xmlrpc_c::value* retvalP) {
	ACE_TRACE("Settings_Interface::set_bool");
	const std::string settingPath = paramList.getString(0);
	const bool val = paramList.getBoolean(1);

	ND_DEBUG("Received an XML-RPC request to set Boolean setting %s to %d.\n", settingPath.c_str(), val);

	CEcfg::instance()->getOrAddBool(settingPath, Setting::TypeBoolean) = val;

	*retvalP = xmlrpc_c::value_nil();
}

void Settings_Interface::get_bool(xmlrpc_c::paramList const& paramList,
	xmlrpc_c::value* retvalP) {
	ACE_TRACE("Settings_Interface::get_bool");
	const std::string settingPath = paramList.getString(0);

	ND_DEBUG("Received an XML-RPC request to get Boolean setting at path %s.\n", settingPath.c_str());

	*retvalP = xmlrpc_c::value_boolean(CEcfg::instance()->get(settingPath));
}

void Settings_Interface::set_string(xmlrpc_c::paramList const& paramList,
	xmlrpc_c::value* retvalP) {
	ACE_TRACE("Settings_Interface::set_string");
	const std::string settingPath = paramList.getString(0);
	const std::string val = paramList.getString(1);

	ND_DEBUG("Received an XML-RPC request to set string setting %s to '%s'.\n", settingPath.c_str(), val.c_str());

	CEcfg::instance()->getOrAddString(settingPath) = val;

	*retvalP = xmlrpc_c::value_nil();
}

void Settings_Interface::get_string(xmlrpc_c::paramList const& paramList,
	xmlrpc_c::value* retvalP) {
	ACE_TRACE("Settings_Interface::get_string");
	const std::string settingPath = paramList.getString(0);

	ND_DEBUG("Received an XML-RPC request to get string setting at path %s.\n", settingPath.c_str());

	*retvalP = xmlrpc_c::value_string(CEcfg::instance()->get(settingPath));
}

void Settings_Interface::set_int(xmlrpc_c::paramList const& paramList,
	xmlrpc_c::value* retvalP) {
	ACE_TRACE("Settings_Interface::set_int");
	const std::string settingPath = paramList.getString(0);
	const int val = paramList.getInt(1);

	ND_DEBUG("Received an XML-RPC request to set integer setting %s to %d.\n", settingPath.c_str(), val);

	CEcfg::instance()->getOrAddInt(settingPath) = val;

	*retvalP = xmlrpc_c::value_nil();
}

void Settings_Interface::get_int(xmlrpc_c::paramList const& paramList,
	xmlrpc_c::value* retvalP) {
	ACE_TRACE("Settings_Interface::get_int");
	const std::string settingPath = paramList.getString(0);

	ND_DEBUG("Received an XML-RPC request to get integer setting at path %s.\n", settingPath.c_str());

	*retvalP = xmlrpc_c::value_int(CEcfg::instance()->get(settingPath));
}

void Settings_Interface::set_int64(xmlrpc_c::paramList const& paramList,
	xmlrpc_c::value* retvalP) {
	ACE_TRACE("Settings_Interface::set_int64");
	const std::string settingPath = paramList.getString(0);
	const long long int val = paramList.getI8(1);

	ND_DEBUG("Received an XML-RPC request to set 64-bit integer setting %s to %ll.\n", settingPath.c_str(), val);

	CEcfg::instance()->getOrAddInt64(settingPath) = val;

	*retvalP = xmlrpc_c::value_nil();
}

void Settings_Interface::get_int64(xmlrpc_c::paramList const& paramList,
	xmlrpc_c::value* retvalP) {
	ACE_TRACE("Settings_Interface::get_int64");
	const std::string settingPath = paramList.getString(0);

	ND_DEBUG("Received an XML-RPC request to get 64-bit integer setting at path %s.\n", settingPath.c_str());

	*retvalP = xmlrpc_c::value_i8(CEcfg::instance()->get(settingPath));
}

void Settings_Interface::set_double(xmlrpc_c::paramList const& paramList,
	xmlrpc_c::value* retvalP) {
	ACE_TRACE("Settings_Interface::set_double");
	const std::string settingPath = paramList.getString(0);
	const double val = paramList.getDouble(1);

	ND_DEBUG("Received an XML-RPC request to set double setting %s to %f.\n", settingPath.c_str(), val);

	CEcfg::instance()->getOrAddFloat(settingPath, Setting::TypeFloat) = val;

	*retvalP = xmlrpc_c::value_nil();
}

void Settings_Interface::get_double(xmlrpc_c::paramList const& paramList,
	xmlrpc_c::value* retvalP) {
	ACE_TRACE("Settings_Interface::get_double");
	const std::string settingPath = paramList.getString(0);

	ND_DEBUG("Received an XML-RPC request to get double setting at path %s.\n", settingPath.c_str());

	*retvalP = xmlrpc_c::value_double(CEcfg::instance()->get(settingPath));
}

void Settings_Interface::set_array(xmlrpc_c::paramList const& paramList,
	xmlrpc_c::value* retvalP) {
	ACE_TRACE("Settings_Interface::set_array");
	const std::string settingPath = paramList.getString(0);

	ND_DEBUG("Received an XML-RPC request to set array setting %s.\n", settingPath.c_str());

	Setting& carray = CEcfg::instance()->getOrAddArray(settingPath);
	x2cArray(paramList[1], carray);

	*retvalP = xmlrpc_c::value_nil();
}

void Settings_Interface::get_array(xmlrpc_c::paramList const& paramList,
	xmlrpc_c::value* retvalP) {
	ACE_TRACE("Settings_Interface::get_array");
	const std::string settingPath = paramList.getString(0);

	ND_DEBUG("Received an XML-RPC request to get array setting %s.\n", settingPath.c_str());

	Setting& carray = CEcfg::instance()->getOrAddArray(settingPath);
	xmlrpc_c::value xval;

	c2xArray(carray, xval);

	*retvalP = xmlrpc_c::value_array(xval);
}

void Settings_Interface::set_list(xmlrpc_c::paramList const& paramList,
	xmlrpc_c::value* retvalP) {
	ACE_TRACE("Settings_Interface::set_list");
	const std::string settingPath = paramList.getString(0);

	ND_DEBUG("Received an XML-RPC request to set list setting %s.\n", settingPath.c_str());

	Setting& clist = CEcfg::instance()->getOrAddList(settingPath);
	x2cArray(paramList[1], clist);

	*retvalP = xmlrpc_c::value_nil();
}

void Settings_Interface::get_list(xmlrpc_c::paramList const& paramList,
	xmlrpc_c::value* retvalP) {
	ACE_TRACE("Settings_Interface::get_list");
	const std::string settingPath = paramList.getString(0);

	ND_DEBUG("Received an XML-RPC request to get list setting %s.\n", settingPath.c_str());

	Setting& clist = CEcfg::instance()->getOrAddList(settingPath);
	xmlrpc_c::value xval;

	c2xArray(clist, xval);

	*retvalP = xmlrpc_c::value_array(xval);
}

void Settings_Interface::create_group(xmlrpc_c::paramList const& paramList,
	xmlrpc_c::value* retvalP) {
	ACE_TRACE("Settings_Interface::create_group");
	const std::string settingPath = paramList.getString(0);

	ND_DEBUG("Received an XML-RPC request to populate group setting %s.\n", settingPath.c_str());

	bool wasCreated;
	Setting& grp = CEcfg::instance()->getOrAdd(settingPath, Setting::TypeGroup, wasCreated);

	if (paramList.size() > 1) {
		const xmlrpc_c::value_struct settingsStruct(paramList[1]);
		const xstruct settingsMap(settingsStruct);
		xstruct::const_iterator pos;

		for ( pos = settingsMap.begin(); pos != settingsMap.end(); ++pos ) {
			Setting& newSetting = grp.add(pos->first, x2cType(pos->second));
			x2cVal(pos->second, newSetting);
		}
	}

	*retvalP = xmlrpc_c::value_nil();
}

void Settings_Interface::get_group(xmlrpc_c::paramList const& paramList,
	xmlrpc_c::value* retvalP) {
	ACE_TRACE("Settings_Interface::get_group");
	const std::string settingPath = paramList.getString(0);

	ND_DEBUG("Received an XML-RPC request to retrieve group setting %s.\n", settingPath.c_str());

	Setting& cgroup = CEcfg::instance()->get(settingPath);
	xmlrpc_c::value xval;
	c2xStruct(cgroup, xval);

	*retvalP = xmlrpc_c::value_struct(xval);
}

void Settings_Interface::set_auto(xmlrpc_c::paramList const& paramList,
	xmlrpc_c::value* retvalP) {
	ACE_TRACE("Settings_Interface::set_auto");
	const std::string settingPath = paramList.getString(0);
	const xmlrpc_c::value val(paramList[1]);

	ND_DEBUG("Received an XML-RPC request to set setting %s with automatically detected value type.\n", settingPath.c_str());

	bool wasCreated;
	Setting& valRef = CEcfg::instance()->getOrAdd(settingPath, x2cType(val), wasCreated);
	x2cVal(val, valRef);

	*retvalP = xmlrpc_c::value_nil();
}

void Settings_Interface::get_auto(xmlrpc_c::paramList const& paramList,
	xmlrpc_c::value* retvalP) {
	ACE_TRACE("Settings_Interface::get_auto");
	const std::string settingPath = paramList.getString(0);

	ND_DEBUG("Received an XML-RPC request to get setting %s with automatically detected value type.\n", settingPath.c_str());

	Setting& valRef = CEcfg::instance()->get(settingPath);

	c2xVal(valRef, *retvalP);
}

void Settings_Interface::get_config_file(xmlrpc_c::paramList const& paramList,
	xmlrpc_c::value* retvalP) {
	ACE_TRACE("Settings_Interface::get_config_file");

	std::string configFileContents;
	CEcfg::instance()->configToString(configFileContents);

	*retvalP = xmlrpc_c::value_string(configFileContents);
}

void Settings_Interface::list_config_files(xmlrpc_c::paramList const& paramList,
	xmlrpc_c::value* retvalP) {
	ACE_TRACE("Settings_Interface::list_config_files");

	_dirListRegularFiles(globalCfg.cfgFilePath, retvalP);
}

void Settings_Interface::list_data_files(xmlrpc_c::paramList const& paramList,
	xmlrpc_c::value* retvalP) {
	ACE_TRACE("Settings_Interface::list_data_files");

	_dirListRegularFiles(globalCfg.fileIOPath, retvalP);
}

void Settings_Interface::_dirListRegularFiles(const std::string& path, xmlrpc_c::value* retvalP) {
	struct stat docStat;

	ACE_Dirent dir;

	if ( dir.open(path.c_str()) ) {
		throw "Unable to open directory " + path + " on the CE.";
	}

	ND_DEBUG("[Settings_Interface] _dirListRegularFiles(): Opened directory %s.\n", path.c_str());

	ACE_DIRENT* entry;
	std::list<string_nocase> entries;

	string_nocase fullPath;
	while ( (entry = dir.read()) != NULL ) {
		fullPath = string_nocase(path.c_str()) + "/" + string_nocase(entry->d_name);
		string_nocase entryName(entry->d_name);

		if ( ! ACE_OS::stat(fullPath.c_str(), &docStat) ) {
			if ( S_ISREG(docStat.st_mode) ) {
				entries.push_back(entryName);
				ND_DEBUG("[Settings_Interface] _dirListRegularFiles(): Found file %s.\n", entryName.c_str());
			}
		}
		else {
			ND_DEBUG("[Settings_Interface] _dirListRegularFiles(): Unable to stat %s.\n", entryName.c_str());
		}
	}

	ND_DEBUG("[Settings_Interface] _dirListRegularFiles(): Found %d total files in %s.\n",
		entries.size(), path.c_str());

	dir.close();
	entries.sort();

	xarray configFileNames;
	std::list<string_nocase>::iterator pos;

	for ( pos = entries.begin(); pos != entries.end(); ++pos ) {
		configFileNames.push_back(xmlrpc_c::value_string(pos->c_str()));
	}

	ND_DEBUG("[Settings_Interface] _dirListRegularFiles(): Returning XML-RPC array with %d filenames.\n",
		configFileNames.size());

	*retvalP = xmlrpc_c::value_array(configFileNames);
}

void Settings_Interface::load_config_file(xmlrpc_c::paramList const& paramList,
	xmlrpc_c::value* retvalP) {
	ACE_TRACE("Settings_Interface::load_config_file");

	const std::string cfgFile = paramList.getString(0);

	xmlrpc_server::instance()->switchToConfigFile(cfgFile);

	*retvalP = xmlrpc_c::value_nil();
}

void Settings_Interface::save_config_file(xmlrpc_c::paramList const& paramList,
	xmlrpc_c::value* retvalP) {
	ACE_TRACE("Settings_Interface::save_config_file");

	const std::string cfgFile = paramList.getString(0);

	CEcfg::instance()->saveConfig(cfgFile);

	*retvalP = xmlrpc_c::value_nil();
}

} // namespace nasaCE
