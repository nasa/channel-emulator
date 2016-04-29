/* -*- C++ -*- */

//=============================================================================
/**
 * @file   MacAddress.hpp
 * @author Tad Kollar  
 *
 * $Id: MacAddress.hpp 1670 2012-08-28 13:25:28Z tkollar $ 
 * Copyright (c) 2008.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _MAC_ADDRESS_HPP_
#define _MAC_ADDRESS_HPP_

#include <string>

#include <ace/Basic_Types.h>
#include <ace/OS_main.h>
#include <ace/OS_Memory.h>

namespace nasaCE {

/**
 * @class MacAddress
 *
 * @brief Contains an Ethernet hardware address and provides several operations for it.
 */
class MacAddress {
private:
	/// The 6-byte MAC address array.
	ACE_UINT8 _address[6];
	
public:

	/// Default constructor.
	MacAddress() { ACE_OS::memset(_address, 0, 6); }
	
	/// Constructor that converts a string to the MAC address
	MacAddress(const std::string&);
	
	/// Constructor that passes a 6-byte array as the MAC address.
	MacAddress(ACE_UINT8*);
	
	/// Copy constructor.
	MacAddress(const MacAddress& other) { ACE_OS::memcpy(_address, other._address, 6); }
	
	/// Convert an array of bytes to a string.
	/// @param addr The start of the 6-byte buffer containing the address.
	/// @return A string in the format 01:23:45:67:89:AB.
	static std::string addr2str(const ACE_UINT8* addr);
	
	/// Convert a string to a 6-byte array and store in a pre-allocated buffer.
	/// @param macStr A string in the format 01:23:45:67:89:AB.
	/// @param buffer The start of the pre-allocated buffer where the address will be stored.
	static void str2addr(const std::string& macStr, ACE_UINT8* buffer);
	
	/// Converts the contained MAC address to a string via the static addr2str().
	std::string addr2str() const { return addr2str(_address); }
	
	/// Returns a pointer to the contained 6-byte address array.
	ACE_UINT8* address() { return _address; }
	
	/// Randomly generate 4 of 5 bytes in the address.
	/// @param addr1 The 2nd octet can be changed from 0xFA by specifying this.
	/// @param doSeed If true, seed the random number generator with the current time in seconds.
	void generateFake(const ACE_UINT8 addr1 = 0xFA, const bool doSeed = true);
	
	/// Compares the contained address array to another.
	bool operator == (const MacAddress& other) const {
		return ( ! ACE_OS::memcmp(_address, other._address, 6) );
	}
	
	/// Negatively compares the contained address array to another.
	bool operator != (const MacAddress& other) {
		return ( ACE_OS::memcmp(_address, other._address, 6) );
	}
	
	/// Copies another MacAddress 6-byte array.
	MacAddress& operator = (const MacAddress& other) {
		ACE_OS::memcpy(_address, other._address, 6);
		return *this;
	}
	
	/// Sets the address by extracting it from a string in the format 11:22:33:44:55:66.
	MacAddress& operator = (const std::string& addrStr) {
		str2addr(addrStr, _address);
		return *this;
	}
	
	/// Returns a MacAddress initialized to 00:00:00:00:00:00.
	static MacAddress empty() { return MacAddress(); }
	
	/// Returns the string FF:FF:FF:FF:FF:FF.
	static std::string broadcastStr() { return "FF:FF:FF:FF:FF:FF"; }
	
	/// Returns a MacAddress initialized with broadcastStr().
	static MacAddress broadcast() { return MacAddress(broadcastStr()); }
};

} // namespace nasaCE

#endif // _MAC_ADDRESS_HPP_
