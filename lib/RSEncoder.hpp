/* -*- C++ -*- */

//=============================================================================
/**
 * @file   RSEncoder.hpp
 * @author Tad Kollar  
 *
 * $Id: RSEncoder.hpp 1670 2012-08-28 13:25:28Z tkollar $ 
 * Copyright (c) 2012.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _NASA_RS_ENCODER_HPP_
#define _NASA_RS_ENCODER_HPP_

#include "RSBase.hpp"

namespace nasaCE {

//=============================================================================
/**
 * @class RSEncoder
 * @author Tad Kollar  
 * @brief Generate Reed-Solomon parity symbols for a provided message.
*/
//=============================================================================
class RSEncoder : public RSBase {
public:
	/// Constructor. Loads the exponential and logrithm tables and other basic coding params.
	/// @param primitivePoly Polynomial used to build a generator polynomial.
	/// @param parityLength Number of parity symbols to generate in each code word.
	/// @param codeLength Total length of the data + parity symbols.
	/// @param bitsPerSymbol Limits the maximum value of each symbol.
	/// @param primitiveRootIdx The index of the primitive root (e.g. 1 if primitive root is a^1, 11 if a^11).
	/// @param firstConsecutiveRoot The degree of the first term in the generating polynomial.
	/// @param interleaveDepth The number of codewords to use per message.
	/// @param doBasisTransform Whether to convert from from dual-basis to conventional before encoding.
	/// @throw ValueTooLarge If one of the provided values is unacceptible.
	RSEncoder(const int primitivePoly,
		const std::size_t parityLength,
		const std::size_t codeLength,
		const std::size_t bitsPerSymbol,
		const Sym primitiveRootIdx,
		const Sym firstConsecutiveRoot = 0,
		const std::size_t interleaveDepth = 1,
		const bool doBasisTransform = false);

	/// Destructor.
    ~RSEncoder();
	
	/// Generate parity symbols for the given vector and return them.
	/// @param msgIn Vector containing data to encode.
	/// @return A vector containing the parity symbols.
	SymVec encode(const SymVec& msgIn) const;
	
	/// Generate parity symbols for the given buffer and return them.
	/// @param data Buffer containing data to encode.
	/// @param dataLen Size of the data buffer.
	/// @return A vector containing the parity symbols.
	SymVec encode(const Sym data[], const std::size_t dataLen) const;
	
	/// Generate parity symbols for the given NetworkData unit and return them.
	/// @param data NetworkData unit containing data to encode.
	/// @return A vector containing the parity symbols.
	SymVec encode(const NetworkData* data) const;	
	
	/// Return the coefficients of the generating polynomial _rsGeneratorPoly.
	SymVec getGeneratorPoly() const { return _rsGeneratorPoly; }

private:
	/// Using the parameters supplied during construction, calculate the
	/// coefficients to the generating polynomial.
	void _initGeneratorPoly();

	/// A vector containing the coefficients of the generating polynomial.
	SymVec _rsGeneratorPoly;
	
protected:

	/// Given a vector containing exactly codeLength - parityLength symbols, produce
	/// parityLength symbols.
	/// @param msgIn A message exactly codeLength - parityLength in size, may be padded with zeroes.
	/// @param copyOrig If true, copy msgIn back into the return value. Otherwise only the parity symbols are valid.
	/// @return Either the entire codeword (if copyOrig is true) or a codeword-size vector with parity at the tail.
	SymVec encodeOneWord_(const SymVec& msgIn, bool copyOrig = true) const;

}; // class RSEncoder

} // namespace nasaCE

#endif // _NASA_RS_ENCODER_HPP_
