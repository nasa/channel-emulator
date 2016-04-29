/* -*- C++ -*- */

//=============================================================================
/**
 * @file   RSDecoder.hpp
 * @author Tad Kollar  
 *
 * $Id: RSDecoder.hpp 1990 2013-05-29 16:10:14Z tkollar $
 * Copyright (c) 2012.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _NASA_RS_DECODER_HPP_
#define _NASA_RS_DECODER_HPP_

#include "RSBase.hpp"
#include "nd_error.hpp"

namespace nasaCE {



//=============================================================================
/**
 * @class RSDecoder
 * @author Tad Kollar  
 * @brief Decode/correct a message containing Reed-Solomon parity symbols.
*/
//=============================================================================
class RSDecoder : public RSBase {
public:
	struct TooManyErrors: public nd_error {
		int errors;

		TooManyErrors(const std::string& e, const int errFound): nd_error(e, "TooManyErrors"), errors(errFound) { }

		static std::string msg(const int errsFound, const int errsMax) {
			std::ostringstream os;

			os << "Reed-Solomon codeword had " << errsFound << "; maximum is " << errsMax << ".";

			return os.str();
		}
	};

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
	RSDecoder(const int primitivePoly,
		const std::size_t parityLength,
		const std::size_t codeLength,
		const std::size_t bitsPerSymbol,
		const Sym primitiveRootIdx,
		const Sym firstConsecutiveRoot = 0,
		const std::size_t interleaveDepth = 1,
		const bool doBasisTransform = false);

	/// Destructor.
    ~RSDecoder();

	/// Perform error correction on a message with Reed-Solomon parity symbols appended.
	/// @param data Vector containing data to decode.
	/// @return A vector containing corrected data/parity symbols.
	SymVec correct(const SymVec& msgIn);

	/// Perform error correction on a vector with Reed-Solomon parity symbols appended
	/// and remove the parity symbols before returning.
	/// @param data Vector containing data to decode.
	/// @return A vector containing only the original message (minus parity).
	SymVec decode(const SymVec& msgIn);

	/// Perform error correction on a data buffer with Reed-Solomon parity symbols appended
	/// and remove the parity symbols before returning.
	/// @param data Buffer containing data to decode.
	/// @param dataLen Length of the data buffer.
	/// @return A vector containing only the original message (minus parity).
	SymVec decode(const Sym data[], const std::size_t dataLen);

	/// Perform error correction on Network Data unit with Reed-Solomon parity symbols appended
	/// and remove the parity symbols before returning.
	/// @param data NetworkData unit containing data to decide.
	/// @return A vector containing only the original message (minus parity).
	SymVec decode(const NetworkData* data);

	/// Write-only accessor to _lastCorrectedErrors.
	void setLastCorrectedErrors(const std::size_t& newVal) {
		_lastCorrectedErrors = newVal;
	}

	/// Add to _lastCorrectedErrors.
	void incLastCorrectedErrors(const std::size_t& newVal) {
		_lastCorrectedErrors += newVal;
	}

	/// Read-only accessor to _lastCorrectedErrors.
	std::size_t getLastCorrectedErrors() const {
		return _lastCorrectedErrors;
	}


protected:

	/// Substitute the appropriate roots into the provided message polynomial.
	/// @param msg The message to calculate syndromes for.
	/// @return A vector containing the syndromes. with no errors, all will be zero.
	SymVec calcSyndromes_(const SymVec& msg) const;

	/// Given a message, syndromes, and error positions, rebuild the message.
	/// @param msg The message to correct.
	/// @param synd A vector of syndromes, obtained from calcSyndromes().
	/// @param pos A vector of error positions.
	void correctErrata_(SymVec& msg, const SymVec& synd, const SymVec& pos) const;

	/// Calculate another set of syndromes so erasures won't interfere with locating errors.
	/// @param synd A vector of syndromes, obtained from calcSyndromes().
	/// @param pos Previously-found erasure positions.
	/// @param msgSize The length of the message from which the syndromes were originally calculated.
	/// @return A vector containing the Forney syndromes.
	SymVec forneySyndromes_(const SymVec& synd, const SymVec& pos, const std::size_t msgSize) const;

	/// Locate the positions containing erasures and errors.
	/// @param synd A vector of syndromes, obtained from calcSyndromes().
	/// @param msgSize The length of the message from which the syndromes were originally calculated.
	/// @return A vector containing the positions of erasures and errors.
	SymVec findErrors_(const SymVec& synd, const std::size_t msgSize) const;

	/// Apply error correction to exactly one, non-interleaved codeword.
	/// @param msgIn A codeword than may contain errors and erasures.
	/// @return The corrected codeword.
	/// @throw logic_error If error correction was unsuccessful.
	SymVec correctOneWord_(const SymVec& msgIn);

private:

	/// The number of errors + erasures corrected during the last correct() operation.
	std::size_t _lastCorrectedErrors;

}; // class RSDecoder

} // namespace nasaCE

#endif // _NASA_RS_DECODER_HPP_
