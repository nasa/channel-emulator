/* -*- C++ -*- */

//=============================================================================
/**
 * @file   MacAddress.cpp
 * @author Tad Kollar  
 *
 * $Id: MacAddress.cpp 1670 2012-08-28 13:25:28Z tkollar $ 
 * Copyright (c) 2008.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================


#include "MacAddress.hpp"

#include <ace/OS_NS_time.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cstdlib>

#include <boost/regex.hpp>

namespace nasaCE {

MacAddress::MacAddress(const std::string& macStr) {
	str2addr(macStr, _address);
}

MacAddress::MacAddress(ACE_UINT8* addrPtr) {
	ACE_OS::memcpy(_address, addrPtr, sizeof(ACE_UINT8) * 6);
}
	
std::string MacAddress::addr2str(const ACE_UINT8* addr) {
	using namespace std;
	ostringstream os;

	for (int x = 0; x < 6; ++x) {
		os << hex << uppercase << setw(2) << setfill('0') << (unsigned) addr[x]; 
		if (x<5) os << ":";
	}
	
	return os.str();
}

void MacAddress::generateFake(const ACE_UINT8 addr1, const bool doSeed /* = true */) {
	_address[0] = 0;
	_address[1] = addr1;
	
	if (doSeed) srand48(ACE_OS::time(0));
	
	for ( int a = 2; a < 6; ++a ) _address[a] = lrand48() % 0x100;
}

void MacAddress::str2addr(const std::string& macStr, ACE_UINT8* buffer) {
	using namespace boost;
	regex mac_re("^([[:xdigit:]]{2}):([[:xdigit:]]{2}):([[:xdigit:]]{2}):([[:xdigit:]]{2}):([[:xdigit:]]{2}):([[:xdigit:]]{2})$");
	match_results<std::string::const_iterator> what;
	
	if ( ! regex_match(macStr, what, mac_re))
		throw std::runtime_error("std::string '" + macStr + "' is not a valid MAC address.");
		
	for ( int i = 0; i < 6; i++) {
		std::string valStr("0x" + what[i+1].str());
		*(buffer + i) = strtol(valStr.c_str(), 0 , 16);
	}	
}

} // namespace nasaCE
