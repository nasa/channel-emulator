/* -*- C++ -*- */

//=============================================================================
/**
 * @file   CE_DLL_Registry.hpp
 * @author Tad Kollar  
 *
 * $Id: CE_DLL_Registry.hpp 1947 2013-04-10 18:35:23Z tkollar $
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _NASA_CE_DLL_REGISTRY_HPP_
#define _NASA_CE_DLL_REGISTRY_HPP_

#include "CE_DLL.hpp"
#include "CE_DLL_Ops.hpp"

#include <vector>
#include <set>
#include <ace/Singleton.h>
#include <boost/property_tree/ptree.hpp>

namespace nasaCE {

//=============================================================================
/**
 * @class CESettingDesc
 * @brief Information about each setting for an instance of a module.
 */
//=============================================================================
class CESettingDesc {
public:
	CESettingDesc(const std::string newPrompt = "", const std::string newPath = "",
		const std::string newType = "", const std::string newDesc = "",
		const bool isReq = false);

	/// Read-only accessor to _prompt.
	std::string getPrompt() const;

	/// Write-only accessor to _prompt.
	void setPrompt(const std::string& newPrompt);

	/// Read-only accessor to _path.
	/// @param instanceName If known, substitute this into template paths.
	std::string getPath(const std::string& instanceName = "") const;

	/// Write-only accessor to _path.
	void setPath(const std::string& newPath);

	/// Read-only accessor to _type.
	std::string getType() const;

	/// Write-only accessor to _type.
	void setType(const std::string& newType);

	/// Read-only accessor to _desc.
	std::string getDesc() const;

	/// Write-only accessor to _desc.
	void setDesc(const std::string& newDesc);

	/// Read-only accessor to _default.
	std::string getDefault() const;

	/// Write-only accessor to _default.
	void setDefault(const std::string& newDefault);

	/// Read-only accessor to _isRequired.
	bool getRequiredFlag() const;

	/// Write-only accessor to _isRequired.
	void setRequiredFlag(const bool isReq);

	/// Compare the paths of two settings.
	bool operator == (const CESettingDesc& other) const;

private:
	/// What to display to the user when viewing/changing this setting.
	std::string _prompt;

	/// The location of the setting in the config file.
	std::string _path;

	/// The data type that the setting holds.
	std::string _type;

	/// Description of the setting.
	std::string _desc;

	/// Description of the setting.
	std::string _default;

	/// Whether this setting must exist before an instance of the module attempts to activate.
	bool _isRequired;
};

typedef std::map<std::string, CESettingDesc> ModuleSettingsMap;
extern CESettingDesc NO_SETTING;

//=============================================================================
/**
 * @class CEModuleDesc
 * @brief Information about available DLLs that may or may not be loaded.
 */
//=============================================================================
class CEModuleDesc {
public:
	/// Constructor.
	/// @param newName The base name of the module.
	/// @param newCat The sort of module it is (device, ipv4, TM, AOS, etc).
	/// @param newPath The full filesystem path including the module filename.
	/// @param newDesc Module description loaded from the desc file.
	CEModuleDesc(const std::string newName = "", const std::string newCat = "",
				 const std::string newPath = "", const std::string newDesc = "");

	/// Read-only accessor to _name.
	std::string getName() const;

	/// Write-only accessor to _name.
	void setName(const std::string& newName);

	/// Read-only accessor to _category.
	std::string getCategory() const;

	/// Write-only accessor to _category.
	void setCategory(const std::string& newCat);

	/// Device is a special category.
	bool isDevice() const;

	/// Macro is a special category.
	bool isMacro() const;

	/// Read-only accessor to _path.
	std::string getPath() const;

	/// Write-only accessor to _path.
	void setPath(const std::string& newPath);

	/// Read-only accessor to _desc.
	std::string getDesc() const;

	/// Write-only accessor to _name.
	void setDesc(const std::string& newDesc);

	/// Write-only accessor to _dependencies.
	/// @param newDep The name of a module that this one depends on.
	void addDependency(const std::string& newDep);

	/// Delete all dependencies.
	void clearDependencies();

	/// Read-only accessor to _dependencies.
	/// @param deps Reference to a set to be replaced by the contents of _dependencies.
	void getDependencies(std::set<std::string>& deps);

	/// Write-only accessor to _reverseDependencies.
	/// @param newDep The name of a module depends on this one.
	void addReverseDependency(const std::string& newDep);

	/// Delete all reverse dependencies.
	void clearReverseDependencies();

	/// Read-only accessor to _reverseDependencies.
	/// @param deps Reference to a set to be replaced by the contents of _reverseDependencies.
	void getReverseDependencies(std::set<std::string>& deps);

	/// Add to _settings.
	/// @param newSetting A pre-configured CESettingDesc object.
	void addSetting(const CESettingDesc& newSetting);

	/// Remove one setting found by its config file path.
	/// @param path The path in the config file.
	void delSettingByPath(const std::string& path);

	/// Retrieve one setting found by its config file path.
	/// @param path The path in the config file.
	const CESettingDesc& getSettingByPath(const std::string& path);

	/// Return a reference to _settings.
	const ModuleSettingsMap& getSettingsMap() const;

	/// Return a reference to _rawProperties.
	boost::property_tree::ptree& getProperties();

	/// Return a const reference to _rawProperties.
	const boost::property_tree::ptree& const_getProperties() const;

private:
	/// The base name of the module.
	std::string _name;

	/// Whether it's a device, or related to IP, AOS, TM, etc.
	std::string _category;

	/// The full filesystem path including the module filename.
	std::string _path;

	/// Module description loaded from the desc file.
	std::string _desc;

	/// Which modules must be loaded first.
	std::set<std::string> _dependencies;

	/// Which modules depend on this one.
	std::set<std::string> _reverseDependencies;

	/// Map of settings for instances of this module.
	ModuleSettingsMap _settings;

	/// The contents of the XML properties file loaded as a Boost property tree.
	boost::property_tree::ptree _rawProperties;
};

typedef std::map<std::string, CEModuleDesc> ModuleDescMap;

typedef std::map<std::string, CE_DLL_Ops*> RegisterMap;

//=============================================================================
/**
 * @class CE_DLL_Registry
 * @brief Maintains a map of all dynamically loaded libraries as CE_DLL_Ops pointers.
 */
//=============================================================================
class CE_DLL_Registry {

public:
	/// Constructor.
	CE_DLL_Registry();

	/// Destructor.
	~CE_DLL_Registry();

	/// @brief Tests whether the DLL is already loaded, and if not, calls on CE_DLL_Ops to load it up.
	/// @param dllName The name, not filename/pathname, of the DLL.
	/// @param rpcRegistry Reference to the registry to add XML-RPC calls to.
	/// @param _loading Internal use for limiting recursion.
	/// @return True if module was loaded (now or previously); false if failure.
	bool loadIfNotLoaded(const std::string& dllName, xmlrpc_c::registryPtr& rpcRegistry, std::string _loading = "");

	/// @brief Tests whether the DLL is loaded, and if it is, calls on CE_DLL_Ops to unload it.
	/// @param dllName The name, not filename/pathname, of the DLL.
	/// @param rpcRegistry Reference to the registry to add XML-RPC calls to.
	/// @param _unloading Internal use for limiting recursion.
	/// @return True is module is unloaded (now or previously); false if failure.
	bool unloadIfLoaded(const std::string& dllName, xmlrpc_c::registryPtr& rpcRegistry, std::string _unloading = "");

	/// @brief Unload every DLL listed in the map.
	/// @param performRebuild If true, rebuild the RPC registry after removing all DLLs.
	void unloadAll(bool performRebuild = false);

	/// @brief An accessor for the DLL map.
	CE_DLL_Ops *operator [](const std::string&);

	/// @brief An accessor for the DLL map.
	CE_DLL_Ops *dll(const std::string&);

	/// @brief Return true if the DLL marked as loaded.
	bool isLoaded(const std::string&);

	/// @brief Provide a list of all DLLs that have been loaded.
	/// @param modList Vector to be cleared and reloaded with loaded module names.
	void listLoaded(std::vector<std::string>& modList);

	/// @brief Iterate through the map and call the registry rebuild function in each DLL.
	void rebuild();

	/// @brief Find what what DLLs are available based on a directory listing of Server.dllPath.
	void refreshAvailableDLLList();

	/// @brief Determine whether the specified module is known.
	/// @param dllName The name (not path/filename) of the module.
	/// @return True if dllName is a key in _availableModules, false otherwise.
	bool isAvailable(const std::string dllName);

	/// @brief Supply a list of all known device modules, loaded or unloaded.
	/// @param modList Vector to be cleared and reloaded with device names.
	void listAvailableDeviceModules(std::vector<std::string>& modList);

	/// @brief Supply a list of all known macro modules, loaded or unloaded.
	/// @param modList Vector to be cleared and reloaded with macro module names.
	void listAvailableMacroModules(std::vector<std::string>& modList);

	/// @brief Supply a list of all known segment modules, loaded or unloaded.
	/// @param modList Vector to be cleared and reloaded with segment module names.
	void listAvailableSegmentModules(std::vector<std::string>& modList);

	/// @brief Describe the specified module.
	/// @param dllName The name (not path/filename) of the module.
	/// @return A text description of the module if found, empty if not.
	std::string getDescription(const std::string& dllName);

	/// @brief Return a reference to the description object of the specified module.
	/// @param dllName The name (not path/filename) of the module.
	/// @return A reference to the CEModuleDesc if the module is found.
	/// @throw DoesNotExist If module identified by dllName is not found.
	const CEModuleDesc& getDescObj(const std::string& dllName);

	/// The extension that every module property file ends with.
	static const std::string propertyFileExt;

private:
	/// The map of loaded DLLs.
	RegisterMap _dllRegister;

	/// All modules that have been discovered.
	ModuleDescMap _availableModules;

	/// Contains groups of settings common to multiple modules.
	ModuleDescMap _commonModuleSettings;

	/// Open the XML file with the properties for the module.
	/// @param modDesc Description to add properties to.
	/// @param path Directory path that the file is located in.
	/// @param fname Filename of the properties file.
	void _loadPropertyFile(CEModuleDesc& modDesc, const std::string& path, const std::string& fname);
};

typedef ACE_Singleton<CE_DLL_Registry, ACE_Recursive_Thread_Mutex> dll_registry;

} // Namespace NASA

#endif

