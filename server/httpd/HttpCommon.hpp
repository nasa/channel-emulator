/* -*- C++ -*- */

//=============================================================================
/**
 * @file   HttpCommon.hpp
 * @author Tad Kollar  
 *
 * $Id: HttpCommon.hpp 1670 2012-08-28 13:25:28Z tkollar $ 
 * Copyright (c) 2010.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _NASA_HTTP_COMMON_HPP_
#define _NASA_HTTP_COMMON_HPP_

#include "string_nocase.hpp"
#include <map>
#include <string>

namespace nasaCE {

typedef std::map<string_nocase, std::string> HeaderMapType;

};

#endif