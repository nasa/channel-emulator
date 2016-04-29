/* -*- C++ -*- */

//=============================================================================
/**
 * @file   IPv4Addr.cpp
 * @author Tad Kollar  
 *
 * $Id: IPv4Addr.cpp 1670 2012-08-28 13:25:28Z tkollar $ 
 * Copyright (c) 2011.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "IPv4Addr.hpp"
#include "utils.hpp"
#include <ace/OS_NS_stdlib.h>
#include <bitset>
#include <boost/regex.hpp>
#include <arpa/inet.h>

namespace nasaCE {

const std::string IPv4Addr::IPv4_Any("0.0.0.0");

bool IPv4Addr::isProperAddress(const std::string& ip) {
	boost::regex ipv4re(IPV4_FULL_REGEX);
	return boost::regex_match(ip, ipv4re);
}

IPv4Addr::IPv4Addr(const std::string& ip /* = "0.0.0.0" */): _addr(inet_ston(ip)) {
		// ACE_Object_Manager::at_exit(this);
}

IPv4Addr::IPv4Addr(const ACE_UINT32 ip): _addr(ip) { }

IPv4Addr::IPv4Addr(const IPv4Addr& other): _addr(other.get()) { }

void IPv4Addr::set(const std::string& ip_str) { _addr = inet_ston(ip_str); }

void IPv4Addr::set(const ACE_UINT32 ip_int) { _addr = ip_int; }

void IPv4Addr::set_shorthand_mask(const ACE_UINT32 mask) { 
	_addr = revbits((0xffffffff << mask ) ^ 0xffffffff);
}

void IPv4Addr::set_shorthand_mask(const std::string& mask) {
	set_shorthand_mask(ACE_OS::atoi(mask.c_str()));
}

ACE_UINT8 IPv4Addr::get_CIDR_mask() {
	std::bitset<32> cidr(_addr);
	
	return cidr.count(); // Although the bits are on the wrong side, the count doesn't change.
}

std::string IPv4Addr::get_CIDR_mask_str() {
	char buf[33];
	return std::string(ACE_OS::itoa(get_CIDR_mask(), buf, 10));
}

ACE_UINT32 IPv4Addr::get() const { return _addr; }

ACE_UINT32 IPv4Addr::hget() const { return ntohl(_addr); }	

std::string IPv4Addr::get_quad() const { return inet_ntos(_addr); }

void IPv4Addr::operator =(const std::string& ip_str) { set(ip_str); }

void IPv4Addr::operator =(const ACE_INT32 ip_int) { set(ip_int); }

bool IPv4Addr::operator <(const IPv4Addr& other) const { return _addr < other._addr; }

bool cmp_ipv4::operator()(IPv4Addr const& a, IPv4Addr const& b) {
	return a < b;
}

} // namespace nasaCE