/* -*- C++ -*- */

//=============================================================================
/**
 * @file   rs_coding_test.cpp
 * @author Tad Kollar <>
 *
 * $Id: rs_coding_test.cpp 1670 2012-08-28 13:25:28Z tkollar $ 
 * Copyright (c) 2012.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "RSEncoder.hpp"
#include "RSDecoder.hpp"
/* #include "ccsdsRSEncoder.hpp" */

using namespace nasaCE;
using namespace std;

int timerIterations = 1000;

const string line("------------------------------------------");

const Sym aosTF128[] = {
	0x6a, 0xc1, 0x00, 0x01, 0x57, 0x00, 0x00, 0x00, 0xea, 0x00, 0x01, 0x8d, 0x21, 0x45, 0x00, 0x01,
	0x88, 0x00, 0x00, 0x40, 0x00, 0x3f, 0x11, 0x8c, 0xc3, 0xc0, 0xa8, 0x64, 0x28, 0xc0, 0xa8, 0xc8,
	0x28, 0x75, 0x30, 0x75, 0x31, 0x01, 0x74, 0x00, 0x00, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88,
	0x77, 0x88, 0x88, 0x88, 0x77, 0x88, 0x88, 0x88, 0x88, 0x88, 0x77, 0x88, 0x88, 0x88, 0x88, 0x77,
	0x77, 0x55, 0x44, 0x44, 0x44, 0x44, 0x33, 0x44, 0x33, 0x44, 0x44, 0x55, 0x55, 0x66, 0x55, 0x88,
	0x88, 0x88, 0x88, 0x88, 0x88, 0x77, 0x66, 0x77, 0x66, 0x66, 0x77, 0x66, 0x77, 0x66, 0x66, 0x66,
	0x55, 0x55, 0x66, 0x66, 0x66, 0x77, 0x88, 0x88, 0x88, 0x77, 0x88, 0x88, 0x77, 0x77, 0x77, 0x77,
	0x77, 0x66, 0x77, 0x77, 0x77, 0x77, 0x77, 0x88, 0x88, 0x88, 0x88, 0x77, 0x88, 0x77, 0x93, 0xfd
};

const Sym buf256[256] = {
	0x6a, 0xc1, 0x00, 0x01, 0x57, 0x00, 0x00, 0x00, 0xea, 0x00, 0x01, 0x8d, 0x21, 0x45, 0x00, 0x01,
	0x88, 0x00, 0x00, 0x40, 0x00, 0x3f, 0x11, 0x8c, 0xc3, 0xc0, 0xa8, 0x64, 0x28, 0xc0, 0xa8, 0xc8,
	0x28, 0x75, 0x30, 0x75, 0x31, 0x01, 0x74, 0x00, 0x00, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88,
	0x77, 0x88, 0x88, 0x88, 0x77, 0x88, 0x88, 0x88, 0x88, 0x88, 0x77, 0x88, 0x88, 0x88, 0x88, 0x77,
	0x77, 0x55, 0x44, 0x44, 0x44, 0x44, 0x33, 0x44, 0x33, 0x44, 0x44, 0x55, 0x55, 0x66, 0x55, 0x88,
	0x88, 0x88, 0x88, 0x88, 0x88, 0x77, 0x66, 0x77, 0x66, 0x66, 0x77, 0x66, 0x77, 0x66, 0x66, 0x66,
	0x55, 0x55, 0x66, 0x66, 0x66, 0x77, 0x88, 0x88, 0x88, 0x77, 0x88, 0x88, 0x77, 0x77, 0x77, 0x77,
	0x77, 0x66, 0x77, 0x77, 0x77, 0x77, 0x77, 0x88, 0x88, 0x88, 0x88, 0x77, 0x88, 0x77, 0x93, 0xfd,
	0x6a, 0xc1, 0x00, 0x01, 0x57, 0x00, 0x00, 0x00, 0xea, 0x00, 0x01, 0x8d, 0x21, 0x45, 0x00, 0x01,
	0x88, 0x00, 0x00, 0x40, 0x00, 0x3f, 0x11, 0x8c, 0xc3, 0xc0, 0xa8, 0x64, 0x28, 0xc0, 0xa8, 0xc8,
	0x28, 0x75, 0x30, 0x75, 0x31, 0x01, 0x74, 0x00, 0x00, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88,
	0x77, 0x88, 0x88, 0x88, 0x77, 0x88, 0x88, 0x88, 0x88, 0x88, 0x77, 0x88, 0x88, 0x88, 0x88, 0x77,
	0x77, 0x55, 0x44, 0x44, 0x44, 0x44, 0x33, 0x44, 0x33, 0x44, 0x44, 0x55, 0x55, 0x66, 0x55, 0x88,
	0x88, 0x88, 0x88, 0x88, 0x88, 0x77, 0x66, 0x77, 0x66, 0x66, 0x77, 0x66, 0x77, 0x66, 0x66, 0x66,
	0x55, 0x55, 0x66, 0x66, 0x66, 0x77, 0x88, 0x88, 0x88, 0x77, 0x88, 0x88, 0x77, 0x77, 0x77, 0x77,
	0x77, 0x66, 0x77, 0x77, 0x77, 0x77, 0x77, 0x88, 0x88, 0x88, 0x88, 0x77, 0x88, 0x77, 0x93, 0xfd
};

SymVec encode(const Sym msg[], const size_t msgLen, const RSEncoder& encoder) {
	SymVec syms(msg, msgLen);

	cout << "Original msg: " << endl << syms << endl;
	
	SymVec parity = encoder.encode(syms);
	syms.insert(syms.end(), parity.begin(), parity.end());
	
	cout << "Encoded msg: " << endl << syms << endl;	
	
	return syms;
}

SymVec decode(const SymVec codeWord, RSDecoder& decoder) {
	cout << "Encoded/error-ridden msg: " << endl << codeWord << endl;
	
	SymVec orig = decoder.decode(codeWord);
	
	cout << "Decoded msg: " << endl << orig << endl;	
	
	return orig;
}

void encodeTimeTest(const Sym msg[], const size_t msgLen, const RSEncoder& encoder,
	const size_t num = timerIterations) {
	SymVec syms(msg, msgLen);
	
	ACE_Time_Value startTime, endTime, totalTime;
	
	cout << "Timing encoding of " << dec << num << " messages with RS(" << encoder.getCodeLength()
		<< "," << encoder.getCodeLength() - encoder.getParityLength() << ")... " << endl;
	
	startTime = ACE_High_Res_Timer::gettimeofday_hr();
	
	for ( size_t i = 0; i < num; ++i ) encoder.encode(syms); 
	
	endTime = ACE_High_Res_Timer::gettimeofday_hr();
	
	totalTime = endTime - startTime;
	ACE_UINT64 usec;
	totalTime.to_usec(usec);
	
	cout << usec << "us total, " << usec/static_cast<double>(num) << " us/msg, " <<
		num / (usec / 1000000.0) << " msg/s" << endl;
}

/* 
void oldEncodeTimeTest(const size_t num = timerIterations) {
	ccsdsRSEncoder oldEncoder(32, 1);
	
	const int codeWordLen = 160;
	Sym dataBuf[codeWordLen];
	ACE_OS::memset(dataBuf, 0, codeWordLen);
	ACE_OS::memcpy(dataBuf, aosTF128, codeWordLen);
	
	
	ACE_Time_Value startTime, endTime, totalTime;
	
	cout << "Timing old-type encoding of " << dec << num << " messages... " << endl;
	
	startTime = ACE_High_Res_Timer::gettimeofday_hr();
	
	for ( size_t i = 0; i < num; ++i ) oldEncoder.encode(dataBuf, 128, codeWordLen);
	
	endTime = ACE_High_Res_Timer::gettimeofday_hr();
	
	totalTime = endTime - startTime;
	ACE_UINT64 usec;
	totalTime.to_usec(usec);
	
	cout << usec << "us total, " << usec/static_cast<double>(num) << " us/msg, " <<
		num / (usec / 1000000.0) << " msg/s" << endl;
}
*/

void decodeTimeTest(const SymVec codeWord, RSDecoder& decoder,
	const size_t num = timerIterations) {

	ACE_Time_Value startTime, endTime, totalTime;
	
	cout << "Timing decoding of " << dec << num << " messages with RS(" << decoder.getCodeLength()
		<< "," << decoder.getCodeLength() - decoder.getParityLength() << ")... " << endl;
	
	startTime = ACE_High_Res_Timer::gettimeofday_hr();
	
	for ( size_t i = 0; i < num; ++i ) decoder.decode(codeWord); 
	
	endTime = ACE_High_Res_Timer::gettimeofday_hr();
	
	totalTime = endTime - startTime;
	ACE_UINT64 usec;
	totalTime.to_usec(usec);
	
	cout << usec << "us total, " << usec/static_cast<double>(num) << " us/msg, " <<
		num / (usec / 1000000.0) << " msg/s" << endl;
}

void testMatch(const Sym orig[], const size_t origLen, const SymVec decoded) {
	SymVec origVec(orig, origLen);
	
	if ( origVec != decoded ) cout << "MIS";
	cout << endl << "MATCH with original!" << endl;
}

SymVec addErrors(const SymVec& codeWord, const int errCount, const Sym maxSymbol) {
	SymVec errored = codeWord;
	vector<size_t> errPos;
	
	vector<size_t>::iterator pos;
	
	for ( int e = 0; e < errCount; ) {
		size_t newPos = lrand48() % codeWord.size(); // choose random position
		pos = find(errPos.begin(), errPos.end(), newPos);	
		if ( pos == errPos.end() ) { // only add error if it's a new position
			++e;
			errPos.push_back(newPos);
			errored[newPos] = (errored[newPos] + 1) % maxSymbol;
		}
	}
	
	cout << "Errors at random positions: " << endl << "[ " << dec;
	 
	for ( pos = errPos.begin(); pos != errPos.end(); ++pos ) {
		if ( pos != errPos.begin() ) cout << ", ";
		cout << *pos;
	}
	
	cout << " ]" << endl;
	return errored;
}

SymVec test_correctness(const Sym msg[], const size_t msgLen,
	const RSEncoder& encoder, RSDecoder& decoder) {
	
	SymVec encoded = encode(msg, msgLen, encoder);
	
	SymVec errored = addErrors(encoded, encoder.getParityLength() / 2, encoder.getMaxSymbol());
	
	SymVec decoded = decode(errored, decoder);
	testMatch(msg, msgLen, decoded);
	
	return encoded;
}

void test_AOS_RS_10_6() {
	cout << line << endl << "Testing AOS_RS(10,6)" << endl << line << endl;
	
	RSEncoder encoder(0x13, 4, 10, 4, 1, 6, 1, false);
	RSDecoder decoder(0x13, 4, 10, 4, 1, 6, 1, false);
			
	Sym msg1[] = { 3, 6, 9, 12, 15, 10 };
	SymVec encoded1 = test_correctness(msg1, 6, encoder, decoder);
	cout << line << endl;
	
	Sym msg2[] = { 4, 0, 0, 0, 0, 0 };
	test_correctness(msg2, 6, encoder, decoder);
	cout << line << endl << endl;
	
	encodeTimeTest(msg1, 6, encoder);
	decodeTimeTest(encoded1, decoder);

	cout << line << endl << endl;	
}

void test_CCSDS_255_223() {
	cout << line << endl << "Testing CCSDS_RS(255,223)" << endl << line << endl;

	cout << "Interleave Depth: 1" << endl;
	RSEncoder encoder1(0x187, 32, 255, 8, 11, 112, 1, true);
	RSDecoder decoder1(0x187, 32, 255, 8, 11, 112, 1, true);

	SymVec encoded1 = test_correctness(aosTF128, 128, encoder1, decoder1);
	cout << line << endl << endl;
	
	encodeTimeTest(aosTF128, 128, encoder1);
	decodeTimeTest(encoded1, decoder1);
	// oldEncodeTimeTest();
	cout << line << endl << endl;

	cout << "Interleave Depth: 2" << endl;
	RSEncoder encoder2(0x187, 32, 255, 8, 11, 112, 2, true);
	RSDecoder decoder2(0x187, 32, 255, 8, 11, 112, 2, true);
		
	SymVec encoded2 = test_correctness(buf256, 256, encoder2, decoder2);
	cout << line << endl << endl;
	
	encodeTimeTest(buf256, 256, encoder2);
	decodeTimeTest(encoded2, decoder2);
	cout << line << endl << endl;

}

void test_CCSDS_255_239() {
	cout << line << endl << "Testing CCSDS_RS(255,239)" << endl << line << endl;

	cout << "Interleave Depth: 1" << endl;
	RSEncoder encoder1(0x187, 16, 255, 8, 11, 112, 1, true);
	RSDecoder decoder1(0x187, 16, 255, 8, 11, 112, 1, true);

	SymVec encoded1 = test_correctness(aosTF128, 128, encoder1, decoder1);
	cout << line << endl << endl;
	
	encodeTimeTest(aosTF128, 128, encoder1);
	decodeTimeTest(encoded1, decoder1);
	cout << line << endl << endl;
	
	cout << "Interleave Depth: 2" << endl;
	RSEncoder encoder2(0x187, 16, 255, 8, 11, 112, 2, true);
	RSDecoder decoder2(0x187, 16, 255, 8, 11, 112, 2, true);
		
	SymVec encoded2 = test_correctness(buf256, 256, encoder2, decoder2);
	cout << line << endl << endl;
	
	encodeTimeTest(buf256, 256, encoder2);
	decodeTimeTest(encoded2, decoder2);
	cout << line << endl << endl;	
}

ostream &outputBits(ostream &stream, const SymVec& v) {
	SymVec::const_iterator pos;
	stream << "[ ";
	
	for ( pos = v.begin(); pos != v.end(); ++pos ) {
		for ( size_t i = 0; i < 8; ++i ) {
			stream << std::right << std::setw(1) << ((*pos & (128 >> i))? '1' : '0');
		}
		stream << " ";
	}
	
	stream << "]";

  return stream;
}

void test_packing_and_unpacking() {
	cout << line << endl << "Packing/unpacking test" << endl;
	cout << line << endl;
	Sym packed[3] = { 0xfe, 0xdc, 0xba };
	SymVec packedVec(packed, 3);
	cout << "Original buffer: " << endl << packedVec << endl;
	cout << "Bits: " << endl;
	outputBits(cout, packedVec) << endl;
	
	
	const SymVec unpacked = RSBase::unpack(4, packed, 3, 6);
		
	cout << "Unpacked using 4-bit symbols: " << endl << unpacked << endl;
	cout << "Bits: " << endl;
	outputBits(cout, unpacked) << endl;	
	
	Sym repacked[3];
	
	RSBase::pack(4, unpacked, 6, repacked, 3);
	SymVec repackedVec(repacked, 3);
	cout << "Repacked using 4-bit symbols: " << endl << repackedVec << endl;
	cout << "Bits: " << endl;
	outputBits(cout, repackedVec) << endl;		
	
	Sym skewpacked[4];
	RSBase::pack(4, unpacked, 6, skewpacked, 4, 2);
	SymVec skewpackedVec(skewpacked, 4);
	cout << "Repacked using 4-bit symbols and 2-bit offset: " << endl << skewpackedVec << endl;
	cout << "Bits: " << endl;
	outputBits(cout, skewpackedVec) << endl;
	
	SymVec skewunpacked = RSBase::unpack(4, skewpacked, 4, 6, 2);
	cout << "Unpacked using 4-bit symbols and 2-bit offset: " << endl << skewunpacked << endl;
	cout << "Bits: " << endl;
	outputBits(cout, skewunpacked) << endl;	
	
	cout << line << endl << endl;
}

int main(int argc, char** argv) {
	
	if ( argc > 1 ) timerIterations = atoi(argv[1]);

	srand48(time(0));
	
	test_packing_and_unpacking();
	test_AOS_RS_10_6();
	test_CCSDS_255_223();
	test_CCSDS_255_239();
	

	
	return 0;
}