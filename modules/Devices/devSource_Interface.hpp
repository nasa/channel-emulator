/* -*- C++ -*- */

//=============================================================================
/**
 * @file   devSource_Interface.hpp
 * @author Tad Kollar  
 *
 * $Id: devSource_Interface.hpp 1883 2013-02-20 19:35:17Z tkollar $
 * Copyright (c) 2011.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================


#ifndef _NASA_SOURCE_DEVICE_INTERFACE_HPP_
#define _NASA_SOURCE_DEVICE_INTERFACE_HPP_

#include "CE_DLL.hpp"
#include "CE_Macros.hpp"
#include "devSource.hpp"
#include "Device_Interface_Templ.hpp"

namespace nasaCE {

//=============================================================================
/**
 *	@class devSource_Interface
 *	@author Tad Kollar  
 *	@brief Provide an XML-RPC interface to devSource.
 */
//=============================================================================

class devSource_Interface: public Device_Interface_Templ<devSource> {
public:
	/// Default constructor.
    devSource_Interface();

	/// Initialize as well as registering exported methods.
	/// @param rpcRegistry XML-RPC method registry referenced by Abyss.
	devSource_Interface(xmlrpc_c::registryPtr& rpcRegistry);

    ~devSource_Interface();

	void set_pattern(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);

	void get_pattern(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);

	void set_pattern_buffer_size(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);

	void get_pattern_buffer_size(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);

	/// Export specified methods via XML-RPC.
	virtual void register_methods(xmlrpc_c::registryPtr&);
}; // class devSource_Interface

extern devSource_Interface* devSource_InterfaceP;
DEV_TEMPLATE_METHOD_CLASSES(devSource_Interface, devSource_InterfaceP);

METHOD_CLASS(set_pattern, "n:sA",
	"Sets the generated traffic pattern to the byte list in the array.", devSource_InterfaceP);
METHOD_CLASS(get_pattern, "A:s",
	"Returns the current pattern being sent.", devSource_InterfaceP);
METHOD_CLASS(set_pattern_buffer_size, "n:si",
	"Sets the size of the pattern buffer to fill with the pattern.", devSource_InterfaceP);
METHOD_CLASS(get_pattern_buffer_size, "i:s",
	"Gets the size of the pattern buffer currently in use.", devSource_InterfaceP);
} // namespace nasaCE

#endif // _NASA_SOURCE_DEVICE_INTERFACE_HPP_