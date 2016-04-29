//=============================================================================
// Macro.js
// Author: Tad Kollar  
// $Id: Macro.js 2024 2013-06-04 17:48:29Z tkollar $
// Copyright (c) 2013.
//      NASA Glenn Research Center.  All rights reserved.
//      See COPYING file that comes with this distribution.
//=============================================================================

//-----------------------------------------------------------------------------
// Macro
//-----------------------------------------------------------------------------
// A way to set up complex CE channels with just a few settings.
// modName: The associated module (DLL) name must be supplied.
// methodName: The XML-RPC method to call on the CE.
// target: The name of the channel.
//-----------------------------------------------------------------------------
function Macro(modName, methodName, target) {
	if ( modName == undefined || modName == '' ) {
		throw "New Macro modName not supplied, cannot initialize.";
	}

	this.modName = modName;

	// Since only one of each macro is necssary, just modify
	// modName to create an identifier.
	DLLInstance.call(this, modName + "_instance");

	this.methodName = methodName;
	this.target = target;
	this.settings = { };
}

Macro.prototype = Object.create(DLLInstance.prototype);

Macro.prototype.addSetting = function(settingObj) {
	msg.debug("Adding setting " + settingObj.templatePath);
	this.settings[settingObj.templatePath] = settingObj.val2XmlrpcVal();
}

Macro.prototype.execute = function() {
	var settingsStruct = new xmlrpcval();
	settingsStruct.addStruct(this.settings);

	var params = [ new xmlrpcval(this.target, "string"), settingsStruct ];

	if (callXmlrpcMethodBase(this.modName + "." + this.methodName, params) != null) {
			return true;
	}

	return false;
}