/* -*- C++ -*- */

//=============================================================================
/**
 * @file   Device_Interface.hpp
 * @author Tad Kollar  
 *
 * $Id: Device_Interface.hpp 2318 2013-08-05 18:32:49Z tkollar $
 * Copyright (c) 2010.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _DEVICE_INTERFACE_HPP_
#define _DEVICE_INTERFACE_HPP_

#include "CE_Macros.hpp"
#include "DeviceRegistry.hpp"
#include <xmlrpc-c/xml.hpp>

namespace nasaCE {

//=============================================================================
/**
 * @class Device_Interface
 * @brief Provide an XML-RPC interface to the channel management structure.
*/
//=============================================================================

class Device_Interface : public CE_DLL {
public:
    Device_Interface(xmlrpc_c::registryPtr& rpcRegistry);

    ~Device_Interface() { }

	void list_devices(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);
	void get_device_type(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);
	void exists(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);

	/// Export specified methods via XML-RPC.
	void register_methods(xmlrpc_c::registryPtr&);
}; // class Device_Interface

extern Device_Interface* Device_InterfaceP;

METHOD_CLASS(list_devices, "A:n",
	"Return an array of all device names.", Device_InterfaceP)
METHOD_CLASS(get_device_type, "s:s",
	"Return the type of device associated with the provided name.", Device_InterfaceP)
METHOD_CLASS(exists, "b:s",
	"Determine whether a device with the specified name exists.", Device_InterfaceP)

} // namespace nasaCE

#endif // _DEVICE_INTERFACE_HPP_

