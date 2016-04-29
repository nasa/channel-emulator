/* -*- C++ -*- */

//=============================================================================
/**
 * @file   hdlc_frame_test.cpp
 * @author Tad Kollar <>
 *
 * $Id: hdlc_frame_test.cpp 1670 2012-08-28 13:25:28Z tkollar $ 
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "HdlcFrame.hpp"
#include <iostream>
#include <iomanip>

using namespace nasaCE;
using namespace std;

void print_info(HdlcFrame* frame) {
	cout << endl
		 << "    Start Flag: " << hex << uppercase << setw(2) << setfill('0') << (int) frame->startFlagVal() << endl
		 << "Address Length: " << (int) frame->getAddressLength() << endl
		 << " Address Value: " << hex << uppercase << setw(2 * frame->getAddressLength()) << setfill('0') << frame->addressVal() << endl
		 << "Control Length: " << (int) frame->getControlLength() << endl
		 << " Control Value: " << hex << uppercase << setw(2 * frame->getControlLength()) << setfill('0') << frame->controlVal() << endl
		 << "   Data Length: " << dec << (int) frame->dataLength() << endl
		 << "    FCS Length: " << (int) frame->getFCSLength() << endl
		 << "     FCS Value: " << hex << uppercase << setw(2 * frame->getFCSLength()) << setfill('0') << frame->fcsVal() << endl
		 << "      End Flag: " << hex << uppercase << setw(2) << setfill('0') << (int) frame->endFlagVal() << endl
		 << "    FCS Valid?: " << ((frame->fcsValid())? "Yes" : "No" ) << endl
		 << "        size(): " << dec << (int) frame->getUnitLength() << endl
		 << endl;
};


int main() {
	ACE_UINT8 buf1[16] = { 0x0, 0x1, 0x2, 0x3, 0x4,	0x5, 0x6, 0x7,
			0x8, 0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF };	
	NetworkData *data1 = new NetworkData(16, buf1);	
	
	cout << "Testing HDLC frame with 2 byte addr, 2 byte control, 4 byte FCS..." << endl;
	HdlcFrame* hdlc1 = new HdlcFrame(2, 2, 4);
	hdlc1->build(2, 0xBEEF, 2, 0x1010, data1, 4);
	hdlc1->dump();
	print_info(hdlc1);
	
	cout << "Testing HDLC frame with 1 byte addr, 1 byte control, 2 byte FCS..." << endl;
	HdlcFrame* hdlc2 = new HdlcFrame(1, 1, 2);
	hdlc2->build(1, 0xEE, 1, 0xDD, data1, 2);
	hdlc2->dump();
	print_info(hdlc2);
	
	return 0;
}