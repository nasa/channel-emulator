/* -*- C++ -*- */

//=============================================================================
/**
 * @file   ChannelRegistry.hpp
 * @author Tad Kollar  
 *
 * $Id: ChannelRegistry.hpp 1778 2012-12-12 16:28:58Z tkollar $ 
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _NASA_CHANNEL_REGISTRY_HPP_
#define _NASA_CHANNEL_REGISTRY_HPP_

#include "Channel.hpp"
#include "nd_error.hpp"
#include "nd_macros.hpp"

#include <string>
#include <ace/Singleton.h>

namespace nasaCE {

typedef std::map<std::string, Channel*> ChannelMap;

//=============================================================================
/**
 *	@author Tad Kollar  
 *	@class ChannelRegistry
 *	@brief Manage a map containing keys to all channels.
*/
//=============================================================================
class ChannelRegistry {
	
public: 
	ChannelRegistry();

    ~ChannelRegistry();
    
	/// Load defined channels in the config file.
	void autoLoad();
	
	/// Creates a new channel if it doesn't already exist.
	/// @param channelName An identifier for the channel, unique among other channels.
	/// @return A pointer to the new channel.
	Channel *add(const std::string& channelName);
	
	/// Destroys the named channel.
	void remove(const std::string& channelName);
	
	/// Destroy all channels.
	void removeAll();
	
	/// An accessor for the channel map.
	Channel *operator [](const std::string& channelName) { return channel(channelName); }
	
	/// An accessor for the channel map.
	Channel *channel(const std::string& channelName);
	
	/// Returns true if the channel name is found in the map as a key.
	bool exists(const std::string& channelName);
	
	/// Clears then loads a vector with channel names, found as keys in the map.
	void listNames(std::vector<std::string>& channel_names);

private:
	ChannelMap _channels;

}; // class ChannelRegistry

typedef ACE_Singleton<ChannelRegistry, ACE_Recursive_Thread_Mutex> channels;

} // namespace nasaCE

#endif // _NASA_CHANNEL_REGISTRY_HPP_
