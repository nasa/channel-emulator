/* -*- C++ -*- */

//=============================================================================
/**
 * @file   RSEncoder.cpp
 * @author Tad Kollar  
 *
 * $Id: RSEncoder.cpp 2308 2013-07-11 13:24:01Z tkollar $
 * Copyright (c) 2012.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "RSEncoder.hpp"

namespace nasaCE {

RSEncoder::RSEncoder(const int primitivePoly,
		const std::size_t parityLength,
		const std::size_t codeLength,
		const std::size_t bitsPerSymbol,
		const Sym primitiveRootIdx,
		const Sym firstConsecutiveRoot /* = 0 */,
		const std::size_t interleaveDepth /* = 1 */,
		const bool doBasisTransform /* = false */)
 : RSBase(primitivePoly, parityLength, codeLength, bitsPerSymbol, primitiveRootIdx,
 	firstConsecutiveRoot, interleaveDepth, doBasisTransform),
	_rsGeneratorPoly(1,1) {

	_initGeneratorPoly();

}


RSEncoder::~RSEncoder() {
}

void RSEncoder::_initGeneratorPoly() {
	SymVec b(2, 1);

	for ( std::size_t i = getFirstConsecutiveRoot() ; i < getParityLength() + getFirstConsecutiveRoot(); ++i ) {
		b[1] = gfExp_[(getPrimitiveRootIdx() * i) % getMaxSymbol()];
		_rsGeneratorPoly = gfPolyMul(_rsGeneratorPoly, b);
	}
}

SymVec RSEncoder::encodeOneWord_(const SymVec& msgIn, bool copyOrig /* = true */) const {
	SymVec msgOut = msgIn;

	msgOut.resize(getCodeLength(), 0);

	for ( std::size_t i = 0; i < getCodeLength() - getParityLength(); ++i ) {
		Sym coef = msgOut[i];
		if (coef != 0) {
			for ( std::size_t j = 0; j < _rsGeneratorPoly.size(); ++j)
				msgOut[i+j] ^= gfMul(_rsGeneratorPoly[j], coef);
		}
	}

	if (copyOrig) ACE_OS::memcpy(&msgOut[0], &msgIn[0], msgIn.size());

	return msgOut;
}

SymVec RSEncoder::encode(const SymVec& msgIn) const {

	SymVec msgOut = (getDoBasisTransform())? ccsdsDualBasisToConventional(msgIn) : msgIn;

	std::vector<SymVec> interleavedSyms = ccsdsInterleave(msgOut);
	std::vector<SymVec> encoded;

	std::vector<SymVec>::iterator pos;
	for ( pos = interleavedSyms.begin(); pos != interleavedSyms.end(); ++pos )
		encoded.push_back(encodeOneWord_(*pos, false));

	SymVec parity(getParityLength() * getInterleaveDepth(), 0);

	for ( std::size_t parityOffset = 0; parityOffset < getParityLength(); ++parityOffset )
		for ( std::size_t row = 0; row < getInterleaveDepth(); ++row )
			parity[parityOffset * getInterleaveDepth() + row ] =
				encoded[row][(getCodeLength() - getParityLength()) + parityOffset];

	// Convert just the parity symbols to dual basis representation.
	if (getDoBasisTransform()) parity = ccsdsConventionalToDualBasis(parity);

	return parity;
}

SymVec RSEncoder::encode(const Sym data[], const std::size_t dataLen) const {
	SymVec msgIn(data, dataLen);
	return encode(msgIn);
}

SymVec RSEncoder::encode(const NetworkData* data) const {
	SymVec msgIn(data);
	return encode(msgIn);
}

}
