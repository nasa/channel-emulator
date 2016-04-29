//=============================================================================
// Preferences.js
// Author: Tad Kollar  
// $Id: Preferences.js 1826 2013-01-14 16:44:43Z tkollar $
// Copyright (c) 2012.
//      NASA Glenn Research Center.  All rights reserved.
//      See COPYING file that comes with this distribution.
//=============================================================================

//=============================================================================
// Preferences
// Set, store, and retrieve the user's preferences.
//=============================================================================

// Constructor.
function Preferences() {
	// this.formSrc = loadFileToString("prefsForm.html");
	this.cname_debugLevel = "debugLevel";

	$("#debugLevel").prop("selectedIndex", this.getDebugLevel());
}

// Adjust the debug level in the Messenger object as well
// as storing it in a cookie for future sessions.
Preferences.prototype.setDebugLevel = function(newLevel) {
	msg.debugLevel = newLevel;
	$.cookie(this.cname_debugLevel, newLevel, { expires: 365} );
}

// Get the debug level from a cookie or use "info" for
// the default if no cookie is set.
Preferences.prototype.getDebugLevel = function() {
	var level = $.cookie(this.cname_debugLevel);
	if ( level == null || level == undefined ) {
		$.cookie(this.cname_debugLevel, level, { expires: 365} );
		level = Messenger.DLVL_INFO;
	}

	return level;
}
