 //=============================================================================
// DeviceManager.js
// Author: Tad Kollar  
// $Id: DeviceManager.js 1995 2013-05-30 17:43:52Z tkollar $
// Copyright (c) 2012.
//      NASA Glenn Research Center.  All rights reserved.
//      See COPYING file that comes with this distribution.
//=============================================================================

//=============================================================================
// DeviceManager
// Aggregate all the devices and present a collective interface to them.
//=============================================================================

// Constructor.
function DeviceManager() {
	this.devices = this.getDevices();

}

// Rebuild the devices list.
DeviceManager.prototype.refresh = function() {
	$('#DeviceManagerTab table.segListTable tbody tr.segRow').remove();
	this.devices = this.getDevices();
}

// Query the CE for a list of devices and return it.
DeviceManager.prototype.getDevices = function () {
	var rval = callXmlrpcMethodBase("device.getList");
	if ( ! rval ) { return; }

	devices = { };

	for ( var i = 0; i < rval.arraySize(); ++i ) {
		var deviceName = rval.arrayMem(i).scalarVal();
		msg.debug("Discovered active device called '" + deviceName + "'.");
		devices[deviceName] = new Device(deviceName, undefined, true);
		devices[deviceName].display();
	}

	return devices;
}

DeviceManager.prototype.deviceExists = function(deviceName) {
	return ( this.devices.hasOwnProperty(deviceName) )? true : false;
}

DeviceManager.prototype.addDevice = function(devRef) {
	devices[devRef.id] = devRef;
	devRef.display();
}

DeviceManager.prototype.removeDevice = function(devName) {
	var confirmDiv = $("<div></div>")
		.append("<p><span class='ui-icon ui-icon-alert' style='float: left; margin: 0 7px 20px 0;'></span>The device will be permanently deleted and cannot be recovered. Are you sure?</p>");

	confirmDiv.dialog({
		autoOpen: true,
		resizable: false,
		height:160,
		minWidth: 350,
		modal: true,
		title: "Remove device " + devName + "?",
		buttons: {
			"Delete Device": function() {
				devMgr.devices[devName].destroy();
				devMgr.refresh();
				$(this).dialog("destroy");
			},
			Cancel: function() {
				$(this).dialog("destroy");
			}
		}
	});
}