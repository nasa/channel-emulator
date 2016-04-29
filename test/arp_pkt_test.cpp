/* -*- C++ -*- */

//=============================================================================
/**
 * @file   arp_pkt_test.cpp
 * @author Tad Kollar <>
 *
 * $Id: arp_pkt_test.cpp 1670 2012-08-28 13:25:28Z tkollar $ 
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "ArpPacket.hpp"
#include "MacAddress.hpp"

#include <iostream>
#include <iomanip>

using namespace nasaCE;
using namespace std;

void print_info(ArpPacket* pkt) {
	cout << endl
		 << "  HW Addr Type: " << hex << uppercase << setw(4) << setfill('0') << pkt->hardwareAddrType() << endl
		 << "Prot Addr Type: " << hex << uppercase << setw(4) << setfill('0') << pkt->protocolAddrType() << endl
		 << "  HW Addr Size: " << (int) pkt->hardwareAddrSize() << endl
		 << "Prot Addr Size: " << (int) pkt->protocolAddrSize() << endl
		 << "     Operation: " << pkt->operation() << endl
		 << "     Is Reply?: " << ((pkt->isReply())? "Yes" : "No") << endl
		 << "   Is Request?: " << ((pkt->isRequest())? "Yes" : "No") << endl
		 << "    Sender MAC: " << pkt->senderMAC().addr2str() << endl
		 << "     Sender IP: " << pkt->senderIP().get_quad() << endl
		 << "    Target MAC: " << pkt->targetMAC().addr2str() << endl
		 << "     Target IP: " << pkt->targetIP().get_quad() << endl
		 << "IsExpectedType: " << ((pkt->isExpectedType())? "Yes" : "No" ) << endl;
	 
	cout << endl;
};

int main() {
	ACE_UINT8 arp1_buf[46] = {
		0x00, 0x01, 0x08, 0x00, 0x06, 0x04, 0x00, 0x01, 0x00, 0x30, 0x48, 0x57, 0x6C, 0x27,	0x0A, 0xFA,
		0x01, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0A, 0xFA, 0x01, 0x03, 0x00, 0x00,	0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	};

	cout << "Testing ARP request packet..." << endl;
	ArpPacket* arp1 = new ArpPacket();
	arp1->replicateBuffer(arp1_buf, 46);
	arp1->dump();
	print_info(arp1);
	
	ACE_UINT8 arp2_buf[46] = {
		0x00, 0x01, 0x08, 0x00, 0x06, 0x04, 0x00, 0x02, 0x00, 0x30, 0x48, 0x57, 0x6C, 0x3F, 0x0A, 0xFA,
		0x01, 0x03, 0x00, 0x30, 0x48, 0x57, 0x6C, 0x27, 0x0A, 0xFA, 0x01, 0x02, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	};

	cout << "Testing ARP reply packet..." << endl;
	ArpPacket* arp2 = new ArpPacket();
	arp2->replicateBuffer(arp2_buf, 46);
	arp2->dump();
	print_info(arp2);

	cout << "Build a reply to the first packet that SHOULD match the second packet..." << endl;
	
	ArpPacket* arp3 = new ArpPacket();
	MacAddress replyMac("00:30:48:57:6C:3F");
	arp3->buildReply(arp1, replyMac);
	arp3->dump();
	print_info(arp3);
	cout << "Comparing buffers with memcmp: "
		<< ((ACE_OS::memcmp(arp2->ptrUnit(), arp3->ptrUnit(), 46))? "MISMATCH!" : "MATCH!")
		<< endl << endl;
		
	cout << "Build a request from scratch that SHOULD match the first packet..." << endl;
	IPv4Addr myIP("10.250.1.2"), reqIP("10.250.1.3");
	MacAddress myMac("00:30:48:57:6C:27");
	ArpPacket* arp4 = new ArpPacket();
	arp4->buildRequest(reqIP, myMac, myIP);
	arp4->dump();
	print_info(arp4);
	cout << "Comparing buffers with memcmp: "
		<< ((ACE_OS::memcmp(arp1->ptrUnit(), arp4->ptrUnit(), 46))? "MISMATCH!" : "MATCH!")
		<< endl;	
		
	return 0;
}
