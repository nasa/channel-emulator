/* -*- C++ -*- */

//=============================================================================
/**
 * @file   utilityMacros.hpp
 * @author Tad Kollar  
 *
 * $Id: utilityMacros.hpp 1356 2011-12-15 19:19:28Z tkollar $ 
 * Copyright (c) 2010.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _UTILITY_MACROS_HPP_
#define _UTILITY_MACROS_HPP_

#include "nd_macros.hpp"

#ifdef DEFINE_DEBUG
#define MOD_DEBUG(FMT, ...)		if (getDebugging()) { ACE_DEBUG(( LM_DEBUG, ND_DEBUG_PREFIX "[%s] " FMT "\n" ND_COLOR_END, getName().c_str(), ## __VA_ARGS__)); }
#else
#define MOD_DEBUG(FMT, ...) { }
#endif

#define MOD_INFO(FMT, ...)		ACE_DEBUG(( LM_INFO, ND_INFO_PREFIX "[%s] " FMT "\n" ND_COLOR_END, getName().c_str(), ## __VA_ARGS__))
#define MOD_NOTICE(FMT, ...)	ACE_DEBUG(( LM_NOTICE, ND_NOTICE_PREFIX "[%s] " FMT "\n" ND_COLOR_END, getName().c_str(), ## __VA_ARGS__))
#define MOD_WARNING(FMT, ...) 	ACE_DEBUG(( LM_WARNING, ND_WARNING_PREFIX "[%s] " FMT "\n" ND_COLOR_END, getName().c_str(), ## __VA_ARGS__))
#define MOD_ERROR(FMT, ...) 	ACE_ERROR(( LM_ERROR, ND_ERROR_PREFIX "[%s] " FMT "\n" ND_COLOR_END, getName().c_str(), ## __VA_ARGS__))
#define MOD_CRITICAL(FMT, ...)	ACE_ERROR(( LM_CRITICAL, ND_CRITICAL_PREFIX "[%s] " FMT "\n" ND_COLOR_END, getName().c_str(), ## __VA_ARGS__))
#define MOD_ALERT(FMT, ...)		ACE_ERROR(( LM_ALERT, ND_ALERT_PREFIX "[%s] " FMT "\n" ND_COLOR_END, getName().c_str(), ## __VA_ARGS__))
#define MOD_EMERGENCY(FMT, ...)	ACE_ERROR(( LM_EMERGENCY, ND_EMERGENCY_PREFIX "[%s] " FMT "\n" ND_COLOR_END, getName().c_str(), ## __VA_ARGS__))

#endif // _UTILITY_MACROS_HPP_