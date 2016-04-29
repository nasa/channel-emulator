//=============================================================================
/**
 * @file   collisions.js
 * @author Tad Kollar  
 *
 * $Id: collisions.js 1118 2010-06-16 19:44:03Z tkollar $
 * Copyright (c) 2010.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

// Determine if the provided shape has the attributes of a rectangle.
function isRectangle(shape) {
	if ( shape.hasAttributeNS(null, "x") && shape.hasAttributeNS(null, "y") && 
		shape.hasAttributeNS(null, "width") && shape.hasAttributeNS(null, "height") )
		return true;
		
	return false;
}

function getDims(rect) {
	if ( ! isRectangle(rect) )
		throw TypeError("Argument to getDims() was not a valid rectangle.");
		
	dims = new Object();
	
	dims.x = globalAbsX(rect);
	dims.y = globalAbsY(rect);
	dims.width = rect.width.baseVal.value;
	dims.height = rect.height.baseVal.value;

	return dims;
};

// Determine if rectangle r1 overlaps at all with rectangle r2.
function svgIntersects(other) {
	if ( ! isRectangle(other) )
		throw TypeError("Argument to intersects() was not a valid rectangle.");
	
	r1 = getDims(this);
	r2 = getDims(other);

	var dx1 = Math.abs(r1.x - r2.x);
	var dx2 = Math.abs((r1.x + r1.width) - (r2.x + r2.width));
	var dy1 = Math.abs(r1.y - r2.y);
	var dy2 = Math.abs((r1.y + r1.height) - (r2.y + r2.height));
	
	if ( ((dx1 <= r1.width && dx2 <= r1.width) || (dx1 <= r2.width && dx2 <= r2.width)) &&
			((dy1 <= r1.height && dy2 <= r1.height) || (dy1 <= r2.height && dy2 <= r2.height)) )
		return true;
		
	return false;
}

// Check all rectangles with ids matched by the regex. If they intersect the
// provided rectangle, add them to the array which is returned.
function svgGetNamedColliders(regex) {

	var container = this.parentNode? this.parentNode : svgRoot;

	var colliders = [];
		
	for ( var shapeIdx in container.childNodes ) {
		var shape = container.childNodes[shapeIdx];
		if ( shape.nodeName != "svg" ) continue;
		
		if ( shape.id != this.id &&	shape.id.match(regex) && this.intersects(shape) ) {
			colliders.push(shape);
		}
	}
	
	return colliders;
}

function svgAdjustArrows() {
	if ( this.arrows ) {
		for ( var idx in this.arrows )
			this.arrows[idx].origin.drawArrowTo(this.arrows[idx].id, this.arrows[idx].target);
	}
}

function svgVerticalMoveAwayFrom(rester, nameRegex, direction) {
	if ( ! direction ) direction = this.movedir;

	var distance = (this.owner.channel == rester.owner.channel)? verticalSpacing : verticalSpacing * 4;

	var newY = rester.y.baseVal.value + rester.height.baseVal.value + distance;
	var sign = (direction == "up" && (this.y.baseVal.val - newY) > 0)? -1 : 1;
	 
	this.y.baseVal.value = newY * sign;
	this.makeSpace(nameRegex, direction);
	
	this.adjustArrows();
	
	// If a segment has just been moved, update its arrows.
	if (this.owner.channel)	channels[this.owner.channel].updateGeometryFromSvg(this);
	
	// If this is a channel that's just been moved, update the arrows for ALL of its segments.
	if (this.owner.segments) {
		for ( var segName in this.owner.segments ) {
			if (segName == "length") continue;
			
			if ( this.owner.segments[segName].shape )
				this.owner.segments[segName].shape.adjustArrows();
		}
	}

}

function svgMakeSpace(nameRegex, direction) {
	if ( ! nameRegex ) nameRegex = this.regex;

	var colliders = this.getNamedColliders(nameRegex);
	
	for ( var cIdx in colliders ) {
		colliders[cIdx].verticalMoveAwayFrom(this, nameRegex, direction);
	}
}
