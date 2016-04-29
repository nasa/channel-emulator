/* -*- C++ -*- */

//=============================================================================
/**
 * @file   IpcpPacket.cpp
 * @author Tad Kollar  
 *
 * $Id: IpcpPacket.cpp 1670 2012-08-28 13:25:28Z tkollar $ 
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "IpcpPacket.hpp"
#include <vector>

namespace nasaCE {

std::string IpcpPacket::optTypeStr[OptionTypesCount] = {
	"IP_Addresses",
	"IP_Compression_Protocol",
	"IP_Address"
};

/// If this packet contains an IP-Address option, return the IP
IPv4Addr IpcpPacket::getIP() {
	for (unsigned x = 0; x < countOpts(); x++) {
		PppConfOption* opt = (*this)[x];
		if ( opt->optType == IP_Address ) {
			IPv4Addr ip(to_u32_(opt->optBuf, false));
			delete opt;
			return ip;
		}
		delete opt;
	}
	
	return IPv4Addr(0);
}

void IpcpPacket::addIP(IPv4Addr& ipAddr) {
	ACE_UINT32 ip = ipAddr.get();
	PppConfOption* ipOpt = _buildIPOption(ip);
	appendOption(ipOpt);
	delete(ipOpt);
}

void IpcpPacket::buildWithIP(const ACE_UINT8 code, const ACE_UINT8 identifier, IPv4Addr& ipAddr) {
	std::vector<PppConfOption*> opts;
	opts.push_back(_buildIPOption(ipAddr.get()));
	build(code, identifier, opts);
}

} // namespace nasaCE