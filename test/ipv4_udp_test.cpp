/* -*- C++ -*- */

//=============================================================================
/**
 * @file   ipv4_udp_test.cpp
 * @author Tad Kollar <>
 *
 * $Id: ipv4_udp_test.cpp 1670 2012-08-28 13:25:28Z tkollar $ 
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "IPv4_UDP_Datagram.hpp"
#include "IPv4Addr.hpp"
#include <iostream>
#include <iomanip>
#include <cstdlib>

using namespace nasaCE;
using namespace std;

int main() {
	NetworkData* data;
	IPv4Addr src("192.168.100.1"), dst("192.168.100.2");
	
	unsigned total = 0, bad = 0;
	
	srand48(time(0));
	
	for (int bufsize = 0; bufsize < 10000; bufsize++) {
		++total;
		data = new NetworkData(bufsize);
		for (int idx = 0; idx < bufsize; idx++) {
			*(data->ptrBuffer() + idx) = lrand48() % 0xFF;
		}
		
		IPv4_UDP_Datagram* dgm = new IPv4_UDP_Datagram();
		dgm->build(src, 4321, dst, 1234, data);
		
		dgm->setChecksum(dgm->computeChecksum());				
		dgm->setUDPChecksum(dgm->computeUDPChecksum());
		
		if ( ! dgm->hasValidUDPChecksum() ) {
			++bad;
			dgm->dump();
			cerr << "Datagram with data field length " << bufsize << " has invalid checksum:" << endl
				<< "  UDPLength: " << dec << dgm->getUDPLength() << endl
				<< "  DataLength: " << dgm->getDataLength() << endl
				<< "  Stored CRC: " << hex << setw(4) << setfill('0') << dgm->getUDPChecksum() << endl
				<< "  Computed CRC: " << hex << setw(4) << setfill('0') << dgm->computeUDPChecksum() << endl
				<< " ---------------- " << endl;
				

		}
			
		data->release();
		dgm->release();
	}
	
	cout << "Datagrams generated: " << dec << total << endl
		<< "Bad checksums: " << bad << endl;

}