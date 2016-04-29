//=============================================================================
// Segment.js
// Author: Tad Kollar  
// $Id: Segment.js 2411 2013-12-13 15:26:21Z tkollar $
// Copyright (c) 2012.
//      NASA Glenn Research Center.  All rights reserved.
//      See COPYING file that comes with this distribution.
//=============================================================================

//=============================================================================
// Segment
// An interface to individual segment functions on the Channel Emulator.
//=============================================================================

//-----------------------------------------------------------------------------
// Constructor.
//-----------------------------------------------------------------------------
// channelName: The channel to which the Segment does/will belong.
// segmentName: The identifier of the segment.
// noQuery: Don't send a request to the CE for info about the segment (may not exist).
// modName: If noQuery is true, the associated module (DLL) name must be supplied.
//-----------------------------------------------------------------------------
function Segment(channelName, segmentName, noQuery, modName) {
	DLLInstance.call(this, segmentName);
	this.channel = channelName;

	if ( modName == undefined || modName == '' ) {
		if ( ! noQuery ) this.findType();
		else throw "New Segment modName not supplied and noQuery is true, cannot initialize."
	}
	else {
		this.modName = modName;
	}

	this.updateInfoCallback = function(response) {
		arguments.callee.segment.updateInfo(response);
	};
    this.updateInfoCallback.segment = this;

	if ( ! noQuery ) { this.queryCE(); this.updateInfo(); }

 	if ( channelMgr && channelMgr.channels && channelMgr.channels[this.channel] ) {
		this.setUpdateTimer(channelMgr.channels[this.channel].updateInterval);
	}
	else {
		this.setUpdateTimer(Segment.defaultUpdateInterval);
	}
}

// Set up inheritance from DLLInstance.
Segment.prototype = Object.create(DLLInstance.prototype);

Segment.defaultUpdateInterval = 2500; // Amount of time between updates (initial)
Segment.minUpdateInterval = 100; // Least amount of time between updates (on the slider)
Segment.maxUpdateInterval = 5000; // Most amount of time between updates (on the slider)

//-----------------------------------------------------------------------------
// Segment.prototype.setUpdateTimer
//-----------------------------------------------------------------------------
// Create a timer that updates information about the segment every so often.
//-----------------------------------------------------------------------------
Segment.prototype.setUpdateTimer = function(newInterval) {
	this.clearUpdateTimer();

	if ( newInterval < Segment.maxUpdateInterval - Segment.minUpdateInterval) {
		this.timer_id = window.setInterval("channelMgr.channels['" + this.channel + "'].segments['" +
			this.id + "'].updateInfoAsync()", newInterval);
	}
}

//-----------------------------------------------------------------------------
// Segment.prototype.clearUpdateTimer
//-----------------------------------------------------------------------------
// If this segment has a timer referencing it, cancel it.
//-----------------------------------------------------------------------------
Segment.prototype.clearUpdateTimer = function() {
	if ( this.timer_id != undefined ) {
		window.clearInterval(this.timer_id);
		this.timer_id = undefined;
	}
}

//-----------------------------------------------------------------------------
// Segment.prototype.findType
//-----------------------------------------------------------------------------
// Determine the name of the module that spawned the segment so
// that XML-RPC calls can be built with it.
//-----------------------------------------------------------------------------
Segment.prototype.findType = function() {
	var rval = callXmlrpcMethodBase("channel.getSegmentType",
		[new xmlrpcval(this.channel, 'string'), new xmlrpcval(this.id, 'string')]);

	if (rval) { this.modName = rval.scalarVal(); }
}

//-----------------------------------------------------------------------------
// Segment.prototype.callXmlrpcMethod
//-----------------------------------------------------------------------------
// Send an XML-RPC call to the CE, for types of calls that require
// the channel name, segment name, and segment type, which are
// stored in the Segment object. The segment type is prepended
// to the provided methodName call and channel name and segment
// name are added to the start of the params array.
// The returned value is an xmlrpcval object, so its .scalarVal() or
// other method must be used to extract information from it.
// methodName: The XML-RPC method on the CE to invoke.
// params: The arguments to the function, minus the channel and segment names.
// callBackMethod: When asynchronous communication is used.
//-----------------------------------------------------------------------------
Segment.prototype.callXmlrpcMethod = function(methodName, params, callBackMethod) {
	if ( ! params ) params = [];
	params.unshift(new xmlrpcval(this.channel, 'string'), new xmlrpcval(this.id, 'string'));

	return callXmlrpcMethodBase( this.modName + "." + methodName, params, callBackMethod);
}

//-----------------------------------------------------------------------------
// Segment.prototype.getReceivedUnitCount
//-----------------------------------------------------------------------------
// Query the number of received units for the segment.
//-----------------------------------------------------------------------------
Segment.prototype.getReceivedUnitCount = function() {
	return this.callXmlrpcMethod("getReceivedUnitCount").scalarVal();
}

//-----------------------------------------------------------------------------
// Segment.prototype.getReceivedOctetCount
//-----------------------------------------------------------------------------
// Query the number of received octets for the segment.
//-----------------------------------------------------------------------------
Segment.prototype.getReceivedOctetCount = function() {
	return this.callXmlrpcMethod("getReceivedOctetCount").scalarVal();
}

//-----------------------------------------------------------------------------
// Segment.prototype.getQueuedUnitCount
//-----------------------------------------------------------------------------
// Query the number of queued (waiting to be processed) units for the segment.
//-----------------------------------------------------------------------------
Segment.prototype.getQueuedUnitCount = function() {
	return this.callXmlrpcMethod("getQueuedUnitCount").scalarVal();
}

//-----------------------------------------------------------------------------
// Segment.prototype.getQueuedOctetCount
//-----------------------------------------------------------------------------
// Query the number of queued (waiting to be processed) octets for the segment.
//-----------------------------------------------------------------------------
Segment.prototype.getQueuedOctetCount = function() {
	return this.callXmlrpcMethod("getQueuedOctetCount").scalarVal();
}

//-----------------------------------------------------------------------------
// Segment.prototype.findInputs
//-----------------------------------------------------------------------------
// Build a map of the segments that this one receives from. Only
// names are added here, references are added by acquireCeInfo().
//-----------------------------------------------------------------------------
Segment.prototype.findInputs = function(inputStruct) {
	if ( inputStruct === undefined || inputStruct === null )
		inputStruct = this.callSegmentMethod("getInputLinks");

	if ( this.hasOwnProperty("inputs") ) this.prevInputs = this.inputs;

	this.inputs = new Object();
	this.inputs.length = 0;

	// if ( ! inputStruct.structSize() ) alert (this.id + " has no inputs");

	var inputObj;
	while ((inputObj = inputStruct.structEach())) {
		// alert(this.id + " gets input from " + inputObj[0]);
		this.inputs[inputObj[0]] = new Object();
		this.inputs[inputObj[0]].type = inputObj[1].scalarVal();
		this.inputs.length++;
	}
}

//-----------------------------------------------------------------------------
// Segment.prototype.findOutputs
//-----------------------------------------------------------------------------
// Build a map of the segments that this one can send to. Only
// names are added here, references are added by acquireCeInfo().
//-----------------------------------------------------------------------------
Segment.prototype.findOutputs = function(outputStruct) {
	if ( outputStruct === undefined || outputStruct === null )
		outputStruct = this.callSegmentMethod("getOutputLinks");

	if ( this.hasOwnProperty("outputs") ) this.prevOutputs = this.outputs;

	this.outputs = {
		length: 0
	};

	var outputObj;
	while ((outputObj = outputStruct.structEach())) {
		this.outputs[outputObj[0]] = {
			type: outputObj[1].scalarVal()
		};
		this.outputs.length++;
	}
}

//-----------------------------------------------------------------------------
// Segment.prototype.updateInfoAsync
//-----------------------------------------------------------------------------
// Whenever the timer executes and the CE responds with a callback, this is
// the function it calls.
//-----------------------------------------------------------------------------
Segment.prototype.updateInfoAsync = function() {
	this.callXmlrpcMethod("getInfo", null, this.updateInfoCallback);
}

//-----------------------------------------------------------------------------
// Segment.prototype.queryCE
//-----------------------------------------------------------------------------
// Get settings for this segment if it exists on the CE.
//-----------------------------------------------------------------------------
Segment.prototype.queryCE = function() {
	if ( this.existsOnCE() ) {
		this.loadSettings("Channels." + this.channel);
	}
}

//-----------------------------------------------------------------------------
// Segment.prototype.existsOnCE
//-----------------------------------------------------------------------------
// Determine whether this segment exists on the CE.
//-----------------------------------------------------------------------------
Segment.prototype.existsOnCE = function() {
	var val = callXmlrpcMethodBase("channel.segmentExists", [new xmlrpcval(this.channel, 'string'), new xmlrpcval(this.id, 'string')]);

	if ( ! val ) { return false; }

	return val.scalarVal();
}

//-----------------------------------------------------------------------------
// Segment.prototype.startTraffic
//-----------------------------------------------------------------------------
// Activate this segment on the CE.
//-----------------------------------------------------------------------------
Segment.prototype.startTraffic = function() {
	this.callXmlrpcMethod("startup");
}

//-----------------------------------------------------------------------------
// Segment.prototype.stopTraffic
//-----------------------------------------------------------------------------
// Deactivate (but not destroy) this segment on the CE.
//-----------------------------------------------------------------------------
Segment.prototype.stopTraffic = function() {
	this.callXmlrpcMethod("pause");
}


//-----------------------------------------------------------------------------
// Segment.prototype.updateInfo
//-----------------------------------------------------------------------------
// Every segment supports a "getInfo" XML-RPC call that sends a nested
// structure containing identifiers, output links, counters, settings,
// and start/end timestamps of the XML-RPC call. The updateInfo method
// extracts the information from those structures and uses timestamps,
// current counters, and previous counters to compute rate information.
//-----------------------------------------------------------------------------
Segment.prototype.updateInfo = function(response) {
	var infoStruct;

	if ( ! response ) infoStruct = this.callXmlrpcMethod("getInfo");
	else infoStruct = response.value();

	if ( ! infoStruct ) {
		msg.error("Did not receive info from CE about segment " + this.id + ".");
		return;
	}

	// var nameStruct = infoStruct.structMem("names"),
	var inputStruct = infoStruct.structMem("inputs"),
		outputStruct = infoStruct.structMem("outputs"),
		counterStruct = infoStruct.structMem("counters"),
		settingStruct = infoStruct.structMem("settings"),
		timestampStruct = infoStruct.structMem("timestamps");

	// Extract identifiers
	/*
	this.channel = nameStruct.structMem("channel").scalarVal();
	this.id = nameStruct.structMem("segment").scalarVal();
	this.modName = nameStruct.structMem("dll").scalarVal();
	this.device = new String(nameStruct.structMemExists("device")?
		nameStruct.structMem("device").scalarVal() : "");
	*/

	// Store previous timestamps and extract new ones. Timestamps are returns
	// as separated second and microsecond values, so these are added together
	// to produce a useful floating point value in seconds only.
	if ( this.hasOwnProperty("startSecs") ) {
		this.prevStartSecs = this.startSecs;
	}

	this.startSecs = timestampStruct.structMem("callStartSec").scalarVal() +
		timestampStruct.structMem("callStartUsec").scalarVal()/1000000;

	if ( this.hasOwnProperty("prevStartSecs") ) {
		this.deltaStartSecs = this.startSecs - this.prevStartSecs;
	}

	if ( this.hasOwnProperty("endSecs") ) {
		this.prevEndSecs = this.endSecs;
	}
	this.endSecs = timestampStruct.structMem("callEndSec").scalarVal() +
		timestampStruct.structMem("callEndUsec").scalarVal()/1000000;

	if ( this.hasOwnProperty("prevEndSecs") ) {
		this.deltaEndSecs = this.endSecs - this.prevEndSecs;
		this.avgDeltaSecs = ( this.deltaStartSecs + this.deltaEndSecs ) / 2;
	}

	// Extract the inputs and outputs
	this.findInputs(inputStruct);
	this.findOutputs(outputStruct);

	// Extract counters and compute rates
	if ( this.hasOwnProperty("counters") ) this.prevCounters = this.counters;
	this.counters = { };

	var counterObj;
	while ((counterObj = counterStruct.structEach())) {
		this.counters[counterObj[0]] = { };
		this.counters[counterObj[0]].value = counterObj[1].scalarVal();
		if (this.hasOwnProperty("prevCounters") && this.prevCounters.hasOwnProperty(counterObj[0])) {
			this.counters[counterObj[0]].delta = this.counters[counterObj[0]].value -
				this.prevCounters[counterObj[0]].value;

			if ( this.avgDeltaSecs > 0 )
				this.counters[counterObj[0]].rate =
					new Number(this.counters[counterObj[0]].delta / this.avgDeltaSecs);
		}
	}

	// Extract the settings
	if ( this.hasOwnProperty("infoSettings") ) this.prevSettings = this.infoSettings;
	this.infoSettings = { };

	var settingObj;
	while ((settingObj = settingStruct.structEach())) {
		this.infoSettings[settingObj[0]] = {
			value: settingObj[1].scalarVal()
		};
	}

	if ( this.counters.queuedOctets && this.infoSettings.highWaterMark ) {
		var maxWidthPercent = 100;

		var newPercent = (1 - (this.counters.queuedOctets.value / this.infoSettings.highWaterMark.value)) * maxWidthPercent;
		var dangerBarCover = $('#' + this.channel + "Tab img#" + this.id + "Cover");

		dangerBarCover.css("width", newPercent + "%");
	}

	if ( this.countersVisible() ) {
		this.display(true);
	}

	// Update pause/play icon
	if (this.hasOwnProperty("prevSettings") &&
		this.infoSettings.processing.value != this.prevSettings.processing.value ) {
		this.toggleProcessing("init");
	}
}

//-----------------------------------------------------------------------------
// Segment.prototype.isOrphan
//-----------------------------------------------------------------------------
// True if the segment has no input, outputs, or connected devices.
//-----------------------------------------------------------------------------
Segment.prototype.isOrphan = function() {
	if ( ! this.device && ! this.outputs.length && ! this.inputs.length ) {
		return true;
	}

	return false;
}

//-----------------------------------------------------------------------------
// Segment.prototype.countersVisible
//-----------------------------------------------------------------------------
// Return true if the counters for this segment are displayed in the GUI.
//-----------------------------------------------------------------------------
Segment.prototype.countersVisible = function(toggleImage) {
	if ( toggleImage == undefined ) {
		toggleImage = $('#' + this.channel + 'Tab table.segListTable tr#' +
			this.id + " span.counterToggleText img");
	}

	if ( toggleImage.attr("alt") == "View" ) { return false; }

	return true;
}

//-----------------------------------------------------------------------------
// Segment.prototype.toggleCounters
//-----------------------------------------------------------------------------
// Hide/show the counters for this segment in the GUI.
//-----------------------------------------------------------------------------
Segment.prototype.toggleCounters = function() {
	var row = $('#' + this.channel + 'Tab table.segListTable tr#' + this.id),
		counterBody = row.find("tbody.counterBody"),
		toggleImage = row.find("span.counterToggleText img");

	if ( ! this.countersVisible(toggleImage) ) {
		counterBody.css("display", "");
		toggleImage.attr("alt", "Hide");
		toggleImage.attr("src", "zoom-out.png");
	}
	else {
		counterBody.css("display", "none");
		toggleImage.attr("alt", "View");
		toggleImage.attr("src", "zoom-in.png");
	}
}

//-----------------------------------------------------------------------------
// Segment.prototype.toggleProcessing
//-----------------------------------------------------------------------------
// If this segment is active, deactivate it; if inactive, activate it. Update
// the appropriate icon in the GUI.
//-----------------------------------------------------------------------------
Segment.prototype.toggleProcessing = function(state) {
	var anchor = this.startStopAnchor;
	if ( anchor == undefined ) return;
	anchor.off("click");

	var newState;
	if ( state == "init") { newState = (this.infoSettings.processing.value)? "run" : "pause"; }
	else { newState = state; }

	switch (newState) {
		case "run":
			if ( state != "init" ) {
				this.startTraffic();
				this.updateInfo();
			}
			anchor.html("<img src='media-playback-pause.png' alt='Pause'/>");
			anchor.on("click", function(event) {
				channelMgr.channels[$(this).data("ids").channel].segments[$(this).data("ids").segment].toggleProcessing("pause");
			});
			break;
		case "pause":
			if ( state != "init" ) {
				this.stopTraffic();
				this.updateInfo();
			}
			anchor.html("<img src='media-playback-start.png' alt='Run'/>");
			anchor.on("click", function(event) {
				channelMgr.channels[$(this).data("ids").channel].segments[$(this).data("ids").segment].toggleProcessing("run");
			});
			break;
	}
}

//-----------------------------------------------------------------------------
// Segment.prototype.display
//-----------------------------------------------------------------------------
// Display all the information about this segment in a row on its
// Channel tab panel.
//-----------------------------------------------------------------------------
Segment.prototype.display = function(refresh) {
	var row, perfCell, countersCell, countersTbody, actionCell;

	if ( refresh ) {
		row = $('#' + this.channel + 'Tab table.segListTable tr#' + this.id);
		countersCell = row.find("td.segCounters");
		countersTbody = row.find("tbody.counterBody");
		countersTbody.empty();
		actionCell = row.find("td.segActions");
	}
	else {
		row = $("<tr class='segRow' id='" + this.id + "'></tr>").appendTo('#' + this.channel + 'Tab table.segListTable > tbody');

		// Segment name
		var segNameHtml = "<td><a class='jlink' noref onclick='dllMgr.dlls." +
			this.modName + ".propertiesForm(\"" + this.channel + "\", channelMgr.channels." + this.channel +
			".segments." + this.id + ")'>" + this.id + "</a></td>";
		row.append(segNameHtml);

		// Module Type
		row.append("<td>" + this.modName + "</td>");

		// Counters table
		countersCell = $("<td class='segCounters'></td>").appendTo(row);
		var countersTable = $("<table class='segCounters'></table>").appendTo(countersCell),
			countersHead = $("<thead class='counterControl'></thead>").appendTo(countersTable),
			countersHeadRow = $("<tr></tr>").appendTo(countersHead),
			gaugeCell = $("<td></td>").appendTo(countersHeadRow),
			gaugeDiv = $("<div class='gauge'></div>").appendTo(gaugeCell),
			gaugeCover = $("<img class='dangerBarCover' src='1pt_white.gif' id='" + this.id + "Cover'/>").appendTo(gaugeDiv),
			toggleCell = $("<td></td>").appendTo(countersHeadRow),
			toggleAnchor = $("<a class='jlink' noref onclick='channelMgr.channels." + this.channel +
				".segments. " + this.id + ".toggleCounters()'></a>").appendTo(toggleCell),
			toggleSpan = $("<span class='counterToggleText'></span>").appendTo(toggleAnchor),
			toggleImage = $("<img class='counterToggleImage' src='zoom-in.png' alt='View'/>").appendTo(toggleSpan);

		countersTbody = $("<tbody class='counterBody' style='display: none'></tbody>").appendTo(countersTable);

		// Actions
		actionCell = $("<td class='segActions'></td>").appendTo(row);
		this.startStopAnchor = $("<a class='startStop jlink' noref></a>").appendTo(actionCell);
		this.startStopAnchor.data("ids", { channel: this.channel, segment: this.id } )
		actionCell.append("&nbsp;");
		actionCell.append("<a class='jlink' noref onclick='channelMgr.channels." + this.channel +
			".removeSegment(\"" + this.id + "\")'><img src='media-eject.png' alt='Remove'/></a>");

		$('#' + this.channel + 'Tab table.segListTable > tbody:last').append(row);
	}

	// Update the counters
	for (c in this.counters) {
		if ( this.counters[c] instanceof Object ) {
			countersTbody.append("<tr><td class='counterLabel'>" + c + "</td><td class='counterValue'>" +
				$.number(this.counters[c].value, 0) + "</td></tr>");
		}

		if ( c == "receivedOctets" && this.counters[c].rate > 0 ) {
			var rate = this.counters[c].rate, unit = "bps";

			if ( rate >= 1 << 27 ) { rate /= ( 1 << 27); unit = "Gbps"; }
			else if ( rate >= 1 << 17 ) { rate /= ( 1 << 17); unit = "Mbps"; }
			else if ( rate >= 1 << 7 ) { rate /= ( 1 << 7 ); unit = "kbps"; }

			countersTbody.append("<tr><td class='counterLabel'>" + c + " rate</td><td class='counterValue'>" +
				$.number(rate, 2) + " " + unit + "</td></tr>");
		}
	}

	// Run/pause
	if ( ! refresh ) this.toggleProcessing("init");
}
