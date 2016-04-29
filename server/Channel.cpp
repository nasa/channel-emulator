/* -*- C++ -*- */

//=============================================================================
/**
 * @file   Channel.cpp
 * @author Tad Kollar  
 *
 * $Id: Channel.cpp 2349 2013-09-03 13:51:02Z tkollar $
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "Channel.hpp"
#include <xmlrpc-c/girerr.hpp>


namespace nasaCE {

Channel::Channel(const std::string& name): _name(name) {
	ND_DEBUG("Constructing channel '%s'.\n", name.c_str());

}

Channel::~Channel() {
	ACE_TRACE("Channel::~Channel");
	ND_INFO("Removing all %d handlers from channel %s.\n", _handlers.size(), _name.c_str());

	pause();
	HandlerMap::iterator pos;

	for ( pos = _handlers.begin(); pos != _handlers.end(); pos = _handlers.begin() ) {
		remove(pos->first);
	}

	_handlers.clear();

	ND_DEBUG("Channel %s now has %d segments.\n", _name.c_str(), _handlers.size());
}

void Channel::autoLoad() {
	ND_DEBUG("Automatically loading segments in channel %s.\n", _name.c_str());

	bool wasCreated;
	Setting& channelSettings = CEcfg::instance()->getOrAdd(cfgKey(), Setting::TypeGroup, wasCreated);

	// Using this convoluted pair instead of a map because the sort has to be
	// completed in a second pass.
	std::list<std::pair<std::string, std::set<std::string> > > loadableSegments;

	for ( int segIdx = 0 ; segIdx < channelSettings.getLength() ; ++segIdx ) {
		// If there's a section with a dllName setting, try to load it
		// as a segment - unless there's also an autoLoad setting and it's false.
		if  ( channelSettings[segIdx].isGroup() &&
		      channelSettings[segIdx].exists("dllName") &&
			( ! channelSettings[segIdx].exists("autoLoad") ||
			( channelSettings[segIdx].exists("autoLoad") &&
				static_cast<bool>(channelSettings[segIdx]["autoLoad"]) == true ) ) ) {
				std::string segPath(channelSettings[segIdx].getPath());
				std::set<std::string> segDeps;
				getDependencies(channelSettings[segIdx], segDeps);
				std::pair<std::string, std::set<std::string> > segWithDeps(segPath, segDeps);
				loadableSegments.push_back(segWithDeps);
				ND_INFO("Will auto-load segment '%s'.\n", channelSettings[segIdx].getName());
		}
	}

	// Sort by dependency
	loadableSegments.sort(doesNotDependOn);

	std::list<std::pair<std::string, std::set<std::string> > >::iterator pos;

	for ( pos = loadableSegments.begin(); pos != loadableSegments.end(); ++pos ) {
		xmlrpc_c::value addSegmentRetvalP;
		xmlrpc_c::paramList addSegmentParams;

		std::string segName = CEcfg::instance()->get(pos->first).getName();
		std::string dllName = CEcfg::instance()->getString(pos->first + ".dllName");

		add(dllName, segName);
	}
}

BaseTrafficHandler* Channel::find(const std::string& handlerName) {
	HandlerMap::iterator pos;

	for ( pos = _handlers.begin(); pos != _handlers.end(); ++pos ) {
		if ( pos->second->getName() == handlerName ) return pos->second;
	}

	return 0;
}

void Channel::add(BaseTrafficHandler* newHandler) {
	if ( find(newHandler->getName()) )
		throw DuplicateHandlerName(std::string("Handler named ") + newHandler->getName() +
			std::string(" already exists in Channel ") + getName() + std::string("."));

	_handlers[newHandler->getName()] = newHandler;
	newHandler->incRefCount();
}


void Channel::add(const std::string& dllName, const std::string& segName) {
	ND_DEBUG("Adding segment %s instance of module %s to channel %s.\n", segName.c_str(), dllName.c_str(), getName().c_str());

	dll_registry::instance()->loadIfNotLoaded(dllName, xmlrpc_server::instance()->rpcRegistry);
	dll_registry::instance()->dll(dllName)->ptrDLL()->addInstance(segName, getName());
}

void Channel::remove(const std::string& handlerName) {
	BaseTrafficHandler* doomed;

	if ( ! (doomed = find(handlerName)) )
		throw HandlerNotFound(std::string("Handler named ") + handlerName +
			std::string(" does not exist in Channel ") + getName() + std::string("."));

	doomed->withdraw();
	doomed->decRefCount();
	_handlers.erase(handlerName);

	if (! doomed->getRefCount() ) delete doomed;
}

void Channel::activate() {
	ND_INFO("[%s] Activating the entire channel.\n", getName().c_str());

	HandlerMap::iterator pos;

	for ( pos = _handlers.begin(); pos != _handlers.end(); ++pos ) {
		pos->second->open();
	}
}

void Channel::pause() {
	ND_INFO("[%s] Pausing the entire channel.\n", getName().c_str());

	HandlerMap::iterator pos;

	for ( pos = _handlers.begin(); pos != _handlers.end(); ++pos ) {
		pos->second->stopTraffic(false);
	}
}

bool Channel::doesNotDependOn(std::pair<std::string, std::set<std::string> >& possibleDependant,
							  std::pair<std::string, std::set<std::string> >& possibleProvider) {
	Setting &seg1 = CEcfg::instance()->get(possibleDependant.first);
	Setting &seg2 = CEcfg::instance()->get(possibleProvider.first);

	#ifdef DEFINE_DEBUG

	std::ostringstream os;

	os << "Segment '" << seg1.getName() << "' depends on the following other segments: ";

	if ( possibleDependant.second.empty()) os << "None!";
	else {
		std::set<std::string>::iterator pos;
		for ( pos = possibleDependant.second.begin(); pos != possibleDependant.second.end(); ++pos ) {
			if ( pos != possibleDependant.second.begin() ) os << ", ";
			os << *pos;
		}
	}

	ND_DEBUG("%s\n", os.str().c_str());

	#endif

	if ( possibleDependant.second.find(seg2.getName()) != possibleDependant.second.end() ) return false;

	if ( possibleDependant.second.size() > possibleProvider.second.size() ) return false;

	return true;
}

void Channel::getDependencies(const Setting& seg, std::set<std::string>& deps) {
	ND_DEBUG("Recursively finding dependencies for segment '%s'\n", seg.getName());

	if ( seg.exists("primaryOutput") )
		addDependency(seg["primaryOutput"][0], seg, deps);

	if ( seg.exists("auxOutput") )
		addDependency(seg["auxOutput"][0], seg, deps);

	if ( seg.exists("primaryOutputs") ) {
		for ( int i = 0; i < seg["primaryOutputs"].getLength(); ++i ) {
			addDependency(seg["primaryOutputs"][i][0], seg, deps);
		}
	}
}

void Channel::addDependency(const std::string& depName, const Setting& seg,
	std::set<std::string>& deps) {

	// Make sure the new dependency isn't already in there to avoid circular loops
	if ( deps.find(depName) == deps.end() ) {
		deps.insert(depName);
		getDependencies(seg.getParent()[depName], deps);
	}
}

} // namespace nasaCE