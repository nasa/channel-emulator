/* -*- C++ -*- */

//=============================================================================
/**
 * @file   CE_Macros.hpp
 * @author Tad Kollar  
 *
 * $Id: CE_Macros.hpp 844 2009-12-10 14:24:29Z tkollar $ 
 * Copyright (c) 2008.
 *      NASA Glenn Research Center.  All rights reserved.
 */
//=============================================================================

#ifndef _CE_MACROS_HPP_
#define _CE_MACROS_HPP_

#include "Time_Handler.hpp"
#include <boost/regex.hpp>

/// @def METHOD_CLASS(method_name, method_sig, method_help, obj_ptr)
/// @brief Generate a new class to export a particular method.
/// @param method_name The name of the method as defined by the class interface.
/// @param method_sig A string representing the XML-RPC param and return types.
/// @param method_help A brief string describing the method.
/// @param obj_ptr The name of the pointer to the instance of the class/DLL.
///
/// To export any method in a derived DLL, a new class is required. An instance
/// is registered with the XML-RPC server so it knows the name of the method and
/// how to associate it with internal functionality. Since most of the format
/// of these method classes is identical from one to another, this macro makes
/// their definition less tedious.
#define METHOD_CLASS(method_name, method_sig, method_help, obj_ptr) \
struct method_name##_type: public xmlrpc_c::method {\
\
	~method_name##_type() {\
		ND_DEBUG("Destroying XML-RPC method class %s_type.\n", #method_name);\
	}\
\
	method_name##_type() {\
		_signature = method_sig;\
		_help = method_help;\
	}\
\
	void execute(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* const retvalP) {\
		if (!obj_ptr) throw std::runtime_error("Requested DLL is not active!");\
\
		ND_DEBUG("Executing %s with %d parameters.\n", #method_name, paramList.size());\
		obj_ptr->method_name(paramList, retvalP);\
	}\
};

#define METHOD_CLASS2(main_class_name, method_name, method_sig, method_help, obj_ptr) \
struct main_class_name##_##method_name##_type: public xmlrpc_c::method {\
\
	~main_class_name##_##method_name##_type() {\
		ND_DEBUG("Destroying XML-RPC method class %s_%s_type.\n", #main_class_name, #method_name);\
	}\
\
	main_class_name##_##method_name##_type() {\
		_signature = method_sig;\
		_help = method_help;\
	}\
\
	void execute(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* const retvalP) {\
		if (!obj_ptr) throw std::runtime_error("Requested DLL is not active!");\
\
		ND_DEBUG("Executing %s.%s with %d parameters.\n", #main_class_name, #method_name, paramList.size());\
		obj_ptr->method_name(paramList, retvalP);\
	}\
};

/// @def REGISTER_METHOD(method_name,xmlrpc_name)
/// @brief Generate code to register an instance of a DLL method
/// @param method_name The name of the method as defined by the class interface.
/// @param xmlrpc_name The name of the method to be advertised by XML-RPC.
///
/// Once a new method class is defined, an instance of it has to be created and
/// put into a registry. This macro does that and catches the registration of
/// duplicate methods (and ignores them).
#define REGISTER_METHOD(method_name, xmlrpc_name) \
	const xmlrpc_c::methodPtr method_name##P(new method_name##_type);\
	try { rpcRegistry->addMethod(xmlrpc_name, method_name##P); }\
	catch (const std::exception& e) { \
		ND_ERROR("Registering method %s failed: %s\n", xmlrpc_name, e.what());\
	}\
	catch (...) {\
		ND_NOTICE("Tried to re-register method %s (ignored).\n", xmlrpc_name);\
	}
	
#define REGISTER_METHOD2(dll_namespace, method_name, xmlrpc_name) \
	const xmlrpc_c::methodPtr method_name##P(new dll_namespace::method_name##_type);\
	try { \
		rpcRegistry->addMethod(xmlrpc_name, method_name##P); \
		ND_DEBUG("Successfully registered method %s.\n", xmlrpc_name);\
	}\
	catch (const std::exception& e) { \
		ND_ERROR("Registering method %s failed: %s\n", xmlrpc_name, e.what());\
	}\
	catch (...) {\
		ND_NOTICE("Tried to re-register method %s (ignored).\n", xmlrpc_name);\
	}

#endif // _CE_MACROS_HPP_
