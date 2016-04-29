/* -*- C++ -*- */

//=============================================================================
/**
 * @file   common_exceptions.hpp
 * @author Tad Kollar  
 *
 * $Id: common_exceptions.hpp 1670 2012-08-28 13:25:28Z tkollar $ 
 * Copyright (c) 2011.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _NASA_COMMON_EXCEPTIONS_
#define _NASA_COMMON_EXCEPTIONS_

#include <string>
#include "CE_Config.hpp"

namespace nasaCE {

/// @class InvalidIPv4Address
/// @brief Thrown when provided with an improperly formatted IPv4 address.
struct InvalidIPv4Address: public Exception {
	InvalidIPv4Address(const std::string e): Exception(e) { }
};

/// @class InvalidPort
/// @brief Thrown when a port number outside the allowed range is given.
struct InvalidPort: public Exception {
	InvalidPort(const std::string e): Exception(e) { }
};

/// @class LiveSocket
/// @brief An attempt was made to change setting information for a socket that's already open.
struct LiveSocket: public Exception {
	LiveSocket(const std::string e): Exception(e) { }
};

} // namespace nasaCE

#endif // _NASA_COMMON_EXCEPTIONS_