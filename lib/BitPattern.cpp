/* -*- C++ -*- */

//=============================================================================
/**
 * @file   BitPattern.cpp
 * @author Tad Kollar < >
 *
 * $Id: BitPattern.cpp 2364 2013-09-17 19:41:51Z tkollar $
 * Copyright (c) 2008-2013.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "BitPattern.hpp"
#include <iostream>

namespace nasaCE {

ACE_UINT8 BitPattern::bitMask_[8] = { 0x1, 0x2, 0x4, 0x8, 0x10, 0x20, 0x40, 0x80 };
ACE_UINT8 BitPattern::invBitMask_[8] = { 0xfe, 0xfd, 0xfb, 0xf7, 0xef, 0xdf, 0xbf, 0x7f };

BitPattern::BitPattern(const size_t patternBitLen, const size_t wordBits /* = 8 */):
	wordBits_(wordBits), pattern_(0), patternBitLen_(patternBitLen), nextBitToCopy_(0) {
	if (wordBits < 2 ) wordBits_ = 2;
	else if (wordBits >24) wordBits_ = 24;
	pattern_ = new NetworkData(getPatternByteLen());

}

BitPattern::~BitPattern() { ndSafeRelease(pattern_); }

size_t BitPattern::getPatternBitLen() const {
	return patternBitLen_;
}

size_t BitPattern::getPatternByteLen() const {
	return ((patternBitLen_/8) + (( patternBitLen_) % 8 > 0));
}

NetworkData* BitPattern::getPattern() { return pattern_; }

NetworkData* BitPattern::makeNewPatternBuffer(const size_t bufLen, const bool restart /* = false */) {
	if ( restart ) nextBitToCopy_ = 0;

	NetworkData* buf = new NetworkData(bufLen);

	if ( ! buf ) throw OutOfMemory(OutOfMemory::msg(bufLen, "Could not create new BitPattern buffer."));

	if (patternBitLen_ % 8 == 0 ) { // Handle an octet-aligned pattern
		size_t x;
		for ( x = 0; x < bufLen; ++x )
			*(buf->ptrUnit(x)) = *(pattern_->ptrUnit((x + nextBitToCopy_ / 8) % pattern_->getUnitLength())) & 0xff;

		nextBitToCopy_ = (x % pattern_->getUnitLength() * 8);
	}
	else {
		size_t bufBitLen = bufLen * 8, bitsToCopy = 0;
		for ( size_t b = 0; b < bufBitLen; b += bitsToCopy) {
			bitsToCopy = (patternBitLen_ + b > bufBitLen)? bufBitLen - b : patternBitLen_;
			pattern_->bitCopyTo(0, b, bitsToCopy, buf->ptrUnit());
		}

		nextBitToCopy_ = bitsToCopy;
	}

	if (nextBitToCopy_ >= patternBitLen_) nextBitToCopy_ = 0;

	return buf;
}

std::string BitPattern::getTypeStr() const { return "None"; }

bool BitPattern::getWordBit(const size_t word, const size_t bit) const {
	if (word * wordBits_ + bit > pattern_->getUnitLength() * 8)
		throw BufferRangeError(BufferRangeError::msg("BitPattern::getWordBit", word * wordBits_, pattern_->getUnitLength()*8));

	if (bit >= wordBits_)
		throw BufferRangeError(BufferRangeError::msg("BitPattern::getWordBit", bit, wordBits_ - 1));

	size_t  bitNum = ((word + 1) * wordBits_) - bit - 1,
			bitIdx = 7 - (bitNum%8),
			byteIdx = bitNum/8;

	ACE_UINT8 bval = *(pattern_->ptrUnit(byteIdx));

	return ( (bval & bitMask_[bitIdx]) != 0 );
}

bool BitPattern::setWordBit(const size_t word, const size_t bit, const bool val) const {
	if (word * wordBits_ + bit > pattern_->getUnitLength() * 8)
		throw BufferRangeError(BufferRangeError::msg("BitPattern::setWordBit", word * wordBits_, pattern_->getUnitLength()*8));

	if (bit >= wordBits_)
		throw BufferRangeError(BufferRangeError::msg("BitPattern::setWordBit", bit, wordBits_ - 1));

	size_t  bitNum = ((word + 1) * wordBits_) - bit - 1,
			bitIdx = 7 - (bitNum%8),
			byteIdx = bitNum/8;

	*(pattern_->ptrUnit(byteIdx)) = (*(pattern_->ptrUnit(byteIdx)) & invBitMask_[bitIdx]) | ( val << bitIdx );

	return val;
}

void BitPattern::copyWord(const size_t srcIdx, const size_t dstIdx) {
	for ( int i = 0; i < wordBits_; ++i ) setWordBit(dstIdx, i, getWordBit(srcIdx, i));
}

void BitPattern::copySubWord(const size_t srcWord, const size_t srcBit, const size_t len,
	const size_t dstWord, const size_t dstBit) {

	for ( int x = 0; x < len && srcBit + x < wordBits_ && dstBit + x < wordBits_; ++x ) {
		setWordBit(dstWord, dstBit + x, getWordBit(srcWord, srcBit + x));
	}
}

void BitPattern::lshWord(const size_t word, const size_t positions) {
	if ( positions > wordBits_ )
		throw BufferRangeError(BufferRangeError::msg("BitPattern::lshWord", positions, wordBits_));

	for ( int i = wordBits_ - 1; i >= 0; --i ) {
		if ( i + 1  <= positions ) setWordBit(word, i, false);
		else setWordBit(word, i, getWordBit(word, i - positions));
	}
}

void BitPattern::rshWord(const size_t word, const size_t positions) {
	if ( positions > wordBits_ )
		throw BufferRangeError(BufferRangeError::msg("BitPattern::rshWord", positions, wordBits_));

	for ( int i = 0; i < wordBits_; ++i ) {
		if ( i + positions >= wordBits_ ) setWordBit(word, i, false);
		else setWordBit(word, i, getWordBit(word, i + positions));
	}
}

unsigned long BitPattern::word2Long(const size_t word) {
	unsigned long ret = 0;

	for ( int x = 0; x < wordBits_; x++ ) {
		ret |= (getWordBit(word, x) << x);
	}

	return ret;
}


EntropyPattern::EntropyPattern(const size_t patternBitLen /* = 2048 */): BitPattern(patternBitLen) {
	try {
		_getEntropy();
	}
	catch (const nd_error& e) {
		_getPseudoRandom();
	}
}

std::string EntropyPattern::getTypeStr() const { return "Entropy"; }

void EntropyPattern::_getEntropy() {
	ACE_HANDLE fd = ACE_OS::open("/dev/urandom", O_RDONLY);

	if ( fd < 0 ) throw nd_error("Unable to open /dev/urandom for reading.");

	if ( ACE_OS::read(fd, pattern_->ptrUnit(), getPatternByteLen()) < 0 ) {
		ACE_OS::close(fd);
		throw nd_error("Unable to read from /dev/urandom.");
	}

	ACE_OS::close(fd);
}

void EntropyPattern::_getPseudoRandom() {
	srand(ACE_OS::time(0));

	for ( int i = 0; i < getPatternByteLen(); ++i ) *(pattern_->ptrUnit(i)) = rand() & 0xff;
}


AllOnesPattern::AllOnesPattern(): BitPattern(8) {
	ACE_OS::memset(pattern_->ptrUnit(), 0xff, getPatternByteLen()); // 1111 1111
}

std::string AllOnesPattern::getTypeStr() const { return "All Ones"; }


AllZeroesPattern::AllZeroesPattern(): BitPattern(8) {
	ACE_OS::memset(pattern_->ptrUnit(), 0x00, getPatternByteLen()); // 0000 0000
}

std::string AllZeroesPattern::getTypeStr() const { return "All Zeroes"; }

OnesAndZeroesPattern::OnesAndZeroesPattern(): BitPattern(8) {
	ACE_OS::memset(pattern_->ptrUnit(), 0xaa, getPatternByteLen());
}

std::string OnesAndZeroesPattern::getTypeStr() const { return "Alternating Ones and Zeroes"; }

S3In24Pattern::S3In24Pattern(): BitPattern(24) {
	setWordBit(0, 6, true);
	setWordBit(0, 1, true);
	setWordBit(2, 2, true);
}

std::string S3In24Pattern::getTypeStr() const { return "3-in-24"; }

S1In8Pattern::S1In8Pattern(): BitPattern(8) {
	ACE_OS::memset(pattern_->ptrUnit(), 0x40, getPatternByteLen()); // 0100 0000
}

std::string S1In8Pattern::getTypeStr() const { return "1-in-8"; }

S2In8Pattern::S2In8Pattern(): BitPattern(8) {
	ACE_OS::memset(pattern_->ptrUnit(), 0x42, getPatternByteLen()); // 0100 0010
}

std::string S2In8Pattern::getTypeStr() const { return "2-in-8"; }

PrbsPattern::PrbsPattern(const size_t shifterBits): BitPattern(((1<<shifterBits) - 1)*shifterBits, shifterBits) {
	_genPattern();
}

std::string PrbsPattern::getTypeStr() const { return "Pseudorandom Bit Pattern"; }

void PrbsPattern::_genWordLsh(const size_t word, int taps[], const size_t tapCount) {
	size_t prevWordIdx = word - 1;
	copySubWord(prevWordIdx, 0, wordBits_ - 1, word, 1); // Copy and left shift 1
	bool i = getWordBit(prevWordIdx, wordBits_ - 1);

	for ( int t = 0; t < tapCount; ++t ) {
		i ^= getWordBit(prevWordIdx, taps[t]); //xor
		// i = (i == getWordBit(prevWordIdx, taps[t])); // xnor
	}

	setWordBit(word, 0, i);
}

void PrbsPattern::_genWordRsh(const size_t word, int taps[], const size_t tapCount) {
	size_t prevWordIdx = word - 1;
	copySubWord(prevWordIdx, 1, wordBits_ - 1, word, 0); // Copy and right shift 1
	bool i = getWordBit(prevWordIdx, 0);

	for ( int t = 0; t < tapCount; ++t ) {
		i ^= getWordBit(prevWordIdx, taps[t]); //xor
		// i = (i == getWordBit(prevWordIdx, taps[t])); // xnor
	}

	setWordBit(word, wordBits_ - 1, i);
}

void PrbsPattern::_genPattern() {
	// Shifters that require more than a single tap are marked with -1.
	// Unsupported shifters are marked with -2.
	int singleTapTable[] = {
		// __0___1___2___3___4___5___6___7
			-2, -2, -1,  1,  2,  2,  4,  5,
		// __8___9__10__11__12__13__14__15
			-1,  4,  6,  8, -1, -1, -1, 13,
		// _16__17__18__19__20__21__22__23
			-1, 13, 10, -1, 16, 18, 20, 17,
		// _24__25
			-1, 21
	};

	// Initialize the sequence to start with 1
	setWordBit(0, 0, true);
	int finalWord = (1 << wordBits_) - 1;

	if ( singleTapTable[wordBits_] > -1 ) {
		int taps[] = { singleTapTable[wordBits_] };
		for ( int w = 1; w < finalWord; ++w) _genWordLsh(w, taps, 1);
	}
	else {
		switch(wordBits_) {
			case 2:
				setWordBit(1,1,true);
				setWordBit(2,0,true);
				setWordBit(2,1,true);
				break;
			case 8: {
					int taps[] = { 5,4,3 };
					for ( int w = 1; w < finalWord; ++w) _genWordLsh(w, taps, 3);
			}
				break;
			case 12: {
					int taps[] = { 5,3,0 };
					for ( int w = 1; w < finalWord; ++w) _genWordLsh(w, taps, 3);
				}
				break;
			case 13: {
					int taps[] = { 3,2,0 };
					for ( int w = 1; w < finalWord; ++w) _genWordLsh(w, taps, 3);
				}
				break;
			case 14: {
					int taps[] = { 4,2,0 };
					for ( int w = 1; w < finalWord; ++w) _genWordLsh(w, taps, 3);
				}
				break;
			case 16: {
					int taps[] = { 14,12,3 };
					for ( int w = 1; w < finalWord; ++w) _genWordLsh(w, taps, 3);
				}
				break;
			case 24: {
					int taps[] = { 22,21,16 };
					for ( int w = 1; w < finalWord; ++w) _genWordLsh(w, taps, 3);
				}
				break;
		}
	}
}

};