/* -*- C++ -*- */

//=============================================================================
/**
 * @file   netdata_test.cpp
 * @author Tad Kollar <>
 *
 * $Id: netdata_test.cpp 1670 2012-08-28 13:25:28Z tkollar $ 
 * Copyright (c) 2008.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "NetworkData.hpp"
#include <iostream>
#include <iomanip>
#include <bitset>
#include <string>
#include <byteswap.h>

using namespace nasaCE;
using namespace std;

void int2revbuf(const ACE_UINT64 val, ACE_UINT8* buf, const int bufLen) {
	int i = 0;
	for ( int x = 8 - bufLen; x < 8; x++ ) {
		buf[i] = *(((ACE_UINT8*) &val) + (7-x));
		// cout << hex << (unsigned) buf[i] << endl;
		i++;
	}
}

int main() {
	NetworkData* alphaData = new NetworkData();
	NetworkData* numData = new NetworkData();
	
	char alphabet[] = "abcdefghijklmnopqrstuvwxyz";
	char numbers[] =  "12345678901234567890123456";
	
	alphaData->replicateBuffer((ACE_UINT8*)alphabet, 26);
	cout << "Alphabet:" << endl;
	alphaData->dump();
	cout << "alphaData creation stamp:     " << dec
		<< alphaData->getTimeStamp().sec() << "."
		<< alphaData->getTimeStamp().usec() << "s" << endl;
			
	numData->replicateBuffer((ACE_UINT8*)numbers, 26);
	cout << "Numbers:" << endl;
	numData->dump();
	
	NetworkData* copiedAlpha = new NetworkData(alphaData);
	cout << "Copied Alphabet:" << endl;
	copiedAlpha->dump();
	
	NetworkData* copiedNums = new NetworkData();
	copiedNums->deepCopy(numData);
	cout << "Copied Numbers:" << endl;
	copiedNums->dump();
	
	cout << endl << "Bit extraction (bits2uint) test: " << endl;
	
	string num1 = "10111011101110111011101110111";
	string buf1 = "11111010101010" + num1 + "00000";
	bitset<29> bits_num1(num1);
	bitset<48> bits_buf1(buf1);
	
	cout << "Bits " << num1 << " (length " << dec << num1.length() << ", value 0x"
		<< hex << bits_num1.to_ulong() << ")" << endl
		<< "embedded as: " << endl
		<< buf1 << " (length " << dec << buf1.length() << ")" << endl;
		
	ACE_UINT8 rev_buf1[6];
	int2revbuf(bits_buf1.to_ulong(), rev_buf1, 6);	
	cout << "Extracted value: 0x" << hex << (unsigned long long) NetworkData::bits2uint(rev_buf1, 14, 29) << endl;

	cout << endl << "Bit storage (uint2bits) test: " << endl;

	ACE_UINT8 buf2[6];
	ACE_UINT32 testVal = 0x555;
	cout << "Storing test value: 0x" << hex << testVal << endl;
	bitset<32> bits_num2(testVal);
	cout << "In binary: " << bits_num2 << endl;
	
	NetworkData::uint2bits(testVal, buf2, 3, 11);
	cout << "New contents of buffer: ";
	for (int x = 0; x < 6; x++) cout << hex << setw(2) << (unsigned) buf2[x] << " ";
	ACE_UINT64 num3 = NetworkData::bits2uint(buf2, 3, 11);
	
	cout << endl << "Re-extracted: 0x"
		<< hex << (unsigned long long) num3 << endl;
	bitset<32> bits_num3(num3);
	cout << "In binary: " << bits_num3 << endl;
	
	cout << dec << ACE_MAXLOGMSGLEN << endl;
	NetworkData empty(2000);
	empty.dump();
}