//=============================================================================
/**
 * @file   RSBase.hpp
 * @author Tad Kollar  
 *
 * $Id: RSBase.hpp 2309 2013-07-11 13:32:23Z tkollar $
 * Copyright (c) 2011.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _NASA_RS_BASE_HPP_
#define _NASA_RS_BASE_HPP_

#include <algorithm>
#include <iostream>
#include <vector>
#include "NetworkData.hpp"

namespace nasaCE {

typedef ACE_UINT8 Sym;

//=============================================================================
/**
 * @class SymVec
 * @author Tad Kollar  
 * @brief A vector based on byte-sized symbols.
*/
//=============================================================================
struct SymVec: public std::vector<Sym> {
	/// Default constructor.
	SymVec();

	/// Construct and preallocate elements.
	/// @param a The number of elements.
	SymVec(const int a);

	/// Construct, preallocate elements, set them to specified value.
	/// @param a The number of elements.
	/// @param b The value to set each element to.
	SymVec(const int a, const Sym b);

	/// Contruct and copy data from the provided buffer.
	/// @param data Pointer to the source buffer.
	/// @param dataLen Amount of data to copy from the buffer.
	SymVec(const Sym *data, const int dataLen);

	/// Construct and copy data from a NetworkData unit.
	/// @param data Pointer to the NetworkData unit to copy from.
	SymVec(const NetworkData* data);

	/// Create this SymVec by copying another.
	/// @param other The SymVec to copy.
	/// @param offset The offset from the start of other to copy into.
	/// @param length The number of symbols to copy.
	void copy(const SymVec& other, const std::size_t offset = 0, const std::size_t length = 0);

	friend ostream &operator<<(ostream &stream, const SymVec& v);
};

/// Output a SymVec in the form of [ x, y, ..., z ].
ostream &operator<<(ostream &stream, const SymVec& v);

//=============================================================================
/**
 * @class RSBase
 * @author Tad Kollar  
 * @brief Tables and shared functions for operating on Reed-Solomon codes.
*/
//=============================================================================
class RSBase {
public:
	/// Constructor. Loads the exponential and logrithm tables and other basic coding params.
	/// @param primitivePoly Polynomial used to build a generator polynomial.
	/// @param parityLength Number of parity symbols to generate in each code word.
	/// @param codeLength Total length of the data + parity symbols.
	/// @param bitsPerSymbol Limits the maximum value of each symbol.
	/// @param primitiveRootIdx Which primitiveRoot is the primitive root.
	/// @param firstConsecutiveRoot The degree of the first term in the generating polynomial.
	/// @param interleaveDepth The number of codewords to use per message.
	/// @param doBasisTransform Whether to convert from from dual-basis to conventional before operating.
	/// @throw ValueTooLarge If one of the provided values is unacceptible.
	/// @throw ValueTooSmall If interleaveDepth is 0.
	RSBase(const int primitivePoly,
		const std::size_t parityLength,
		const std::size_t codeLength,
		const std::size_t bitsPerSymbol,
		const Sym primitiveRootIdx,
		const Sym firstConsecutiveRoot = 0,
		const std::size_t interleaveDepth = 1,
		const bool doBasisTransform = false);

	/// Destructor.
	~RSBase();

	/// Multiply x and y together in the generated Galois Field.
	/// @return The product of x and y.
	inline Sym gfMul(const Sym& x, const Sym& y) const {
		if ( x == 0 || y == 0 ) return 0;
		return gfExp_[gfLog_[x] + gfLog_[y]];
	}

	/// Divide x by y in the generated Galois Field.
	/// @throw ValueTooSmall When y is zero.
	/// @return The result of x/y over the GF.
	Sym gfDiv(const Sym& x, const Sym& y) const;

	/// Multiply polynomial p by scalar x.
	/// @return A polynomial representing p(y) * x.
	SymVec gfPolyScale(const SymVec& p, const Sym& x) const;

	/// Add polynomial p to polynomial q.
	/// @return The result of p(x) + q(x).
	SymVec gfPolyAdd(const SymVec& p, const SymVec& q) const;

	/// Multiply polynomial p by polynomial q.
	/// @return The result of p(x) * q(x).
	SymVec gfPolyMul(const SymVec& p, const SymVec& q) const;

	/// Substitute x in polynomial p(x).
	/// @return The result of p(x).
	Sym gfPolyEval(const SymVec& p, const Sym& x) const;

	/// Take a simple vector and interleave it into Reed-Solomon blocks. CCSDS uses
	/// a round robin approach where the symbol at position 0 in the buffer goes into the
	/// first block's position 0, position 1 of the buffer goes into the second block's
	/// position 0, buffer pos. 3 into the 3rd block's pos. 0, etc. When the end
	/// of the block stack is reached, the next symbol from the buffer goes into the first
	/// block's position 1, etc. If the data is too short, virtual fill (zeroes) is placed
	/// at the front.
	/// @param msg The vector to copy symbols from.
	/// @param hasParity Whether the message already has parity symbols appended.
	/// @return A vector of SymVecs that contain the interleaved symbols.
	std::vector<SymVec> ccsdsInterleave(const SymVec& msg, const bool hasParity = false) const;

	/// Provide the inverse function to interleave(). Given a stack of codewords, in a round-
	/// robin fashion take one symbol from each vector and place it serially into the
	/// new vector. Parity symbols are left off in the process.
	/// @param interleavedSyms The provided array of vectors containing the interleaved symbols.
	/// @return A SymVec containing the de-interleaved message.
	/// @see interleave()
	SymVec ccsdsDeinterleave(const std::vector<SymVec>& interleavedSyms) const;

	/// Take bits represented in dual basis (Berlekamp) and transform to conventional.
	/// @param data Vector of symbols to transform.
	/// @param start The index to begin converting.
	/// @param size The number of symbols to convert.
	SymVec ccsdsDualBasisToConventional(const SymVec& data,
		const std::size_t start = 0, const std::size_t size = 0 ) const;

	/// Take bits in conventional representation and transform to dual basis (Berlekamp).
	/// @param data Vector of symbols to transform.
	/// @param start The index to begin converting.
	/// @param size The number of symbols to convert.
	SymVec ccsdsConventionalToDualBasis(const SymVec& data,
		const std::size_t start = 0, const std::size_t size = 0) const;

	/// Write-only accessor to _parityLength.
	void setParityLength(const std::size_t& newVal) { _parityLength = newVal; }

	/// Read-only accessor to _parityLength.
	std::size_t getParityLength() const { return _parityLength; }

	/// Calculate the number of parity symbols in the entire message.
	std::size_t getTotalParityLength() const { return _parityLength * _interleaveDepth; }

	/// Write-only accessor to _codeLength.
	void setCodeLength(const std::size_t& newVal) { _codeLength = newVal; }

	/// Read-only accessor to _codeLength.
	std::size_t getCodeLength() const {return _codeLength; }

	/// Write-only accessor to _bitsPerSymbol.
	void setBitsPerSymbol(const std::size_t& newVal) {
		_bitsPerSymbol = newVal;
		_maxSymbol = 0xff >> ( 8 - _bitsPerSymbol);
	}

	/// Read-only accessor to _bitsPerSymbol.
	std::size_t getBitsPerSymbol() const { return _bitsPerSymbol; }

	/// Read-only accessor to _maxSymbol.
	Sym getMaxSymbol() const { return _maxSymbol; }

	/// Write-only accessor to _primitiveRootIdx.
	void setPrimitiveRootIdx(const Sym& newVal) { _primitiveRootIdx = newVal; }

	/// Read-only accessor to _primitiveRootIdx.
	Sym getPrimitiveRootIdx() const { return _primitiveRootIdx; }

	/// Write-only accessor to _firstConsecutiveRoot.
	void setFirstConsecutiveRoot(const Sym& newVal) {
		_firstConsecutiveRoot = newVal;
	}

	/// Read-only accessor to _firstConsecutiveRoot.
	Sym getFirstConsecutiveRoot() const {
		return _firstConsecutiveRoot;
	}

	/// Write-only accessor to _interleaveDepth.
	void setInterleaveDepth(const std::size_t& newVal) { _interleaveDepth = newVal; }

	/// Read-only accessor to _interleaveDepth.
	std::size_t getInterleaveDepth() const { return _interleaveDepth; }

	/// Read-only accessor to gfExp_.
	SymVec getExponentials() const { return SymVec(gfExp_, gfExpLen_); }

	/// Read-only accessor to gfLog_.
	SymVec getLogrithms() const { return SymVec(gfLog_, gfLogLen_); }

	/// Write-only accessor to _doBasisTransform.
	void setDoBasisTransform(bool newVal) {	_doBasisTransform = newVal;	}

	/// Read-only accessor to _doBasisTransform.
	bool getDoBasisTransform() const { return _doBasisTransform; }

	/// Utility function - copy _bitsPerSymbol-wide units from a buffer into a SymVec.
	/// MSB is on the left. The maximum number of whole symbols are extracted; last
	/// partial, if one exists, is dropped.
	/// @param bitsPerSymbol Width of each symbols, 1-8 bits.
	/// @param buf The buffer containing the data to expand.
	/// @param bufLen Size of the data; if not multiple of _bitsPerSymbol, the remainder is dropped.
	/// @param maxSymbols Limit the number of symbols to retrieve (may not be met if buf is too small)
	/// @param initialBitOffset The index of the first significant bit in buf[0].
	/// @return A SymVec containing the symbols exctracted from buf.
	static SymVec unpack(const std::size_t& bitsPerSymbol,
		const Sym* buf, const std::size_t& bufLen,
		const std::size_t maxSymbols,
		const std::size_t& initialBitOffset = 0);

	/// Utility function - copy _bitsPerSymbol-wide units from a buffer into a SymVec.
	/// MSB is on the left. The maximum number of whole symbols are extracted; last
	/// partial, if one exists, is dropped.
	/// @param buf The buffer containing the data to expand.
	/// @param bufLen Size of the data; if not multiple of _bitsPerSymbol, the remainder is dropped.
	/// @param maxSymbols Limit the number of symbols to retrieve (may not be met if buf is too small)
	/// @param initialBitOffset The index of the first significant bit in buf[0].
	/// @return A SymVec containing the symbols exctracted from buf.
	SymVec unpack(const Sym* buf, const std::size_t& bufLen,
		const std::size_t maxSymbols,
		const std::size_t& initialBitOffset = 0) const;


	/// Utility function - take Symbols and put them into a buffer right next to each other.
	/// For example four symbols that are 4-bits-wide will take up two bytes.
	/// @param bitsPerSymbol Width of each symbols, 1-8 bits.
	/// @param syms A vector containing the symbols to compress.
	/// @param maxSymbols Limit the number of symbols to use (may not be met if buf is too small)
	/// @param buf The pre-allocated target buffer.
	/// @param bufLen The size of the target buffer.
	/// @param initialBitOffset The index of the first significant bit in buf[0].
	static void pack(const std::size_t& bitsPerSymbol,
		const SymVec syms, const std::size_t maxSymbols,
		Sym buf[], const std::size_t& bufLen,
		const std::size_t& initialBitOffset = 0 );

	/// Utility function - take Symbols and put them into a buffer right next to each other.
	/// For example four symbols that are 4-bits-wide will take up two bytes.
	/// @param syms A vector containing the symbols to compress.
	/// @param maxSymbols Limit the number of symbols to use (may not be met if buf is too small)
	/// @param buf The pre-allocated target buffer.
	/// @param bufLen The size of the target buffer.
	/// @param initialBitOffset The index of the first significant bit in buf[0].
	void pack(const SymVec syms, const std::size_t maxSymbols, Sym buf[], const std::size_t& bufLen,
		const std::size_t& initialBitOffset = 0 ) const;

private:
	/// Set up the exponential and logrithm tables.
	void _loadTables(const int primitivePoly);

	/// The number of parity symbols per codeword (not per message).
	std::size_t _parityLength;

	/// Number of symbols in one codeword (message + parity symbols).
	std::size_t _codeLength;

	/// The number of bits in each symbol.
	std::size_t _bitsPerSymbol;

	/// Largest value a symbol can have (changes with _bitsPerSymbol).
	Sym _maxSymbol;

	/// The primitive root.
	Sym _primitiveRootIdx;

	/// First consecutive root of the field generator polynomial.
	Sym _firstConsecutiveRoot;

	/// The number of codewords to use per message.
	std::size_t _interleaveDepth;

	/// Whether to convert from from dual-basis to conventional before encoding/decoding.
	bool _doBasisTransform;

protected:

	/// Table of exponentials.
	Sym* gfExp_;

	/// Size of gfExp_.
	std::size_t gfExpLen_;

	/// Table of logrithms.
	Sym* gfLog_;

	/// Size of gfLog_.
	std::size_t gfLogLen_;

	/// Only for CCSDS basis transformation.
	Sym talTab_[256];

	/// Only for CCSDS basis transformation.
	Sym talTabInv_[256];

};

} // namespace nasaCE

#endif // _NASA_RS_BASE_HPP_
