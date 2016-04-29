//=============================================================================
// Device.js
// Author: Tad Kollar  
// $Id: Device.js 1995 2013-05-30 17:43:52Z tkollar $
// Copyright (c) 2012.
//      NASA Glenn Research Center.  All rights reserved.
//      See COPYING file that comes with this distribution.
//=============================================================================

//=============================================================================
// Device
// Provide an interface to the device functions on the Channel Emulator
//=============================================================================

// Constructor.
// devName [REQ]: The name of the active device on the CE.
// modName [OPT]: The name of the module the device is an instance of. If not
//          provided, it will be queried from the CE.
function Device(devName, modName, doQuery) {
	this.valid = false;

	if ( modName ) {
		this.modName = modName;
		this.valid = true;
	}
	else {
		var rval = callXmlrpcMethodBase("device.getType", [ new xmlrpcval(devName, 'string') ]);

		if (rval) {
			this.modName = 'dev' + rval.scalarVal();
			this.valid = true;
		}
		else {
			throw "Initialization failed: Cannot find module name for new device '" + devName + "'.";
		}
	}

	DLLInstance.call(this, devName);
	this.reader = { name: "none", channel: "none" };
	this.writer = { name: "none", channel: "none" };

	if ( doQuery ) { this.queryCE(); }
}

Device.prototype = Object.create(DLLInstance.prototype);

// Send an XML-RPC call to the CE, for types of calls that require
// the device name and device type, which are stored in the Device
// object. The Device type is prepended to the provided methodName
// call and the Device name is added to the start of the params array.
// The returned value is an xmlrpcval object, so its .scalarVal() or
// other method must be used to extract information from it.
Device.prototype.callXmlrpcMethod = function(methodName, params) {
	if ( ! params ) params = [];
	params.unshift(new xmlrpcval(this.id, 'string'));

	return callXmlrpcMethodBase(this.modName + "." + methodName, params);
}

Device.prototype.display = function() {
	$('#DeviceManagerTab table.segListTable > tbody.existingSeg').append("<tr class='segRow'><td><a class='jlink' noref onclick='dllMgr.dlls." + this.modName +
		".propertiesForm(undefined, devMgr.devices." + this.id + ")'>" + this.id + "</a></td><td>" + this.modName +
		"</td><td>" + this.reader.channel + "." + this.reader.name +
		"</td><td>" + this.writer.channel + "." + this.writer.name +
		"</td><td><a class='jlink' noref onclick='devMgr.removeDevice(\"" + this.id + "\")'><img src='media-eject.png' alt='Remove'/></a></td></tr>");
}

Device.prototype.existsOnCE = function() {
	var val = callXmlrpcMethodBase("device.exists", [new xmlrpcval(this.id, 'string')]);

	if ( ! val ) { return false; }

	return val.scalarVal();
}

Device.prototype.queryCE = function(doDisplay) {
	if ( this.existsOnCE() ) {
		this.loadSettings("Devices");

		var oldReader = this.reader;
		var readerStruct = this.callXmlrpcMethod('getReaderInfo');
		if ( (this.reader.name = readerStruct.structMem('name').scalarVal()) == '' ) { this.reader.name = 'none'; }
		if ( (this.reader.channel = readerStruct.structMem('channel').scalarVal()) == '' ) { this.reader.channel = 'none'; }

		var oldWriter = this.writer;
		var writerStruct = this.callXmlrpcMethod('getWriterInfo');
		if ( (this.writer.name = writerStruct.structMem('name').scalarVal()) == '' ) { this.writer.name = 'none'; }
		if ( (this.writer.channel = writerStruct.structMem('channel').scalarVal()) == '') { this.writer.channel = 'none'; }

		if ( doDisplay == 1 ) {
			if ( oldReader.name != this.reader.name || oldReader.channel != this.reader.channel ||
				oldWriter.name != this.writer.name || oldWriter.channel != this.writer.channel ) {
				this.display();
			}
		}
		else if ( doDisplay > 1 ) { this.display(); }
	}
}

Device.prototype.destroy = function() {
	var params = [new xmlrpcval(this.id, 'string')];
	var rpc = new xmlrpcmsg(this.modName + ".remove", params);
	var response = client.send(rpc);

	if ( response.faultCode() ) {
		msg.xmlrpcError(response);
		return false;
	}

	msg.debug("Successfully deleted device named " + this.id + ".");
	// var rval = response.value();
	this.valid = false;
	return true;
}