/* -*- C++ -*- */

//=============================================================================
/**
 * @file   ChannelRegistry.cpp
 * @author Tad Kollar  
 *
 * $Id: ChannelRegistry.cpp 2336 2013-08-30 17:50:59Z tkollar $
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "ChannelRegistry.hpp"

namespace nasaCE {

ChannelRegistry::ChannelRegistry() {

}

ChannelRegistry::~ChannelRegistry() {
	removeAll();
}

void ChannelRegistry::autoLoad() {
	ND_DEBUG("[ChannelRegistry] Running autoLoad().\n");
	bool wasCreated;
	Setting& channelSection = CEcfg::instance()->getOrAdd("Channels", Setting::TypeGroup, wasCreated);

	// Any group in the Channels section is taken to be a channel.
	// Automatically load it - unless there's an autoLoad setting and it's false.
	for ( int chIdx = 0; chIdx < channelSection.getLength() ; ++chIdx ) {
		if (  channelSection[chIdx].isGroup() &&
			( ! channelSection[chIdx].exists("autoLoad") ||
			( channelSection[chIdx].exists("autoLoad") &&
				static_cast<bool>(channelSection[chIdx]["autoLoad"]) == true ))) {

			std::string channelName = channelSection[chIdx].getName();
			ND_INFO("Auto-loading channel '%s'.\n", channelName.c_str());

			try {
				add(channelName)->autoLoad();
			}
			catch (const AlreadyExists& e) {
				ND_WARNING("[ChannelRegistry] %s: cannot duplicate or autoload segments.", e.what(), channelName.c_str());
			}
		}
	}
}

Channel* ChannelRegistry::add(const std::string& channelName) {
	ND_DEBUG("[ChannelRegistry] Request to add new Channel named %s.\n", channelName.c_str());

	if ( exists(channelName) ) throw AlreadyExists(AlreadyExists::msg("channel", channelName));

	Channel* channelPtr = new Channel(channelName);
	if ( !channelPtr ) throw OutOfMemory("ChannelRegistry::autoLoad: Unable to allocate memory for new Channel.");
	_channels[channelName] = channelPtr;

	return (channelPtr);
}

void ChannelRegistry::remove(const std::string& channelName) {
	delete _channels[channelName];
	_channels.erase(channelName); // Remove the key from the map.
}

void ChannelRegistry::removeAll() {
	ND_DEBUG("[ChannelRegistry] Deleting all channels.\n");

	ChannelMap::iterator pos;
	Channel* channel;

	while (_channels.size()) {
		pos = _channels.begin();
		channel = pos->second;
		ND_DEBUG("[ChannelRegistry] Deleting channel %s.\n", pos->first.c_str());
		pos->second = 0;
		_channels.erase(pos);
		delete channel;
	}
}

Channel *ChannelRegistry::channel(const std::string& channelName) {
	if (exists(channelName)) {
		return _channels[channelName];
	}
	throw nd_error(std::string("Channel " + channelName + " does not exist."));
}

bool ChannelRegistry::exists(const std::string& channelName) {
	if ( _channels.find(channelName) == _channels.end() ) return false;
	return true;
}

void ChannelRegistry::listNames(std::vector<std::string>& channel_names) {
	channel_names.clear();

	ChannelMap::iterator pos;
	for ( pos = _channels.begin(); pos != _channels.end(); ++pos )
		channel_names.push_back(pos->first);
}

} // namespace nasaCE