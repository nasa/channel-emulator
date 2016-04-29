/* -*- C++ -*- */

//=============================================================================
/**
 * @file   RSDecoder.cpp
 * @author Tad Kollar  
 *
 * $Id: RSDecoder.cpp 2309 2013-07-11 13:32:23Z tkollar $
 * Copyright (c) 2012.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "RSDecoder.hpp"

namespace nasaCE {

RSDecoder::RSDecoder(const int primitivePoly,
		const std::size_t parityLength,
		const std::size_t codeLength,
		const std::size_t bitsPerSymbol,
		const Sym primitiveRootIdx,
		const Sym firstConsecutiveRoot /* = 0 */,
		const std::size_t interleaveDepth /* = 1 */,
		const bool doBasisTransform /* = false */ )
 : RSBase(primitivePoly, parityLength, codeLength, bitsPerSymbol, primitiveRootIdx,
 	firstConsecutiveRoot, interleaveDepth, doBasisTransform) {
}


RSDecoder::~RSDecoder()
{
}

SymVec RSDecoder::calcSyndromes_(const SymVec& msg) const {
	SymVec synd(getParityLength());

	for ( std::size_t j = 0; j < getParityLength(); ++j )
		synd[j] = gfPolyEval(msg, gfExp_[(getPrimitiveRootIdx() * (j+ getFirstConsecutiveRoot())) % getMaxSymbol()]);

	return synd;
}

void RSDecoder::correctErrata_(SymVec& msg, const SymVec& synd, const SymVec& pos) const {
	// calculate error locator polynomial
	SymVec q(1, 1), a(2, 1);

	std::size_t i;

	for ( i = 0; i < pos.size(); ++i ) {
		a[0] = gfExp_[(getPrimitiveRootIdx() * (msg.size() - 1 - pos[i])) % getMaxSymbol()];
		q = gfPolyMul(q, a);
	}

	// calculate error evaluator polynomial
	SymVec p(pos.size());
	for ( i = 0; i < pos.size(); ++i )
		p[p.size() - (i+1)] = synd[i]; 	// copy syndromes in reverse order

	p = gfPolyMul(p, q);
	p.assign(&p[p.size() - pos.size()], &p[p.size()]);

	// formal derivative of error locator eliminates even terms
	SymVec q_tmp;
	for ( i = q.size() & 1; i < q.size(); i += 2 ) q_tmp.push_back(q[i]);
	q = q_tmp;

	// compute corrections
	Sym x, y, z;
	for ( i = 0; i < pos.size(); ++i ) {
		x = gfExp_[(getPrimitiveRootIdx() * (pos[i] + getMaxSymbol() + 1 - msg.size())) % getMaxSymbol()];
		y = gfPolyEval(p, x);
		z = gfPolyEval(q, gfMul(x,x));
		msg[pos[i]] ^= gfMul(gfExp_[(getPrimitiveRootIdx() * getFirstConsecutiveRoot()*(pos[i]+1)) % getMaxSymbol()], gfDiv(y,gfMul(x,z)));
	}
}

SymVec RSDecoder::forneySyndromes_(const SymVec& synd, const SymVec& pos, const std::size_t msgSize) const {
	SymVec fsynd = synd;

	for ( std::size_t i = 0; i < pos.size(); ++i ) {
		Sym x = gfExp_[msgSize - 1 - pos[i]];
		for ( std::size_t i = 0; i < fsynd.size() - 1; ++i )
			fsynd[i] = gfMul(fsynd[i], x) ^ fsynd[i+1];
		fsynd.pop_back();
	}

	return fsynd;
}

SymVec RSDecoder::findErrors_(const SymVec& synd, const std::size_t msgSize) const {
	// find error locator polynomial with Berlekamp-Massey algorithm
	SymVec errPoly(1,1);
	SymVec oldPoly(1,1);

	for ( std::size_t i = 0; i < synd.size(); ++i ) {
		oldPoly.push_back(0);
		Sym delta = synd[i];

		for ( std::size_t j = 1; j < errPoly.size(); ++j )
			delta ^= gfMul(errPoly[errPoly.size() - 1 - j], synd[i - j]);

		if ( delta != 0 ) {
			if ( oldPoly.size() > errPoly.size() ) {
				SymVec newPoly = gfPolyScale(oldPoly, delta);
				oldPoly = gfPolyScale(errPoly, gfDiv(1, delta));
				errPoly = newPoly;
			}
			errPoly = gfPolyAdd(errPoly, gfPolyScale(oldPoly, delta));
		}
	}

	unsigned errs = errPoly.size() - 1;

	if ( errs * 2U > synd.size() ) throw TooManyErrors(TooManyErrors::msg(errs, synd.size() / 2U), errs);

	// find zeros of error polynomial
	SymVec errPos;

	for ( std::size_t i = 0; i < msgSize; ++i ) {
		if ( gfPolyEval(errPoly, gfExp_[getPrimitiveRootIdx()*(getMaxSymbol() - i) % getMaxSymbol()]) == 0 ) {
			errPos.push_back(msgSize - 1 - i);
		}
	}

	if ( errPos.size() != errs ) throw std::logic_error("Couldn't find error locations.");

	return errPos;
}


SymVec RSDecoder::correctOneWord_(const SymVec& msgIn) {
	if (msgIn.size() != getCodeLength()) throw std::logic_error("Codeword with bad length received.");

	SymVec msgOut = msgIn;

	// find erasures
	SymVec erasePos;

	for ( std::size_t i = 0; i < msgOut.size(); ++i ) {
		if ( msgOut[i] < 0 || msgOut[i] > getMaxSymbol()) {
			msgOut[i] = 0;
			erasePos.push_back(i);
		}
	}

	if (erasePos.size() > getParityLength()) throw std::logic_error("Too many erasures to correct.");

	SymVec synd = calcSyndromes_(msgOut);

	int syndSum = 0;
	SymVec::iterator pos;
	for ( pos = synd.begin(); pos != synd.end(); ++pos ) syndSum += *pos;

	if ( syndSum == 0 ) {
		return msgOut; // no errors
	}

	SymVec fsynd = forneySyndromes_(synd, erasePos, msgOut.size());
	SymVec errPos = findErrors_(fsynd, msgOut.size());

	erasePos.insert(erasePos.end(), errPos.begin(), errPos.end());

	correctErrata_(msgOut, synd, erasePos);
	synd = calcSyndromes_(msgOut);

	for ( pos = synd.begin(); pos != synd.end(); ++pos ) {
		if ( *pos > 0 ) {
			throw std::logic_error("Non-zero syndromes after error correction.");
		}
	}

	incLastCorrectedErrors(erasePos.size());

	return msgOut;
}

SymVec RSDecoder::correct(const SymVec& msgIn) {
	setLastCorrectedErrors(0);
	std::string errMsg("");
	int uncorrectedErrCount(0);

	SymVec msgOut = (getDoBasisTransform())? ccsdsDualBasisToConventional(msgIn) : msgIn;

	std::vector<SymVec> interleavedSyms = ccsdsInterleave(msgOut, true);
	std::vector<SymVec> corrected;

	std::vector<SymVec>::iterator pos;
	for ( pos = interleavedSyms.begin(); pos != interleavedSyms.end(); ++pos ) {
		try {
			corrected.push_back(correctOneWord_(*pos));
		}
		catch (TooManyErrors& tme) {
			// Continue to the end of the entire message to find all the errors.
			errMsg += std::string(tme.what()) + " ";
			uncorrectedErrCount += tme.errors;
		}
	}

	// Report the total uncorrected error count back to the caller.
	if (uncorrectedErrCount > 0)
		throw TooManyErrors(errMsg, uncorrectedErrCount);

	msgOut = (getDoBasisTransform())?
		ccsdsConventionalToDualBasis(ccsdsDeinterleave(corrected)) : ccsdsDeinterleave(corrected);

	return msgOut;
}

SymVec RSDecoder::decode(const SymVec& msgIn) {
	SymVec msgOut = correct(msgIn), msgNoVirtualFill;
	std::size_t virtualFillOctets = getCodeLength() * getInterleaveDepth() - msgIn.size();

	msgNoVirtualFill.assign(msgOut.begin() + virtualFillOctets, msgOut.end());

	return msgNoVirtualFill;
}

SymVec RSDecoder::decode(const Sym data[], const std::size_t dataLen) {
	SymVec msgIn(data, dataLen);
	return decode(msgIn);
}

SymVec RSDecoder::decode(const NetworkData* data) {
	SymVec msgIn(data);
	return decode(msgIn);
}

}
