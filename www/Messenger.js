//=============================================================================
// Messenger.js
// Author: Tad Kollar  
// $Id: Messenger.js 1895 2013-03-05 19:50:15Z tkollar $
// Copyright (c) 2012.
//      NASA Glenn Research Center.  All rights reserved.
//      See COPYING file that comes with this distribution.
//=============================================================================

//=============================================================================
// Messenger
// Manage messages to display to the user.
//=============================================================================

// Constructor.
function Messenger() {
	prefs = new Preferences();
	this.debugLevel = prefs.getDebugLevel();
}

// Log colors
Messenger.DARK_GREEN = "#004000";
Messenger.LIGHT_GREEN = "#008000";
Messenger.GRAY = "#808080";
Messenger.YELLOW = "#808000";
Messenger.RED = "#FF0000";

// Recognized message/debug levels
Messenger.DLVL_DEBUG = 5;
Messenger.DLVL_INFO = 4;
Messenger.DLVL_NOTICE = 3;
Messenger.DLVL_WARNING = 2;
Messenger.DLVL_ERROR = 1;
Messenger.DLVL_MUTE = 0;

// Add a message to the message div area.
Messenger.prototype.msg = function(msg, color) {
	if ( color == null ) { color = Messenger.LIGHT_GREEN; }

	var d = new Date();

	var newEntry = "<span class='logTime'>[" + d.toISOString() + "] </span><span style='color: " +
		color + "'>" + msg + "</span><br/>\n";

	var newContent = $("#msgArea").html() + newEntry;
	$("#msgArea").html(newContent);
	$("#msgArea").scrollTop(1000);
}

// Print a debug message, if the debug level is high enough.
Messenger.prototype.debug = function(newMsg) {
	if (this.debugLevel >= Messenger.DLVL_DEBUG) {
		this.msg(newMsg, Messenger.DARK_GREEN);
	}
}

// Print an info message, if the debug level is high enough.
Messenger.prototype.info = function(newMsg) {
	if (this.debugLevel >= Messenger.DLVL_INFO) {
		this.msg(newMsg, Messenger.LIGHT_GREEN);
	}
}

// Print a notice message, if the debug level is high enough.
Messenger.prototype.notice = function(newMsg) {
	if (this.debugLevel >= Messenger.DLVL_NOTICE) {
		this.msg(newMsg, Messenger.GRAY);
	}
}

// Print a warning message, if the debug level is high enough.
Messenger.prototype.warning = function(newMsg) {
	if (this.debugLevel >= Messenger.DLVL_WARNING) {
		this.msg(newMsg, Messenger.YELLOW);
	}
}

// Print an error message, if the debug level is high enough.
Messenger.prototype.error = function(newMsg) {
	if (this.debugLevel >= Messenger.DLVL_ERROR) {
		this.msg(newMsg, Messenger.RED);
	}
}

// Report an error from XML-RPC.
Messenger.prototype.xmlrpcError = function(xmlrpcResponse, notes) {
	if ( ! notes ) { notes = ""; }
	this.error("XML-RPC fault: " +xmlrpcResponse.faultCode() + ", " + xmlrpcResponse.faultString() + "<br/>Notes: " + notes);
}

Messenger.MAX_DUMP_DEPTH = 10;
Messenger.prototype.dumpObj = function(obj, name, indent, depth) {
	if (depth > Messenger.MAX_DUMP_DEPTH) {
		this.msg(indent + name + ": <Maximum Depth Reached>", Messenger.RED);
		return;
	}

	if (typeof obj == "object") {
		var child = null;
		this.msg(indent + name, Messenger.YELLOW);
		indent += "&nbsp;&nbsp;";
		for (var item in obj)
		{
			try {
				child = obj[item];
			} catch (e) {
				child = "<Unable to Evaluate>";
			}

			if (typeof child == "object") {
				this.dumpObj(child, item, indent, depth + 1);
			} else {
				this.msg(indent + item + ": " + child, Messenger.GRAY);
			}
		}
	}
}
