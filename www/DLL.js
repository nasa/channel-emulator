//=============================================================================
// DLL.js
// Author: Tad Kollar  
// $Id: DLL.js 2419 2013-12-13 19:01:43Z tkollar $
// Copyright (c) 2012.
//      NASA Glenn Research Center.  All rights reserved.
//      See COPYING file that comes with this distribution.
//=============================================================================

//=============================================================================
// DLL
// Provide an interface to an individual DLL on the CE.
//=============================================================================


// Constructor.
function DLL(newModName) {
	this.id = newModName;
	this.loadProperties();
	// this.loadSettings();
	// this.loadDesc();
}


// Send an XML-RPC call to the CE, for dll.* calls that require
// the DLL name. The returned value is an xmlrpcval object, so its
// .scalarVal() or other method must be used to extract information from it.
DLL.prototype.callXmlrpcMethod = function(methodName, params) {
	if ( params === undefined ) params = [];
	var nameVal = new xmlrpcval(this.id, 'string');
	params.unshift(nameVal);

	return callXmlrpcMethodBase("dll." + methodName, params);
}

// Retrieve the description of the module from the CE.
DLL.prototype.loadProperties = function() {
	msg.debug("Running DLL.loadProperties().");

	var response = this.callXmlrpcMethod("getPropertiesXML");

	if ( response == null ) { this.properties = null; }
	else {
		var xml = xmlParser.parseFromString(response.scalarVal(), "text/xml");
		this.properties = this.xmlToJson(xml).modinfo;
	}
}

// Replace [+ and +] in text with < and >.
DLL.prototype.parsePseudoHTML = function(rawString) {
	var str = new String(rawString);

	return str.replace(/\[\+/g,"<").replace(/\+\]/g,">");
}

// Retrieve the description of a particular module from the CE.
DLL.prototype.loadDesc = function() {
	msg.debug("Running DLL.loadDesc().");

	var response = this.callXmlrpcMethod("getDescription");

	var newDesc = new String(( response == null )? null : response.scalarVal());
	this.desc = this.parsePseudoHTML(newDesc);
}

DLL.prototype.showInfo = function() {
	alert(this.properties.desc);
}

DLL.prototype.isDevice = function() {
	if ( this.properties == undefined ) { return undefined; }

	if ( this.properties.cat.toString() == "device" ) { return true; }

	return false;
}

DLL.prototype.isMacro = function() {
	if ( this.properties == undefined ) { return undefined; }

	if ( this.properties.cat.toString() == "macro" ) { return true; }

	return false;
}

DLL.prototype.isSegment = function() {
	if ( this.properties == undefined ) { return undefined; }

	if ( ! ( this.isDevice() || this.isMacro() ) ) { return true; }

	return false;
}

// Changes XML to JSON
// Found at http://davidwalsh.name/convert-xml-json
DLL.prototype.xmlToJson = function(xml) {
	// Create the return object
	var obj = {};

	if (xml.nodeType == 1) { // element
		// do attributes
		if (xml.attributes.length > 0) {
			obj["@attributes"] = {};
			for (var j = 0; j < xml.attributes.length; j++) {
				var attribute = xml.attributes.item(j);
				obj["@attributes"][attribute.nodeName] = attribute.value;
			}
		}
	}
	else if (xml.nodeType == 3) { // text
		obj = xml.nodeValue;
	}

	// do children
	if (xml.hasChildNodes()) {
		for (var i = 0; i < xml.childNodes.length; i++) {
			var item = xml.childNodes.item(i);
			var nodeName = item.nodeName;
			if (typeof(obj[nodeName]) == "undefined") {
				obj[nodeName] = this.xmlToJson(item);
			}
			else {
				if (typeof(obj[nodeName].push) == "undefined") {
					var old = obj[nodeName];
					obj[nodeName] = [];
					obj[nodeName].push(old);
				}
				obj[nodeName].push(this.xmlToJson(item));
			}
		}
	}
	obj.toString = function() { return this["#text"]; }

	if (obj.hasOwnProperty("#text")) {
		obj["#text"] = this.parsePseudoHTML(obj["#text"]);
	}

	return obj;
};

DLL.prototype.validate = function() {
	var valid = true, dllRef = this, errLog = "";

	this.spForm.find("input,select").each(function() {
		if ( $(this).get(0).settingRef ) {
			var setting = $(this).get(0).settingRef;

			// Catch missing required values that the browser didn't.
			if ( setting.isRequired && Setting.static_inputIsEmpty($(this)) ) {
				errLog += "<li>Please fill in the '" + setting.prompt + "' field before submitting.</li>";
				valid = false;
			}
			else if ( $(this).val() && $(this).val() != "" ) {
				if ( setting.isArray == true ) {
					// Check each element of the array individually
					var valArray = $(this).val().toString().split(/\s*\,\s*/);
					for ( i = 0; i < valArray.length; ++i ) {
						try {
							setting.validate(valArray[i]);
						}
						catch(errMsg) {
							alert(errMsg);
							valid = false;
						}
					}
				}
				else {
					try {
						setting.validate($(this).val());
					}
					catch(err) {
						errLog += "<li>" + err + "</li>";
						valid = false;
					}
      
					// Check for duplicate identifer
					if (valid && setting instanceof IdSetting && ! dllRef.spForm.segmentExists) {
						if (dllRef.isDevice() ) {
							if ( devMgr.devices.hasOwnProperty($(this).val()) ) {
								errLog += "<li>A device named <b>" + $(this).val() + "</b> already exists in this configuration.</li>";
								valid = false;
							}
						}
						else if (dllRef.isSegment()) {
							if ( channelMgr.channels[dllRef.spForm.channelName].segments.hasOwnProperty($(this).val()) ) {
								errLog += "<li>A segment named <b>" + $(this).val() + "</b> already exists in channel " + dllRef.spForm.channelName + ".</li>";
								valid = false;
							}
						}
					}
				}
			}
		}
	});

	if ( ! valid ) {
		var errWin = $("<div id='errWin' title='Errors in Segment Form'><ul>" + errLog + "</ul></div>");
		errWin.dialog({ width: "500px" });
	}

	return valid;
}

DLL.prototype.addSettingTableRow = function(inputName, prompt, desc, target) {
	var inputRow = $("  <tr class='row'><td><label for='" + inputName +
				"'>" + prompt + "</label></td></tr>").appendTo(target),
		inputCell = $("<td></td>").appendTo(inputRow);

	$("<td>" + desc + "</td>").appendTo(inputRow);

	return inputCell;
}


//-----------------------------------------------------------------------------
// SegmentDLL Constructor.
//-----------------------------------------------------------------------------
function SegmentDLL(newModName) {
	DLL.call(this, newModName);
}

SegmentDLL.prototype = Object.create(DLL.prototype);

// Query the CE and parse all of the possible settings of the module.
SegmentDLL.prototype.loadSettings = function() {
	msg.debug("Running SegmentDLL.loadSettings().");
	var rval = this.callXmlrpcMethod("getSettings");
	if ( rval != null ) {
		this.settings = new Array();

		for ( var i = 0; i < rval.arraySize(); ++i ) {
			this.settings[i] = new Object();
			this.settings[i].prompt = rval.arrayMem(i).structMem('prompt').scalarVal();
			this.settings[i].path = rval.arrayMem(i).structMem('path').scalarVal();
			this.settings[i].type = rval.arrayMem(i).structMem('type').scalarVal();
			this.settings[i].desc = this.parsePseudoHTML(rval.arrayMem(i).structMem('desc').scalarVal());
			this.settings[i].default = rval.arrayMem(i).structMem('default').scalarVal();
			this.settings[i].isRequired = rval.arrayMem(i).structMem('req').scalarVal();
			msg.debug("Discovered setting " +  this.settings[i].prompt);
		}
	}
}

//-----------------------------------------------------------------------------
// SegmentDLL.prototype.getSettingPropertyByPath
//-----------------------------------------------------------------------------
// Iterate over the settings array of a device/segment in properties looking
// for one with the matching path.
// settingName: The path of the setting in template form.
// Return: Reference on successs, undefined on failure.
//-----------------------------------------------------------------------------
SegmentDLL.prototype.getSettingPropertyByPath = function(settingName) {
	for (var i = 0; i < this.properties.config.setting.length; i++ ) {
		if (this.properties.config.setting[i].path.toString() == settingName) {
			return this.properties.config.setting[i];
		}
	}

	return undefined;
}

//-----------------------------------------------------------------------------
// SegmentDLL.prototype.propertiesForm
//-----------------------------------------------------------------------------
// Get info from the user to be able to generate a new device/segment
// channelName: If this is for a segment, it must be set.
// existingSegment: A reference to a Segment/Device that is already on the CE
//-----------------------------------------------------------------------------
SegmentDLL.prototype.propertiesForm = function(channelName, existingSegment) {
	// If this is a non-device module, channelName will have something in it.
	if ( ! channelName ) { channelName = ""; }

	// Create the form.
	this.spForm = $("form#segmentProperties");
	this.spForm.data("dllName", this.id);
	this.spForm.channelName = channelName;
	this.spForm.segmentExists = (existingSegment)? true : false;

	// Add the module description area.
	if ( existingSegment ) { existingSegment.queryCE(); }
	var desc = (existingSegment)? existingSegment.id : "New instance of " + this.id;
	if (this.properties.title) { desc += " (" + this.properties.title.toString() + ")"; }
	$("p#dllDesc").html(this.properties.desc.toString());

	// A table to give structure to the labels/inputs/descs.
	var spTableTail = $("#segmentPropertiesTable > tbody:last");
	spTableTail.html("");

	// The identifier special field.
	var identifier = new IdSetting({
		segment: existingSegment,
		channel: channelName,
		defaultVal: (existingSegment)? existingSegment.id : "",
		templatePath: "identifier",
		isRequired: true,
		isArray: false,
		prompt: "Identifier",
		desc: "What to call this instance of the module. Must start with a letter (a-z or A-Z) and the remainder can contain only letters, numbers, dashes (-), or underscores (_)."
	});

	identifier.insertNewInput(this.addSettingTableRow(identifier.getInputName(),
		identifier.prompt, identifier.desc, spTableTail)	);

	for (var i = 0; this.properties.config.setting != undefined && i < this.properties.config.setting.length; i++ ) {
		var setting = Setting.makeFromProps(existingSegment,
				this.properties.config.setting[i], channelName);

		setting.insertNewInput(this.addSettingTableRow(setting.getInputName(),
				setting.prompt, setting.desc, spTableTail));
	}

	propertiesDialog.dialog("option", "title", desc);
	propertiesDialog.dialog("open");
}

// Send the submitted, validated form inputs to the CE.
SegmentDLL.prototype.applyChangesToCE = function() {
	var channelName = this.spForm.channelName,
		identifier = this.spForm.find("input[id=identifier]").val(),
		segmentExists = this.spForm.segmentExists,
		dllRef = this;

	// Make sure this DLL is loaded on the CE.
	if ( ! segmentExists && this.callXmlrpcMethod("load") == undefined ) {
		alert("Failed to load the " + this.id + " DLL on the CE, cannot continue.");
		return;
	}

	var segRef;

	// Get a reference to/create the client-side segment object.
	if ( this.isDevice() ) {
		if (segmentExists) { segRef = devMgr.devices[identifier]; }
		else {
			Setting.static_sendToCE('Devices.' + identifier + '.devType', this.id.replace(/^dev/, ''), 'string');
			segRef = new Device(identifier, this.id);
		}
	}
	else {
		if (segmentExists) { segRef = channelMgr.channels[channelName].segments[identifier]; }
		else {
			Setting.static_sendToCE('Channels.' + channelName + '.' + identifier + '.dllName', this.id, 'string');
			segRef = new Segment(channelName, identifier, true, this.id);
		}
	}

	// Iterate over the form inputs and send each setting to the CE.
	this.spForm.find("input,select").each(function() {
		var setting = $(this).get(0).settingRef;

		if (setting != undefined && ! (setting instanceof IdSetting) &&
			! setting.hasOwnProperty("parentSetting") ) {
			setting.sendIfNeeded(identifier);
		}
	});

	// Create the instance of the module on the server side.
	if ( ! segmentExists ) {
		msg.debug("Creating new instance of " + this.id + " named " + segRef.id + ".");
		segRef.callXmlrpcMethod('add');

		// Append to appropriate device or channel array if it worked.
		if ( segRef.existsOnCE() ) {
			if ( this.isDevice() ) { devMgr.addDevice(segRef); }
			else {
				segRef.updateInfo();
				channelMgr.channels[channelName].addSegment(segRef);
			}
		}
		else {
			msg.error("The CE failed to add a new instance of the module!");
		}
	}

	// Read the settings back from the CE.
	segRef.queryCE();
}

//-----------------------------------------------------------------------------
// DeviceDLL Constructor.
//-----------------------------------------------------------------------------
function DeviceDLL(newModName) {
	SegmentDLL.call(this, newModName);
}

DeviceDLL.prototype = Object.create(SegmentDLL.prototype);

//-----------------------------------------------------------------------------
// MacroDLL Constructor.
//-----------------------------------------------------------------------------
function MacroDLL(newModName) {
	DLL.call(this, newModName);
}

MacroDLL.prototype = Object.create(DLL.prototype);

//-----------------------------------------------------------------------------
// MacroDLL.prototype.propertiesForm
//-----------------------------------------------------------------------------
// Get info from the user to be able to invoke a macro on the CE, usually
// a template for a new channel.
// methodName: The CE method associated with the macro, for ID purposes here.
//-----------------------------------------------------------------------------
MacroDLL.prototype.propertiesForm = function(channelName, methodName) {
	// Find the right macro.

	var macroProps = undefined;

	if ( $.isArray(this.properties.macro) ) {
		for ( var macroIdx = 0; macroProps == undefined &&
				macroIdx < this.properties.macro.length; ++macroIdx ) {
			if ( this.properties.macro[macroIdx].method.toString() == methodName ) {
				macroProps = this.properties.macro[macroIdx];
			}
		}
	}
	else {
		if ( this.properties.macro.method.toString() == methodName ) {
			macroProps = this.properties.macro[macroIdx];
		}
	}

	if ( macroProps == undefined ) {
		throw "Could not find macro identified with " + methodName;
	}

	// Create the form.
	this.spForm = $("form#segmentProperties");
	this.spForm.data("dllName", this.id);
	this.spForm.channelName = channelName;
	this.spForm.methodName = methodName;

	// Add the module description area.
	var desc = macroProps.title.toString() + " Channel Template";
	$("p#dllDesc").html(macroProps.desc.toString());

	// A table to give structure to the labels/inputs/descs.
	var spTableTail = $("#segmentPropertiesTable > tbody:last");
	spTableTail.html("");

	for (var i = 0; i < macroProps.setting.length; i++ ) {
		var setting = Setting.makeFromProps(undefined, macroProps.setting[i]);
		setting.insertNewInput(this.addSettingTableRow(setting.getInputName(),
			setting.prompt, setting.desc, spTableTail));
	}

	propertiesDialog.dialog("option", "title", desc);
	propertiesDialog.dialog("open");
}

MacroDLL.prototype.applyChangesToCE = function() {
	var identifier = this.spForm.find("select[id=identifier]").val(),
		macro = new Macro(this.id, this.spForm.methodName, this.spForm.channelName);

	if ( this.callXmlrpcMethod("load") == undefined ) {
		alert("Failed to load the " + this.id + " DLL on the CE, cannot continue.");
		return;
	}

	// Iterate over the form inputs and add each setting to the macro
	this.spForm.find("input,select").each(function() {
		var setting = $(this).get(0).settingRef;

		if ( setting != undefined && setting.partOfSettingsStruct == true &&
			! setting.hasOwnProperty("parentSetting") ) {
			macro.addSetting(setting);
		}
	});

	if ( macro.execute() ) {
		channelMgr.addChannel(this.spForm.channelName);
	}

}
