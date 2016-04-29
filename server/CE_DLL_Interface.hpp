/* -*- C++ -*- */

//=============================================================================
/**
 * @file   CE_DLL_Interface.hpp
 * @author Tad Kollar  
 *
 * $Id: CE_DLL_Interface.hpp 1947 2013-04-10 18:35:23Z tkollar $
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _NASA_CE_DLL_INTERFACE_HPP_
#define _NASA_CE_DLL_INTERFACE_HPP_

#include "CE_Macros.hpp"
#include "CE_Config.hpp"
#include <xmlrpc-c/xml.hpp>

namespace nasaCE {

/**
	@author Tad Kollar  
*/
class CE_DLL_Interface : public CE_DLL {
public:
    CE_DLL_Interface(xmlrpc_c::registryPtr& rpcRegistry);
    ~CE_DLL_Interface();

	void is_loaded(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);
	void list_loaded(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);
	void load(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);
	void unload(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);

	void is_available(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);
	void list_available_devs(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);
	void list_available_segs(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);
	void list_available_macros(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);
	void desc_available(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);
	void refresh_available(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);
	void settings_available(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);
	void propfile_available(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);

	/// Export specified methods via XML-RPC.
	void register_methods(xmlrpc_c::registryPtr&);
}; // class CE_DLL_Interface

extern CE_DLL_Interface* CE_DLL_InterfaceP;

METHOD_CLASS(is_loaded, "b:s",
	"Returns true if the specified module is loaded, otherwise false.", CE_DLL_InterfaceP)
METHOD_CLASS(list_loaded, "A:n",
	"Returns an array containing loaded modules.", CE_DLL_InterfaceP)
METHOD_CLASS(load, "b:s",
	"Returns true if the specified module was not already loaded.", CE_DLL_InterfaceP)
METHOD_CLASS(unload, "b:s",
	"Unloads a module if it's loaded, otherwise does nothing.", CE_DLL_InterfaceP)

METHOD_CLASS(is_available, "b:s",
	"Returns true if the specified module is known and available, otherwise false.", CE_DLL_InterfaceP)
METHOD_CLASS(list_available_devs, "A:n",
	"Returns an array containing known, available device modules.", CE_DLL_InterfaceP)
METHOD_CLASS(list_available_macros, "A:n",
	"Returns an array containing known, available macro modules.", CE_DLL_InterfaceP)
METHOD_CLASS(list_available_segs, "A:n",
	"Returns an array containing known, available channel segment modules.", CE_DLL_InterfaceP)
METHOD_CLASS(desc_available, "s:s",
	"Returns a string describing the available module.", CE_DLL_InterfaceP)
METHOD_CLASS(settings_available, "A:s",
	"Returns an array of structures describing the module settings.", CE_DLL_InterfaceP)
METHOD_CLASS(propfile_available, "s:s",
	"Returns the raw XML properties file of the module.", CE_DLL_InterfaceP)
METHOD_CLASS(refresh_available, "n:n",
	"Rebuild the list of known modules by searching Server.dllPath.", CE_DLL_InterfaceP)

} // namespace nasaCE

#endif // _NASA_CE_DLL_INTERFACE_HPP_
