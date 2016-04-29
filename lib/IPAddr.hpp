/* -*- C++ -*- */

//=============================================================================
/**
 * @file   IPAddr.hpp
 * @author Tad Kollar  
 *
 * $Id: IPAddr.hpp 1670 2012-08-28 13:25:28Z tkollar $ 
 * Copyright (c) 2008.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _IPADDR_HPP_
#define _IPADDR_HPP_

#include "IPv4Addr.hpp"
#include "utils.hpp"

#include <bitset>

#include <boost/regex.hpp>
#include <ace/OS_main.h>
#include <ace/Basic_Types.h>

namespace nasaCE {

/**
 * @class IPAddr
 *
 * @brief Represents complete IP address information.
 */
class IPAddr {
private:
	/// The actual IPv4 address.
	IPv4Addr _ipv4_address;
	
	/// The IPv4 network mask.
	IPv4Addr _ipv4_mask;
	
	/// The IPv4 broadcast address.
	IPv4Addr _ipv4_broadcast;

public:	
	/// Default constructor.
	IPAddr() { /* ACE_Object_Manager::at_exit(this); */ }

	/// Set the address based on dotted-quad std::strings.
	///
	/// The network mask can be either CIDR or dotted-quad notation.
	void set_ipv4_info(const std::string& addr, const std::string& mask, const std::string& broadcast) {
		_ipv4_address.set(addr);

		if (mask.size() <= 2) { _ipv4_mask.set_shorthand_mask(mask); }
		else { _ipv4_mask.set(mask); }
		
		_ipv4_broadcast.set(broadcast);
	}
	
	/// Set the address and net mask based on dotted quad/CIDR notation.
	void set_ipv4_info(const std::string& addr_slash_mask) {

		boost::regex ipv4_re(IPV4_FULL_REGEX + "/(\\d+)");
		boost::match_results<std::string::const_iterator> what;

		if (boost::regex_match(addr_slash_mask, what, ipv4_re)) {
			_ipv4_address.set(what[1]);
			_ipv4_mask.set_shorthand_mask(what[2]);
		}
		autoset_broadcast();
	}
	
	/// Compute the broadcast address from the IP and mask.
	void autoset_broadcast() {
		_ipv4_broadcast.set( (! _ipv4_mask.get()) | _ipv4_address.get() );
	}
	
	/// IPv4 address accessor.
	IPv4Addr& ipv4_address() { return _ipv4_address; }
	
	/// IPv4 mask accessor.
	IPv4Addr& ipv4_mask() { return _ipv4_mask; }
	
	/// IPv4 broadcast accessor.
	IPv4Addr& ipv4_broadcast() { return _ipv4_broadcast; }
	
	/// IPv4 address accessor.
	/// @return std::string containing the IPv4 address, a slash, and the network prefix.
	std::string IPv4CIDR() {
		return _ipv4_address.get_quad() + "/" + _ipv4_mask.get_CIDR_mask_str();
	}
};

} // namespace nasaCE

#endif // _IPADDR_HPP_