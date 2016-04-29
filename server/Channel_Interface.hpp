/* -*- C++ -*- */

//=============================================================================
/**
 * @file   Channel_Interface.hpp
 * @author Tad Kollar  
 *
 * $Id: Channel_Interface.hpp 2016 2013-06-04 14:22:55Z tkollar $
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _CHANNEL_INTERFACE_HPP_
#define _CHANNEL_INTERFACE_HPP_

#include "CE_Macros.hpp"
#include "ChannelRegistry.hpp"
#include <xmlrpc-c/xml.hpp>
#include <set>

namespace nasaCE {

//=============================================================================
/**
 * @class Channel_Interface
 * @brief Provide an XML-RPC interface to the channel management structure.
*/
//=============================================================================

class Channel_Interface : public CE_DLL {
public:
    Channel_Interface(xmlrpc_c::registryPtr& rpcRegistry);

    ~Channel_Interface() { }

	void add_channel(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);
	void remove_channel(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);
	void activate_channel(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);
	void pause_channel(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);
	void list_channels(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);

	void add_segment(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);
	void remove_segment(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);
	void list_segments(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);
	void get_segment_type(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);
	void segment_exists(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);

	/// Export specified methods via XML-RPC.
	void register_methods(xmlrpc_c::registryPtr&);

}; // class Channel_Interface



extern Channel_Interface* Channel_InterfaceP;

METHOD_CLASS(add_channel, "n:s",
	"Add an empty channel for network traffic to flow through.", Channel_InterfaceP);
METHOD_CLASS(remove_channel, "n:s",
	"Remove an existing network traffic channel.", Channel_InterfaceP);
METHOD_CLASS(activate_channel, "n:s",
	"Start traffic in all segments contained in the channel.", Channel_InterfaceP);
METHOD_CLASS(pause_channel, "n:s",
	"Stop traffic in all segments contained in the channel.", Channel_InterfaceP);
METHOD_CLASS(list_channels, "A:n",
	"Return a list of all channel names.", Channel_InterfaceP);
METHOD_CLASS(add_segment, "s:sss",
	"As a new segment to an existing network traffic channel.", Channel_InterfaceP);
METHOD_CLASS(remove_segment, "n:ss",
	"Remove an existing segment from a network traffic channel.", Channel_InterfaceP);
METHOD_CLASS(list_segments, "A:s",
	"Return a list of segments in the specified channel.", Channel_InterfaceP);
METHOD_CLASS(get_segment_type, "s:ss",
	"Return the name of the module that the segment originates from.", Channel_InterfaceP);
METHOD_CLASS(segment_exists, "b:ss",
	"Return whether a segment with the specified name exists in the channel.", Channel_InterfaceP);

} // namespace nasaCE

#endif // _CHANNEL_INTERFACE_HPP_
