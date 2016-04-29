/* -*- C++ -*- */

//=============================================================================
/**
 * @file   Settings_Interface.hpp
 * @author Tad Kollar  
 *
 * $Id: Settings_Interface.hpp 2311 2013-07-30 15:47:37Z tkollar $
 * Copyright (c) 2012.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _SETTINGS_INTERFACE_HPP_
#define _SETTINGS_INTERFACE_HPP_

#include "CE_Macros.hpp"
#include "SettingsManager.hpp"
#include <xmlrpc-c/xml.hpp>

namespace nasaCE {

//=============================================================================
/**
 * @class Settings_Interface
 * @brief Provide an XML-RPC interface to the configuration file.
*/
//=============================================================================

class Settings_Interface : public CE_DLL {
public:
    Settings_Interface(xmlrpc_c::registryPtr& rpcRegistry);

    ~Settings_Interface() { }

    Setting::Type xArray2cArrayOrListType(const xmlrpc_c::value_array& xarray);
	Setting::Type x2cType(const xmlrpc_c::value::type_t xtype);
	Setting::Type x2cType(const xmlrpc_c::value& xval);
	xmlrpc_c::value::type_t c2xType(const Setting::Type ctype);
	void x2cArray(const xmlrpc_c::value& xval, Setting& carray);
	void c2xArray(const Setting& carray, xmlrpc_c::value& xval);
	void c2xStruct(const Setting& cgroup, xmlrpc_c::value& xval);
	void x2cVal(const xmlrpc_c::value& xval, Setting& cval);
	void c2xVal(const Setting& cval, xmlrpc_c::value& xval);

	void remove(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);
	void setting_exists(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);
	void get_type(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);

	void set_bool(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);
	void get_bool(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);
	void set_string(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);
	void get_string(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);
	void set_int(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);
	void get_int(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);
	void set_int64(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);
	void get_int64(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);
	void set_double(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);
	void get_double(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);
	void set_array(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);
	void get_array(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);
	void set_list(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);
	void get_list(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);
	void create_group(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);
	void get_group(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);
	void set_auto(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);
	void get_auto(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);
	void get_config_file(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);
	void list_config_files(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);
	void list_data_files(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);
	void load_config_file(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);
	void save_config_file(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);

	/// Export specified methods via XML-RPC.
	void register_methods(xmlrpc_c::registryPtr&);

private:
	/// Fill a vector with XML-RPC strings containing all filenames of regular files
	/// in the specified directory.
	/// @param path The path to the directory.
	/// @param retvalP Pointer the XML-RPC value where the vector will be stored.
	void _dirListRegularFiles(const std::string& path, xmlrpc_c::value* retvalP);

}; // class Settings_Interface



extern Settings_Interface* Settings_InterfaceP;

METHOD_CLASS(remove, "n:s",
	"Delete the specified setting from the config.", Settings_InterfaceP);
METHOD_CLASS(setting_exists, "b:s",
	"Return whether the provided setting path exists in the config.", Settings_InterfaceP);
METHOD_CLASS(get_type, "s:s",
	"Return as a string the type of the setting in the provided path.", Settings_InterfaceP);
METHOD_CLASS(set_bool, "n:sb",
	"Set the path to the provided Boolean value.", Settings_InterfaceP);
METHOD_CLASS(get_bool, "b:s",
	"Return the Boolean value at the provided setting path.", Settings_InterfaceP);
METHOD_CLASS(set_string, "n:ss",
	"Set the path to the provided string value.", Settings_InterfaceP);
METHOD_CLASS(get_string, "s:s",
	"Return the string value at the provided setting path.", Settings_InterfaceP);
METHOD_CLASS(set_int, "n:si",
	"Set the path to the provided integer value.", Settings_InterfaceP);
METHOD_CLASS(get_int, "i:s",
	"Return the integer value at the provided setting path.", Settings_InterfaceP);
METHOD_CLASS(set_int64, "n:si",
	"Set the path to the provided 64-bit integer value.", Settings_InterfaceP);
METHOD_CLASS(get_int64, "i:s",
	"Return the 64-bit integer value at the provided setting path.", Settings_InterfaceP);
METHOD_CLASS(set_double, "n:sd",
	"Set the path to the provided double value.", Settings_InterfaceP);
METHOD_CLASS(get_double, "d:s",
	"Return the double value at the provided setting path.", Settings_InterfaceP);
METHOD_CLASS(set_array, "n:sA",
	"Set the path with the provided array contents.", Settings_InterfaceP);
METHOD_CLASS(get_array, "A:s",
	"Get the contents of the array setting at the provided path.", Settings_InterfaceP);
METHOD_CLASS(set_list, "n:sS",
	"Set the list at the path to the contents of the provided structure.", Settings_InterfaceP);
METHOD_CLASS(get_list, "S:s",
	"Get the contents of the list setting at the provided path.", Settings_InterfaceP);
METHOD_CLASS(create_group, "n:sS",
	"Create a new group at the specified path (if it doesn't exist).", Settings_InterfaceP);
METHOD_CLASS(get_group, "S:s",
	"Get the group at the specified path as a structure.", Settings_InterfaceP);
METHOD_CLASS(set_auto, "n:si,n:sb,n:sd,n:ss,n:sS,n:sA",
	"Set the path to the value with auto-selected type.", Settings_InterfaceP);
METHOD_CLASS(get_auto, "i:s,b:s,d:s,s:s,S:s,A:s",
	"Return a value with auto-selected type for the specified path.", Settings_InterfaceP);
METHOD_CLASS(get_config_file, "s:n",
	"Get the contents of the complete config file as a string.", Settings_InterfaceP);
METHOD_CLASS(list_config_files, "A:n",
	"Get the list of existing config file names as an array of strings.", Settings_InterfaceP);
METHOD_CLASS(list_data_files, "A:n",
	"Get the list of existing data file names as an array of strings.", Settings_InterfaceP);
METHOD_CLASS(load_config_file, "n:s",
	"Load the specified configuration file name, destroying the current configuration.", Settings_InterfaceP);
METHOD_CLASS(save_config_file, "n:s",
	"Save the current configuration to the specified file name in the config file folder.", Settings_InterfaceP);
} // namespace nasaCE

#endif // _SETTINGS_INTERFACE_HPP_
