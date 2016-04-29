/* -*- C++ -*- */

//=============================================================================
/**
 * @file   libNetworkData_macros.hpp
 * @author Tad Kollar  
 *
 * $Id: nd_macros.hpp 1670 2012-08-28 13:25:28Z tkollar $ 
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _ND_MACROS_HPP_
#define _ND_MACROS_HPP_

#include <ace/Log_Msg.h>
#include <ace/Process_Mutex.h>
#include <ace/SString.h>

#undef ACE_NTRACE
#define ACE_NTRACE 0

namespace nasaCE {

#define ND_COLOR_LOG 1

#ifdef ND_COLOR_LOG
	
#define ND_DEBUG_PREFIX		ACE_TEXT("%T (%t) \033[0;32m")
#define ND_INFO_PREFIX		ACE_TEXT("%T (%t) \033[32;1m")
#define ND_NOTICE_PREFIX	ACE_TEXT("%T (%t) \033[0;33m")
#define ND_WARNING_PREFIX	ACE_TEXT("%T (%t) \033[33;1m")
#define ND_ERROR_PREFIX		ACE_TEXT("%T (%t) \033[31;1m")
#define ND_CRITICAL_PREFIX	ACE_TEXT("%T (%t) \033[35;1m")
#define ND_ALERT_PREFIX		ACE_TEXT("%T (%t) \033[34;1m")
#define ND_EMERGENCY_PREFIX	ACE_TEXT("%T (%t) \033[36;1m")
#define ND_COLOR_END		ACE_TEXT("\033[0m")

#else

#define ND_DEBUG_PREFIX		ACE_TEXT("DEBUG %T (%t) ")
#define ND_INFO_PREFIX		ACE_TEXT("INFO %T (%t) ")
#define ND_NOTICE_PREFIX	ACE_TEXT("NOTICE %T (%t) ")
#define ND_WARNING_PREFIX	ACE_TEXT("WARNING %T (%t) ")
#define ND_ERROR_PREFIX		ACE_TEXT("ERROR %T (%t) ")
#define ND_CRITICAL_PREFIX	ACE_TEXT("CRITICAL %T (%t) ")
#define ND_ALERT_PREFIX		ACE_TEXT("ALERT %T (%t) ")
#define ND_EMERGENCY_PREFIX	ACE_TEXT("EMERGENCY %T (%t) ")
#define ND_COLOR_END		ACE_TEXT("")

#endif

#ifdef DEFINE_DEBUG
#define ND_DEBUG(FMT, ...)		ACE_DEBUG(( LM_DEBUG, ND_DEBUG_PREFIX FMT ND_COLOR_END, ## __VA_ARGS__))
#else
#define ND_DEBUG(FMT, ...) { }
#endif

#define ND_INFO(FMT, ...)		ACE_DEBUG(( LM_INFO, ND_INFO_PREFIX FMT ND_COLOR_END, ## __VA_ARGS__))
#define ND_NOTICE(FMT, ...)		ACE_DEBUG(( LM_NOTICE, ND_NOTICE_PREFIX FMT ND_COLOR_END, ## __VA_ARGS__))
#define ND_WARNING(FMT, ...) 	ACE_DEBUG(( LM_WARNING, ND_WARNING_PREFIX FMT ND_COLOR_END, ## __VA_ARGS__))
#define ND_ERROR(FMT, ...) 		ACE_ERROR(( LM_ERROR, ND_ERROR_PREFIX FMT ND_COLOR_END, ## __VA_ARGS__))
#define ND_CRITICAL(FMT, ...)	ACE_ERROR(( LM_CRITICAL, ND_CRITICAL_PREFIX FMT ND_COLOR_END, ## __VA_ARGS__))
#define ND_ALERT(FMT, ...)		ACE_ERROR(( LM_ALERT, ND_ALERT_PREFIX FMT ND_COLOR_END, ## __VA_ARGS__))
#define ND_EMERGENCY(FMT, ...)	ACE_ERROR(( LM_EMERGENCY, ND_EMERGENCY_PREFIX FMT ND_COLOR_END, ## __VA_ARGS__))

} // namespace nasaCE

#endif // NASACE_MACROS_HPP
