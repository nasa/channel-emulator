/* -*- C++ -*- */

//=============================================================================
/**
 * @file   ipv4_pkt_test.cpp
 * @author Tad Kollar <>
 *
 * $Id: ipv4_pkt_test.cpp 1670 2012-08-28 13:25:28Z tkollar $ 
 * Copyright (c) 2008.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "IPv4Packet.hpp"
#include "IPv4Addr.hpp"
#include <iostream>
#include <iomanip>

using namespace nasaCE;
using namespace std;

void print_info(IPv4Packet* pkt) {
	cout << endl
		 << "*** IPv4 PACKET BREAKDOWN ***" << endl
		 << "   Packet Type: " << pkt->typeStr() << endl
		 << "    IP Version: " << dec << (int) pkt->getVersion() << endl
		 << "  Header Bytes: " << dec << pkt->getHeaderLength() << endl
		 << "     TOS Value: " << hex << (int) pkt->getTOS() << endl
		 << "  Total Length: " << dec << pkt->getTotalLength() << endl
		 << "      Identity: " << hex << pkt->getIdent() << endl
		 << "         Flags: " << hex << pkt->getFlags() << endl
		 << "        Offset: " << dec << pkt->getOffset() << endl
		 << "           TTL: " << dec << (int) pkt->getTTL() << endl
		 << " Protocol Type: " << hex << (int) pkt->getProtocol() << endl
		 << "      Checksum: " << hex << pkt->getChecksum() << endl
		 << "     Source IP: " << pkt->getSrcIP().get_quad() << endl
		 << "      Dest. IP: " << pkt->getDstIP().get_quad() << endl
		 << "   Data Length: " << dec << pkt->getDataLength() << endl
		 << "  Is Fragment?: " << ((pkt->isFragment())? "Yes" : "No") << endl
		 << "Checksum Okay?: " << ((pkt->hasValidChecksum())? "Yes" : "No") << endl;
	 
	cout << endl;
};

int main() {
	// An IPv4 packet containing a short ICMP payload.
	ACE_UINT8 ipv4_buf_icmp[84] = {
		0x45, 0x00, 0x00, 0x54, 0x00, 0x00,
		0x40, 0x00, 0x40, 0x01, 0x22, 0xB1, 0x0A, 0xFA, 0x01, 0x02, 0x0A, 0xFA,
		0x01, 0x03, 0x08, 0x00, 0x16, 0xFF, 0xB8, 0x6B, 0x00, 0x01, 0xA7, 0x63,
		0x59, 0x48, 0x00, 0x00, 0x00, 0x00, 0x5B, 0x15, 0x0E, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,
		0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25,
		0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30, 0x31,
		0x32, 0x33, 0x34, 0x35, 0x36, 0x37
	};
	
	IPv4Packet* ipv4_icmp = new IPv4Packet(84, ipv4_buf_icmp);
	
	ipv4_icmp->dump();
	print_info(ipv4_icmp);
	
	// Test packet generation, fragmentation, and defragmentation with a fake payload.
	ACE_UINT8 big_buf[5000];

	for ( int i = 0; i < 5000; i++ ) big_buf[i] = i & 0xff;
	NetworkData *big_data = new NetworkData(5000, big_buf);
	IPv4Addr src("192.168.100.1"), dst("192.168.100.2");
	IPv4Packet* ipv4_built = new IPv4Packet();
	
	ipv4_built->build(src, dst, big_data);
	ipv4_built->setChecksum(ipv4_built->computeChecksum());
	print_info(ipv4_built);
	
	std::vector<IPv4Packet*> ipv4_frags;
	ipv4_built->fragment(ipv4_frags, 1000);
	
	std::vector<IPv4Packet*>::iterator pos;
	
	cout << "*** FRAGMENT PACKET INFO ***" << endl;
	for ( pos = ipv4_frags.begin(); pos != ipv4_frags.end(); pos++ ) {
 		print_info(*pos);
	}
	
	IPv4Packet* ipv4_defragged = ipv4_frags.back();
	
	// Cheat a little by using the original buffer. Normally, we'd extract
	// the data from each packet we've received, but this at least tests
	// the functionality contained within IPv4Packet.
	ipv4_defragged->defragment(big_buf, false);

	cout << "*** DEFRAGMENTED PACKET INFO ***" << endl;
	print_info(ipv4_defragged);
	
	// These should be identical.
	cout << "Original and defragmented packets are ";
	if ( *ipv4_built != *ipv4_defragged ) cout << "DIFFERENT (bad)." << endl;
	else cout << "IDENTICAL (good)." << endl;
	
	// These should be different.
	cout << "ICMP and big 'fake' packets are ";
	if ( *ipv4_built != *ipv4_icmp ) cout << "DIFFERENT (good)." << endl;
	else cout << "IDENTICAL (bad)." << endl;
		
	return 0;
}