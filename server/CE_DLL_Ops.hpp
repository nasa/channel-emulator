/* -*- C++ -*- */

//=============================================================================
/**
 * @file    CE_DLL_Ops.hpp
 * @author Tad Kollar  
 *
 * $Id: CE_DLL_Ops.hpp 1671 2012-08-28 15:19:00Z tkollar $ 
 * Copyright (c) 2008.
 *      NASA Glenn Research Center.  All rights reserved.
 */
//=============================================================================

#ifndef _CE_DLL_OPS_HPP_
#define _CE_DLL_OPS_HPP_

#include "CE_DLL.hpp"
#include <vector>

namespace nasaCE {

struct RegistryError: public Exception {
	RegistryError(const std::string& msg = "") : Exception(msg) {}
};

struct FailedLoad: public RegistryError {
	FailedLoad(const std::string& msg = "") : RegistryError(msg) {}
};

struct SymbolNotFound: public RegistryError {
	SymbolNotFound(const std::string& msg = "") : RegistryError(msg) {}
};

struct DLLNotLoaded: public RegistryError {
	DLLNotLoaded(const std::string& msg = "") : RegistryError(msg) {}
};

// CE_DLL_Ops handles the loading and unloading of classes/symbols derived
// from nasaCE::Module that are stored as dynamically loaded modules. 
class CE_DLL_Ops {
private:
	std::string _name;
	std::string _path;
	void* _handle;
	activate_t* _activate;
	deactivate_t* _deactivate;
	bool _is_loaded;
	CE_DLL* _dllP;
	
public:
	CE_DLL_Ops(): _name(""), _path(""), _handle(0), _activate(0),
		_deactivate(0), _is_loaded(false), _dllP(0) { }
		
	~CE_DLL_Ops();
	
	void load(const std::string&, const std::string&, xmlrpc_c::registryPtr&);
	void unload();
	bool is_loaded() { return _is_loaded; }
	void register_methods(xmlrpc_c::registryPtr& rpcRegistry) { _dllP->register_methods(rpcRegistry); }
	CE_DLL* ptrDLL() { return _dllP; }
};

} // namespace nasaCE

#endif
