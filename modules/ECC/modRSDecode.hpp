/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modRSDecode.hpp
 * @author Tad Kollar  
 *
 * $Id: modRSDecode.hpp 2322 2013-08-07 16:14:42Z tkollar $
 * Copyright (c) 2011.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _NASA_MOD_RS_DECODE_HPP_
#define _NASA_MOD_RS_DECODE_HPP_

#include <BaseTrafficHandler.hpp>
#include "RSDecoder.hpp"
#include "nd_error.hpp"

namespace nRSDecode {

//=============================================================================
/**
 * @class modRSDecode
 * @brief Extract messages from Reed-Solomon codewords for incoming data blocks.
 */
//=============================================================================
class modRSDecode : public BaseTrafficHandler {
public:
	/// Primary constructor.
	/// @param newName An identifier for the module that's unique within the channel.
	/// @param newChannelName An identifier for the containing channel.
	/// @param newDLLPtr Pointer to the DLL management structure.
    modRSDecode(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr);

	/// Destructor.
    ~modRSDecode();

	/// Reads in data, extracts a portion, and sends it to the output handler.
	int svc();

	/// Write-only accessor to _maxErrorsPerCodeword/_maxErrorsPerCodewordSetting.
	/// @throw BadValue If newVal is other than 8 or 16.
	void setMaxErrorsPerCodeword(const int& newVal) {
		if ( newVal != 8 && newVal != 16 ) throw BadValue(BadValue::msg("Max errors per codeword", newVal));
		_maxErrorsPerCodewordSetting = static_cast<int>(newVal);
		_maxErrorsPerCodeword = newVal;
	}

	/// Read-only accessor to _maxErrorsPerCodeword/_maxErrorsPerCodewordSetting.
	int getMaxErrorsPerCodeword() const { return _maxErrorsPerCodeword; }

	/// Write-only accessor to _interleavingDepth/_interleavingDepthSetting.
	void setInterleavingDepth(const int newVal) {
		_interleavingDepthSetting = static_cast<int>(newVal);
		_interleavingDepth = newVal;
	}

	/// Read-only accessor to _interleavingDepth/_interleavingDepthSetting.
	int getInterleavingDepth() const { return _interleavingDepth; }

	/// If interleave depth or max errors have changed, this must be called for the
	/// changes to take effect.
	void rebuildDecoder();

	/// Write-only accessor to _correctedErrorCount.
	void setCorrectedErrorCount(const uint64_t& newVal) {
		_correctedErrorCount = newVal;
	}

	/// Read-only accessor to _correctedErrorCount.
	uint64_t getCorrectedErrorCount() const {
		return _correctedErrorCount;
	}

	/// Write-only accessor to _uncorrectedErrorCount.
	void setUncorrectedErrorCount(const uint64_t& newVal) {
		_uncorrectedErrorCount = newVal;
	}

	/// Read-only accessor to _uncorrectedErrorCount.
	uint64_t getUncorrectedErrorCount() const {
		return _uncorrectedErrorCount;
	}

	/// Write-only accessor to _errorlessUnitCount.
	void setErrorlessUnitCount(const uint64_t& newVal) {
		_errorlessUnitCount = newVal;
	}

	/// Read-only accessor to _errorlessUnitCount.
	uint64_t getErrorlessUnitCount() const {
		return _errorlessUnitCount;
	}

	/// Write-only accessor to _correctedUnitCount.
	void setCorrectedUnitCount(const uint64_t& newVal) {
		_correctedUnitCount = newVal;
	}

	/// Read-only accessor to _correctedUnitCount.
	uint64_t getCorrectedUnitCount() const {
		return _correctedUnitCount;
	}

	/// Write-only accessor to _uncorrectedUnitCount.
	void setUncorrectedUnitCount(const uint64_t& newVal) {
		_uncorrectedUnitCount = newVal;
	}

	/// Read-only accessor to _uncorrectedUnitCount.
	uint64_t getUncorrectedUnitCount() const {
		return _uncorrectedUnitCount;
	}

	/// Encoder attribute that will not change.
	static const int primitivePoly;

	/// Encoder attribute that will not change.
	static const std::size_t codeLength;

	/// Encoder attribute that will not change.
	static const std::size_t bitsPerSymbol;

	/// Encoder attribute that will not change.
	static const Sym primitiveRootIdx;

	/// Encoder attribute that will not change.
	static const bool doBasisTransform;

private:
	/// Reed-Solomon error correction capability, in symbols, within an R-S codeword.
	/// May be selected to be 16 or 8 R-S symbols.
	int _maxErrorsPerCodeword;

	/// The config file setting for _maxErrorsPerCodeword.
	Setting& _maxErrorsPerCodewordSetting;

	/// The allowable values of interleaving depth are I=1, 2, 3, 4, 5, and 8.
	/// I=1 is equivalent to the absence of interleaving.
	int _interleavingDepth;

	/// The config file setting for _interleavingDepth.
	Setting& _interleavingDepthSetting;

	/// The size of the data area in the codeword.
	int _dataLength;

	/// Recompute _fecLength and _dataLength based on _maxErrorsPerCodeword and codeLength.
	void _resetLengths() {
		_dataLength = codeLength - (_maxErrorsPerCodeword * 2 * _interleavingDepth);
	}

	/// The total number of errors fixed by the module.
	uint64_t _correctedErrorCount;

	/// The total number of errors that could not be fixed by the module.
	uint64_t _uncorrectedErrorCount;

	/// The total number of received units that had no errors.
	uint64_t _errorlessUnitCount;

	/// The total number of received units that were repaired.
	uint64_t _correctedUnitCount;

	/// The total number of received units that were unrepaired.
	uint64_t _uncorrectedUnitCount;

	/// Pointer to the object that will actually handle the decoding.
	RSDecoder* _rsDecoder;

}; // class modRSDecode

} // namespace nRSDecode

#endif // _NASA_MOD_RS_DECODE_HPP_
