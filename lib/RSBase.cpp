//=============================================================================
/**
 * @file   RSBase.cpp
 * @author Tad Kollar  
 *
 * $Id: RSBase.cpp 2309 2013-07-11 13:32:23Z tkollar $
 * Copyright (c) 2011.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include <algorithm>
#include <iomanip>
#include "RSBase.hpp"

namespace nasaCE {

SymVec::SymVec(): std::vector<Sym>() { }

SymVec::SymVec(const int a): std::vector<Sym>(a) { }

SymVec::SymVec(const int a, const Sym b): std::vector<Sym>(a, b) { }

SymVec::SymVec(const Sym* data, const int dataLen): std::vector<Sym>() {
	insert(begin(), data, data + dataLen);
}

SymVec::SymVec(const NetworkData* data): std::vector<Sym>() {
	// ACE_OS::memcpy(&at(0), data->ptrUnit(), data->getUnitLength());
	insert(begin(), data->ptrUnit(), data->ptrUnit() + data->getUnitLength());
}

void SymVec::copy(const SymVec& other, const std::size_t offset /* = 0 */, const std::size_t length /* = 0 */) {
	const std::size_t trueLen = (length > 0)? length : other.size();

	if (trueLen + offset > size())
		throw BufferRangeError(BufferRangeError::msg("SymVec::copy: Destination SymVec: ", trueLen + offset, size()));

	if ( trueLen > other.size())
		throw BufferRangeError(BufferRangeError::msg("SymVec::copy: Source SymVec: ", trueLen, other.size()));

	insert(begin() + offset, other.begin(), other.begin() + length);
}

ostream &operator<<(ostream &stream, const SymVec& v) {
	SymVec::const_iterator pos;
	stream << "[";

	for ( pos = v.begin(); pos != v.end(); ++pos ) {
		if ( pos != v.begin() ) stream << ",";
		stream << std::hex << std::uppercase << std::right << std::setw(2) << static_cast<int>(*pos);
	}

	stream << "]";

  return stream;
}

RSBase::RSBase(const int primitivePoly,
	const std::size_t parityLength,
	const std::size_t codeLength,
	const std::size_t bitsPerSymbol,
	const Sym primitiveRootIdx,
	const Sym firstConsecutiveRoot /* = 0 */,
	const std::size_t interleaveDepth /* = 1 */,
	const bool doBasisTransform /* = false */):
	_parityLength(parityLength),
	_codeLength(codeLength),
	_bitsPerSymbol(bitsPerSymbol),
	_maxSymbol( 0xff >> ( 8 - bitsPerSymbol)),
	_primitiveRootIdx(primitiveRootIdx),
	_firstConsecutiveRoot(firstConsecutiveRoot),
	_interleaveDepth(interleaveDepth),
	_doBasisTransform(doBasisTransform),
	gfExp_(0),
	gfExpLen_((_maxSymbol + 1) * 2),
	gfLog_(0),
	gfLogLen_(_maxSymbol + 1) {

	if ( _bitsPerSymbol > 8 )
		throw ValueTooLarge(ValueTooLarge::msg("bits per symbol", _bitsPerSymbol, 8));

	if ( _interleaveDepth < 1 )
		throw ValueTooSmall("Interleave depth must be greater than 0.");

	gfExp_ = new Sym[gfExpLen_];
	ACE_OS::memset(gfExp_, 1, gfExpLen_);

	gfLog_ = new Sym[gfLogLen_];
	ACE_OS::memset(gfLog_, 0, gfLogLen_);

	ACE_OS::memset(talTab_, 0, 256);
	ACE_OS::memset(talTabInv_, 0, 256);


	_loadTables(primitivePoly);
}

RSBase::~RSBase() {

	delete[] gfExp_;
	delete[] gfLog_;
}

void RSBase::_loadTables(const int primitivePoly) {
	std::size_t i, j, k, x = 1;

	for ( i = 1; i < _maxSymbol; ++i ) {
		x <<= 1;
		if (x & (_maxSymbol + 1)) x ^= primitivePoly;
		gfExp_[i] = x;
		gfLog_[x] = i;
	}

	gfLog_[0] = _maxSymbol;

	for ( i = _maxSymbol + 1U; i < (_maxSymbol + 1U) * 2U; ++i ) {
		gfExp_[i] = gfExp_[i - _maxSymbol];
	}

	// Set up Berlekamp dual-basis to conventional lookup tables.
	Sym tal[] = { 0x8d, 0xef, 0xec, 0x86, 0xfa, 0x99, 0xaf, 0x7b };

	for ( i = 0; i < 256 ; ++i) {
		talTab_[i] = 0;

		for ( j = 0 ; j < 8 ; ++j ) {
			for( k = 0 ; k < 8 ; ++k ) {
				if ( i & (1<<k) ) talTab_[i] ^= tal[7-k] & (1<<j);
			}
		}

    talTabInv_[talTab_[i]] = i;

	}
}

Sym RSBase::gfDiv(const Sym& x, const Sym& y) const {
	if ( y == 0 ) throw ValueTooSmall("Division by zero attempted in RSBase::gfDiv()");
	if ( x == 0 ) return 0;
	return gfExp_[gfLog_[x] + _maxSymbol - gfLog_[y]];
}

SymVec RSBase::gfPolyScale(const SymVec& p, const Sym& x) const {
	SymVec r(p.size(), 0);
	for ( std::size_t i = 0; i < p.size(); ++i ) r[i] = gfMul(p[i], x);

	return r;
}

SymVec RSBase::gfPolyAdd(const SymVec& p, const SymVec& q) const {
	SymVec r(std::max<int>(p.size(), q.size()), 0);

	for ( std::size_t i = 0; i < p.size(); ++i ) {
		r[i + r.size() - p.size()] = p[i];
	}

	for ( std::size_t i = 0; i < q.size(); ++i ) {
		r[i + r.size() - q.size()] ^= q[i];
	}

	return r;
}

SymVec RSBase::gfPolyMul(const SymVec& p, const SymVec& q) const {
	SymVec r(p.size() + q.size() - 1, 0);

	for ( std::size_t j = 0; j < q.size(); ++j )
		for ( std::size_t i = 0; i < p.size(); ++i ) {
			r[i+j] ^= gfMul(p[i], q[j]);
		}

	return r;
}

Sym RSBase::gfPolyEval(const SymVec& p, const Sym& x) const {
	Sym y = p[0];
	for ( std::size_t i = 1; i < p.size(); ++i ) y = gfMul(y,x) ^ p[i];
	return y;
}

std::vector<SymVec> RSBase::ccsdsInterleave(const SymVec& msg, const bool hasParity /* = false */) const {
	// Allocate a vector of codewords to interleave into
	std::vector<SymVec> interleavedSyms(getInterleaveDepth(), SymVec(getCodeLength(), 0));

	// Fill in from the back because virtual space belongs in front.
	std::size_t row = getInterleaveDepth() - 1,
				offset = hasParity? getCodeLength() - 1 : getCodeLength() - getParityLength() - 1;

	SymVec::const_reverse_iterator pos;

	for ( pos = msg.rbegin(); pos != msg.rend(); ++pos) {
		interleavedSyms[row][offset] = *pos;
		if ( row == 0 ) { row = getInterleaveDepth() - 1; --offset; }
		else --row;
	}

	return interleavedSyms;
}

SymVec RSBase::ccsdsDeinterleave(const std::vector<SymVec>& interleavedSyms) const {
	const std::size_t maxDataPerWord = getCodeLength() - getParityLength();
	const std::size_t msgLen = maxDataPerWord * getInterleaveDepth();
	SymVec msg(msgLen, 0);

	std::size_t row = getInterleaveDepth() - 1,
				offset = maxDataPerWord - 1;

	SymVec::reverse_iterator pos;

	for ( pos = msg.rbegin(); pos != msg.rend(); ++pos) {
		*pos = interleavedSyms[row][offset];
		if ( row == 0 ) { row = getInterleaveDepth() - 1; --offset; }
		else --row;
	}

	return msg;
}

SymVec RSBase::ccsdsDualBasisToConventional(const SymVec& data,
		const std::size_t start /* = 0 */, const std::size_t size /* = 0 */) const {
	SymVec converted = data;
	std::size_t stop = (size > 0)? start + size : data.size();

	for ( std::size_t i = start; i < stop; ++i ) converted[i] = talTabInv_[data[i]];

	return converted;
}

SymVec RSBase::ccsdsConventionalToDualBasis(const SymVec& data,
		const std::size_t start /* = 0 */, const std::size_t size /* = 0 */ ) const {
	SymVec converted = data;
	std::size_t stop = (size > 0)? start + size : data.size();

	for ( std::size_t i = start; i < stop; ++i ) converted[i] = talTab_[data[i]];

	return converted;
}

SymVec RSBase::unpack(const std::size_t& bitsPerSymbol, const Sym* buf,
	const std::size_t& bufLen,
	const std::size_t maxSymbols, const std::size_t& initialBitOffset /* = 0 */) {

	const std::size_t symCount = std::min(maxSymbols, (bufLen * 8 - initialBitOffset) / bitsPerSymbol);
	const std::size_t mask = 0xff >> (8 - bitsPerSymbol);
	const std::size_t maxBitOffsetForOneByte = 8 - bitsPerSymbol;
	SymVec syms(symCount);
	std::size_t bufOffset, bitOffset;

	for ( std::size_t i = 0; i < symCount; ++i ) {
		bufOffset = (i * bitsPerSymbol) / 8;
		bitOffset = ((i * bitsPerSymbol) % 8) + initialBitOffset;

		if ( bitOffset > maxBitOffsetForOneByte )
			// The symbol overlaps the byte boundary so operate on two.
			syms[i] = ((buf[bufOffset] << 8 | buf[bufOffset + 1]) >> (16 - bitOffset - bitsPerSymbol)) & mask ;
		else
			// The symbol resides completely within a single byte.
			syms[i] = (buf[bufOffset] >> (8 - bitOffset - bitsPerSymbol)) & mask;
	}

	return syms;
}

SymVec RSBase::unpack(const Sym* buf, const std::size_t& bufLen,
	const std::size_t maxSymbols,
	const std::size_t& initialBitOffset /* = 0 */) const {

	return unpack(_bitsPerSymbol, buf, bufLen, maxSymbols, initialBitOffset);
}

void RSBase::pack(const std::size_t& bitsPerSymbol,
	const SymVec syms, const std::size_t maxSymbols,
	Sym buf[], const std::size_t& bufLen,
	const std::size_t& initialBitOffset /* = 0 */) {

	// Operate on the max number of symbols that can be packed into buf.
	const std::size_t symCount = std::min(maxSymbols, (( syms.size() * bitsPerSymbol + initialBitOffset ) <= ( bufLen * 8))?
		syms.size() : (bufLen * 8 - initialBitOffset) / bitsPerSymbol);
	const std::size_t mask = 0xff >> (8 - bitsPerSymbol);
	const std::size_t maxBitOffsetForOneByte = 8 - bitsPerSymbol;
	std::size_t bufOffset, bitOffset;
	ACE_UINT16 wideVal;

	// Important to clear the buffer since it will be set with logical ORs:
	ACE_OS::memset(buf, 0, bufLen);

	for ( std::size_t i = 0; i < symCount; ++i ) {
		bufOffset = (i * bitsPerSymbol) / 8;
		bitOffset = ((i * bitsPerSymbol) % 8) + initialBitOffset;

		if ( bitOffset > maxBitOffsetForOneByte ) {
			// The symbol overlaps the byte boundary so operate on two.
			wideVal = (syms[i] & mask) << (16 - bitOffset - bitsPerSymbol);
			buf[bufOffset] |= wideVal >> 8;
			buf[bufOffset + 1] |= wideVal & 0xff;
		}
		else
			// The symbol is placed completely within a single byte.
			buf[bufOffset] |= (syms[i] & mask) << (8 - bitOffset - bitsPerSymbol);
	}
}

void RSBase::pack(const SymVec syms, const std::size_t maxSymbols,
	Sym buf[], const std::size_t& bufLen,
	const std::size_t& initialBitOffset /* = 0 */) const {

	pack(_bitsPerSymbol, syms, maxSymbols, buf, bufLen, initialBitOffset);
}


} // namespace nasaCE
