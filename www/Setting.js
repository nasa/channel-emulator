//=============================================================================
// Setting.js
// Author: Tad Kollar  
// $Id: Setting.js 2538 2014-02-20 18:03:19Z tkollar $
// Copyright (c) 2013.
//      NASA Glenn Research Center.  All rights reserved.
//      See COPYING file that comes with this distribution.
//=============================================================================

//=============================================================================
// Setting
// A way to contain and process CE device/segment settings.
//=============================================================================

//-----------------------------------------------------------------------------
// Setting
//-----------------------------------------------------------------------------
// Base class constructor.
// props: Object that should contain:
// templatePath: REQUIRED. The generic path of the setting in the CE config file.
// dataType: REQUIRED. The kind of data (string, int, etc) used with the setting.
// prompt: REQUIRED. The label the user sees on the form.
// desc: REQUIRED. Text explaining what the setting does on the CE.
// segment: If setting is associated with an existing segment, this is a reference to it.
// channelName: The name of the associated channel, if available.
// defaultVal: Value to display if none has been provided yet.
// isRequired: Whether the setting must have a value to submit the form.
// setMethod: The XML-RPC call to make to change the setting.
// unsetMethod: The XML-RPC call to make to erase the setting.
// getMethod: The XML-RPC call to make to retrieve the setting.
// isArray: Whether this setting contains an array of values.
// selector: For certain compound settings, the name of the value to make a choice with.
// min: The lowest acceptable numerical value.
// max: The highest acceptable numerical value.
// acceptedValues: An array of values that are turning into a select rather than
//                 an editable field.
// order: For member settings in a structList, what column they appear in (1, 2, 3, etc)
// key: For a sorted member setting in a structList, an integer indicating to sort
//      by this setting and its priority. 1 is the primary, 2 is secondary, etc.
// partOfSettingsStruct: For macro settings only, whether the setting will be
//                       combined with others in a single XML-RPC struct when
//                       sent to the CE.
//-----------------------------------------------------------------------------
function Setting(props) {
	var req = [ "templatePath", "dataType", "prompt", "desc" ];

	for ( i = 0; i < req.length; ++i ) {
		if ( ! props.hasOwnProperty(req[i]) ) {
			throw "Setting " + props.dataType + " constructor invoked with improperly-formed props object (missing " + req[i] + ")."
		}
	}

	$.extend(true, this, props);
	this.validObj = true;
}

Setting.channelNameTemplate = 'XYZ_CHANNEL_XYZ';
Setting.segmentNameTemplate = 'XYZ_MODNAME_XYZ';
Setting.emptyVal = "XX__empty__XX";
Setting.globalRegex = new RegExp(".*" + Setting.segmentNameTemplate + ".*");

//-----------------------------------------------------------------------------
// Setting.makeFromProps
//-----------------------------------------------------------------------------
// Construct the right type of settings object using properties from the CE.
// existingSegment: Reference to an existing segment if it exists.
// propEntry: The reference to the setting in the properties file.
// channelName: Name of the channel, if applicable.
// templatePath: If part of a compound setting, the parent must specify this.
//-----------------------------------------------------------------------------
Setting.makeFromProps = function (existingSegment, propEntry, channelName,
	templatePath, noFindDefault) {
	if ( channelName == undefined ) { channelName = ""; }

	var props = {
		segment: existingSegment,
		channelName: channelName,
		defaultVal: (propEntry.hasOwnProperty("default"))? propEntry.default.toString() : undefined,
		templatePath: (propEntry.hasOwnProperty("path"))? propEntry.path.toString() : templatePath,
		dataType: propEntry.type.toString(),
		dataFormat: (propEntry.type.hasOwnProperty("@attributes") &&
			propEntry.type["@attributes"].hasOwnProperty("format"))? propEntry.type["@attributes"].format: undefined,
		prompt: propEntry.prompt.toString(),
		desc: propEntry.desc.toString(),
		isRequired: (propEntry.hasOwnProperty("required"))? true : false,
		setMethod: (propEntry.hasOwnProperty("setMethod"))? propEntry.setMethod.toString() : undefined,
		unsetMethod: (propEntry.hasOwnProperty("unsetMethod"))? propEntry.unsetMethod.toString() : undefined,
		getMethod: (propEntry.hasOwnProperty("getMethod"))? propEntry.getMethod.toString() : undefined,
		isArray: propEntry.hasOwnProperty("array"),
		ethernetInterfaces: propEntry.hasOwnProperty("ethernetInterfaces"),
		min: (propEntry.hasOwnProperty("min"))? propEntry.min.toString() : undefined,
		max: (propEntry.hasOwnProperty("max"))? propEntry.max.toString() : undefined,
		order: (propEntry.hasOwnProperty("order"))? propEntry.order.toString() : undefined,
		key: (propEntry.hasOwnProperty("key"))? propEntry.key.toString() : undefined,
		partOfSettingsStruct: (propEntry.hasOwnProperty("partOfSettingsStruct"))? true : false
	};

	if ( propEntry.hasOwnProperty("acceptedValues")) {
		props.acceptedValues = [];
		for ( i = 0; i < propEntry.acceptedValues.item.length; ++i ) {
			var propItem = propEntry.acceptedValues.item[i],
				newVal = {
					prompt: propItem.toString(),
					value: propItem.toString()
				};

			if ( propItem.hasOwnProperty("@attributes") &&
				propItem["@attributes"].hasOwnProperty("value") ) {
				newVal.value = propItem["@attributes"].value.toString();
			}

			props.acceptedValues.push(newVal);
		}
	}

	var newSetting;

	switch (props.dataType) {
		case "string":
			newSetting = new StringSetting(props);
			break;

		case "id":
			newSetting = new IdSetting(props);
			break;

		case "boolean":
			newSetting = new BooleanSetting(props);
			break;

		case "ipv4":
			newSetting = new IPv4Setting(props);
			break;

		case "mac":
			newSetting = new MacSetting(props);
			break;

		case "int":
			newSetting = new IntSetting(props);
			break;

		case "int64":
			newSetting = new Int64Setting(props);
			break;

		case "double":
			newSetting = new DoubleSetting(props);
			break;

		case "device":
			newSetting = new DeviceSetting(props);
			break;

		case "output":
			newSetting = new OutputSetting(props);
			break;

		case "structList":
			newSetting = new StructListSetting(props, propEntry.structList);
			break;

		default:
			newSetting = new Setting(props);
	}

	if ( ! noFindDefault ) { newSetting.findBestDefault(); }

	return newSetting;
}

//-----------------------------------------------------------------------------
// Setting.prototype.checkValidObj
//-----------------------------------------------------------------------------
// Throw an exception if the object was not properly created.
// callerName: The name of the calling function if it won't otherwise be found.
//-----------------------------------------------------------------------------
Setting.prototype.checkValidObj = function(callerName) {
	if (callerName == undefined) { callerName = Setting.prototype.checkValidObj.caller.name; }
	if ( ! this.valid ) {
		throw callerName + " method attempted on invalid Setting object.";
	}
}

//-----------------------------------------------------------------------------
// Setting.static_inputIsEmpty
//-----------------------------------------------------------------------------
// Static function to determine whether an input is empty/unselected.
//-----------------------------------------------------------------------------
Setting.static_inputIsEmpty = function(inputObj) {
	return (inputObj.val() == "" || inputObj.val() == Setting.emptyVal);
}

//-----------------------------------------------------------------------------
// Setting.prototype.inputIsEmpty
//-----------------------------------------------------------------------------
// Return true if the value of the associated input element is empty.
//-----------------------------------------------------------------------------
Setting.prototype.inputIsEmpty = function() {
	return ( this.getInputVal() == "" || this.getInputVal() == Setting.emptyVal );
}


//-----------------------------------------------------------------------------
// Setting.prototype.checkValidInput
//-----------------------------------------------------------------------------
// Throw an exception if the setting doesn't reference an input.
// callerName: The name of the calling function if it won't otherwise be found.
//-----------------------------------------------------------------------------
Setting.prototype.checkValidInput = function(callerName) {
	if (callerName == undefined) {
		callerName = Setting.prototype.checkValidObj.caller.name;
	}
	if ( ! this.input ) {
		throw callerName + " method attempted on a " + this.dataType +
			" setting object without an input.";
	}
}

//-----------------------------------------------------------------------------
// Setting.prototype.getPath
//-----------------------------------------------------------------------------
// Substitute the segment name and channel name for the template names in
// the config file path.
// segmentName: Required if the segment does not exist yet.
//-----------------------------------------------------------------------------
Setting.prototype.getPath = function(segmentName) {
	if ( this.segment == undefined ) {
		if ( segmentName == undefined ) {
			throw "getPath called on " + this.templatePath +
				" setting without existing segment or specifying segment name.";
		}
	}
	else {
		segmentName = this.segment.id;
	}

	return this.templatePath.replace(Setting.channelNameTemplate, this.channelName)
		.replace(Setting.segmentNameTemplate, segmentName);
}

//-----------------------------------------------------------------------------
// Setting.prototype.getInputName
//-----------------------------------------------------------------------------
// Substitute __ for periods in the template config file path of the setting.
//-----------------------------------------------------------------------------
Setting.prototype.getInputName = function() {
	return this.templatePath.replace(/\./g, "__");
}

//-----------------------------------------------------------------------------
// Setting.prototype.findBestDefault
//-----------------------------------------------------------------------------
// Determine whether an existing segment has an existing value, or
// whether there's a global value; if neither, use the existing default.
//-----------------------------------------------------------------------------
Setting.prototype.findBestDefault = function() {
	var def = null;
	
	if (this.segment) {
		if (this.segment.settings[this.getPath()] != undefined) {
			def = this.segment.settings[this.getPath()];
			msg.debug("Using existing value of '" + this.defaultVal + "'.");
		}
		else {
			msg.debug("No existing value for setting '" + this.getPath() + "'.");
		}
	}
	else {
		// Look for global settings that may already be in place.
		var globalPath = this.templatePath.replace(Setting.channelNameTemplate, this.channelName);
		if ( ! Setting.globalRegex.exec(globalPath) ) {

			// If substituting only the channel name gets the complete path, it's global.
			msg.debug("Checking for global setting " + globalPath);

			var params = [ new xmlrpcval(globalPath, "string") ];
			var exists = callXmlrpcMethodBase("setting.exists", params).scalarVal();
			msg.debug(exists);
			if (exists) {
				msg.debug("Setting " + globalPath + " found on the CE, retrieving.");
				var settingVal = callXmlrpcMethodBase("setting.getAutoType", params);

				if ( settingVal ) {
					msg.debug("Successfully retrieved " + globalPath + " from the CE.");
					def = xmlrpcVal2JS(settingVal);
				}
			}
			else {
				msg.debug("Setting " + globalPath + " not found on the CE, using default.");
			}
		}
	}
	
	if ( def != null ) {
		if ( this.dataFormat != "hex" ) {
			this.defaultVal = def.toString();
		}
		else {
			if ( this.isArray ) {
				this.defaultVal = (def.map(function (x) {return '0x' + x.toString(16);})).toString();
			}
			else {
				this.defaultVal = '0x' + def.toString(16);
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Setting.baseMaskInput
//-----------------------------------------------------------------------------
// Return true if input character is acceptable.
// event: A reference to the event that caused the invocation.
//-----------------------------------------------------------------------------
Setting.baseMaskInput = function(event) {

	var ret = { allowed: false, fieldRef: undefined, triggerChar: undefined };

	// Allow cursor keys, home/end, backspace.
	if ( event.which == 0 || event.which == 8) { ret.allowed = true; }
	else {
		ret.fieldRef = $(event.currentTarget);
		ret.triggerChar = String.fromCharCode(event.which);

		// Commas are array separators
		if ( ret.fieldRef.get(0).settingRef.isArray && ret.triggerChar == "," ) {
			ret.fieldRef.data('lastGoodVal', ret.fieldRef.val());
			ret.allowed = true;
		}
	}

	return ret;
}

//-----------------------------------------------------------------------------
// Setting.prototype.buildSelection
//-----------------------------------------------------------------------------
// Create a select control using the setting's accepted values.
// addEmpty: If true, add an empty value to the select.
//-----------------------------------------------------------------------------
Setting.prototype.buildSelection = function(addEmpty) {
	var selector = $("<select class='segmentPropertiesField'></select>");
	if (addEmpty) { selector.append("<option value='" + Setting.emptyVal + "'> </option>"); }

	if ( this.ethernetInterfaces == true ) {
		if ( dllMgr.dlls["devEthernet"].callXmlrpcMethod("load") != undefined ) {
			var ret = callXmlrpcMethodBase("devEthernet.listAllInterfaces");
			for ( i = 0; i < ret.arraySize(); ++i ) {
				var arrayItem = ret.arrayMem(i).scalarVal();
				selector.append("<option value='" + arrayItem + "'>" + arrayItem + "</option>");
			}
		}
	}
	else if ( this.hasOwnProperty("acceptedValues") ) {
		for ( i = 0; i < this.acceptedValues.length; ++i ) {
			var item = this.acceptedValues[i];
			selector.append("<option value='" + item.value + "'>" + item.prompt + "</option>");
		}
	}

	return selector;
}

//-----------------------------------------------------------------------------
// Setting.prototype.insertNewInput
//-----------------------------------------------------------------------------
// Create a new jQuery element associated with the setting.
// target: The object to append the element to.
// noAppend: If true, do *not* append the element to anything, yet.
//-----------------------------------------------------------------------------
Setting.prototype.insertNewInput = function(target, noAppend) {
	if ( this.input == undefined ) {
		if ( this.hasOwnProperty("acceptedValues") || this.ethernetInterfaces == true ) {
			this.input = this.buildSelection(true);
		}
		else {
			this.input = $("<input type='text' class='segmentPropertiesField'/>");
			this.input.off("keypress");
			// newInput.on("keypress", function(event) { maskInput(event); });
		}
	}

	if ( this.isRequired ) { this.input.attr("required", "required"); }
	this.input.val(this.defaultVal);

	if ( this.input.attr("id") == undefined ) {
		this.input.attr({ "id": this.getInputName(), "name": this.prompt });
	}

	this.input.get(0).settingRef = this;

	if ( noAppend != true ) {
		target.append(this.input);
	}
}

//-----------------------------------------------------------------------------
// Setting.prototype.validate
//-----------------------------------------------------------------------------
// In the base, this is just a dummy function that returns the object.
//-----------------------------------------------------------------------------
Setting.prototype.validate = function() {
	return this;
}

//-----------------------------------------------------------------------------
// Setting.prototype.getInputVal
//-----------------------------------------------------------------------------
// Return the value contained by the associated input element.
// Throw if no input element exists.
//-----------------------------------------------------------------------------
Setting.prototype.getInputVal = function() {
	this.checkValidInput("getInputVal");
	return this.input.val();
}

//-----------------------------------------------------------------------------
// Setting.prototype.needsUpdate
//-----------------------------------------------------------------------------
// Return true if the setting's value needs to be updated on the CE.
// oldVal: The value to test the associated input's value against. Use the
//         default value if unspecified.
//-----------------------------------------------------------------------------
Setting.prototype.needsUpdate = function(oldVal) {
	this.checkValidInput("needsUpdate");

	if ( this.isNew == true || this.segment == undefined ) {
		if ( ! this.inputIsEmpty() ) { return true; }
		return false;
	}

	if ( oldVal == undefined ) { oldVal = this.defaultVal; }

	return (oldVal != this.input.val());
}

//-----------------------------------------------------------------------------
// Setting.prototype.simpleVal2XmlrpcVal
//-----------------------------------------------------------------------------
// Return an xmlrpcval object containing the value of a single setting.
// settingVal: The value to use instead of the one from the associated input.
//-----------------------------------------------------------------------------
Setting.prototype.simpleVal2XmlrpcVal = function(settingVal) {
	if ( settingVal == undefined ) { settingVal = this.getInputVal(); }

	return new xmlrpcval(settingVal, this.dataType);
}

//-----------------------------------------------------------------------------
// Setting.prototype.val2XmlrpcVal
//-----------------------------------------------------------------------------
// Return an xmlrpcval object containing the value of a single setting or
// array of settings.
// settingVal: The value to use instead of the one from the associated input.
//-----------------------------------------------------------------------------
Setting.prototype.val2XmlrpcVal = function(settingVal) {
	if ( settingVal == undefined ) { settingVal = this.getInputVal(); }
	var val;

	if ( this.isArray ) {
		var xmlRpcvalArr = [ ],
			settingValArr = settingVal.toString().split(/\s*\,\s*/);

		for ( i = 0; i < settingValArr.length; ++i ) {
			xmlRpcvalArr.push(this.simpleVal2XmlrpcVal(settingValArr[i]));
		}
		val = new xmlrpcval(xmlRpcvalArr, "array");
	}
	else {
		val = this.simpleVal2XmlrpcVal(settingVal);
	}

	return val;
}

//-----------------------------------------------------------------------------
// Setting.static_getFromCE
//-----------------------------------------------------------------------------
// Return an xmlrpcval object with the retrieved value.
// configPath: REQUIRED. The path to the setting in the config file.
//-----------------------------------------------------------------------------
Setting.static_getFromCE = function(configPath) {
	return callXmlrpcMethodBase("setting.getAutoType",
		[ new xmlrpcval(configPath, "string") ]);
}

//-----------------------------------------------------------------------------
// Setting.prototype.getFromCE
//-----------------------------------------------------------------------------
// Return an xmlrpcval object with the retrieved value.
//-----------------------------------------------------------------------------
Setting.prototype.getFromCE = function() {
	return Setting.staticGetFromCE(this.getPath());
}

//-----------------------------------------------------------------------------
// Setting.static_sendToCE
//-----------------------------------------------------------------------------
// Send the setting's value to the CE using XML-RPC.
// configPath: REQUIRED. The path to the setting in the config file.
// settingVal: REQUIRED. The scalar value to set OR an xmlrpcval object.
// settingType: Required if settingVal is not an xmlrpcval. The type of xmlrpcval to create.
//-----------------------------------------------------------------------------
Setting.static_sendToCE = function(configPath, settingVal, settingType) {
	var xval = ( settingVal instanceof xmlrpcval )? settingVal : new xmlrpcval(settingVal, settingType);

	// msg.dumpObj(xval, "XML-RPC", "", 5);

	return callXmlrpcMethodBase("setting.setAutoType",
		[ new xmlrpcval(configPath, "string"),
		xval ] );
}

//-----------------------------------------------------------------------------
// Setting.prototype.sendToCE
//-----------------------------------------------------------------------------
// Send the setting's value to the CE using XML-RPC.
// settingVal: A value to use other than the one from the associated input.
//-----------------------------------------------------------------------------
Setting.prototype.sendToCE = function(settingVal) {
	return Setting.static_sendToCE(this.getPath(), this.val2XmlrpcVal(settingVal));
}

//-----------------------------------------------------------------------------
// Setting.static_deleteFromCE
//-----------------------------------------------------------------------------
// Remove the setting from the config file on the CE using XML-RPC.
// configPath: REQUIRED. The path to the setting in the config file.
//-----------------------------------------------------------------------------
Setting.static_deleteFromCE = function(configPath) {
	return callXmlrpcMethodBase("setting.remove", [ new xmlrpcval(configPath, "string") ]);
}

//-----------------------------------------------------------------------------
// Setting.prototype.deleteFromCE
//-----------------------------------------------------------------------------
// Remove the setting from the config file on the CE using XML-RPC.
//-----------------------------------------------------------------------------
Setting.prototype.deleteFromCE = function() {
	return Setting.static_deleteFromCE(this.getPath());
}

//-----------------------------------------------------------------------------
// Setting.prototype.sendIfNeeded
//-----------------------------------------------------------------------------
// Transmit the setting's value to the CE if it's new or different.
// newSegmentName: Required if the segment doesn't already exist.
//-----------------------------------------------------------------------------
Setting.prototype.sendIfNeeded = function(newSegmentName) {
	msg.debug("Running Setting.prototype.sendIfNeeded(" + newSegmentName + ")");

	if (this.needsUpdate()) {
		if ( this.segment && ((! this.inputIsEmpty() && this.setMethod) ||
			(this.inputIsEmpty() && this.unsetMethod))) {
			var methodName, params;
			if ( ! this.inputIsEmpty() ) {
				// Existing segments with a setMethod
				methodName = this.setMethod;
				params = [ this.val2XmlrpcVal() ];
				msg.debug("Changing setting with " + methodName + "(" + this.getInputVal() + ")");
			}
			else {
				// Existing segments with an unsetMethod
				methodName = this.unsetMethod;
				params = [];
				msg.debug("Deleting setting with " + methodName + "()");
			}

			ret = this.segment.callXmlrpcMethod(methodName, params);
		}
		else {
			if ( ! this.inputIsEmpty() ) {
				// Either new segments or existing ones without a setMethod
				msg.debug("Changing setting " + this.getPath(newSegmentName) + " only in config file.");
				ret = Setting.static_sendToCE(this.getPath(newSegmentName), this.val2XmlrpcVal());
			}
			else if (this.segment) {
				// Existing segments without an unsetMethod.
				// For new segments, the value is already unset.
				msg.debug("Deleting setting " + this.getPath() + " only in config file.");
				ret = this.deleteFromCE();
			}
		}

		if ( ! ret ) {
			$("<div>The CE did not permit changing the setting<br/> <b>" + this.getPath(newSegmentName) +
				"</b><br/><br/>Please see the <b>Log</b> tab for details.</div>")
				.dialog({
					autoOpen: true,
					modal: true,
					minWidth: 350,
					title: "Setting Rejected by CE",
					buttons: [ { text: "Acknowledge", click: function() { $( this ).dialog( "close" ); } } ]
				});
		}
	}
}

//=============================================================================
// StringSetting
//=============================================================================
function StringSetting(props) {
	if ( props.dataType == undefined) { props.dataType = "string"; }

	Setting.call(this, props);
}

StringSetting.prototype = Object.create(Setting.prototype);

//-----------------------------------------------------------------------------
// StringSetting.prototype.simpleVal2XmlrpcVal
//-----------------------------------------------------------------------------
// Return an xmlrpcval object containing the value of a single setting.
// settingVal: The value to use instead of the one from the associated input.
//-----------------------------------------------------------------------------
StringSetting.prototype.simpleVal2XmlrpcVal = function(settingVal) {
	if ( settingVal == undefined ) { settingVal = this.getInputVal(); }

	return new xmlrpcval(settingVal, "string");
}

//=============================================================================
// BooleanSetting
//=============================================================================
function BooleanSetting(props) {
	if ( props.dataType == undefined) { props.dataType = "boolean"; }
	Setting.call(this, props);
}

BooleanSetting.prototype = Object.create(Setting.prototype);

//-----------------------------------------------------------------------------
// BooleanSetting.prototype.getInputVal
//-----------------------------------------------------------------------------
// Return true if the associated checkbox element is checked.
// Throw if no input element exists.
//-----------------------------------------------------------------------------
BooleanSetting.prototype.getInputVal = function() {
	this.checkValidInput("getInputVal");
	return this.input.is(':checked');
}

//-----------------------------------------------------------------------------
// BooleanSetting.prototype.inputIsEmpty
//-----------------------------------------------------------------------------
// Return true if the associated checkbox element is unchanged.
//-----------------------------------------------------------------------------
BooleanSetting.prototype.inputIsEmpty = function() {
	return ( ! this.needsUpdate() );
}

BooleanSetting.prototype.insertNewInput = function(target) {
	this.input = $("<input type='checkbox'/>");
	if ( String(this.defaultVal).match(/true/i) ) { this.input.prop("checked", "checked"); }
	Setting.prototype.insertNewInput.call(this, target);
}

//-----------------------------------------------------------------------------
// BooleanSetting.prototype.needsUpdate
//-----------------------------------------------------------------------------
// Return true if the setting's value needs to be updated on the CE.
// oldVal: The value to test the associated input's value against. Use the
//         default value if unspecified.
//-----------------------------------------------------------------------------
BooleanSetting.prototype.needsUpdate = function(oldVal) {
	var doUpdate = false;
	if ( this.isNew == true || this.segment == undefined ) {
		doUpdate = true;
	}
	else {
		if ( oldVal == undefined ) { oldVal = this.defaultVal; }

		doUpdate = (oldVal != this.getInputVal() && boolStr2Int(oldVal) != this.getInputVal());
	}

	return doUpdate;
}

//-----------------------------------------------------------------------------
// BooleanSetting.prototype.simpleVal2XmlrpcVal
//-----------------------------------------------------------------------------
// Return an xmlrpcval object containing the value of a single setting.
// settingVal: The value to use instead of the one from the associated input.
//-----------------------------------------------------------------------------
BooleanSetting.prototype.simpleVal2XmlrpcVal = function(settingVal) {
	if ( settingVal == undefined ) { settingVal = this.getInputVal(); }

	return new xmlrpcval(settingVal, "boolean");
}

//=============================================================================
// IdSetting
//=============================================================================
function IdSetting(props) {
	if ( props.dataType == undefined) { props.dataType = "id"; }
	StringSetting.call(this, props);
}

IdSetting.prototype = Object.create(StringSetting.prototype);

IdSetting.prototype.validate = function(testVal) {
	if ( testVal == undefined ) { testVal = this.getInputVal(); }

	if ( ! testVal.match(/^[a-z\*][\-a-z0-9_\*]*$/i)) {
		throw "<span class='segFormErr'>" + this.getPath() +
			"</span> must start with a letter (a-z or A-Z) and the remainder can contain only letters, numbers, dashes (-), or underscores (_).";
	}

	return this;
}

IdSetting.maskInput = function(event) {
	var baseMask = Setting.baseMaskInput(event);

	if ( ! baseMask.allowed ) {
		if ( ! baseMask.triggerChar.match(/[a-z0-9_\*]/i) ) { event.preventDefault(); }
		else { baseMask.fieldRef.data('lastGoodVal', baseMask.fieldRef.val()); }
	}
}

IdSetting.prototype.getPath = function() {
	if (this.segment == undefined) {
		return this.templatePath;
	}

	return Setting.prototype.getPath.call(this);
}

IdSetting.prototype.insertNewInput = function(target) {
	Setting.prototype.insertNewInput.call(this, target);

	if ( this.segment ) {
		this.input.prop("disabled", true);
	}
	else {
		this.input.removeProp("disabled");
		this.input.on("keypress", function(event) { IdSetting.maskInput(event); });
	}
}

//=============================================================================
// IPv4Setting
//=============================================================================
function IPv4Setting(props) {
	if ( props.dataType == undefined) { props.dataType = "ipv4"; }
	StringSetting.call(this, props);
}

IPv4Setting.prototype = Object.create(StringSetting.prototype);

IPv4Setting.prototype.validate = function(testVal) {
	if ( testVal == undefined ) { testVal = this.getInputVal()(); }

	var IPv4_OK = true;
	if ( ! testVal.match(/^([0-9]{1,3}\.){3}[0-9]{1,3}$/) ) {
		IPv4_OK = false;
	}
	else {
		var octets = testVal.split(".");
		$.each(octets, function(idx, val) { if ( val > 255 ) { IPv4_OK = false; } });
	}

	if ( ! IPv4_OK ) {
		throw "<span class='segFormErr'>" + this.input.attr("name") + "</span> must be an IPv4 address in the form X.X.X.X, where X is a decimal integer 0-255.";
	}

	return this;
}

IPv4Setting.maskInput = function(event) {
	var baseMask = Setting.baseMaskInput(event);

	if ( ! baseMask.allowed ) {
		if ( ! baseMask.triggerChar.match(/[0-9\.]/) ) { event.preventDefault(); }
		else { baseMask.fieldRef.data('lastGoodVal', baseMask.fieldRef.val()); }
	}
}

IPv4Setting.prototype.insertNewInput = function(target) {
	Setting.prototype.insertNewInput.call(this, target);
	this.input.on("keypress", function(event) { IPv4Setting.maskInput(event); });
}

//=============================================================================
// MacSetting
//=============================================================================
function MacSetting(props) {
	if ( props.dataType == undefined) { props.dataType = "mac"; }
	StringSetting.call(this, props);
}

MacSetting.prototype = Object.create(StringSetting.prototype);

MacSetting.prototype.validate = function(testVal) {
	if ( testVal == undefined ) { testVal = this.getInputVal()(); }

	var MAC_OK = true;
	if ( ! testVal.match(/^([0-9a-f]{1,2}\:)+[0-9a-f]{1,2}$/i) ) {
		MAC_OK = false;
	}
	else {
		var octets = testVal.split(":");
		$.each(octets, function(idx, val) {
			if ( parseInt(val, 16) > 255 ) MAC_OK = false;
		});
	}

	if ( ! MAC_OK ) {
		throw "<span class='segFormErr'>" + this.input.attr("name") + "</span> must be in the form XY:XY:XY:XY:XY:XY, where XY is a hexidecimal integer 00-FF.";
	}

	return this;
}

MacSetting.maskInput = function(event) {
	var baseMask = Setting.baseMaskInput(event);

	if ( ! baseMask.allowed ) {
		if ( ! baseMask.triggerChar.match(/[0-9a-f\:]/i) ) { event.preventDefault(); }
		else { baseMask.fieldRef.data('lastGoodVal', baseMask.fieldRef.val()); }
	}
}

MacSetting.prototype.insertNewInput = function(target) {
	Setting.prototype.insertNewInput.call(this, target);
	this.input.on("keypress", function(event) { MacSetting.maskInput(event); });
}

//=============================================================================
// NumberSetting
//=============================================================================
function NumberSetting(props) {
	Setting.call(this, props);
}

NumberSetting.prototype = Object.create(Setting.prototype);

NumberSetting.prototype.validate = function(testVal) {
	var checkVal = new Number(this.input.val().toString().replace(/l/i, ""));
	if ( this.min != undefined && checkVal < this.min ) {
		throw "<span class='segFormErr'>" + this.input.attr("name") + "</span> has a minimum value of " + this.min + ".";
	}
	else if ( this.max != undefined && checkVal > this.max ) {
		throw "<span class='segFormErr'>" + this.input.attr("name") + "</span> has a maximum value of " + this.max + ".";
	}
}

NumberSetting.prototype.simpleVal2XmlrpcVal = function(settingVal) {
	if ( settingVal == undefined ) { settingVal = this.getInputVal(); }

	return new xmlrpcval(Number(settingVal), this.dataType.replace(/int64/i, "i8"));
}

//=============================================================================
// IntSetting
//=============================================================================
function IntSetting(props) {
	if ( props.dataType == undefined) { props.dataType = "int"; }
	NumberSetting.call(this, props);
}

IntSetting.prototype = Object.create(NumberSetting.prototype);

IntSetting.prototype.validate = function(testVal) {
	if ( testVal == undefined ) { testVal = this.getInputVal()(); }

	if ( ! testVal.match(/^(\-?[0-9]+|0x[0-9a-f]+)$/i)  ) {
		throw "<span class='segFormErr'>" + this.input.attr("name") + "</span> must be a valid decimal or hexidecimal (prefaced with 0x) integer.";
	}

	NumberSetting.prototype.validate.call(this, testVal);

	return this;
}

IntSetting.maskInput = function(event) {
	var baseMask = Setting.baseMaskInput(event);

	if ( ! baseMask.allowed ) {
		if ( ! baseMask.triggerChar.match(/[x0-9a-f\-]/i) ) { event.preventDefault(); }
		else { baseMask.fieldRef.data('lastGoodVal', baseMask.fieldRef.val()); }
	}
}

IntSetting.prototype.insertNewInput = function(target) {
	Setting.prototype.insertNewInput.call(this, target);
	this.input.on("keypress", function(event) { IntSetting.maskInput(event); });
}

//=============================================================================
// Int64Setting
//=============================================================================
function Int64Setting(props) {
	if ( props.dataType == undefined) { props.dataType = "int64"; }
	NumberSetting.call(this, props);
}

Int64Setting.prototype = Object.create(NumberSetting.prototype);

Int64Setting.prototype.validate = function(testVal) {
	if ( testVal == undefined ) { testVal = this.getInputVal()(); }

	if ( ! testVal.match(/^(\-?[0-9]+|0x[0-9a-f]+)l?$/i)  ) {
		throw "<span class='segFormErr'>" + this.input.attr("name") + "</span> must be a valid 64-bit decimal or hexidecimal (prefaced with 0x) integer.";
	}

	NumberSetting.prototype.validate.call(this, testVal);

	return this;
}

Int64Setting.maskInput = function(event) {
	var baseMask = Setting.baseMaskInput(event);

	if ( ! baseMask.allowed ) {
		if ( ! baseMask.triggerChar.match(/[x0-9a-f\-l]/i) ) { event.preventDefault(); }
		else { baseMask.fieldRef.data('lastGoodVal', baseMask.fieldRef.val()); }
	}
}

Int64Setting.prototype.insertNewInput = function(target) {
	Setting.prototype.insertNewInput.call(this, target);
	this.input.on("keypress", function(event) { Int64Setting.maskInput(event); });
}

//=============================================================================
// DoubleSetting
//=============================================================================
function DoubleSetting(props) {
	if ( props.dataType == undefined) { props.dataType = "double"; }
	NumberSetting.call(this, props);
}

DoubleSetting.prototype = Object.create(NumberSetting.prototype);

DoubleSetting.prototype.validate = function(testVal) {
	if ( testVal == undefined ) { testVal = this.getInputVal()(); }

	if ( ! testVal.match(/^\-?([0-9]*\.?[0-9]+|[0-9]+\.?[0-9]*)$/)  ) {
		throw "<span class='segFormErr'>" + this.input.attr("name") + "</span> must be a valid floating point number.";
	}

	NumberSetting.prototype.validate.call(this, testVal);

	return this;
}

DoubleSetting.maskInput = function(event) {
	var baseMask = Setting.baseMaskInput(event);

	if ( ! baseMask.allowed ) {
		if ( ! baseMask.triggerChar.match(/[0-9\-\.]/) ) { event.preventDefault(); }
		else { baseMask.fieldRef.data('lastGoodVal', baseMask.fieldRef.val()); }
	}
}

DoubleSetting.prototype.insertNewInput = function(target) {
	Setting.prototype.insertNewInput.call(this, target);
	this.input.on("keypress", function(event) { DoubleSetting.maskInput(event); });
}

//=============================================================================
// DeviceSetting
//=============================================================================
function DeviceSetting(props) {
	if ( props.dataType == undefined) { props.dataType = "device"; }
	StringSetting.call(this, props);

	if ( devMgr.devices ) {
		var devices = devMgr.devices;
		this.acceptedValues = [];

		for ( i in devices ) {
			if ( devices[i] instanceof Device ) {
				this.acceptedValues.push({ prompt: i, value: i });
			}
		}
	}
}

DeviceSetting.prototype = Object.create(StringSetting.prototype);

//=============================================================================
// OutputSetting
//=============================================================================
function OutputSetting(props) {
	if ( props.dataType == undefined) { props.dataType = "output"; }
	StringSetting.call(this, props);

	if ( this.channelName && channelMgr.channels[this.channelName].segments ) {
		var segments = channelMgr.channels[this.channelName].segments;
		this.acceptedValues = [];

		for ( i in segments ) {
			if ( segments[i] instanceof Segment && i != this.segment ) {
				this.acceptedValues.push({ prompt: i, value: i});
			}
		}
	}
}

OutputSetting.prototype = Object.create(StringSetting.prototype);

//=============================================================================
// StructListSetting
//-----------------------------------------------------------------------------
//
//=============================================================================
function StructListSetting(props, propEntryStructList) {
	if ( props.dataType == undefined) { props.dataType = "structList"; }
	Setting.call(this, props);
	this.dim = 2;
	this.grow = true;

	if ( propEntryStructList.hasOwnProperty("@attributes") ) {
		var attribs = propEntryStructList["@attributes"];
		if (attribs.hasOwnProperty("dimensions")) { this.dim = attribs.dimensions; }
		if (attribs.hasOwnProperty("grow")) { this.grow = boolStr2Int(propEntryStructList["@attributes"].grow); }
		if (attribs.hasOwnProperty("sort")) { this.sort = attribs.sort; }
	}

	this.columns = [];
	this.desc += "<br/><dl>";

	for (var i = 0; i < propEntryStructList.setting.length; ++i ) {

		var propSetting = propEntryStructList.setting[i];
		if ( propSetting.hasOwnProperty("type") && propSetting.hasOwnProperty("desc") &&
				propSetting.hasOwnProperty("prompt") ) {
			var	setting = Setting.makeFromProps(this.segment, propSetting,
					this.channelName, this.templatePath, true);

			if (propSetting.desc.hasOwnProperty("@attributes") &&
				propSetting.desc["@attributes"].dup == "true" ) {
				setting.desc = "See above.";
			}

			this.columns.push(setting);

			this.desc += "<dt class='spreadsheetSetting'>" + setting.prompt + "</dt><dd>" + setting.desc + "</dd>";
		}
	}

	this.desc += "</dl>";

	this.columns.sort(function(a, b) {
		return a.order - b.order;
	});
}

StructListSetting.prototype = Object.create(Setting.prototype);

StructListSetting.prototype.addValSheetIfNeeded = function() {
	if ( ! this.hasOwnProperty("valSheet") ) {
		this.valSheet = [];
	}
}

StructListSetting.prototype.findBestDefault = function() {
	if (this.segment) {
		var existingList = this.segment.settings[this.getPath()];

		if (existingList != undefined) {
			msg.debug("Using existing value for setting '" + this.getPath() + "'.");
			this.dim = ($.isArray(existingList[0]))? 2 : 1;
			this.addValSheetIfNeeded();
			for ( var row = 0; row < existingList.length; ++row ) {

				if ( this.dim > 1 ) {
					var valRow = [];
					for ( var col = 0; col < existingList[row].length; ++col ) {
						var newVal = {};
						$.extend(true, newVal, this.columns[col]);
						newVal.input = undefined;
						newVal.defaultVal = existingList[row][col];
						newVal.parentSetting = this;
						newVal.isNew = false;
						valRow.push(newVal);
					}
					this.valSheet.push(valRow);
				}
				else {
					if ( this.valSheet[0] == undefined ) { this.valSheet.push([]) }
					var newVal = {};
					$.extend(true, newVal, this.columns[row]);
					newVal.input = undefined;
					newVal.defaultVal = existingList[row];
					newVal.parentSetting = this;
					newVal.isNew = false;
					this.valSheet[0].push(newVal);
				}
			}
		}
		else {
			msg.debug("No existing value for setting '" + this.getPath() + "'.");
		}
	}
}

StructListSetting.prototype.addEmptySpreadsheetRow = function() {
	var ssFooterRow = $("<tr></tr>").appendTo(this.ssFooter);

	this.addValSheetIfNeeded();
	var valRow = [], ssFooterRow = $("<tr></td>").appendTo(this.ssFooter);

	for ( var col = 0; col < this.columns.length; ++col ) {
		var newVal = {};
		$.extend(true, newVal, this.columns[col]);
		newVal.parentSetting = this;
		newVal.isNew = true;

		var newCell = $("<td></td>").appendTo(ssFooterRow);
		newCell.css("padding", "0px");
		newVal.insertNewInput(newCell);
		valRow.push(newVal);
	}

	this.valSheet.push(valRow);
}

//-----------------------------------------------------------------------------
// StructListSetting.prototype.insertNewInput
//-----------------------------------------------------------------------------
// Create a spreadsheet-like matrix of input elements and add them to the form.
// target: The parent element to append the new elements to.
// noAppend: If true, do not append the new elements.
//-----------------------------------------------------------------------------
StructListSetting.prototype.insertNewInput = function(target, noAppend) {
	// Setting.prototype.insertNewInput.call(this, target);

	this.worksheet = $("<div></div>");
	this.spreadsheet = $("<table id='" + this.getInputName() + "' class='spreadsheetSetting'></table>").appendTo(this.worksheet);
	this.ssHeader = $("<tbody></tbody>").appendTo(this.spreadsheet);
	this.ssHeaderRow = $("<tr></tr>").appendTo(this.ssHeader);
	this.ssBody = $("<tbody></tbody>").appendTo(this.spreadsheet);
	this.ssFooter = $("<tbody></tbody>").appendTo(this.spreadsheet);

	for ( var i = 0; i < this.columns.length; ++i ) {
		this.ssHeaderRow.append("<th style='width: " + 100/this.columns.length +"%'><label for='" + this.columns[i].getInputName() + "'>" + this.columns[i].prompt + "</label></th>");
	}

	if ( this.grow != false ) {
		this.input = $("<input type='button' value='Add Row'/>").appendTo(this.worksheet);
		this.input.button().click( function(event) {
			$(this).get(0).settingRef.addEmptySpreadsheetRow();
		});
		this.input.css("margin-top", "3px");
		this.input.css("font-size", "10px");
	}
	else {
		this.input = $("<input type='hidden'/>").appendTo(this.worksheet);
	}

	this.input.get(0).settingRef = this;
	this.input.attr({ "id": this.getInputName(), "name": this.prompt });

	if ( this.hasOwnProperty("valSheet") ) {
		for ( var row = 0; row < this.valSheet.length; row++ ) {
			var ssBodyRow = $("<tr></tr>").appendTo(this.ssBody);

			for ( var col = 0; col < this.valSheet[row].length; ++col ) {
				var newCell = $("<td></td>").appendTo(ssBodyRow);
				newCell.css("padding", "0px");

				this.valSheet[row][col].insertNewInput(newCell);
			}
		}
	}
	else {
		this.addEmptySpreadsheetRow();
	}

	if ( noAppend != true ) {
		target.append(this.worksheet);
	}
}

//-----------------------------------------------------------------------------
// StructListSetting.prototype.rowInputIsEmpty
//-----------------------------------------------------------------------------
// True if all the values in one valSheet row are empty.
// row: The index of the row to inspect.
//-----------------------------------------------------------------------------
StructListSetting.prototype.rowInputIsEmpty = function(row) {
	for ( var col = 0; col < this.valSheet[row].length; ++col ) {
		if ( ! this.valSheet[row][col].inputIsEmpty() ) { return false; }
	}

	return true;
}

//-----------------------------------------------------------------------------
// StructListSetting.prototype.inputIsEmpty
//-----------------------------------------------------------------------------
// True if all the values in valSheet are empty.
//-----------------------------------------------------------------------------
StructListSetting.prototype.inputIsEmpty = function() {
	for ( var row = 0; row < this.valSheet.length; ++row ) {
		if ( this.rowInputIsEmpty(row) ) { return true; }
	}

	return true;
}

//-----------------------------------------------------------------------------
// StructListSetting.prototype.validateRow
//-----------------------------------------------------------------------------
// Make sure if there's a value in one cell of a row, all the cells
// in that row also have a value.
// row: The index of the row to validate.
//-----------------------------------------------------------------------------
StructListSetting.prototype.validateRow = function(row) {
	var hasVal = false, hasEmpty = false;

	for ( var col = 0; col < this.valSheet[row].length; ++col ) {
		if ( this.valSheet[row][col].inputIsEmpty() ) { hasEmpty = true; }
		else { hasVal = true; }

		if ( hasVal && hasEmpty ) {
			throw "Missing value for " + this.valSheet[row][col].input.attr("name") + " in a row where other values are set.";
		}
	}

	return this;
}

//-----------------------------------------------------------------------------
// StructListSetting.prototype.validate
//-----------------------------------------------------------------------------
// Make sure if there's a value in one cell of any row in the entire valSheet,
// all the cells in that row also have a value.
//-----------------------------------------------------------------------------
StructListSetting.prototype.validate = function() {
	for ( var row = 0; row < this.valSheet.length; ++row ) {
		this.validateRow(row);
	}

	return this;
}

//-----------------------------------------------------------------------------
// StructListSetting.prototype.rowNeedsUpdate
//-----------------------------------------------------------------------------
// Iterate over all the columns in the row and determine if something has
// changed.
// row: The index of the row to inspect.
//-----------------------------------------------------------------------------
StructListSetting.prototype.rowNeedsUpdate = function(row) {
	for ( var col = 0; col < this.valSheet[row].length; ++col ) {
		var cell = this.valSheet[row][col];
		if ( cell.needsUpdate() ) {
			msg.debug("Cell " + row + "," + col + " (" + cell.input.attr("name") +
				") has changed from '" + cell.defaultVal + "' to '" + cell.input.val() + "' and needs updating.");
			return true;
		}
	}

	return false;
}


//-----------------------------------------------------------------------------
// StructListSetting.prototype.needsUpdate
//-----------------------------------------------------------------------------
// Iterate over all rows in valSheet and determine if something has changed.
//-----------------------------------------------------------------------------
StructListSetting.prototype.needsUpdate = function() {
	for ( var row = 0; row < this.valSheet.length; ++row ) {
		if ( this.rowNeedsUpdate(row) ) { return true; }
	}

	return false;
}

//-----------------------------------------------------------------------------
// StructListSetting.prototype.rowVal2XmlrpcVal
//-----------------------------------------------------------------------------
// Convert the values in one row to an array of XML-RPC values.
// row: The index of the row in valSheet.
// settingVal: Override the contained value of the inputs.
//-----------------------------------------------------------------------------
StructListSetting.prototype.rowVal2XmlrpcVal = function(row) {
	var xRowArr = [];

	this.valSheet[row].sort(function(a, b) {
		return a.order - b.order;
	});

	for ( var col = 0; col < this.valSheet[row].length; ++col ) {
		xRowArr.push(this.valSheet[row][col].simpleVal2XmlrpcVal());
	}

	return new xmlrpcval(xRowArr, "array");
}

//-----------------------------------------------------------------------------
// StructListSetting.prototype.val2XmlrpcVal
//-----------------------------------------------------------------------------
// Convert the values in valSheet to a two-dimensional array of XML-RPC values.
// settingVal: Override the contained value of the inputs.
//-----------------------------------------------------------------------------
StructListSetting.prototype.val2XmlrpcVal = function() {
	if ( this.dim > 1 ) {
		var xVarArr = [];

		for ( var row = 0; row < this.valSheet.length; ++row ) {
			if ( ! this.rowInputIsEmpty(row) ) {
				xVarArr.push(this.rowVal2XmlrpcVal(row));
			}
		}

		return new xmlrpcval(xVarArr, "array");
	}

	// one-dimensional only:
	return this.rowVal2XmlrpcVal(0);
}

//-----------------------------------------------------------------------------
// StructListSetting.prototype.sendIfNeeded
//-----------------------------------------------------------------------------
// If there were updates to this setting, sort the rows, wipe the setting
// on the CE, and send a whole new one. May be very disruptive when the
// CE is active.
// newSegmentName: If this is a new segment, passing it's name is required
//                 to substitute into template strings.
//-----------------------------------------------------------------------------
StructListSetting.prototype.sendIfNeeded = function(newSegmentName) {
	if ( ! this.needsUpdate() ) { return; }

	// Sort the valSheet if necessary
	if ( this.hasOwnProperty("sort") && this.dim > 1 ) {
		// Find the primary key column
		var keyCol;

		for ( keyCol = 0; keyCol < this.valSheet[0].length &&
			this.valSheet[0][keyCol].key != 1; ++keyCol ) { }
		msg.debug("Sorting valSheet by column " + keyCol + ".");

		if ( keyCol < this.valSheet[0].length ) {

			if ( this.sort.match(/^asc.*/i) ) {
				this.valSheet.sort(function(a, b) {
					return a[keyCol].getInputVal() - b[keyCol].getInputVal();
				});
			}
			else if ( this.sort.match(/^desc.*/i) ) {
				this.valSheet.sort(function(a, b) {
					return b[keyCol].getInputVal() - a[keyCol].getInputVal();
				});
			}
			else {
				msg.warning("Unrecognized sort order '" + this.sort + "' specified for " +
					this.getPath(newSegmentName) + ".");
			}
		}
		else {
			msg.warning("Unable to find key column to sort valSheet by.");
		}
	}

	// Clear out the existing setting
	if ( this.segment ) {
		if ( this.hasOwnProperty("unsetMethod") ) {
			if ( this.unsetMethod != "DO_NOT_UNSET" ) {
				msg.debug("Preparatory clearing of setting with " + this.unsetMethod + "()");
				this.segment.callXmlrpcMethod(this.unsetMethod, []);
			}
		}
		else {
			msg.debug("Preparatory clearing of setting " + this.getPath() +
				" only in the config file.");
			this.deleteFromCE();
		}
	}

	// Send each row with something in it.
	var ret;
	if ( this.segment && this.setMethod ) {
		for ( var row = 0; row < this.valSheet.length; ++row ) {
			var xrow = this.rowVal2XmlrpcVal(row);

			if ( ! this.rowInputIsEmpty(row) ) {
				msg.debug("Changing setting with " + this.setMethod + "()");
				ret = this.segment.callXmlrpcMethod(this.setMethod, [ xrow ]);
			}
		}
	}
	else {
		msg.debug("Changing setting " + this.getPath(newSegmentName) + " only in config file.");
		var xval = this.val2XmlrpcVal();

		ret = Setting.static_sendToCE(this.getPath(newSegmentName), xval);
	}

	if ( ! ret ) {
		$("<div>The CE did not permit changing the setting<br/> <b>" + this.getPath(newSegmentName) +
			"</b><br/><br/>To review, please examine the Log in the <b>Details</b> tab.</div>")
			.dialog({
				autoOpen: true,
				modal: true,
				minWidth: 350,
				title: "Setting Rejected by CE",
				buttons: [ { text: "Acknowledge", click: function() { $( this ).dialog( "destroy" ); } } ]
			});
	}
}
