/* -*- C++ -*- */

//=============================================================================
/**
 * @file   aos_frame_test.cpp
 * @author Tad Kollar <>
 *
 * $Id: aos_frame_test.cpp 1670 2012-08-28 13:25:28Z tkollar $ 
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "AOS_Transfer_Frame.hpp"
#include "AOS_Multiplexing_PDU.hpp"
#include "AOS_Bitstream_PDU.hpp"
#include "EncapsulationPacket.hpp"
#include "IPv4_UDP_Datagram.hpp"
#include "printers.hpp"

#include <iostream>
#include <iomanip>

using namespace nasaCE;

int main() {
	ACE_UINT8 real_frame_buf[] = {
		0x6a, 0xc1, 0x00, 0x01, 0x57, 0x00, 0x00, 0x00, 0xea, 0x00, 0x01, 0x8d, 0x21, 0x45, 0x00, 0x01,
		0x88, 0x00, 0x00, 0x40, 0x00, 0x3f, 0x11, 0x8c, 0xc3, 0xc0, 0xa8, 0x64, 0x28, 0xc0, 0xa8, 0xc8,
		0x28, 0x75, 0x30, 0x75, 0x31, 0x01, 0x74, 0x00, 0x00, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88,
		0x77, 0x88, 0x88, 0x88, 0x77, 0x88, 0x88, 0x88, 0x88, 0x88, 0x77, 0x88, 0x88, 0x88, 0x88, 0x77,
		0x77, 0x55, 0x44, 0x44, 0x44, 0x44, 0x33, 0x44, 0x33, 0x44, 0x44, 0x55, 0x55, 0x66, 0x55, 0x88,
		0x88, 0x88, 0x88, 0x88, 0x88, 0x77, 0x66, 0x77, 0x66, 0x66, 0x77, 0x66, 0x77, 0x66, 0x66, 0x66,
		0x55, 0x55, 0x66, 0x66, 0x66, 0x77, 0x88, 0x88, 0x88, 0x77, 0x88, 0x88, 0x77, 0x77, 0x77, 0x77,
		0x77, 0x66, 0x77, 0x77, 0x77, 0x77, 0x77, 0x88, 0x88, 0x88, 0x88, 0x77, 0x88, 0x77, 0x93, 0xfd
	};

	cout << "Received frame: " << endl;
	AOS_Transfer_Frame* aos = new AOS_Transfer_Frame(128, real_frame_buf, false, 0, false, true);
	cout << "Reference count before wrapping: " << aos->reference_count() << endl;


	// Create wrappers but don't duplicate the buffer at all.
	AOS_Multiplexing_PDU* aos_m_pdu = aos->wrapInnerPDU<AOS_Multiplexing_PDU>();
	EncapsulationPacketWithIPE* encap_pkt = aos_m_pdu->wrapInnerPDU<EncapsulationPacketWithIPE>();
	IPv4_UDP_Datagram* dgm = encap_pkt->wrapInnerPDU<IPv4_UDP_Datagram>();
	NetworkData* data = dgm->wrapInnerPDU<NetworkData>();
	
	cout << "Reference count after wrapping: " << aos->reference_count() << endl;
	
	// Dump data after all the wrapping is done, to make sure that
	// the rd_ptrs and wr_ptrs stayed in the right places.
	aos->dump();
	print_transfer_frame_info(aos);
	// delete aos;
	
	aos_m_pdu->dump(); 
	print_mpdu_info(aos_m_pdu);
	delete aos_m_pdu;
	cout << "Reference count after 1st delete: " << aos->reference_count() << endl;
	
	encap_pkt->dump();
	print_encap_info(encap_pkt);	
	delete encap_pkt;
	cout << "Reference count after 2nd delete: " << aos->reference_count() << endl;
	
	dgm->dump();
	print_udp_info(dgm);	
	delete dgm;
	cout << "Reference count after 3rd delete: " << aos->reference_count() << endl;
	
	data->dump();
	
	// data->release();
		
/*	NetworkData* data = aos->
		wrapInnerPDU<AOS_Multiplexing_PDU>()->
		wrapInnerPDU<EncapsulationPacket>()->
		wrapInnerPDU<IPv4_UDP_Datagram>()->
		wrapInnerPDU<NetworkData>();
	data->dump(); */
	
	cout << "Built frame: " << endl;	
	// Build a new transfer frame. Should match the first one.
	// Allocate the data buffer.
	NetworkData* data2 = new NetworkData(data);

	// Add the IP/UDP header
	IPv4_UDP_Datagram* dgm2 = new IPv4_UDP_Datagram();
	IPv4Addr src("192.168.100.40"), dst("192.168.200.40");
	dgm2->build(src, 30000, dst, 30001, data2);
	dgm2->setTotalLength(392);
	dgm2->setUDPLength(372);
	dgm2->setChecksum(dgm2->computeChecksum());
	dgm2->setUDPChecksum(0);
	
	// Add the Encapsulation Packet header
	EncapsulationPacketWithIPE* encap_pkt2 = new EncapsulationPacketWithIPE();
	encap_pkt2->build(2, 2, 397, dgm2, 0x21);
	encap_pkt2->dump();
	
	// Add the AOS M_PDU
	AOS_Multiplexing_PDU* aos_m_pdu2 = new AOS_Multiplexing_PDU();
	aos_m_pdu2->build(0, encap_pkt2);
	delete encap_pkt2;
	
	// Finally add the transfer frame
	AOS_Transfer_Frame* aos2 = new AOS_Transfer_Frame(false, 0, false, true);
	aos2->build(0xAB, 1, 343, false, false, 0, 2, aos_m_pdu2);
	delete aos_m_pdu2;
	
	aos2->dump();
	print_transfer_frame_info(aos2);
	
	cout << "Result of memcmp on data buffers: " << endl;
	cout << (ACE_OS::memcmp(aos->ptrData(), aos2->ptrData(), aos->getDataLength())? "Different!" : "Perfect Match!")
		<< endl << endl;
	
	delete aos;
	delete aos2;
	
	cout << "Putting a " << dec << dgm2->getUnitLength() << " octet datagram into an encap w/out IPE: " << endl;
	EncapsulationPacketPreIPE* encap_no_ipe = new EncapsulationPacketPreIPE();
	encap_no_ipe->build(EncapsulationPacketPreIPE::IPv4, 3, 0, dgm2, 14, 15, 0xBEEF);
	encap_no_ipe->dump();
	delete dgm2;
	
	
	return 0;
	
};