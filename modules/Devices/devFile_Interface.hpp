/* -*- C++ -*- */

//=============================================================================
/**
 * @file   devFile_Interface.hpp
 * @author Tad Kollar  
 *
 * $Id: devFile_Interface.hpp 1670 2012-08-28 13:25:28Z tkollar $ 
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _NASA_FILE_DEVICE_INTERFACE_HPP_
#define _NASA_FILE_DEVICE_INTERFACE_HPP_

#include "CE_DLL.hpp"
#include "CE_Macros.hpp"
#include "devFile.hpp"

namespace nasaCE {

//=============================================================================
/**
 *	@class devFile_Interface
 *	@author Tad Kollar  
 *	@brief Provide an XML-RPC interface to devFile.
 */
//=============================================================================

class devFile_Interface: public Device_Interface_Templ<devFile> {
public:
	/// Default constructor.
    devFile_Interface();
	
	/// Initialize as well as registering exported methods.
	/// @param rpcRegistry XML-RPC method registry referenced by Abyss. 
	devFile_Interface(xmlrpc_c::registryPtr& rpcRegistry);
	
    ~devFile_Interface();
	
	void manage(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);

	void unmanage(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);
	
	void open_for_reading(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);

	void open_for_writing(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);
	
	void close(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);
	
	void is_open(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);
	
	void set_filename(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);
	
	void get_filename(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);

	/// Export specified methods via XML-RPC.
	virtual void register_methods(xmlrpc_c::registryPtr&);
}; // class devFile_Interface

extern devFile_Interface* devFile_InterfaceP;
DEV_TEMPLATE_METHOD_CLASSES(devFile_Interface, devFile_InterfaceP);

METHOD_CLASS(manage, "n:ss",
	"Begins managing a file as the named device.", devFile_InterfaceP);
METHOD_CLASS(unmanage, "n:s",
	"End management of the named device.", devFile_InterfaceP);
METHOD_CLASS(open_for_reading, "n:s",
	"Open the file managed by the named device for reading.", devFile_InterfaceP);
METHOD_CLASS(open_for_writing, "n:s",
	"Open the file managed by the named device for writing.", devFile_InterfaceP);
METHOD_CLASS(close, "n:s",
	"Close the file managed by the named device.", devFile_InterfaceP);
METHOD_CLASS(is_open, "b:s",
	"Determines whether the file managed by the named device is open.", devFile_InterfaceP);
METHOD_CLASS(set_filename, "n:ss",
	"Set a new filename for the named device.", devFile_InterfaceP);
METHOD_CLASS(get_filename, "s:s",
	"Return the filename managed by the named device.", devFile_InterfaceP);
	
} // namespace nasaCE

#endif // _NASA_FILE_DEVICE_INTERFACE_HPP_
