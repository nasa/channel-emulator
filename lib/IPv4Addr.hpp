/* -*- C++ -*- */

//=============================================================================
/**
 * @file   IPv4Addr.hpp
 * @author Tad Kollar  
 *
 * $Id: IPv4Addr.hpp 1670 2012-08-28 13:25:28Z tkollar $ 
 * Copyright (c) 2008.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _IPV4ADDR_HPP_
#define _IPV4ADDR_HPP_

#include <string>
#include <ace/OS_main.h>
#include <ace/Basic_Types.h>

namespace nasaCE {

//=============================================================================
/**
 * @class IPv4Addr
 * @brief Represents IPv4 addresses usefully for channel emulation.
 */
//============================================================================= 
class IPv4Addr {
private:
	/// The network-ordered IP data.
	ACE_UINT32 _addr;

public:
	static const std::string IPv4_Any;

	static bool isProperAddress(const std::string& ip);

	/// Default constructor.
	IPv4Addr(const std::string& ip = IPv4_Any);
	
	IPv4Addr(const ACE_UINT32 ip);
	
	/// Copy constructor
	IPv4Addr(const IPv4Addr& other);
	
	/// Convert the supplied std::string ip_str to network-ordered data and
	/// store it as a private member.
	void set(const std::string& ip_str);
	
	/// Assumed supplied data is in network order and store it internally.
	void set(const ACE_UINT32 ip_int);
	
	/// Take a mask integer in CIDR notation and convert it to a full 32-bit address.
	void set_shorthand_mask(const ACE_UINT32 mask);
	
	/// Take a mask std::string in CIDR notation and convert it to a full 32-bit address.
	void set_shorthand_mask(const std::string& mask);
	
	/// Compute and return a CIDR mask from the stored network-ordered address.
	ACE_UINT8 get_CIDR_mask();
	
	/// Compute and return a CIDR mask in string form.
	std::string get_CIDR_mask_str();
	
	/// Return the stored IP address as network-ordered data.
	ACE_UINT32 get() const;
	
	/// Return the stored IP address as host-ordered data.
	ACE_UINT32 hget() const;
	
	/// Convert the stored IP address to dotted-quad notation.
	/// @return A std::string copy containing the dotted-quad.
	std::string get_quad() const;
	
	/// A convenient way to set the address using a std::string.
	void operator =(const std::string& ip_str);
	
	/// A convenient way to set the address using an integer.
	void operator =(const ACE_INT32 ip_int);
	
	bool operator <(const IPv4Addr& other) const;
}; // class IPv4Addr

struct cmp_ipv4 
{
	bool operator()(IPv4Addr const& a, IPv4Addr const& b);
};

} // namespace nasaCE

#endif // _IPV4ADDR_HPP_
