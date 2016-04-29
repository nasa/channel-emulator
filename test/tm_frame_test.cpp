/* -*- C++ -*- */

//=============================================================================
/**
 * @file   tm_frame_test.cpp
 * @author Tad Kollar <>
 *
 * $Id: tm_frame_test.cpp 1670 2012-08-28 13:25:28Z tkollar $
 * Copyright (c) 2012.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "TM_Transfer_Frame.hpp"
#include "printers.hpp"

#include <iostream>
#include <iomanip>

using namespace nasaCE;

void basic_test1() {
	const size_t frameLen = 256;
	TM_Transfer_Frame* frame = new TM_Transfer_Frame(frameLen, 0, 1, 1, 16);

	ACE_UINT8 sbuf[15] = { 83,83,83,83,83,83,83,83,83,83,83,83,83,83,83 };
	ACE_UINT8 dbuf[256] = {
		68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,
		68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,
		68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,
		68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,
		68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,
		68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,
		68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,
		68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,
		68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,
		68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,
		68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,
		68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,
		68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,
		68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,
		68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,
		68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,68
	};

	NetworkData* dataField = new NetworkData(frameLen - frame->getNonDataLength(), dbuf);
	NetworkData* secondaryHead = new NetworkData(15, sbuf);

	TC_Comm_Link_Control_Word* control_word = new TC_Comm_Link_Control_Word;
	control_word->setCOPInEffect();
	control_word->setNoRFAvailFlag();
	control_word->setWaitFlag();
	control_word->setFARMBCounter(2);

	frame->build(0x234, 3, 123, 234, dataField, true, 100, secondaryHead, control_word, 0);

	frame->dump();

	print_tm_transfer_frame_info(frame);

}

int main() {
	basic_test1();



	return 0;
}