/* -*- C++ -*- */

//=============================================================================
/**
 * @file   Channel_Interface.cpp
 * @author Tad Kollar  
 *
 * $Id: Channel_Interface.cpp 2016 2013-06-04 14:22:55Z tkollar $
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "Channel_Interface.hpp"
#include "xmlrpc-c/girerr.hpp"

using namespace std;

namespace nasaCE {

Channel_Interface* Channel_InterfaceP;

Channel_Interface::Channel_Interface(xmlrpc_c::registryPtr& rpcRegistry):
	CE_DLL("Channel_Interface") {
	ACE_TRACE("Channel_Interface::Channel_Interface");
	register_methods(rpcRegistry);
}

void Channel_Interface::register_methods(xmlrpc_c::registryPtr& rpcRegistry) {
	ACE_TRACE("Channel_Interface::register_methods");
	REGISTER_METHOD(add_channel, "channel.addChannel");
	REGISTER_METHOD(remove_channel, "channel.removeChannel");
	REGISTER_METHOD(activate_channel, "channel.activateChannel");
	REGISTER_METHOD(pause_channel, "channel.pauseChannel");
	REGISTER_METHOD(list_channels, "channel.listChannels");

	REGISTER_METHOD(add_segment, "channel.addSegment");
	REGISTER_METHOD(remove_segment, "channel.removeSegment");
	REGISTER_METHOD(list_segments, "channel.listSegments");
	REGISTER_METHOD(get_segment_type, "channel.getSegmentType");
	REGISTER_METHOD(segment_exists, "channel.segmentExists");

}

void Channel_Interface::add_channel(xmlrpc_c::paramList const& paramList,
	xmlrpc_c::value* retvalP) {
	ACE_TRACE("Channel_Interface::add_channel");
	const std::string channelName(paramList.getString(0));

	ND_DEBUG("Received an XML-RPC request to add a traffic channel named %s.\n", channelName.c_str());

	channels::instance()->add(channelName);
	CEcfg::instance()->getOrAddGroup("Channels." + channelName);

	*retvalP = xmlrpc_c::value_nil();
}

void Channel_Interface::remove_channel(xmlrpc_c::paramList const& paramList,
	xmlrpc_c::value* retvalP) {
	ACE_TRACE("Channel_Interface::remove_channel");

	const std::string channelName(paramList.getString(0));

	ND_DEBUG("Received a request to remove a traffic channel named %s.\n", channelName.c_str());

	if ( ! channels::instance()->exists(channelName) ) {
		throw nd_error("Requested channel name does not exist, no changes made.");
	}

	channels::instance()->remove(channelName);
	CEcfg::instance()->remove("Channels." + channelName);

	*retvalP = xmlrpc_c::value_nil();
}

void Channel_Interface::activate_channel(xmlrpc_c::paramList const& paramList,
	xmlrpc_c::value* retvalP) {
	ACE_TRACE("Channel_Interface::activate_channel");
	const std::string channelName(paramList.getString(0));

	channels::instance()->channel(channelName)->activate();

	*retvalP = xmlrpc_c::value_nil();
}

void Channel_Interface::pause_channel(xmlrpc_c::paramList const& paramList,
	xmlrpc_c::value* retvalP) {
	ACE_TRACE("Channel_Interface::pause_channel");
	const std::string channelName(paramList.getString(0));

	channels::instance()->channel(channelName)->pause();

	*retvalP = xmlrpc_c::value_nil();
}

void Channel_Interface::list_channels(xmlrpc_c::paramList const& paramList,
	xmlrpc_c::value* retvalP) {
	ACE_TRACE("Channel_Interface::list_channels");

	vector<xmlrpc_c::value> xml_channels;
	vector<std::string> string_channels;

	channels::instance()->listNames(string_channels);

	vector<std::string>::iterator pos;
	for (pos = string_channels.begin(); pos != string_channels.end(); ++pos) {
		xml_channels.push_back(xmlrpc_c::value_string(*pos));
	}

	*retvalP = xmlrpc_c::value_array(xml_channels);
}

void Channel_Interface::add_segment(xmlrpc_c::paramList const& paramList,
	xmlrpc_c::value* retvalP) {
	ACE_TRACE("Channel_Interface::add_segment");

	const std::string moduleName(paramList.getString(0));
	const std::string channelName(paramList.getString(1));
	const std::string segmentName(paramList.getString(2));

	channels::instance()->channel(channelName)->add(moduleName, segmentName);

	*retvalP = xmlrpc_c::value_nil();
}

void Channel_Interface::remove_segment(xmlrpc_c::paramList const& paramList,
	xmlrpc_c::value* retvalP) {
	ACE_TRACE("Channel_Interface::remove_segment");
	const std::string channelName(paramList.getString(0));
	const std::string segmentName(paramList.getString(1));

	channels::instance()->channel(channelName)->remove(segmentName);
	CEcfg::instance()->remove("Channels." + channelName + "." + segmentName);

	*retvalP = xmlrpc_c::value_nil();
}

void Channel_Interface::list_segments(xmlrpc_c::paramList const& paramList,
	xmlrpc_c::value* retvalP) {
	ACE_TRACE("Channel_Interface::list_segments");

	const std::string channelName(paramList.getString(0));

	if ( ! channels::instance()->exists(channelName) ) {
		throw nd_error("Specified channel name " + channelName + " not found.");
	}

	vector<xmlrpc_c::value> xml_segments;

	Channel* channel = channels::instance()->channel(channelName);
	HandlerMap::iterator pos;
	for (pos = channel->handlers().begin(); pos != channel->handlers().end(); ++pos) {
		xml_segments.push_back(xmlrpc_c::value_string(pos->second->getName()));
	}

	*retvalP = xmlrpc_c::value_array(xml_segments);
}

void Channel_Interface::get_segment_type(xmlrpc_c::paramList const& paramList,
	xmlrpc_c::value* retvalP) {
	ACE_TRACE("Channel_Interface::get_segment_type");
	const std::string channelName(paramList.getString(0));
	const std::string segmentName(paramList.getString(1));

	BaseTrafficHandler* segment = channels::instance()->channel(channelName)->find(segmentName);

	if ( !segment ) throw nd_error("Specified segment '" + segmentName + "' does not exist.");

	*retvalP = xmlrpc_c::value_string(segment->ptrDLL()->getName());
}

void Channel_Interface::segment_exists(xmlrpc_c::paramList const& paramList,
	xmlrpc_c::value* retvalP) {
	ACE_TRACE("Channel_Interface::segment_exists");
	const std::string channelName(paramList.getString(0));
	const std::string segmentName(paramList.getString(1));

	BaseTrafficHandler* segment = channels::instance()->channel(channelName)->find(segmentName);

	*retvalP = xmlrpc_c::value_boolean((segment != 0)? true : false);
}

} // namespace nasaCE
