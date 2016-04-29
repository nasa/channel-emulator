/* -*- C++ -*- */

//=============================================================================
/**
 * @file   devSink_Interface.hpp
 * @author Tad Kollar  
 *
 * $Id: devSink_Interface.hpp 1883 2013-02-20 19:35:17Z tkollar $
 * Copyright (c) 2011.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _NASA_SINK_DEVICE_INTERFACE_HPP_
#define _NASA_SINK_DEVICE_INTERFACE_HPP_

#include "CE_DLL.hpp"
#include "CE_Macros.hpp"
#include "devSink.hpp"

namespace nasaCE {

//=============================================================================
/**
 *	@class devSink_Interface
 *	@author Tad Kollar  
 *	@brief Provide an XML-RPC interface to devSink.
 */
//=============================================================================

class devSink_Interface: public Device_Interface_Templ<devSink> {
public:
	/// Default constructor.
    devSink_Interface();

	/// Initialize as well as registering exported methods.
	/// @param rpcRegistry XML-RPC method registry referenced by Abyss.
	devSink_Interface(xmlrpc_c::registryPtr& rpcRegistry);

    ~devSink_Interface();

	/// Export specified methods via XML-RPC.
	virtual void register_methods(xmlrpc_c::registryPtr&);
}; // class devSink_Interface

extern devSink_Interface* devSink_InterfaceP;
DEV_TEMPLATE_METHOD_CLASSES(devSink_Interface, devSink_InterfaceP);

} // namespace nasaCE

#endif // _NASA_SINK_DEVICE_INTERFACE_HPP_
