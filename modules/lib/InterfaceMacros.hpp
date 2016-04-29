/* -*- C++ -*- */

//=============================================================================
/**
 * @file   InterfaceMacros.hpp
 * @author Tad Kollar  
 *
 * $Id: InterfaceMacros.hpp 2068 2013-06-11 18:47:18Z tkollar $
 * Copyright (c) 2010.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _INTERFACE_MACROS_HPP_
#define _INTERFACE_MACROS_HPP_

#include <vector>
#include <map>
#include <xmlrpc-c/base.hpp>

//=============================================================================
/// @def GENERATE_ACCESSORS
/// @brief Create set_* and get_* accessors for the supplied types.
/// @param fn_base_name The name to append to set and get for the newly generated methods.
/// @param wo_accessor The name of the write-only accessor to call.
/// @param ro_accessor The name of the read-only accessor to call.
/// @param cpp_type The C++ type to use.
/// @param xmlrpc_type1 The type to use with paramList (e.g. String, Int)
/// @param xmlrpc_type2 The type to use with xmlrpc_c::value* (e.g. string, int)
//=============================================================================
#define GENERATE_ACCESSORS(fn_base_name, wo_accessor, ro_accessor,\
	cpp_type, xmlrpc_type1, xmlrpc_type2)\
	virtual void set_##fn_base_name(xmlrpc_c::paramList const& paramList,\
		xmlrpc_c::value* retvalP) {\
\
		cpp_type valToSet = (cpp_type) paramList.get##xmlrpc_type1(2);\
\
		find_handler(paramList)->wo_accessor(valToSet);\
\
		*retvalP = xmlrpc_c::value_nil();\
	}\
\
	virtual void get_##fn_base_name(xmlrpc_c::paramList const& paramList,\
		xmlrpc_c::value* retvalP) {\
\
		*retvalP = xmlrpc_c::value_##xmlrpc_type2(find_handler(paramList)->ro_accessor());\
	}

//=============================================================================
/// @def GENERATE_STRING_ACCESSORS
/// @brief Create set_* and get_* accessors for string variables.
/// @param fn_base_name The name to append to set and get for the newly generated methods.
/// @param wo_accessor The name of the write-only accessor to call.
/// @param ro_accessor The name of the read-only accessor to call.
//=============================================================================
#define GENERATE_STRING_ACCESSORS(fn_base_name, wo_accessor, ro_accessor)\
	GENERATE_ACCESSORS(fn_base_name, wo_accessor, ro_accessor, std::string,\
	String, string)

//=============================================================================
/// @def GENERATE_INT_ACCESSORS
/// @brief Create set_* and get_* accessors for 32-bit integer variables.
/// @param fn_base_name The name to append to set and get for the newly generated methods.
/// @param wo_accessor The name of the write-only accessor to call.
/// @param ro_accessor The name of the read-only accessor to call.
//=============================================================================
#define GENERATE_INT_ACCESSORS(fn_base_name, wo_accessor, ro_accessor)\
	GENERATE_ACCESSORS(fn_base_name, wo_accessor, ro_accessor, int,\
	Int, int)

//=============================================================================
/// @def GENERATE_I8_ACCESSORS
/// @brief Create set_* and get_* accessors for 64-bit integer variables.
/// @param fn_base_name The name to append to set and get for the newly generated methods.
/// @param wo_accessor The name of the write-only accessor to call.
/// @param ro_accessor The name of the read-only accessor to call.
//=============================================================================
#define GENERATE_I8_ACCESSORS(fn_base_name, wo_accessor, ro_accessor)\
	GENERATE_ACCESSORS(fn_base_name, wo_accessor, ro_accessor, long long,\
	I8, i8)

//=============================================================================
/// @def GENERATE_BOOL_ACCESSORS
/// @brief Create set_* and get_* accessors for boolean variables.
/// @param fn_base_name The name to append to set and get for the newly generated methods.
/// @param wo_accessor The name of the write-only accessor to call.
/// @param ro_accessor The name of the read-only accessor to call.
//=============================================================================
#define GENERATE_BOOL_ACCESSORS(fn_base_name, wo_accessor, ro_accessor)\
	GENERATE_ACCESSORS(fn_base_name, wo_accessor, ro_accessor, bool,\
	Boolean, boolean)

//=============================================================================
/// @def GENERATE_FLOAT_ACCESSORS
/// @brief Create set_* and get_* accessors for double precision float variables.
/// @param fn_base_name The name to append to set and get for the newly generated methods.
/// @param wo_accessor The name of the write-only accessor to call.
/// @param ro_accessor The name of the read-only accessor to call.
//=============================================================================
#define GENERATE_FLOAT_ACCESSORS(fn_base_name, wo_accessor, ro_accessor)\
	GENERATE_ACCESSORS(fn_base_name, wo_accessor, ro_accessor, double,\
	Double, double)

//=============================================================================
/// @def GENERATE_ACCESSOR_METHODS
/// @brief Create XML-RPC method classes for the named accessors.
/// @param fn_base_name The name to append to set and get for the newly generated methods.
/// @param xmlrpc_type An XML-RPC signature type, like "i", "s", "b".
/// @param short_desc Will be appended to "Sets " and "Returns " in the respective method descriptions.
/// @param ptr_name The name of the handler pointer.
//=============================================================================
#define GENERATE_ACCESSOR_METHODS(fn_base_name, xmlrpc_type, short_desc, ptr_name)\
METHOD_CLASS(set_##fn_base_name, "n:ss" #xmlrpc_type,\
	"Sets " short_desc, ptr_name)\
METHOD_CLASS(get_##fn_base_name, #xmlrpc_type ":ss", \
	"Returns " short_desc, ptr_name)

//=============================================================================
/// @def REGISTER_ACCESSOR_METHODS
/// @brief Generate code to register XML-RPC methods inside a register_methods() definition.
/// @param fn_base_name The name to append to set and get for the newly generated methods.
/// @param module_name The name of the module, used to define the exported name.
/// @param xmlrpc_base_name The name to append to .set and .get for the XML-RPC name.
//=============================================================================
#define REGISTER_ACCESSOR_METHODS(fn_base_name, module_name, xmlrpc_base_name)\
	REGISTER_METHOD(set_##fn_base_name, #module_name ".set" #xmlrpc_base_name)\
	REGISTER_METHOD(get_##fn_base_name, #module_name ".get" #xmlrpc_base_name)

#endif // _INTERFACE_MACROS_HPP_
