//=============================================================================
// ChannelManager.js
// Author: Tad Kollar  
// $Id: ChannelManager.js 2411 2013-12-13 15:26:21Z tkollar $
// Copyright (c) 2012.
//      NASA Glenn Research Center.  All rights reserved.
//      See COPYING file that comes with this distribution.
//=============================================================================

//=============================================================================
// ChannelManager
// Provide the top interface to all channel functions on the Channel Emulator
//=============================================================================

//-----------------------------------------------------------------------------
// Constructor.
//-----------------------------------------------------------------------------
function ChannelManager() {
	this.channels = { };
	this.getChannels();
}

//-----------------------------------------------------------------------------
// ChannelManager.prototype.clearChannels
//-----------------------------------------------------------------------------
// Delete the locally-stored list of channels.
//-----------------------------------------------------------------------------
ChannelManager.prototype.clearChannels = function() {
	var channelsToDelete = [ ];
	for (var x in this.channels) {
		if ( this.channels[x] instanceof Channel) {
			this.channels[x].clearSegments();
			channelsToDelete.push(x);
		}
	}

	// Delete segments in a separate loop so there's no confusion
	for (var c = 0; c < channelsToDelete.length; c++ ) {
		delete this.channels[c];
	}

	this.channels = { };
}

//-----------------------------------------------------------------------------
// ChannelManager.prototype.refresh
//-----------------------------------------------------------------------------
// Rebuild the channels list.
//-----------------------------------------------------------------------------
ChannelManager.prototype.refresh = function() {
	this.getChannels();
	this.display(0);
}

//-----------------------------------------------------------------------------
// ChannelManager.prototype.listChannels
//-----------------------------------------------------------------------------
// Query the CE for a list of channels and return it.
//-----------------------------------------------------------------------------
ChannelManager.prototype.listChannels = function() {
	var rval = callXmlrpcMethodBase("channel.listChannels"),
		channelArr = [];

	for ( var i = 0; i < rval.arraySize(); ++i ) {
		channelArr.push(rval.arrayMem(i).scalarVal());
	}

	return channelArr;
}

//-----------------------------------------------------------------------------
// ChannelManager.prototype.getChannels
//-----------------------------------------------------------------------------
// Using a list of channels from the CE, populate this.channels with new
// Channel objects.
//-----------------------------------------------------------------------------
ChannelManager.prototype.getChannels = function () {
	this.clearChannels();

	var channelArr = this.listChannels();

	for ( var i = 0; i < channelArr.length; ++i ) {
		var channelName = channelArr[i], newPanel = addTab(channelName);
		this.channels[channelName] = new Channel(channelName, false, newPanel);
		this.channels[channelName].display();
	}
}

//-----------------------------------------------------------------------------
// ChannelManager.prototype.isManaged
//-----------------------------------------------------------------------------
// Return true if the named channel can be found in this.channels
//-----------------------------------------------------------------------------
ChannelManager.prototype.isManaged = function(channelName) {
	return this.channels.hasOwnProperty(channelName);
}

//-----------------------------------------------------------------------------
// ChannelManager.prototype.channelExistsOnCE
//-----------------------------------------------------------------------------
// Return true if the named channel can be found in the CE's channel list.
//-----------------------------------------------------------------------------
ChannelManager.prototype.channelExistsOnCE = function(channelName) {
	return ( this.listChannels().indexOf(channelName) > -1 );
}

//-----------------------------------------------------------------------------
// ChannelManager.prototype.addChannel
//-----------------------------------------------------------------------------
// If the named channel is already known, refresh it. If it's unknown, create
// a new Channel object and create it on the CE as well if it doesn't already
// exist.
//-----------------------------------------------------------------------------
ChannelManager.prototype.addChannel = function(channelName) {

	if ( channelName == null || channelName == "" ) {
		throw "ChannelManager.prototype.addChannel called with empty channel name.";
	}

	var channel;
	if ( this.isManaged(channelName) ) {
		// The channel information may just need to be updated.
		msg.info("Updating information for channel " + channelName + ".");
		channel = this.channels[channelName];
		channel.getSegments();
	}
	else {
		msg.info("Creating/loading new channel " + channelName + ".");
		var newPanel = addTab(channelName),
			doCreate = ( ! this.channelExistsOnCE(channelName) );

		channel = new Channel(channelName, doCreate, newPanel);

		if (channel.valid) { this.channels[channelName] = channel; }
		else { return false; }
	}

	channel.display();
	return true;
}

//-----------------------------------------------------------------------------
// ChannelManager.prototype.removeChannel
//-----------------------------------------------------------------------------
// Remove the channel from the CE and from local management.
//-----------------------------------------------------------------------------
ChannelManager.prototype.removeChannel = function(channelName) {
	var confirmDiv = $("<div></div>")
		.append("<p><span class='ui-icon ui-icon-alert' style='float: left; margin: 0 7px 20px 0;'></span>The channel will be permanently deleted and cannot be recovered. Are you sure?</p>");

	confirmDiv.dialog({
		autoOpen: true,
		resizable: false,
		height:160,
		minWidth: 350,
		modal: true,
		title: "Delete channel " + channelName + "?",
		buttons: {
			"Delete Channel": function() {
				channelMgr.removeOneChannelTab(channelName);
				channelMgr.channels[channelName].destroy();
				delete channelMgr.channels[channelName];
				$(this).dialog("destroy");
			},
			Cancel: function() {
				$(this).dialog("destroy");
			}
		}
	});
}

//-----------------------------------------------------------------------------
// ChannelManager.prototype.removeOneChannelTab
//-----------------------------------------------------------------------------
// Removed the tab panel of the named channel.
//-----------------------------------------------------------------------------
ChannelManager.prototype.removeOneChannelTab = function(channelName) {
	var tabNum = this.channels[channelName].tabPanel.prop('tabNum');

	$("#tabs a.tablink").each(function() {
		var href = $(this).attr('href');
		if ( href ) {
			var curTabNum = href.replace(/#tabs-/, '');

			if ( curTabNum == tabNum ) {
				$('#tabs-' + tabNum).remove();
				$(this).parent().remove();
			}
		}
	});

	$('#tabs').tabs({ active: 0 });
	$('#tabs').tabs("refresh")
}

//-----------------------------------------------------------------------------
// ChannelManager.prototype.removeAllChannelTabs
//-----------------------------------------------------------------------------
// Remove all existing tab/panels for channels only.
//-----------------------------------------------------------------------------
ChannelManager.prototype.removeAllChannelTabs = function() {
	// Return if there are no channel tabs yet.
	if (tabCounter == minTabs + 1) { return; }

	$("#tabs a.tablink").each(function() {
		var href = $(this).attr('href');
		if ( href ) {
			var tabNum = href.replace(/#tabs-/, '');

			if ( tabNum > minTabs ) {
				$('#tabs-' + tabNum).remove();
				$(this).parent().remove();
			}
		}
	});
}

//-----------------------------------------------------------------------------
// ChannelManager.prototype.display
//-----------------------------------------------------------------------------
// List existing channels
// activeTab: the tab # to display after the channel tabs have been rebuilt.
//-----------------------------------------------------------------------------
ChannelManager.prototype.display = function(activeTab) {
	this.removeAllChannelTabs();

	tabCounter = minTabs + 1;

	// Refill with current list of channels.
	for ( var chanName in this.channels ) {
		var newPanel = addTab(chanName);
		this.channels[chanName].tabPanel = newPanel;
		this.channels[chanName].display();
	}

	if ( ! activeTab ) activeTab = 0;
	$('#tabs').tabs({ active: activeTab });
	$('#tabs').tabs("refresh");
}