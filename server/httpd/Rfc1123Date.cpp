/* -*- C++ -*- */

//=============================================================================
/**
 * @file   Rfc1123Date.cpp
 * @author Tad Kollar  
 *
 * $Id: Rfc1123Date.cpp 1670 2012-08-28 13:25:28Z tkollar $ 
 * Copyright (c) 2010.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "Rfc1123Date.hpp"
#include <ace/OS_NS_time.h>

namespace nasaCE {

Rfc1123Date::Rfc1123Date(const time_t seconds /* = 0 */): _seconds(seconds) {
	if ( ! seconds ) _seconds = ACE_OS::time(0);
}

string Rfc1123Date::convertSecondsToString(const time_t seconds) {
	tm* breakdown = gmtime(&seconds);
	ostringstream os;
	
	os << weekdays[breakdown->tm_wday] << ", " 
		<< setw(2) << setfill('0') << breakdown->tm_mday << " "
		<< months[breakdown->tm_mon] << " " 
		<< breakdown->tm_year + 1900 << " "
		<< setw(2) << setfill('0') << breakdown->tm_hour << ":"
		<< setw(2) << setfill('0') << breakdown->tm_min << ":"
		<< setw(2) << setfill('0') << breakdown->tm_sec << " GMT";
		
	return os.str();
}
	

const string Rfc1123Date::weekdays[7] =
	{ "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
	
const string Rfc1123Date::months[12] =
	{ "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

};