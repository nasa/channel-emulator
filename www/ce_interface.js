//=============================================================================
/**
 * @file   ce_interface.js
 * @author Tad Kollar  
 *
 * $Id: ce_interface.js 1324 2011-09-30 19:27:18Z tkollar $ 
 * Copyright (c) 2010.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

var svgDocument = null;
var svgRoot = null;

// The global client variable initializes to point to the server from which this
// file was loaded. This probably should never need to be changed. If it is,
// chances are that the browser will report a security error.
var client = new xmlrpc_client('/RPC2', top.location.hostname, top.location.port, 'http');
// client.setDebug(2);

// Global device map.
var devices;

// Global channel map.
var channels;

//=============================================================================
// Global "constants"
//=============================================================================

// Set the SVG namespace.
var svgns = "http://www.w3.org/2000/svg";
var xlinkns = "http://www.w3.org/1999/xlink";

var segmentWidth = 105;
var segmentHeight = 80;

var horizontalSpacing = 35;
var verticalSpacing = 15;

var yAxis = 40;

var do_debug = false;

//=============================================================================
// Utility functions
//=============================================================================
function findSvgVars(evt) {
	if ( svgDocument === null ) svgDocument = evt.target.ownerDocument;
	if ( svgRoot === null ) svgRoot = svgDocument.documentElement;
	
	// alert(svgRoot.constructor);
	
}

function makeAttributeListStr(obj) {
	var attribs = "";
	for ( var x = 0; x < obj.attributes.length; ++x ) {
		attribs += obj.attributes[x].name + ": " + obj.attributes[x].value + "\n";
	}
	return attribs;
}

// Place a message in the "debugArea" textarea object, IF do_debug is true,
// AND "debugArea" exists.
function debug(msg) {
	if ( ! do_debug ) return;
	
	alert(msg);
}

// Report an error from XML-RPC if debugging is enabled.
function xmlrpc_error(response) {
	debug("XML-RPC FAULT:\n" + response.faultCode() + ", " + response.faultString());
}

function createSVG(id, x, y, width, height, direction, cssClass, prefix) {
	if ( ! cssClass ) cssClass = "standardNode";
	if ( ! prefix ) prefix = "ce_svg_";
	
	var svg = svgDocument.createElementNS(svgns, "svg");
	svg.setAttribute("x", x);
	svg.setAttribute("y", y);
	svg.setAttribute("width", width);
	svg.setAttribute("height", height);
	svg.setAttribute("class", cssClass);
	svg.id = prefix + id;
	svg.movedir = direction;
	svg.regex = prefix + ".*";
	svg.intersects = svgIntersects;
	svg.getNamedColliders = svgGetNamedColliders;
	svg.adjustArrows = svgAdjustArrows;
	svg.verticalMoveAwayFrom = svgVerticalMoveAwayFrom;
	svg.makeSpace = svgMakeSpace;
	
	return svg;
}

var draggee;
function mousedownRect(evt) {
	draggee = evt.currentTarget.parentNode;
	draggee.dragX = draggee.x.baseVal.value - evt.clientX;
	draggee.dragY = draggee.y.baseVal.value - evt.clientY;
	document.addEventListener("mousemove", mousemoveRect, true);
	document.addEventListener("mouseup", mouseupRect, true);

}

function mouseupRect(evt) {
	document.removeEventListener("mousemove", mousemoveRect, true);
	document.removeEventListener("mouseup", mouseupRect, true);
}

function mousemoveRect(evt){
	var id = svgRoot.suspendRedraw(1000);

	draggee.x.baseVal.value = evt.clientX + draggee.dragX;
	draggee.y.baseVal.value = evt.clientY + draggee.dragY;
	
	svgRoot.unsuspendRedraw(id);
}

function AddInfoButton() {
	var svg = this.shape;
	
	var image = svgDocument.createElementNS(svgns, "image");
	image.segment = this;
	svg.button = image;
	
	image.setAttribute("onclick", "this.segment.displayInfo()");
	image.setAttribute("visibility", "hidden");
	image.setAttributeNS(xlinkns, "href", "infoButton.png");
	image.setAttribute("width", "18px");
	image.setAttribute("height", "18px");	
	image.setAttribute("x", svg.width.baseVal.value - 21);
	image.setAttribute("y", svg.height.baseVal.value - 21);
	image.setAttribute("style", "opacity: 0.3");
	
	svg.setAttribute("onmouseover", "this.button.setAttribute('visibility', 'visible')");
	svg.setAttribute("onmouseout", "this.button.setAttribute('visibility', 'hidden')");
	svg.appendChild(image);
}

function addRow(table, labelText, valueText) {
	var counterLabel = svgDocument.createElementNS(svgns, "tspan");
	counterLabel.setAttribute("x", 5);		
	if ( table.childNodes.length ) {
		counterLabel.setAttribute("dy", 20);
	}
	else counterLabel.setAttribute("y", 40);
	var counterLabelText = svgDocument.createTextNode(labelText);
	counterLabel.appendChild(counterLabelText);
	table.appendChild(counterLabel);
	
	if ( valueText ) {
		var counterValue = svgDocument.createElementNS(svgns, "tspan");
		counterValue.setAttribute("x", "65%");
		var counterValueText = svgDocument.createTextNode(valueText);
		counterValue.appendChild(counterValueText);
		table.appendChild(counterValue);
	}
}

function AdjustHeight(newHeight) {
	var infoBg = svgDocument.getElementById(this.id + "_infoBg");
	if ( infoBg.height.baseVal.value == newHeight ) return;
	
	infoBg.height.baseVal.value = newHeight;
	
	var leftTab = svgDocument.getElementById(this.id + "_leftTab");
	leftTab.y.baseVal.value = infoBg.y.baseVal.value + newHeight;
	
	var rightTab = svgDocument.getElementById(this.id + "_rightTab");
	rightTab.y.baseVal.value = leftTab.y.baseVal.value;
}

function swapTabs(evt) {
	var itemName = evt.target.item.id;	
	var leftTab = svgDocument.getElementById(itemName + "_leftTab");
	var rightTab = svgDocument.getElementById(itemName + "_rightTab");
	
	if ( evt.target.side == "left" ) {
		// Make left side active
		leftTab.setAttributeNS(xlinkns, "xlink:href", "#activeLeftBottomTab");
		leftTab.removeEventListener("click", swapTabs, false);
				
		rightTab.setAttributeNS(xlinkns, "xlink:href", "#inactiveRightBottomTab");
		rightTab.addEventListener("click", swapTabs, false);
		
		evt.target.item.infoType = "counters";
	}
	else {
		// Make right side active
		rightTab.setAttributeNS(xlinkns, "xlink:href", "#activeRightBottomTab");
		rightTab.removeEventListener("click", swapTabs, false);
		
		leftTab.setAttributeNS(xlinkns, "xlink:href", "#inactiveLeftBottomTab");
		leftTab.addEventListener("click", swapTabs, false);
		
		evt.target.item.infoType = "settings";
	}
	
	evt.target.item.displayInfo();
}

function GetInfoWindow() {
	if ( this.infowin ) return this.infowin;
	
	var x = globalAbsX(this.shape);
	var y = globalAbsY(this.shape) + this.shape.height.baseVal.value + 5;
	var svg = createSVG(this.id, x, y, 330, 300, null, "info", "info_svg_");
	this.infowin = svg;
	// svg.setAttribute("visibility", "hidden");
	
	this.infoType = "counters";
	
	var titleBar = svgDocument.createElementNS(svgns, "use");
	titleBar.setAttribute("x", "0");
	titleBar.setAttribute("y", "0");
	titleBar.setAttribute("width", "100%");
	titleBar.setAttribute("height", "26");	
	titleBar.setAttributeNS(xlinkns, "xlink:href", "#infoWinTitleBar");
	svg.appendChild(titleBar);
	
	var winTitle = svgDocument.createElementNS(svgns, "text");
    winTitle.id = this.id + "_infowin_title";
	winTitle.setAttribute("x", "50%");
	winTitle.setAttribute("y", "18");
	winTitle.setAttribute("class", "infoWinTitle");
	var winTitleText = svgDocument.createTextNode(this.id);
	winTitle.appendChild(winTitleText);
	svg.appendChild(winTitle);
	
	// Put this over the title bar and the title text so all events go here.
	// Otherwise the title text will intercept events we want to go to the bar.
	var titleBarCover = svgDocument.createElementNS(svgns, "use");
	titleBarCover.setAttribute("x", "0");
	titleBarCover.setAttribute("y", "0");
	titleBarCover.setAttribute("width", "100%");
	titleBarCover.setAttribute("height", "26");	
	titleBarCover.setAttributeNS(xlinkns, "xlink:href", "#infoWinTitleBarCover");
	svg.appendChild(titleBarCover);
	titleBarCover.addEventListener("mousedown", mousedownRect, false);

	var closeButton = svgDocument.createElementNS(svgns, "use");
	closeButton.setAttribute("x", "304");
	closeButton.setAttribute("y", "0");
	closeButton.setAttribute("width", "26");
	closeButton.setAttribute("height", "26");	
	closeButton.setAttributeNS(xlinkns, "xlink:href", "#infoWinCloseButton");
	closeButton.setAttribute("onclick", "this.parentNode.setAttribute('visibility', 'hidden')");
	svg.appendChild(closeButton);

	var infoBg = svgDocument.createElementNS(svgns, "rect");
	infoBg.setAttribute("id", this.id + "_infoBg");
	infoBg.setAttribute("x", "0");
	infoBg.setAttribute("y", "26");	
	infoBg.setAttribute("width", "100%");
	infoBg.setAttribute("height", "200");
	infoBg.setAttribute("class", "info");
	svg.appendChild(infoBg);
	
	var leftTab = svgDocument.createElementNS(svgns, "use");
	leftTab.setAttribute("id", this.id + "_leftTab");
	leftTab.setAttribute("x", "0");
	leftTab.setAttribute("y", "226");
	leftTab.setAttribute("width", "165");
	leftTab.setAttribute("height", "26");	
	leftTab.setAttributeNS(xlinkns, "xlink:href", "#activeLeftBottomTab");
	leftTab.side = "left";
	leftTab.item = this;
	svg.appendChild(leftTab);
	
	var rightTab = svgDocument.createElementNS(svgns, "use");
	rightTab.setAttribute("id", this.id + "_rightTab");
	rightTab.setAttribute("x", "165");
	rightTab.setAttribute("y", "226");
	rightTab.setAttribute("width", "165");
	rightTab.setAttribute("height", "26");	
	rightTab.setAttributeNS(xlinkns, "xlink:href", "#inactiveRightBottomTab");
	rightTab.side = "right";
	rightTab.item = this;
	svg.appendChild(rightTab);
	rightTab.addEventListener("click", swapTabs, false);
	
	svgRoot.appendChild(svg);
	
	this.adjustHeight = AdjustHeight;
	return svg;
}

function DisplayInfoWindow() {
	var svg = this.getInfoWin();
	
	if ( this.infoTable ) {
		svg.removeChild(this.infoTable);
		this.infoTable = null;
	}
	
    this.infoTable = svgDocument.createElementNS(svgns, "text");
    this.infoTable.id = this.id + "_info_text";
	this.infoTable.setAttribute("x", "10");
	this.infoTable.setAttribute("y", "12");
	this.infoTable.setAttribute("class", "infoWinText");
	
	var lastLabelY = 0;
	
	if ( this.infoType == "counters" ) {
		var displayedCounters = 0;
		for ( var counterName in this.counters ) {
			if ( this.counters[counterName].value > 0 ) {
				addRow(this.infoTable, counterName, this.counters[counterName].value);
			
				if ( this.counters[counterName].rate && this.counters[counterName].rate != 0 )
					addRow(this.infoTable, counterName + "/s", this.counters[counterName].rate.toFixed(2));
					
				++displayedCounters;
			}
		}
		
		if ( ! displayedCounters ) addRow(this.infoTable, "No positive counter values found.");
	}
	else {
		var displayedSettings = 0;
		for ( var settingName in this.settings ) {
			if ( this.settings[settingName].value != "" ) {
				addRow(this.infoTable, settingName, this.settings[settingName].value);
				++displayedSettings;
			}
		}
		
		if ( ! displayedSettings ) addRow(this.infoTable, "No setting values found.");
	}	
	
	svg.appendChild(this.infoTable);
	this.adjustHeight(this.infoTable.getBBox().height + 15);
	svg.setAttribute("visibility", "visible");
}

function globalAbsX(shape) {
	if ( shape == undefined || shape.x == undefined || ! shape.x.baseVal == undefined )
		throw TypeError("Object supplied to globalAbsX has no 'x' property.");
	
	var x = shape.x.baseVal.value;
		
	if ( ! shape.parentNode || shape.parentNode.id == "mainCanvas" ) return x;
	else return x + globalAbsX(shape.parentNode);
}

function globalAbsY(shape) {
	if ( shape == undefined || shape.y == undefined || ! shape.y.baseVal == undefined )
		throw TypeError("Object supplied to globalAbsY has no 'y' property.");
	
	var y = shape.y.baseVal.value;
		
	if ( ! shape.parentNode || shape.parentNode.id == "mainCanvas" ) return y;
	else return y + globalAbsY(shape.parentNode);
}

//=============================================================================
// Arrow handling
//=============================================================================

function Arrow(arrowName, origin, target) {
	this.id = arrowName;
	this.origin = origin;
	this.target = target;
	this.line = null;
	this.pointer = null;
	
}

Arrow.prototype.draw = function(x1, y1, x2, y2) {
	var redraw = true;
	
	var signX = (x1 > x2)? 1 : -1;
	var deltaX = x2 - x1;
	var deltaY = y2 - y1;
	
	if ( ! this.line ) {
		this.line = svgDocument.createElementNS(svgns, "path");
		this.line.id = this.id + "_line";
		redraw = false;
	}
	
	var coordX = new Array();
	var coordY = new Array();
	coordX[0] = x1 + deltaX * 0.25;
	coordY[0] = y1 + deltaY * 0.05;
	coordX[1] = x1 + deltaX * 0.50;
	coordY[1] = y1 + deltaY * 0.50;
	coordX[2] = x1 + deltaX * 0.75;
	coordY[2] = y1 + deltaY * 0.95;

	var linePathStr =
		"M" + x1 + " " + y1 +
		" T" + coordX[0] + " " + coordY[0] +
		" T" + coordX[1] + " " + coordY[1] +
		" T" + coordX[2] + " " + coordY[2] +
		" T" + x2 + " " + y2;
	
	this.line.setAttribute("d", linePathStr);
	var classStr = (deltaX > 0)? "leftToRight" : "rightToLeft";
	this.line.setAttribute("class", "line_" + classStr);
	
	if ( !redraw ) svgRoot.appendChild(this.line);

	if ( ! this.pointer ) {
		this.pointer = svgDocument.createElementNS(svgns, "path");
		this.pointer.id = this.id + "_pointer";
	}
		
	var pointerPathStr =
		"M" + x2 + " " + y2 +
		" L" + (x2 + (10*signX)) + " " + (y2 - 7) +
		" L" + (x2 + (5*signX)) + " " + y2 +
		" L" + (x2 + (10*signX)) + " " + (y2 + 7) +
		" L" + x2 + " " + y2 + " " + "Z";
	
	this.pointer.setAttribute("d", pointerPathStr);
	
	if (deltaY != 0 ) {
		var angle = deltaY/deltaX * 10;
		this.pointer.setAttribute("transform", "rotate(" + angle + " " + x2 + " " + y2 + ")");
	}
	else if ( redraw ) {
		this.pointer.removeAttributeNS(null, "transform");
	}
	
	this.pointer.setAttribute("class", "pointer_" + classStr);
		
	if ( ! redraw ) svgRoot.appendChild(this.pointer);
}

function drawArrowToShape(arrowId, other) {
	if ( ! ( this.shape && other.shape ) )
		throw ReferenceError("drawArrowTo called on Segments missing one or more rendered shapes.");

	var svg1 = this.shape;
	var svg2 = other.shape;
	
	if ( ! this.shape.arrows ) this.shape.arrows = new Object();
	if ( ! other.shape.arrows ) other.shape.arrows = new Object();
	
	var arrow;
	if ( this.shape.arrows[arrowId] ) arrow = this.shape.arrows[arrowId];
	else if ( other.shape.arrows[arrowId] ) arrow = other.shape.arrows[arrowId];
	else {
		var arrow = new Arrow(arrowId, this, other);
		this.shape.arrows[arrowId] = arrow;
		other.shape.arrows[arrowId] = arrow;
	}
	
	if (globalAbsX(svg1) < globalAbsX(svg2)) 
		arrow.draw(
			globalAbsX(svg1) + svg1.width.baseVal.value,
			globalAbsY(svg1) + svg1.height.baseVal.value/2,
			globalAbsX(svg2),
			globalAbsY(svg2) + svg2.height.baseVal.value/2
		);
	else
		arrow.draw(
			globalAbsX(svg1),
			globalAbsY(svg1) + svg1.height.baseVal.value/2,
			globalAbsX(svg2) + svg2.width.baseVal.value,
			globalAbsY(svg2) + svg2.height.baseVal.value/2
		);
		
	return true;
}

//=============================================================================
// Device handling
//=============================================================================

// The Device prototype.
function Device(devName) {
	this.id = devName;
	this.drawArrowTo = drawArrowToShape;

	this.findType(); // Must be called before others!
	this.findState();
	this.findSide();	
	this.findConnectorInfo();
	
	// If the side is unknown, make a weak guess... if a segment reads from
	// it, say the device on the left. Will look ugly in a lot of
	// configurations but without the channel won't draw at all.	
	if ( this.side == "Unknown" )
		this.side = ( this.reader && ! this.writer )? "Left" : "Right";
}

Device.prototype.onLeft = function() {
	if ( ! this.side ) throw Error("No side has been set for this device.");

	if ( this.side == "Left" ) return true;
	
	if ( this.side == "Right" ) return false;
}

// Determine the type of Device so that XML-RPC calls that require
// the type as part of the call name can be made.
Device.prototype.findType = function() {
	var nameVal = new xmlrpcval(this.id, 'string');
	var params = [nameVal];	
	var rpc = new xmlrpcmsg("device.getType", params);
	var response = client.send(rpc);
	
	delete nameVal;
	delete rpc;
	
	if ( response.faultCode() ) {
		xmlrpc_error(response);
		return;
	}
	
	this.devType = response.value().scalarVal();
}

// Determine whether the device still exists on the CE.
Device.prototype.exists = function() {
	var nameVal = new xmlrpcval(this.id, 'string');
	var params = [nameVal];	
	var rpc = new xmlrpcmsg("device.exists", params);
	var response = client.send(rpc);
	
	delete nameVal;
	delete rpc;
	
	if ( response.faultCode() ) {
		xmlrpc_error(response);
		return;
	}
	
	return response.value().scalarVal();
}

// Send an XML-RPC call to the CE, for types of calls that require
// the device name and device type, which are stored in the Device
// object. The Device type is prepended to the provided methodName
// call and the Device name is added to the start of the params array.
// The returned value is an xmlrpcval object, so its .scalarVal() or
// other method must be used to extract information from it.
Device.prototype.callXmlrpcMethod = function(methodName, params) {
	if ( params === undefined ) params = [];
	var nameVal = new xmlrpcval(this.id, 'string');
	params.unshift(nameVal);
	var rpc = new xmlrpcmsg( "dev" + this.devType + "." + methodName, params);
	var response = client.send(rpc);
	
	delete nameVal;
	delete rpc;
	
	if ( response.faultCode() ) {
		xmlrpc_error(response);
		return null;
	}
	
	return response.value();
}

// Set the channel and segment names of the reader and writer, if
// they exist.
Device.prototype.findConnectorInfo = function() {
	var readerInfo = this.callXmlrpcMethod("getReaderInfo");
	var readerName = readerInfo.structMem("name").scalarVal();
	if ( readerName.length ) {
		this.reader = new Object();
		this.reader.channel = readerInfo.structMem("channel").scalarVal();
		this.reader.id = readerInfo.structMem("name").scalarVal();
	}
	
	var writerInfo = this.callXmlrpcMethod("getWriterInfo");
	var writerName = writerInfo.structMem("name").scalarVal();
	if ( writerName.length ) {
		this.writer = new Object();	
		this.writer.channel = writerInfo.structMem("channel").scalarVal();
		this.writer.id = writerInfo.structMem("name").scalarVal();
	}
}

// Determine which side of the visualization the device should
// be drawn on.
Device.prototype.findSide = function() {
	this.side = this.callXmlrpcMethod("getSide").scalarVal();
}

// Determine whether the device has been properly initialized.
Device.prototype.findState = function() {
	this.isInitialized = this.callXmlrpcMethod("isInitialized").scalarVal();
}

Device.prototype.render = function(x, y, goingRight, cssClass) {
	if ( ! cssClass ) cssClass = "deviceNode";
	
	var newlyDrawn = false;
	
	if ( ! this.shape ) {
		newlyDrawn = true;
	
		var svg = createSVG(this.id, x, y, segmentWidth * 0.7, segmentHeight, "down", cssClass)
		this.shape = svg;
		svg.owner = this;
		svg.makeSpace();
				
		var rect = svgDocument.createElementNS(svgns, "rect");
		rect.setAttribute("x", 0);
		rect.setAttribute("y", 0);
		rect.setAttribute("rx", 10);
		rect.setAttribute("ry", segmentHeight/2);	
		rect.setAttribute("width", "100%");
		rect.setAttribute("height", "100%");
		rect.setAttribute("class", cssClass);
		rect.id = "main_rect_" + this.id;
		svg.appendChild(rect);
		
		var labelText = svgDocument.createTextNode(this.id);
		var label = svgDocument.createElementNS(svgns, "text");
		
		label.setAttribute("x", "50%");
		label.setAttribute("y", "50%");
		label.setAttribute("class", "label");
		label.setAttribute("dominant-baseline", "middle");
		label.id = "main_label_" + this.id;
		label.appendChild(labelText);
			
		svgRoot.appendChild(svg);
		svg.appendChild(label);
	}
		
	var nextY = verticalSpacing;
		
	if ( this.onLeft() ) {
		var nextX = horizontalSpacing/2;
		
		if (this.reader && this.reader.obj ) {
			this.reader.obj.render(nextX, nextY, true, true);
			var arrowId = this.id + "_arrow_to" + this.reader.obj.id;
			this.drawArrowTo(arrowId, this.reader.obj);
		}
		
		if (this.writer && this.writer.obj ) {
			this.writer.obj.render(nextX, nextY, false, true);
			var arrowId = this.writer.obj.id + "_arrow_to" + this.id;
			this.writer.obj.drawArrowTo(arrowId, this);
		}
	}
	else if ( ! goingRight ) {	
		
		if (this.reader && this.reader.obj ) {
			var nextX = channels[this.reader.obj.channel].maxSegmentX - segmentWidth;

			this.reader.obj.render(nextX, nextY, true, false);
			var arrowId = this.id + "_arrow_to" + this.reader.obj.id;
			this.drawArrowTo(arrowId, this.reader.obj);
		}
		
		if (this.writer && this.writer.obj ) {
			var nextX = channels[this.writer.obj.channel].maxSegmentX;
			
			this.writer.obj.render(nextX, nextY, false, false);
			var arrowId = this.writer.obj.id + "_arrow_to" + this.id;
			this.writer.obj.drawArrowTo(arrowId, this);
		}		
	}
	
	return newlyDrawn;
}

Device.prototype.toString = function() {
	return this.id;
}

// Associated segments can move, so adjust the device's vertical position as well.
Device.prototype.adjustPosition = function(x) {
	if ( x != null ) this.shape.x.baseVal.value = x;

	if ( this.shape == null || this.shape == undefined ) return false;
	
	// If there are both reader and writer segments, move halfway between them.
	if ( this.reader && this.reader.obj && this.writer && this.writer.obj )
		this.shape.y.baseVal.value +=
			Math.abs((globalAbsY(this.reader.obj.shape) - globalAbsY(this.writer.obj.shape))/2);
	
	// If there's just a reader segment, line up with it.
	else if ( this.reader && this.reader.obj && this.reader.obj.shape )
		this.shape.y.baseVal.value = globalAbsY(this.reader.obj.shape);
		
	// If there's just a writer segment, line up with it.
	else if ( this.writer && this.writer.obj && this.writer.obj.shape )
		this.shape.y.baseVal.value = globalAbsY(this.writer.obj.shape);
		
	// Nothing to line up with.
	else return false;
	
	// Fix the arrows coming into and going out of the shape.
	this.shape.adjustArrows();
	
	return true;
}

//=============================================================================
// Segment handling
//=============================================================================

// Segment prototype
function Segment(channelName, segmentName) {
	this.channel = channelName;
	this.id = segmentName;
	this.drawArrowTo = drawArrowToShape;
	this.addInfoButton = AddInfoButton;
	this.displayInfo = DisplayInfoWindow;
	this.getInfoWin = GetInfoWindow;
    this.updateInfoCallback = new Function("response", "arguments.callee.segment.updateInfo(response)");
    this.updateInfoCallback.segment = this;	
		
	this.findType(); // Must be called first!	
	
	this.updateInfo();
	
	this.timer_id = window.setInterval("channels." + channelName + ".segments." +
		segmentName + ".updateInfoAsync()", 1000);
}

// Determine the name of the module that spawned the segment so
// that XML-RPC calls can be built with it.
Segment.prototype.findType = function() {
	var cval = new xmlrpcval(this.channel, 'string');
	var sval = new xmlrpcval(this.id, 'string');
	var params = [cval, sval];	
	var rpc = new xmlrpcmsg("channel.getSegmentType", params);
	var response = client.send(rpc);
	
	delete cval;
	delete sval;
	delete rpc;
	
	if ( response.faultCode() ) {
		xmlrpc_error(response);
		return;
	}
	
	this.segmentType = response.value().scalarVal();
}

// Send an XML-RPC call to the CE, for types of calls that require
// the channel name, segment name, and segment type, which are
// stored in the Segment object. The segment type is prepended
// to the provided methodName call and channel name and segment
// name are added to the start of the params array.
// The returned value is an xmlrpcval object, so its .scalarVal() or
// other method must be used to extract information from it.
Segment.prototype.callXmlrpcMethod = function(methodName, params, callBackMethod) {
	if ( ! params ) params = [];
	var cval = new xmlrpcval(this.channel, 'string');
	var sval = new xmlrpcval(this.id, 'string');
	params.unshift(cval, sval);
	
	var rpc = new xmlrpcmsg( this.segmentType + "." + methodName, params);
		
	if ( ! callBackMethod ) {		
		var response = client.send(rpc);
			
		delete cval;
		delete sval;
		delete rpc;
		
		if ( response.faultCode() ) {
			xmlrpc_error(response);
			return null;
		}
		
		return response.value();
	}
	else {
        client.send(rpc, 10, callBackMethod);
        delete rpc;
	}
}

// Query the number of received units for the segment.
Segment.prototype.getReceivedUnitCount = function() {
	return this.callXmlrpcMethod("getReceivedUnitCount").scalarVal();
}

// Query the number of received octets for the segment.
Segment.prototype.getReceivedOctetCount = function() {
	return this.callXmlrpcMethod("getReceivedOctetCount").scalarVal();	
}

// Query the number of queued (waiting to be processed) units for the segment.
Segment.prototype.getQueuedUnitCount = function() {
	return this.callXmlrpcMethod("getQueuedUnitCount").scalarVal();
}

// Query the number of queued (waiting to be processed) octets for the segment.
Segment.prototype.getQueuedOctetCount = function() {
	return this.callXmlrpcMethod("getQueuedOctetCount").scalarVal();	
}

// Build a map of the segments that this one receives from. Only
// names are added here, references are added by acquireCeInfo().
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

// Build a map of the segments that this one can send to. Only
// names are added here, references are added by acquireCeInfo().
Segment.prototype.findOutputs = function(outputStruct) {
	if ( outputStruct === undefined || outputStruct === null )
		outputStruct = this.callSegmentMethod("getOutputLinks");
	
	if ( this.hasOwnProperty("outputs") ) this.prevOutputs = this.outputs;
	
	this.outputs = new Object();
	this.outputs.length = 0;
	
	var outputObj;
	while ((outputObj = outputStruct.structEach())) {
		this.outputs[outputObj[0]] = new Object();
		this.outputs[outputObj[0]].type = outputObj[1].scalarVal();
		this.outputs.length++;
	}
}

Segment.prototype.updateInfoAsync = function() {
	this.callXmlrpcMethod("getInfo", null, this.updateInfoCallback);
}

// Every segment supports a "getInfo" XML-RPC call that sends a nested
// structure containing identifiers, output links, counters, settings,
// and start/end timestamps of the XML-RPC call. The updateInfo method
// extracts the information from those structures and uses timestamps,
// current counters, and previous counters to compute rate information.
Segment.prototype.updateInfo = function(response) {
	var infoStruct;
	 
	if ( ! response ) infoStruct = this.callXmlrpcMethod("getInfo");
	else infoStruct = response.value();

	var nameStruct = infoStruct.structMem("names");
	var inputStruct = infoStruct.structMem("inputs");	
	var outputStruct = infoStruct.structMem("outputs");
	var counterStruct = infoStruct.structMem("counters");
	var settingStruct = infoStruct.structMem("settings");
	var timestampStruct = infoStruct.structMem("timestamps");
	
	// Extract identifiers
	this.channel = nameStruct.structMem("channel").scalarVal();
	this.id = nameStruct.structMem("segment").scalarVal();
	this.segmentType = nameStruct.structMem("dll").scalarVal();
	this.device = new String(nameStruct.structMemExists("device")?
		nameStruct.structMem("device").scalarVal() : "");
	
	// Store previous timestamps and extract new ones. Timestamps are returns
	// as separated second and microsecond values, so these are added together
	// to produce a useful floating point value in seconds only.
	if ( this.hasOwnProperty("startSecs") ) this.prevStartSecs = this.startSecs;
	this.startSecs = timestampStruct.structMem("callStartSec").scalarVal() +
		timestampStruct.structMem("callStartUsec").scalarVal()/1000000;
		
	if ( this.hasOwnProperty("prevStartSecs") ) {
		this.deltaStartSecs = this.startSecs - this.prevStartSecs;		
	}
	
	if ( this.hasOwnProperty("endSecs") ) this.prevEndSecs = this.endSecs;
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
	this.counters = new Object();
	
	var counterObj;
	while ((counterObj = counterStruct.structEach())) {
		this.counters[counterObj[0]] = new Object();
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
	if ( this.hasOwnProperty("settings") ) this.prevSettings = this.settings;
	this.settings = new Object();
	
	var settingObj;
	while ((settingObj = settingStruct.structEach())) {
		this.settings[settingObj[0]] = new Object();
		this.settings[settingObj[0]].value = settingObj[1].scalarVal();
	}

	if ( this.shape ) {
		if ( this.counters.receivedOctets && this.counters.receivedOctets.rate ) {
			if ( ! this.rateCaption ) {
				this.rateCaption = svgDocument.createElementNS(svgns, "text");
				this.rateCaption.setAttribute("x", "50%");
				this.rateCaption.setAttribute("y", "20%");
				this.rateCaption.setAttribute("class", "rate_caption");
				this.rateCaption.setAttribute("dominant-baseline", "middle");
				this.rateCaption.id = this.id + "_rate_caption";
				this.shape.appendChild(this.rateCaption);
			}
			else {
				if (this.rateCaption.childNodes[0])
					this.rateCaption.removeChild(this.rateCaption.childNodes[0]);
			}
			
			if ( this.counters.receivedOctets.rate > 0 ) {
				var labelText = svgDocument.createTextNode((this.counters.receivedOctets.rate>>7) + " kbps");
				this.rateCaption.appendChild(labelText);
			}
		}
		
		if ( this.counters.queuedOctets && this.settings.highWaterMark ) {
			var maxWidthPercent = 70;
			
			var newPercent = (1 - (this.counters.queuedOctets.value / this.settings.highWaterMark.value)) * maxWidthPercent;
			this.dangerBarCover.setAttribute("width", newPercent + "%");
			this.dangerBarCover.setAttribute("x", (15 + maxWidthPercent - newPercent) + "%");
		}		
	}

	if ( this.infowin && this.infowin.getAttribute("visibility") == "visible" ) 
		this.displayInfo();
}

Segment.prototype.isOrphan = function() {
	if ( ! this.device && ! this.outputs.length && ! this.inputs.length ) {
		return true;
	}
	
	return false;
}

Segment.prototype.render = function(x, y, showOutputs, goingRight, cssClass) {
	if ( this.shape ) return false;

	if ( ! cssClass ) cssClass = "standardNode";

	var svg = createSVG(this.id, x, y, segmentWidth, segmentHeight, "down", cssClass)
	this.shape = svg;
	svg.owner = this;
	
	// MAIN RECTANGLE
	var rect = svgDocument.createElementNS(svgns, "rect");
	rect.setAttribute("x", 0);
	rect.setAttribute("y", 0);
	rect.setAttribute("rx", 3);
	rect.setAttribute("ry", 3);	
	rect.setAttribute("width", "100%");
	rect.setAttribute("height", "100%");
	rect.setAttribute("class", cssClass);
	rect.id = "main_rect_" + this.id;
	svg.appendChild(rect);
	
	// MAIN LABEL
	var labelText = svgDocument.createTextNode(this.id);
    var label = svgDocument.createElementNS(svgns, "text");
	
	label.setAttribute("x", "50%");
	label.setAttribute("y", "85%");
	label.setAttribute("class", "label");
	label.setAttribute("dominant-baseline", "middle");
	label.id = "main_label_" + this.id;
    label.appendChild(labelText);	
	svg.appendChild(label);
	
	// QUICK INFO RECTANGLE
	var quickInfo = svgDocument.createElementNS(svgns, "rect");
	quickInfo.setAttribute("x", "8%");
	quickInfo.setAttribute("y", "8%");
	quickInfo.setAttribute("rx", 3);
	quickInfo.setAttribute("ry", 3);	
	quickInfo.setAttribute("width", "84%");
	quickInfo.setAttribute("height", "62%");
	quickInfo.setAttribute("class", "quick_info");
	quickInfo.id = "quick_info_rect_" + this.id;
	svg.appendChild(quickInfo);
	
	// QUEUE CAPACITY BARS
	var dangerBar = svgDocument.createElementNS(svgns, "rect");
	dangerBar.setAttribute("x", "15%");
	dangerBar.setAttribute("y", "40%");
	dangerBar.setAttribute("rx", 1);
	dangerBar.setAttribute("ry", 1);	
	dangerBar.setAttribute("width", "70%");
	dangerBar.setAttribute("height", "15");
	dangerBar.setAttribute("class", "danger_bar");
	dangerBar.id = "danger_bar_rect_" + this.id;
	svg.appendChild(dangerBar);
	
	this.dangerBarCover = svgDocument.createElementNS(svgns, "rect");
	this.dangerBarCover.setAttribute("x", "15%");
	this.dangerBarCover.setAttribute("y", "40%");
	this.dangerBarCover.setAttribute("width", "70%");
	this.dangerBarCover.setAttribute("height", "15");
	this.dangerBarCover.setAttribute("class", "danger_bar_cover");
	this.dangerBarCover.id = "danger_bar_cover_rect_" + this.id;
	svg.appendChild(this.dangerBarCover);		
	
	channels[this.channel].shape.appendChild(svg);
	svg.makeSpace();
	// Update channel geometry info and possibly draw the channel container graphic.
	channels[this.channel].drawnSegments++;
	channels[this.channel].updateGeometry(this);	
		
	var neighborSegments = showOutputs? this.outputs : this.inputs;
	
	var nextX = goingRight? x + segmentWidth + horizontalSpacing :
		x - horizontalSpacing - segmentWidth;	
	
	this.addInfoButton();
	
	for ( var linkIdx in neighborSegments ) {
		if ( linkIdx == "length" ) continue;
		var segment = neighborSegments[linkIdx].obj;
		
		// if ( ! showOutputs ) alert( this.id + " doing input " + linkIdx );
		
		if ( segment ) {		
			segment.render(nextX, y, showOutputs, goingRight, cssClass)
			var arrowId = showOutputs? this.id + "_arrow_to_" + segment.id : 
				segment.id + "_arrow_to_" + this.id;
				
			if (showOutputs) this.drawArrowTo(arrowId, segment);
			else segment.drawArrowTo(arrowId, this);
		}
		else if ( !segment ) {
			throw Error(this.id + ".render(): Missing segment object where there should be one.");
		}
	}
	
	// If there's a connected input device, draw an arrow to it.
	if ( this.device && this.device.obj ) {
		nextX = goingRight? globalAbsX(svg) + segmentWidth + horizontalSpacing :
		globalAbsY(svg) - horizontalSpacing - segmentWidth;
	
		if ( showOutputs && this.device.obj.writer &&
			this.device.obj.writer.id == this.id ) {
			
			this.device.obj.render(nextX, globalAbsY(svg), goingRight);
			var arrowId = this.id + "_arrow_to_" + this.device.obj.id;
			this.drawArrowTo(arrowId, this.device.obj);
		}
		else if ( ! showOutputs && this.device.obj.reader &&
			this.device.obj.reader.id == this.id ) {
			
			this.device.obj.render(nextX, globalAbsY(svg), goingRight);
			var arrowId = this.device.obj.id + "_arrow_to_" + this.id;
			this.device.obj.drawArrowTo(arrowId, this);
		}
	}
	
	return true;
}

Segment.prototype.toString = function() {
	return this.id;
}

//=============================================================================
// Channel handling
//=============================================================================

// The Channel prototype.
// The associated drawings need to be created first so that
// shapes rendered later will be on top.
function Channel(channelName) {
	this.id = channelName;
	this.orphanX = horizontalSpacing/2;
	
	var svg = createSVG(this.id,
		horizontalSpacing * 1.5 + (segmentWidth * 0.7), yAxis,
		20, 20,
		"down", "standardNode", "channel_svg_");
	svg.setAttribute("visibility", "hidden");

	this.shape = svg;
	svg.owner = this;
	
	var rect = svgDocument.createElementNS(svgns, "rect");
	rect.setAttribute("class", "channel");
	rect.setAttribute("x", 0);
	rect.setAttribute("y", 0);
	rect.setAttribute("width", "100%");
	rect.setAttribute("height", "100%");	
	rect.setAttribute("rx", 6);
	rect.setAttribute("ry", 6);
	svg.appendChild(rect);
	this.rect = rect;
	
	var labelBg = svgDocument.createElementNS(svgns, "rect");
	labelBg.setAttribute("class", "channel_label_bg");
	labelBg.setAttribute("x", 0);
	labelBg.setAttribute("y", 0);
	labelBg.setAttribute("width", "100%");
	labelBg.setAttribute("height", "20");	
	labelBg.setAttribute("rx", 10);
	labelBg.setAttribute("ry", 10);
	svg.appendChild(labelBg);
	this.labelBg = labelBg;
	
	var labelText = svgDocument.createTextNode(this);
	var label = svgDocument.createElementNS(svgns, "text");
	
	label.setAttribute("x", "50%" );
	label.setAttribute("class", "label");
	label.appendChild(labelText);
	svg.appendChild(label);
	this.label = label;
	
	svgRoot.appendChild(svg);	
	
	this.getSegments();
	this.drawnSegments = 0;
	this.maxSegmentX = 0;
	this.maxSegmentY = 0;
}

// Visualize all unconnected segments.
Channel.prototype.renderOrphans = function() {
	for (var segName in this.segments) {
		if ( segName == "length" ) continue;
		
		var segment = this.segments[segName];
		if ( segment.isOrphan() ) {
			segment.render(this.orphanX, verticalSpacing, true, true);
			this.orphanX += segmentWidth + horizontalSpacing;
		}
	}
}

// Query the CE for a list of segments in this channel.
Channel.prototype.getSegments = function() {
	var sval = new xmlrpcval(this.id, 'string');
	var params = [sval];	
	var rpc = new xmlrpcmsg("channel.listSegments", params);
	var response = client.send(rpc);
	
	delete sval;
	delete rpc;
	
	if ( response.faultCode() ) {
		xmlrpc_error(response);
		return;
	}
	
	var rval = response.value();
	
	this.segments = new Object();
	for ( var i = 0; i < rval.arraySize(); ++i ) {
		var segmentName = rval.arrayMem(i).scalarVal();
		this.segments[segmentName] = new Segment(this.id, segmentName);
	}
	
	this.segments.length = i;
}

Channel.prototype.toString = function() {
	return this.id;
}

Channel.prototype.updateGeometryFromSvg = function(svg) {
	if ( ! svg ) return;
	
	var leftX = svg.x.baseVal.value;
	var rightX = leftX + svg.width.baseVal.value;
	var topY = svg.y.baseVal.value;
	var bottomY = topY + svg.height.baseVal.value;
	
	if ( rightX > this.maxSegmentX ) this.maxSegmentX = rightX;
	if ( bottomY > this.maxSegmentY ) this.maxSegmentY = bottomY;
}

Channel.prototype.updateGeometry = function(segment) {
	if ( ! segment ) return;

	this.updateGeometryFromSvg(segment.shape)
	
	// All the segments have been rendered, so draw the channel.
	if ( this.drawnSegments == this.segments.length )
		this.render();
}

Channel.prototype.render = function() {
	var svg = this.shape;
	
	if ( this.rendered ) return;
	
	svg.id = "channel_svg_" + this.id;
	var width = this.maxSegmentX + horizontalSpacing/2;
	var height = this.maxSegmentY + verticalSpacing * 2;

	svg.width.baseVal.value = width;
	svg.height.baseVal.value = height;

	this.label.setAttribute("y", height - verticalSpacing * 0.5);
	this.labelBg.setAttribute("y", height - verticalSpacing * 0.5 - 16);
	svg.makeSpace("channel_svg_.*", "down");	
	
	svg.setAttribute("visibility", "visible");
	this.rendered = true;
}

//=============================================================================
// Initialization
//=============================================================================

function Layout() {
}

// Query the CE for a list of all devices and create a map containing
// a reference to a Device for each one.
Layout.prototype.getDevices = function () {
	var rpc = new xmlrpcmsg("device.getList", null);
	var response = client.send(rpc);
	
	delete rpc;
	
	if ( response.faultCode() ) {
		xmlrpc_error(response);
		// alert(response.serialize());
		return;
	}
	
	var rval = response.value();
	
	var devices = new Object();
	
	for ( var i = 0; i < rval.arraySize(); ++i ) {
		var deviceName = rval.arrayMem(i).scalarVal();
		devices[deviceName] = new Device(deviceName);
	}
	
	return devices;
}

// Query the CE for a list of channels and return it.
Layout.prototype.getChannels = function () {
	var rpc = new xmlrpcmsg("channel.listChannels", null);
	var response = client.send(rpc);
	
	delete rpc;
	
	if ( response.faultCode() ) {
		xmlrpc_error(response);
		return;
	}
	
	var rval = response.value();
	
	channels = new Object();
	
	for ( var i = 0; i < rval.arraySize(); ++i ) {
		var channelName = rval.arrayMem(i).scalarVal();
		channels[channelName] = new Channel(channelName);
		channels[channelName].renderOrphans();
	}
	
	return channels;
}

Layout.prototype.render = function () {
	// First pass, get everything drawn from the left side.
	for ( var devName in devices ) {
		var dev = devices[devName];
			
		if (dev.onLeft()) dev.render(horizontalSpacing, yAxis + verticalSpacing, true);
	}
	
	var longestChannelRightEdge = 0;
	for ( var chanName in channels ) {
		if (chanName == "length") continue;
		var channel = channels[chanName];
		var rightEdge = channel.shape.x.baseVal.value + channel.shape.width.baseVal.value;
		if ( rightEdge > longestChannelRightEdge) longestChannelRightEdge = rightEdge;
	}
	
	// Second pass, get everything drawn from the right side.
	for ( var devName in devices ) {
		var dev = devices[devName];
			
		if ( ! dev.onLeft()) {
			dev.render(longestChannelRightEdge + horizontalSpacing,
				yAxis + verticalSpacing, false);
		}
			
	}	
	
	// Third pass, make sure everything lines up.
	for ( var devName in devices ) {
		if ( devices[devName].onLeft() ) devices[devName].adjustPosition();
		else devices[devName].adjustPosition(longestChannelRightEdge + horizontalSpacing);
	}

	var title = svgDocument.createElementNS(svgns, "text");
    title.id = "mainTitle";
	title.setAttribute("x", "50%");
	title.setAttribute("y", "32");
	var titleText = svgDocument.createTextNode("NASA Channel Emulator Configuration and Metrics (" +
		top.location.hostname + ")");
	title.appendChild(titleText);
	svgRoot.appendChild(title);
}

// Query all required information from the CE and build a structure
// connecting it all together with references to output links.
Layout.prototype.acquireCeInfo = function (evt) {

	// alert("'" + evt.currentTarget.parentNode.nodeName + "'");
	findSvgVars(evt);
	devices = this.getDevices();
	channels = this.getChannels();
	
	// For every device with a reader and/or writer segment,
	// add a reference to that segment.
	for ( var devName in devices ) {
		var dev = devices[devName];
		
		// If the device has an associated reader segment (its name has a non-zero
		// length), and that segment has already been loaded, add a reference to
		// it in the device object as an output.
		if ( dev.hasOwnProperty("reader") && channels.hasOwnProperty(dev.reader.channel) &&
			channels[dev.reader.channel].segments.hasOwnProperty(dev.reader.id)) {
			dev.reader.obj = channels[dev.reader.channel].segments[dev.reader.id];
			channels[dev.reader.channel].segments[dev.reader.id].device.obj = dev;
		}

		// If the device has an associated writer segment (its name has a non-zero
		// length), and that segment has already been loaded, add a reference in
		// that segment to the device object as an output.
		if ( dev.hasOwnProperty("writer") && channels.hasOwnProperty(dev.writer.channel) &&
			channels[dev.writer.channel].segments.hasOwnProperty(dev.writer.id)) {
			dev.writer.obj = channels[dev.writer.channel].segments[dev.writer.id];			
			channels[dev.writer.channel].segments[dev.writer.id].device.obj = dev;
		}
	}
	
	// For every output of every segment in every channel, add a reference to the
	// actual object in addition to the name and type.
	for ( var channelName in channels ) {
		var channel = channels[channelName];
		
		for ( var segmentName in channel.segments ) {
			var segment = channel.segments[segmentName];
			
			for ( var outputName in segment.outputs ) {
				var output = segment.outputs[outputName];
				
				if ( channel.segments.hasOwnProperty(outputName) ) {
					output.obj = channel.segments[outputName];
				}
			}
			
			for ( var inputName in segment.inputs ) {
				var input = segment.inputs[inputName];
				
				if ( channel.segments.hasOwnProperty(inputName) ) {
					input.obj = channel.segments[inputName];
				}
			}
		}
	}
	
	this.render();
}

Layout.prototype.update = function () {
	for ( var channelName in channels ) {
		if ( channelName == "length" ) continue;
		var channel = channels[channelName];
		
		for ( var segmentName in channel.segments ) {
			if ( segmentName == "length" ) continue;
//			channel.segments[segmentName].updateInfo();
			channel.segments[segmentName].updateInfoAsync();	
		}
	}
}

var layout = new Layout();
// var timer_id = window.setInterval(layout.update, 1000);