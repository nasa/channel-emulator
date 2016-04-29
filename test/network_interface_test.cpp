/* -*- C++ -*- */

//=============================================================================
/**
 * @file   network_interface_test.cpp
 * @author Tad Kollar <>
 *
 * $Id: network_interface_test.cpp 1670 2012-08-28 13:25:28Z tkollar $ 
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "devEthernet.hpp"
#include "NetworkData.hpp"
#include "EthernetFrame.hpp"
#include "Dot1qFrame.hpp"
#include "SettingsManager.hpp"
#include "devEthernet.hpp"

#include <iostream>
#include <iomanip>

#define TEST_IFACE "dummy0"

using namespace std;
using namespace nasaCE;
using namespace Linux;

namespace nasaCE {
	bool exit_requested;
	libconfig::Config global_config;
	#ifdef DEFINE_DEBUG
	bool globalDefaultDebug = true;
	#endif	
}


void send_output(ACE_HANDLE fd) {
	unsigned char garbage[1000];
	
	int written = ACE_OS::write(fd, garbage, 1000);
	if ( written >= 0 ) {
		cout << "Sent " << written << " bytes." << endl;
	}
	else {
		cerr << "Output error: " << (string) ACE_OS::strerror(errno) << endl;
	}
}

void get_input(ACE_HANDLE fd) {
	unsigned char buf[2000];
	
	int read_count = ACE_OS::read(fd, buf, 2000);
		
	if ( read_count < 0 ) {
		cerr << "Input error: " << (string) ACE_OS::strerror(errno) << endl;
	}
	else {
		NetworkData *nd = new NetworkData(read_count, buf);
		cout << "Received " << read_count << " bytes: " << endl;
		nd->dump();
	}
}

void print_vlan_info(Dot1qFrame* frame) {
		cout << "    802.1q Tag: " << hex << uppercase << setw(4) << setfill('0') << frame->getTag() << endl
			 << "   802.1q Type: " << hex << uppercase << setw(4) << setfill('0') << frame->getEther8021qType() << endl;
}

void print_info(EthernetFrame* frame) {
	MacAddress dstAddr(frame->ptrDstAddr()), srcAddr(frame->ptrSrcAddr());

	cout << endl
		 << "    Frame Type: " << frame->typeStr() << endl
//		 << "      Preamble: " << hex << uppercase << setw(4) << setfill('0') << frame->preamble() << endl
//		 << " Preamble Type: " << hex << uppercase << setw(4) << setfill('0') << frame->preambleType() << endl
		 << "      Dst. MAC: " << dstAddr.addr2str() << endl
		 << "      Src. MAC: " << srcAddr.addr2str() << endl
		 << " Ethernet Type: " << hex << uppercase << setw(4) << setfill('0') << frame->getEtherType() << endl
		 << "  Payload Type: " << hex << uppercase << setw(4) << setfill('0') << frame->getPayloadType() << endl
		 << "   Type String: " << frame->getEtherTypeStr() << endl
		 << "Payload Length: " << frame->extractPayloadLength() << endl
		 << "       Is ARP?: " << ((frame->isArp())? "Yes" : "No") << endl
		 << "      Is VLAN?: " << ((frame->is8021q())? "Yes" : "No") << endl;
	 
	if ( frame->is8021q() ) { print_vlan_info((Dot1qFrame*) frame); }
	
	cout << endl;
};

int ACE_TMAIN(int argc, ACE_TCHAR* argv[]) {
	devEthernet* netif = 0;
	
	if ( argc < 2 ) {
		cerr << "Usage: network_interface_test <interface_name>" << endl;
		return 1;
	}
	
	std::string test_iface(argv[1]);

	try {
		netif = new devEthernet(test_iface);
	}
	catch (const Exception& e) {
		cerr << "Unable to create devEthernet for " << test_iface << ": " << e.what() << endl;
		return 1;
	}
	
	netif->activate();
//	netif->pcapInitialize();
	EthernetFrame* eframe = new EthernetFrame(EthernetFrame::PCap);
	
//	(*netif) >> eframe;
	
	eframe->dump();
	print_info(eframe);
	
	netif->deactivate();

	return 0;
};
