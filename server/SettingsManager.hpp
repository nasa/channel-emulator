/* -*- C++ -*- */

//=============================================================================
/**
 * @file   SettingsManager.hpp
 * @author Tad Kollar  
 *
 * $Id: SettingsManager.hpp 2060 2013-06-11 16:16:02Z tkollar $
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _SETTINGS_MANAGER_HPP_
#define _SETTINGS_MANAGER_HPP_

#include <ace/Singleton.h>
#include <libconfig.h++>
#include <string>

using namespace libconfig;

namespace nasaCE {

//=============================================================================
/**
 * @class SettingsManager
 * @author Tad Kollar  
 * @brief Provide a centralized interface to configurable settings.
 */
//=============================================================================
class SettingsManager {
public:

	/// Default constructor.
    SettingsManager();

    /// Destructor.
    ~SettingsManager();

 	/// Read in both the server and device/channel config files from disk. Die if there's an error.
	/// @param cmdLinePath True if the device/channel config file path came from the command line.
	/// @param cmdLineFile True if the device/channel config file name came from the command line.
    void initialize(const bool cmdLinePath, const bool cmdLineFile);

	/// A way to deactivate before being destroyed.
	void shutdown();

	/// @brief Provide a string representative of the specified Setting type.
	/// @param settingType One of the types supported by libconfig::Setting.
	/// @return A string representing the setting type.
	std::string settingType2Str(const Setting::Type settingType);

	/// @brief Attempt to retrieve a reference a setting; if it doesn't exist, create it.
	/// @param path The entire name of the setting.
	/// @param settingType One of the types supported by libconfig::Setting.
	/// @param wasCreated This flag is set if the setting didn't already exist.
	/// @return A reference to the found or newly created Setting.
    Setting& getOrAdd(const std::string& path, const Setting::Type settingType, bool& wasCreated);

	Setting& getOrAddString(const std::string& path, const std::string& defaultVal = "");
	Setting& getOrAddInt(const std::string& path, const int& defaultVal = 0);
	Setting& getOrAddInt64(const std::string& path, const int64_t& defaultVal = 0);
	Setting& getOrAddBool(const std::string& path, const bool defaultVal = false);
	Setting& getOrAddFloat(const std::string& path, const double& defaultVal = 0.0);
	Setting& getOrAddList(const std::string& path);
	Setting& getOrAddArray(const std::string& path);
	Setting& getOrAddGroup(const std::string& path);

    /// @brief Retrieve a setting that must already exist in the channel/device config file.
    /// @param path The entire name of the setting.
	/// @return A reference to the found Setting.
    Setting& get(const std::string& path);

	/// @brief Retrieve a setting that must already exist in the server config file.
    /// @param path The entire name of the setting.
	/// @return A reference to the found Setting.
    Setting& getSrv(const std::string& path);

    /// @brief Remove a setting if it exists.
    /// @param path The entire name of the setting.
    void remove(const std::string& path);

    /// @brief Retrieve a string value from an existing Setting.
    /// @param path The entire name of the setting.
    ///
    /// Useful because Setting has trouble casting to std::string in many situations.
    std::string getString(const std::string& path);

	/// Determine whether the setting has already been created in the channel/device config.
	/// @param path The entire name of the setting.
    bool exists(const std::string& path) const;

	/// Determine whether the setting has already been created in the server config.
	/// @param path The entire name of the setting.
    bool existsSrv(const std::string& path) const;

	/// Determine whether the setting was stored in the config file or added later.
	/// @param path The entire name of the setting.
    bool added(const std::string& path) const;

	/// Extract all but the last identifier from the path.
	/// E.g. Provided @c Grandparent.parent.child , return @c Grandparent.parent .
	/// @param path The entire name of the setting.
    static std::string getParentPath(const std::string& path);

	/// Extract the last identifier from the path.
	/// E.g. Provided @c Grandparent.parent.child , return @c child .
	/// @param path The entire name of the setting.
	static std::string getChildName(const std::string& path);

	/// Write-only accessor to _saveFlag.
	void setSaveFlag(const bool newVal = true) { _saveFlag = newVal; }

	/// Read-only accessor to _saveFlag.
	bool getSaveFlag() const { return _saveFlag; }

 	/// Read in the channel/device config file from disk. Die if there's an error.
	/// @param configFilePath Path to the configuration directory.
	/// @param configFileName Filename of the configuration file.
    void loadConfig(const std::string& configFilePath, const std::string& configFileName);

	/// Copy the entire config file to a string.
	/// @param configStr The string to copy the file into.
	void configToString(std::string& configStr) const;

	/// Save the current configuration to a file.
	/// @param filename The name of the config file (not including the path).
	void saveConfig(const std::string& filename);

private:

	/// The object that stores the channel/device configuration.
	Config _cfgRoot;

	/// Erase all the settings in the channel/device config.
	void _wipeCfgRoot();

	/// Path to the channel/device configuration file directory.
	std::string _cfgFilePath;

	/// Path to the original channel/device configuration file.
	std::string _cfgFileName;

	/// Whether to save off the channel/device config tree when shutting down.
	bool _saveFlag;

	/// The object that stores the server configuration.
	Config _srvCfgRoot;

}; // class SettingsManager

bool addedPostBoot(Setting& setting);

typedef ACE_Singleton<SettingsManager, ACE_Recursive_Thread_Mutex> CEcfg;

} // namespace nasaCE

#endif // _SETTINGS_MANAGER_HPP_
