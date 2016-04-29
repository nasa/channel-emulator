//=============================================================================
// Channel.js
// Author: Tad Kollar  
// $Id: Channel.js 2411 2013-12-13 15:26:21Z tkollar $
// Copyright (c) 2012, 2013.
//      NASA Glenn Research Center.  All rights reserved.
//      See COPYING file that comes with this distribution.
//=============================================================================

//=============================================================================
// Channel
// Provide an interface to single channel functions on the Channel Emulator
//=============================================================================

//-----------------------------------------------------------------------------
// Channel constructor.
//-----------------------------------------------------------------------------
// channelName: The identifier for the new channel both on the CE and control panel
// doCreate: If true, make a new channel on the CE; if false, assume channel exists
//           on CE and get the segment list.
// tabPanel: Reference to the div object that the channel is associated with.
//-----------------------------------------------------------------------------
function Channel(channelName, doCreate, tabPanel) {
	this.id = channelName;
	this.valid = false;
	this.tabPanel = tabPanel;
	this.updateInterval = Segment.defaultUpdateInterval;
	this.segments = { };

	if ( ! doCreate ) {
		this.valid = this.getSegments();
	}
	else {
		var rpc = new xmlrpcmsg("channel.addChannel", [ new xmlrpcval(this.id, 'string') ]);
		var response = client.send(rpc);

		if ( response.faultCode() ) { msg.xmlrpcError(response); }
		else {
			msg.debug("Successfully created channel named " + channelName + ".");
			this.valid = true;
		}
	}
}

Channel.prototype.toString = function() { return this.id; }

//-----------------------------------------------------------------------------
// Channel.prototype.refresh
//-----------------------------------------------------------------------------
// Reload the list of segments from the CE and repopulate the list in the table.
//-----------------------------------------------------------------------------
Channel.prototype.refresh = function() {
	// Clear the list of segments from the table before reloading.
	this.tabPanel.find("table.segListTable > tbody > tr.segRow").remove();

	this.valid = this.getSegments();
	this.display();
}

//-----------------------------------------------------------------------------
// Channel.prototype.clearSegments
//-----------------------------------------------------------------------------
// Delete the locally-stored list of segments.
//-----------------------------------------------------------------------------
Channel.prototype.clearSegments = function() {
	var segsToDelete = [ ];
	for (var x in this.segments) {
		if ( this.segments[x] instanceof Segment) {
			this.segments[x].clearUpdateTimer();
			segsToDelete.push(x);
		}
	}

	// Delete segments in a separate loop so there's no confusion
	for (var s = 0; s < segsToDelete.length; s++ ) {
		delete this.segments[s];
	}

	this.segments = { };
}

//-----------------------------------------------------------------------------
// Channel.prototype.getSegments
//-----------------------------------------------------------------------------
// Query the CE for a list of segments in this channel.
//-----------------------------------------------------------------------------
Channel.prototype.getSegments = function() {
	var rpc = new xmlrpcmsg("channel.listSegments", [ new xmlrpcval(this.id, 'string') ] );
	var response = client.send(rpc);

	if ( response.faultCode() ) {
		msg.xmlrpcError(response);
		return false;
	}

	var rval = response.value();

	this.clearSegments();

	for ( var i = 0; i < rval.arraySize(); ++i ) {
		var segmentName = rval.arrayMem(i).scalarVal();
		msg.debug("Found segment " + this.id + "." + segmentName + ".");
		this.segments[segmentName] = new Segment(this.id, segmentName);
	}

	this.segments.length = i;
	msg.debug("Found " + i + " segments in channel " + this.id + ".");
	return true;
}

Channel.prototype.displayNewInterval = function() {
	var newInterval = this.tabPanel.find("div.rateSlider").slider("value"),
		intervalSpan = this.tabPanel.find("span.updateInterval");

	if ( newInterval > Segment.maxUpdateInterval - Segment.minUpdateInterval ) {
		intervalSpan.text("Off");
	}
	else {
		intervalSpan.text(newInterval + "ms");
	}

	return newInterval;
}

Channel.prototype.resetSegmentTimers = function() {
	this.updateInterval = this.displayNewInterval();

	for (seg in this.segments) {
		if ( this.segments[seg] instanceof Segment) { this.segments[seg].setUpdateTimer(this.updateInterval); }
	}
}

//-----------------------------------------------------------------------------
// Channel.prototype.display
//-----------------------------------------------------------------------------
// Nuke any current display and recreate the tab panel for this channel.
//-----------------------------------------------------------------------------
Channel.prototype.display = function() {
	var channel = this;
	this.tabPanel.html("");
	var newDiv = $('.tabTemplate').clone(false).appendTo(this.tabPanel);
	newDiv.removeClass('tabTemplate');
	newDiv.attr('id', this.id + 'Tab');
	// newDiv.find("table.segListTable > thead > tr > th").append("Channel " + this.id + " Segments");
	newDiv.find("table.segListTable > tbody:last")
		.append("<tr><th>Segment Identifier</th><th>Module Type</th><th class='perf'>Performance Metrics</th><th>Available Actions</th></tr>");

	for (seg in this.segments) {
		if ( this.segments[seg] instanceof Segment) { this.segments[seg].display(); }
	}

	var tabActionsCell = $("<td colspan='4'></td>").appendTo(newDiv.find("table.segListTable > thead > tr")),
		tabActionsTable = $("<table class='tabActions'></table>").appendTo(tabActionsCell),
		tabActionsRow = $("<tr></tr>").appendTo(tabActionsTable);

	$("<td class='newSegmentCell'></td>").appendTo(tabActionsRow);
	$("<td class='channelTemplateCell'>Templates Go Here</td>").appendTo(tabActionsRow);

	$("<button class='activateChannel'>Activate All</button>").appendTo($("<td></td>").appendTo(tabActionsRow))
		.button()
		.click(function() {
			channelMgr.channels[channel.id].activate();
		});

	$("<button class='pauseChannel'>Pause All</button>").appendTo($("<td></td>").appendTo(tabActionsRow))
		.button()
		.click(function() {
			channelMgr.channels[channel.id].pause();
		});


	var sliderTopCell = $("<td></td>").appendTo(tabActionsRow),
		sliderTable = $("<table class='sliderContainer'></table>").appendTo(sliderTopCell),
		sliderTopRow = $("<tr></tr>").appendTo(sliderTable),
		sliderCell = $("<td class='sliderCell'></td>").appendTo(sliderTopRow),
		sliderBottomRow = $("<tr></tr>").appendTo(sliderTable),
		sliderLabel = $("<td><span class='updateIntervalLabel'>Update Interval:</span> <span class='updateInterval'>" +
			this.updateInterval + "ms</span></td>").appendTo(sliderBottomRow);

	$("<div class='rateSlider' style='width: 150px'></div>").appendTo(sliderCell)
		.slider({
			orientation: "horizontal",
			min: Segment.minUpdateInterval,
			max: Segment.maxUpdateInterval,
			value: this.updateInterval,
			slide: function() {
				channelMgr.channels[channel.id].displayNewInterval();
			},
			change: function() {
				channelMgr.channels[channel.id].resetSegmentTimers();
			}
		});

	$("<button class='removeChannel'>Remove Channel</button>").appendTo($("<td></td>").appendTo(tabActionsRow))
		.button()
		.click(function() {
			channelMgr.removeChannel(channel.id);
		});

	newDiv.css('visibility', 'visible');

	dllMgr.displayAvailableSegments(this.id);
	dllMgr.displayAvailableMacros(this.id);
}

//-----------------------------------------------------------------------------
// Channel.prototype.destroy
//-----------------------------------------------------------------------------
// Remove this channel completely from the CE (and the GUI).
//-----------------------------------------------------------------------------
Channel.prototype.destroy = function() {
	var val = callXmlrpcMethodBase("channel.removeChannel", [new xmlrpcval(this.id, 'string')]);

	if ( val ) {
		msg.debug("Successfully deleted channel named " + this.id + ".");

		this.valid = false;
	}
	else {
		msg.error("Channel " + this.id + " apparently not deleted by CE.");
	}
}

//-----------------------------------------------------------------------------
// Channel.prototype.pause
//-----------------------------------------------------------------------------
// Stop traffic in every segment managed by the channel.
//-----------------------------------------------------------------------------
Channel.prototype.pause = function() {
	var val = callXmlrpcMethodBase("channel.pauseChannel", [new xmlrpcval(this.id, 'string')]);

	if ( val ) {
		msg.debug("Successfully paused channel named " + this.id + ".");
	}
	else {
		msg.error("Channel " + this.id + " apparently not paused by CE.");
	}
}

//-----------------------------------------------------------------------------
// Channel.prototype.activate
//-----------------------------------------------------------------------------
// Start traffic in every segment managed by the channel.
//-----------------------------------------------------------------------------
Channel.prototype.activate = function() {
	var val = callXmlrpcMethodBase("channel.activateChannel", [new xmlrpcval(this.id, 'string')]);

	if ( val ) {
		msg.debug("Successfully activated channel named " + this.id + ".");
	}
	else {
		msg.error("Channel " + this.id + " apparently not activated by CE.");
	}
}

//-----------------------------------------------------------------------------
// Channel.prototype.addSegment
//-----------------------------------------------------------------------------
// Add a new segment reference to our list and display it. This only affects
// the GUI; the segment must already exist on the CE.
// segRef: Reference to the segment to handle.
//-----------------------------------------------------------------------------
Channel.prototype.addSegment = function(segRef) {
	if ( this.segments == undefined ) { this.segments = { }; }
	this.segments[segRef.id] = segRef;
	segRef.display();
}

//-----------------------------------------------------------------------------
// Channel.prototype.removeSegment
//-----------------------------------------------------------------------------
// Remove a segment completely, from both the CE and the GUI.
// segName: The name of the segment to remove.
//-----------------------------------------------------------------------------
Channel.prototype.removeSegment = function(segName) {
	var channel = this,
		confirmDiv = $("<div></div>")
			.append("<p><span class='ui-icon ui-icon-alert' style='float: left; margin: 0 7px 20px 0;'></span>" +
			"The segment will be permanently deleted and cannot be recovered. Are you sure?</p>");

	confirmDiv.dialog({
		autoOpen: true,
		resizable: false,
		height:160,
		minWidth: 350,
		modal: true,
		title: "Remove segment " + segName + "?",
		buttons: {
			"Remove Segment": function() {
				ret = callXmlrpcMethodBase("channel.removeSegment", [new xmlrpcval(channel.id, 'string'), new xmlrpcval(segName, 'string')])
				if ( ret == null ) {
					$("<div>The segment '" + segName + "' could not be removed.<br/><br/>Please see the message log for details.</div>")
					.dialog({
						autoOpen: true,
						modal: true,
						minWidth: 350,
						title: "Segment Not Removed on CE",
						buttons: [ { text: "Acknowledge", click: function() { $( this ).dialog("destroy"); } } ]
					});
				}
				else {
					msg.debug("Successfully deleted segment named '" + segName + "'.");
					channel.refresh();
				}

				$(this).dialog("destroy");
			},
			Cancel: function() {
				$(this).dialog("destroy");
			}
		}
	});
}