/* -*- C++ -*- */

//=============================================================================
/**
 * @file    CE_DLL_Ops.cpp
 * @author Tad Kollar  
 * @todo Convert DLL functions to use ACE equivalents
 *
 * $Id: CE_DLL_Ops.cpp 1671 2012-08-28 15:19:00Z tkollar $ 
 * Copyright (c) 2008.
 *      NASA Glenn Research Center.  All rights reserved.
 */
//=============================================================================

#include "CE_DLL_Ops.hpp"

#include <ace/OS_main.h>
#include <ace/Reactor.h>
#include <ace/Log_Msg.h>
#include <ace/OS_NS_dlfcn.h>

namespace nasaCE {

void CE_DLL_Ops::load(const std::string& name, const std::string& path,
	xmlrpc_c::registryPtr& rpcRegistry) {
	ND_DEBUG("Loading DLL %s from %s with registry at 0x%X.\n", name.c_str(), path.c_str(), &rpcRegistry);
	if ( _is_loaded) { return; }

	// The RTLD_GLOBAL is important for loading exceptions properly
	_handle = ACE_OS::dlopen(path.c_str(), RTLD_LAZY | RTLD_GLOBAL);
	
	if (!_handle) { throw FailedLoad("Cannot load library " + name + ": " + ACE_OS::dlerror()); }
	
	_name = name;
	_path = path;
	
	// reset errors
	ACE_OS::dlerror();
	
	// load the symbols
	_activate = (activate_t*) ACE_OS::dlsym(_handle, "activate");
	const char* dlsym_error = ACE_OS::dlerror();
	if (dlsym_error) {
		std::string err(dlsym_error);
		throw SymbolNotFound("Cannot load symbol activate: " + err);
	}
	
	_deactivate = (deactivate_t*) ACE_OS::dlsym(_handle, "deactivate");
	dlsym_error = ACE_OS::dlerror();
	if (dlsym_error) {
		std::string err(dlsym_error);
		throw SymbolNotFound("Cannot load symbol deactivate: " + err);
	}
	
	_dllP = _activate(rpcRegistry);
	
	_is_loaded = true;
}

void CE_DLL_Ops::unload() {
	if (! _is_loaded ) { return; }
	
	ND_NOTICE("Unloading symbols for class \"%s\".\n", _name.c_str());
	
	_deactivate();
	
	// Probably should make sure all instances are destroyed first?
	// We can keep a list of created objects but not all references
	ACE_OS::dlclose(_handle);
	// mutex.release();
	
	ND_NOTICE("Handle for module \"%s\" closed.\n", _name.c_str());
}

CE_DLL_Ops::~CE_DLL_Ops() {
	ACE_TRACE("~CE_DLL_Ops");
	
	unload();
	
	_is_loaded = false;
	_name = "";
	_path = "";
	_handle = 0;
	_activate = 0;
	_deactivate = 0;
	_dllP = 0;
}

}