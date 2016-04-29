/* -*- C++ -*- */

//=============================================================================
/**
 * @file   Rfc1123Date.hpp
 * @author Tad Kollar  
 *
 * $Id: Rfc1123Date.hpp 1670 2012-08-28 13:25:28Z tkollar $ 
 * Copyright (c) 2010.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _NASA_RFC_1123_DATE_HPP_
#define _NASA_RFC_1123_DATE_HPP_

#include <ace/OS_main.h>
#include <iomanip>
#include <string>
#include <sstream>
#include <ctime>

using namespace std;

namespace nasaCE {

class Rfc1123Date {
public:
	static const string weekdays[7];
	static const string months[12];

	Rfc1123Date(const time_t seconds = 0);

	static string convertSecondsToString(const time_t seconds);
	
	string convertSecondsToString() { return convertSecondsToString(_seconds); }

private:
	time_t _seconds;

};

};

#endif