//=============================================================================
// DLLInstance.js
// Author: Tad Kollar  
// $Id: DLLInstance.js 2078 2013-06-12 18:50:12Z tkollar $
// Copyright (c) 2013.
//      NASA Glenn Research Center.  All rights reserved.
//      See COPYING file that comes with this distribution.
//=============================================================================

//=============================================================================
// DLLInstance
// Provide an interface to the basic functions of a DLL instance on the CE
//=============================================================================

//-----------------------------------------------------------------------------
// DLLInstance Constructor.
//-----------------------------------------------------------------------------
function DLLInstance(instanceName) {
	this.id = instanceName;
}

DLLInstance.prototype.toString = function() {
	return this.id;
}

//-----------------------------------------------------------------------------
// DLLInstance.prototype.loadSettings
//-----------------------------------------------------------------------------
// Retrieve default and user-selected settings for a device/segment/macro DLL.
// groupNamePrefix: For devices/segments, the config file group path
// channelName: For a segment, the name of the channel it belongs to
//-----------------------------------------------------------------------------
DLLInstance.prototype.loadSettings = function(groupNamePrefix) {
	if ( dllMgr.dlls[this.modName].isMacro() ) { this.settings = { } }
	else { this.loadNonMacroSettings(groupNamePrefix, channelName); }
}

//-----------------------------------------------------------------------------
// DLLInstance.prototype.loadNonMacroSettings
//-----------------------------------------------------------------------------
// Retrieve default and user-selected settings for a device/segment, from
// both the CE and the properties stored by the associated DLL object.
// groupNamePrefix: Path to the config file group where the settings are found
// channelName: For a segment, the name of the channel it belongs to
//-----------------------------------------------------------------------------
DLLInstance.prototype.loadNonMacroSettings = function(groupNamePrefix) {
	// Get the explicitly set, local settings for the instance first.
	var groupName = groupNamePrefix + "." + this.id,
		groupVal = callXmlrpcMethodBase("setting.getGroup",
			[ new xmlrpcval(groupName, 'string') ]),
		channelName = ( this.channel == undefined )? "" : this.channel;

	if ( ! groupVal ) { this.settings = { }; }
	else {
		msg.debug("Found " + groupVal.structSize() + " settings in group " + groupName + ".");
		this.settings = xmlrpcVal2JS(groupVal);
	}

	// Check for default and global settings.
	var dll = dllMgr.dlls[this.modName];
	if ( dll.properties.config.setting == undefined ) return;

	for (var i = 0; i < dll.properties.config.setting.length; i++ ) {
		var setting = dll.properties.config.setting[i],
			settingPath = setting.path.toString()
				.replace(Setting.segmentNameTemplate, this.id)
				.replace(Setting.channelNameTemplate, channelName);

		// If the setting is not already accounted for, look for it on the CE.
		if ( this.settings[settingPath] == undefined ) {
			var params = [ new xmlrpcval(settingPath, 'string') ];

			msg.debug("Looking for missing setting " + settingPath + " on the CE.");
			var existsVal = callXmlrpcMethodBase("setting.exists", params);

			if ( existsVal ) {
				if (existsVal.scalarVal()) {
					msg.debug("Setting " + settingPath + " found on the CE, retrieving.");
					var settingVal = callXmlrpcMethodBase("setting.getAutoType", params);

					if ( settingVal ) {
						msg.debug("Successfully retrieved " + settingPath + " from the CE.");
						this.settings[settingPath] = xmlrpcVal2JS(settingVal);
					}
				}
				else {
					msg.debug("Setting " + settingPath + " not found on the CE, using default.");
				}
			}
		}
	}

	// msg.dumpObj(this.settings, this.id + " settings", "", 0);
}