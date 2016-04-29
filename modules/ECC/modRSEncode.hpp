/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modRSEncode.hpp
 * @author Tad Kollar  
 *
 * $Id: modRSEncode.hpp 2322 2013-08-07 16:14:42Z tkollar $
 * Copyright (c) 2011.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _NASA_MOD_RS_ENCODE_HPP_
#define _NASA_MOD_RS_ENCODE_HPP_

#include <BaseTrafficHandler.hpp>
#include "RSEncoder.hpp"
#include "nd_error.hpp"

namespace nRSEncode {

//=============================================================================
/**
 * @class modRSEncode
 * @brief Generate Reed-Solomon codewords for incoming data blocks
 */
//=============================================================================
class modRSEncode : public BaseTrafficHandler {
public:

	/// Primary constructor.
	/// @param newName An identifier for the module that's unique within the channel.
	/// @param newChannelName An identifier for the containing channel.
	/// @param newDLLPtr Pointer to the DLL management structure.
    modRSEncode(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr);

	/// Destructor.
    ~modRSEncode();

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
	void rebuildEncoder();

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

	/// Recompute _dataLength based on _maxErrorsPerCodeword and codeLength.
	void _resetLengths() {
		_dataLength = codeLength - (_maxErrorsPerCodeword * 2);
	}

	/// Pointer to the object that will actually handle the encoding.
	RSEncoder* _rsEncoder;

}; // class modRSEncode

} // namespace nRSEncode

#endif // _NASA_MOD_RS_ENCODE_HPP_
