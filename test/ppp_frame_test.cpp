/* -*- C++ -*- */

//=============================================================================
/**
 * @file   ppp_frame_test.cpp
 * @author Tad Kollar <>
 *
 * $Id: ppp_frame_test.cpp 1670 2012-08-28 13:25:28Z tkollar $ 
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

/*
#undef ACE_NTRACE
#define ACE_NTRACE 0 
*/
#include "PppFrame.hpp"
#include "PppOptions.hpp"
#include "LcpPacket.hpp"
#include "IpcpPacket.hpp"
#include "LcpFsm.hpp"
#include <iostream>
#include <iomanip>

using namespace std;
using namespace nasaCE;

int write_fd, read_fd;
struct PPPcon {
	
	LcpFsm lcpFsm;
	
	PPPcon() {

	}
};

void print_ppp_info(PppFrame* frame) {
	NetworkData* nd = frame;

	cout << endl
		 << "    Start Flag: " << hex << uppercase << setw(2) << setfill('0') << (int) frame->startFlagVal() << endl
		 << "Address Length: " << (int) frame->getAddressLength() << endl
		 << " Address Value: " << hex << uppercase << setw(2 * frame->getAddressLength()) << setfill('0') << frame->addressVal() << endl
		 << "Control Length: " << (int) frame->getControlLength() << endl
		 << " Control Value: " << hex << uppercase << setw(2 * frame->getControlLength()) << setfill('0') << frame->controlVal() << endl
		 << "Protocol Value: " << hex << uppercase << setw(4) << setfill('0') << frame->protocolVal() << endl
		 << "   Data Length: " << dec << (int) frame->dataLength() << endl
		 << "    FCS Length: " << (int) frame->getFCSLength() << endl
		 << "     FCS Value: " << hex << uppercase << setw(2 * frame->getFCSLength()) << setfill('0') << frame->fcsVal() << endl
		 << "      End Flag: " << hex << uppercase << setw(2) << setfill('0') << (int) frame->endFlagVal() << endl
		 << "    FCS Valid?: " << ((frame->fcsValid())? "Yes" : "No" ) << endl
		 << "        size(): " << dec << (int) frame->getUnitLength() << endl
		 << "   Type String: " << nd->typeStr() << endl
		 << endl;
};

void print_ppp_conf_pkt_info(PppConfPacket* pkt) {
	cout << endl
		<< "   Code Value: " << dec << (int) pkt->codeVal() << endl
		<< "  Code String: " << PppConfPacket::codeTypeStr[pkt->codeVal()] << endl
		<< "   Identifier: " << hex << uppercase << setw(2) << setfill('0') << (int) pkt->identVal() << endl
		<< "Stored Length: " << dec << (int) pkt->lengthVal() << endl
		<< "  Data Length: " << dec << (int) pkt->dataLength() << endl
		<< " Option Count: " << dec << (int) pkt->countOpts() << endl
		<< "       size(): " << dec << (int) pkt->getUnitLength() << endl
		<< "  Type String: " << pkt->typeStr() << endl
		<< endl;
};

void print_ppp_opt_info(PppConfOption* opt) {
	cout << "  Option Type #: " << hex << uppercase << setw(2) << setfill('0') << (int) opt->optType << endl
 		 << "Option Type Str: " << LcpPacket::optTypeStr[opt->optType] << endl
		 << "  Buffer Length: " << (int) opt->bufLen << endl
		 << "Buffer Contents:";
		 
	for (int x = 0; x < opt->bufLen; x++) {
		cout << " " << hex << uppercase << setw(2) << setfill('0') << (int) *((char*)opt->optBuf + x);
	}
	
	cout << endl;
};

int ACE_TMAIN(int, ACE_TCHAR *[]) {

	ACE_UINT8 bogus_buf[16] = { 0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7,
			0x8, 0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF };	
	NetworkData *data1 = new NetworkData(16, bogus_buf);
	PppOptions ppp_opts;
	
	cout << "PPP frame with a bogus payload identified as an LCP packet:" << endl;
	PppFrame *ppp1 = new PppFrame();
	ppp1->build(0xC021, data1);
	ppp1->dump();
	print_ppp_info(ppp1);
	
	cout << "Escaping the same frame: " << endl;
	ppp1->escape(ppp_opts.sending_accm);
	ppp1->dump();
	
	cout << endl << "Unescaping again: " << endl;
	ppp1->unescape();
	ppp1->dump();
	print_ppp_info(ppp1);
	
	cout << endl << "Testing PppConfOption: " << endl;
	vector<PppConfOption*> lcp_opts;
	ACE_UINT8 mru[2] = { 0xF, 0xA };
	lcp_opts.push_back(new PppConfOption(LcpPacket::MRU, 2, mru));
	print_ppp_opt_info(lcp_opts[0]);
	
	cout << endl << "Testing an LCP packet build: " << endl;
	LcpPacket *lcp1 = new LcpPacket();
	lcp1->build(LcpPacket::Configure_Request, 1, lcp_opts);
	lcp1->dump();
	print_ppp_conf_pkt_info(lcp1);
	print_ppp_opt_info(lcp1->optionAt(0));
	
	cout << endl << "Testing an IPCP packet build: " << endl;
	IpcpPacket *ipcp1 = new IpcpPacket();
	IPv4Addr ip1("10.20.30.40");
	ipcp1->buildWithIP(IpcpPacket::Configure_Request, 1, ip1);
	ipcp1->dump();
	print_ppp_conf_pkt_info(ipcp1);
	cout << "Contains IP address: " << ipcp1->getIP().get_quad() << endl;
	
	cout << "PPP frame with 'real' LCP packet:" << endl;
	PppFrame *ppp2 = new PppFrame();
	ppp2->build(0xC021, lcp1);
	ppp2->dump();
	print_ppp_info(ppp2);
	
	// Test with a fake PPP connection
	cout << "Testing thecd . LCP FSM: " << endl;
	PPPcon sideA, sideB;
	
	ACE_UINT8 buf[10000];
	int write_count, read_count;
	int fd[2];
	
	if ( pipe(fd) ) {
		cerr << "Error creating pipe: " << ACE_OS::strerror(errno) << endl;
		exit(1);
	}
	read_fd = fd[0];
	write_fd = fd[1];
	
	if ( (write_count = ACE_OS::write(write_fd, ppp2->ptrUnit(), ppp2->getUnitLength()) < 0 )) {
		cerr << "Write error: " << ACE_OS::strerror(errno) << endl;
		exit(1);
	}
	read_count = ACE_OS::read(read_fd, buf, 10000);
	cout << "Read " << read_count << " bytes." << endl;
	
	PppFrame* ppp3 = new PppFrame();
	
	ppp3->replicateBuffer(buf, read_count);	
	ppp3->dump();	
	print_ppp_info(ppp3);
	
	return 0;
}
