/* -*- C++ -*- */

//=============================================================================
/**
 * @file    utils.cpp
 * @author Tad Kollar  
 *
 * $Id: utils.cpp 1670 2012-08-28 13:25:28Z tkollar $ 
 * Copyright (c) 2008.
 *      NASA Glenn Research Center.  All rights reserved.
 */
//=============================================================================

#include "utils.hpp"

#include <fstream>
#include <iostream>
#include <list>

#include <boost/regex.hpp>

#include <ace/Log_Msg.h>
#include <ace/Pipe.h>
#include <ace/Process_Manager.h>
#include <ace/OS_main.h>
#include <ace/Basic_Types.h>
#include <ace/OS_NS_arpa_inet.h>

namespace nasaCE {

std::string read_one_liner(const std::string& file_name) {
	char buffer[1025];
	ifstream file;
	
	try {
		file.open(file_name.c_str());
	}
	catch (const std::exception& e) {
		throw std::runtime_error(e.what()); 
	}
	
	file.getline(buffer, 1024);
	file.close();
	
	return std::string(buffer);
}

ACE_UINT32 inet_ston(const std::string& ip_str) {
	boost::match_results<std::string::const_iterator> what;
	in_addr ip_struct;
	
	if (boost::regex_match(ip_str, what, boost::regex(IPV4_QUAD_REGEX))) {
		ACE_OS::inet_aton(ip_str.c_str(), &ip_struct);
	}
	
	return ip_struct.s_addr;
}

std::string inet_ntos(const int ip_int) {
	in_addr ip_struct;
	ip_struct.s_addr = ip_int;
	std::string ip_str(ACE_OS::inet_ntoa(ip_struct));
	return ip_str;
}

ACE_UINT8 revbits8(const ACE_UINT8 a) {
/*	short b = 0;

	b = ( ((a & 0x80) >> 7) + (((a & 0x40) >> 6) << 1) +
		(((a & 0x20) >> 5) << 2) + (((a & 0x10) >> 4) << 3) +
		(((a & 0x08) >> 3) << 4) + (((a & 0x04) >> 2) << 5) +
		(((a & 0x02) >> 1) << 6) + ((a & 0x01) << 7) );
*/
	return ( ((a & 0x80) >> 7) + (((a & 0x40) >> 6) << 1) +
		(((a & 0x20) >> 5) << 2) + (((a & 0x10) >> 4) << 3) +
		(((a & 0x08) >> 3) << 4) + (((a & 0x04) >> 2) << 5) +
		(((a & 0x02) >> 1) << 6) + ((a & 0x01) << 7) );
}

ACE_UINT32 revbits(const ACE_UINT32 a) {
	return ( ((revbits8((a & 0xFF) >> 0)) << 0) +
		((revbits8((a & 0xFF00) >> 8)) << 8) +
		((revbits8((a & 0xFF0000) >> 16)) << 16) +
		((revbits8((a & 0xFF000000) >> 24)) << 24) );
}

ACE_HANDLE get_ctl_fd(void) {
	int s_errno;
	ACE_HANDLE fd;

	fd = ACE_OS::socket(PF_INET, SOCK_RAW, 0);
	if (fd >= 0) return fd;

	s_errno = errno;
	
	fd = ACE_OS::socket(PF_PACKET, SOCK_RAW, 0);
	if (fd >= 0) return fd;
	
	fd = ACE_OS::socket(PF_INET6, SOCK_RAW, 0);
	if (fd >= 0) return fd;
	errno = s_errno;
	
	throw std::runtime_error("Cannot create control socket.");
	return -1;
}

} // namespace nasaCE
