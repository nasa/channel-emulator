//=============================================================================
// DLLManager.js
// Author: Tad Kollar  
// $Id: DLLManager.js 2016 2013-06-04 14:22:55Z tkollar $
// Copyright (c) 2012.
//      NASA Glenn Research Center.  All rights reserved.
//      See COPYING file that comes with this distribution.
//=============================================================================

//=============================================================================
// DLLManager
// Provide an interface to the modular functions on the Channel Emulator
//=============================================================================

// Constructor.
function DLLManager() {
	this.categories = { };
	this.dlls = { };
	this.knownSegmentModules = this.getAvailableSegments();
	this.knownDeviceModules = this.getAvailableDevices();
	this.knownMacroModules = this.getAvailableMacros();
	this.displayAvailableDevices();
	this.displayAvailableMacros();
}

// Retrieve a list of all non-Device/segment modules from the CE.
DLLManager.prototype.getAvailableSegments = function () {
	msg.debug("Running DLLManager.listAvailableSegments().");

	var rval = callXmlrpcMethodBase("dll.listAvailableSegments");
	if ( ! rval ) { return; }

	var modules = [ ];

	for ( var i = 0; i < rval.arraySize(); ++i ) {
		modules[i] = new SegmentDLL(rval.arrayMem(i).scalarVal());
		this.dlls[modules[i].id] = modules[i];
		msg.debug("Found segment module " +  modules[i].id);
		this.categories[modules[i].properties.cat] = true;
	}

	return modules;
}

// Retrieve a list of all Device modules from the CE.
DLLManager.prototype.getAvailableDevices = function () {
	msg.debug("Running DLLManager.listAvailableDevices().");
	var rval = callXmlrpcMethodBase("dll.listAvailableDevices");
	if ( ! rval ) { return; }

	var modules = [ ];

	for ( var i = 0; i < rval.arraySize(); ++i ) {
		modules[i] = new DeviceDLL(rval.arrayMem(i).scalarVal());
		this.dlls[modules[i].id] = modules[i];
		msg.debug("Found device module " +  modules[i].id);
	}

	return modules;
}

// Retrieve a list of all Macro modules from the CE.
DLLManager.prototype.getAvailableMacros = function () {
	msg.debug("Running DLLManager.listAvailableMacros().");
	var rval = callXmlrpcMethodBase("dll.listAvailableMacros");
	if ( ! rval ) { return; }

	var modules = [ ];

	for ( var i = 0; i < rval.arraySize(); ++i ) {
		modules[i] = new MacroDLL(rval.arrayMem(i).scalarVal());
		this.dlls[modules[i].id] = modules[i];
		msg.debug("Found macro module " +  modules[i].id);
	}

	return modules;
}

// Display a list of all available devices in the browser.
DLLManager.prototype.displayAvailableDevices = function() {
	var devTab = $("#DeviceManagerTab");

	var newSegRow = devTab.find("tfoot > tr");
	newSegRow.html("");
	var menuCell = $("<td></td>").appendTo(newSegRow);
	var outerMenu = $("<ul></ul>").appendTo(menuCell);
	var outerMenuItem = $("<li></li>").appendTo(outerMenu);
	outerMenuItem.append("<a href='#'>Add New Device</a>");
	var menuList = $("<ul id='deviceModules'></ul>").appendTo(outerMenuItem);

	for ( var devIdx in this.knownDeviceModules ) {
		var dev = this.knownDeviceModules[devIdx];
		var displayName = (dev.properties.title)? dev.properties.title.toString() : dev.id;
		menuList.append("<li><a href='javascript:dllMgr.knownDeviceModules[" + devIdx + "].propertiesForm();'>" + displayName + "</a></li>");
	}

	outerMenu.menu();
	newSegRow.append("<td colspan='4'> </td>");
}

// Display a list of all available segments in the browser.
DLLManager.prototype.displayAvailableSegments = function(channelName) {
	var channelTab = $("#" + channelName + "Tab"),
		menuCell = channelTab.find("td.newSegmentCell");

	menuCell.html("");
	var outerMenu = $("<ul></ul>").appendTo(menuCell),
		outerMenuItem = $("<li></li>").appendTo(outerMenu);
	outerMenuItem.append("<a href='#'>Add New Segment</a>");
	var menuList = $("<ul id='categories'></ul>").appendTo(outerMenuItem);

	// Sort segment modules by category
	var catArr = [ ];
	for ( var segIdx in this.knownSegmentModules ) {
		var seg = this.knownSegmentModules[segIdx];
		var cat = seg.properties.cat.toString();

		if (! catArr.hasOwnProperty(cat)) {
			var newCatItem = $("<li></li>").appendTo(menuList).append("<a href='#'>" + cat + "</a>");
			catArr[cat] = $("<ul id='" + cat + "'></ul>").appendTo(newCatItem);
		}

		var displayName = (seg.properties.title)? seg.properties.title.toString() : seg.id;

		catArr[cat].append("<li><a href='javascript:dllMgr.knownSegmentModules[" + segIdx + "].propertiesForm(\"" + channelName + "\");'>" + displayName + "</a></li>");
	}

	outerMenu.menu();
}

DLLManager.prototype.displayAvailableMacros = function(channelName) {
	var macroListCell = $("#" + channelName + "Tab td.channelTemplateCell");
	macroListCell.html("");

	var outerMenu = $("<ul></ul>").appendTo(macroListCell),
		outerMenuItem = $("<li></li>").appendTo(outerMenu);

	outerMenuItem.append("<a href='#'>Apply Template</a>");

	var menuList = $("<ul id='channelTemplates'></ul>").appendTo(outerMenuItem);

	menuList.addMacro = function(idx, method, label) {
		var newItem = $("<li></li>").appendTo(this),
			newAnchor = $("<a href='#'>" + label.toString() + "</a>").appendTo(newItem);

		newAnchor.click(function() {
			dllMgr.knownMacroModules[idx].propertiesForm(channelName, method.toString());
		});
	};

	for ( var modIdx in this.knownMacroModules ) {
		var mod = this.knownMacroModules[modIdx];
		if ( $.isArray(mod.properties.macro) ) {
			for ( var macroIdx = 0; macroIdx < mod.properties.macro.length; ++macroIdx ) {
				menuList.addMacro(modIdx, mod.properties.macro[macroIdx].method,
					mod.properties.macro[macroIdx].title);
			}
		}
		else {
			menuList.addMacro(modIdx, mod.properties.macro.method,
				mod.properties.macro.title);
		}
	}

	outerMenu.menu();
}