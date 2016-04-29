/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modASM_Remove.hpp
 * @author Tad Kollar  
 *
 * $Id: modASM_Remove.hpp 2482 2014-02-13 15:10:19Z tkollar $
 * Copyright (c) 2010.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _MOD_ASM_REMOVE_HPP_
#define _MOD_ASM_REMOVE_HPP_

#include "BaseTrafficHandler.hpp"

namespace nASM_Remove {

//=============================================================================
/**
	@class modASM_Remove
	@author Tad Kollar  
	@brief Remove the ASM markers from a traffic stream.
*/
//=============================================================================
class modASM_Remove: public BaseTrafficHandler {
public:
	/// Primary constructor.
	/// @param newName An identifier for the module that's unique within the channel.
	/// @param newChannelName An identifier for the containing channel.
	/// @param newDLLPtr Pointer to the DLL management structure.
    modASM_Remove(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr);

	/// Destructor.
    ~modASM_Remove();

	/// Reads in some data and looks for sync markers.
	int svc();

	/// Set the contents of _marker.
	/// @param asmBuf The buffer holding the new marker.
	/// @param bufLen The length of the pattern in the buffer.
	void setMarker(const uint8_t* asmBuf, const int bufLen);

	/// Set the contents of _marker.
	/// @param newMarker A vector containing the new pattern.
	void setMarker(const std::vector<uint8_t>& newMarker);

	/// Return the contents of _marker.
	/// @param marker A vector to copy _marker into.
	void getMarker(std::vector<uint8_t>& marker) const;

	/// Write-only accessor to _expectedUnitLength.
	void setExpectedUnitLength(const int& newVal) {
		_expectedUnitLength = newVal;
		_expectedUnitLengthSetting = newVal;
	}

	/// Read-only accessor to _expectedUnitLength.
	int getExpectedUnitLength() const { return _expectedUnitLength; }

	/// Write-only accessor to _allowedMarkerBitErrors and _allowedMarkerBitErrorsSetting.
	void setAllowedMarkerBitErrors(const int newVal) {
		_allowedMarkerBitErrors = newVal;
		_allowedMarkerBitErrorsSetting = newVal;
	}

	/// Read-only accessor to _allowedMarkerBitErrors.
	int getAllowedMarkerBitErrors() const {
		return _allowedMarkerBitErrors;
	}

	/// Write-only accessor to _asmCount.
	void setAsmCount(const uint64_t& newVal) {
		_asmCount = newVal;
	}

	/// Read-only accessor to _asmCount.
	uint64_t getAsmCount() const {
		return _asmCount;
	}

	/// Write-only accessor to _asmValidCount.
	void setAsmValidCount(const uint64_t& newVal) {
		_asmValidCount = newVal;
	}

	/// Read-only accessor to _asmValidCount.
	uint64_t getAsmValidCount() const {
		return _asmValidCount;
	}

	/// Write-only accessor to _asmMissedCount.
	void setAsmMissedCount(const uint64_t& newVal) {
		_asmMissedCount = newVal;
	}

	/// Read-only accessor to _asmMissedCount.
	uint64_t getAsmMissedCount() const {
		return _asmMissedCount;
	}

	/// Write-only accessor to _asmDiscoveredCount.
	void setAsmDiscoveredCount(const uint64_t& newVal) {
		_asmDiscoveredCount = newVal;
	}

	/// Read-only accessor to _asmDiscoveredCount.
	uint64_t getAsmDiscoveredCount() const {
		return _asmDiscoveredCount;
	}

	/// Write-only accessor to _asmBitErrorsAllowed.
	void setAsmBitErrorsAllowed(const uint64_t& newVal) {
		_asmBitErrorsAllowed = newVal;
	}

	/// Increase _asmBitErrorsAllowed by the specified amount.
	void incAsmBitErrorsAllowed(const uint16_t& inc = 1) {
		_asmBitErrorsAllowed += inc;
	}

	/// Read-only accessor to _asmBitErrorsAllowed.
	uint64_t getAsmBitErrorsAllowed() const {
		return _asmBitErrorsAllowed;
	}

	/// Write-only accessor to _asmBitErrorsRejected.
	void setAsmBitErrorsRejected(const uint64_t& newVal) {
		_asmBitErrorsRejected = newVal;
	}

	/// Increase _asmBitErrorsRejected by the specified amount.
	void incAsmBitErrorsRejected(const uint16_t& inc = 1) {
		_asmBitErrorsRejected += inc;
	}

	/// Read-only accessor to _asmBitErrorsRejected.
	uint64_t getAsmBitErrorsRejected() const {
		return _asmBitErrorsRejected;
	}

	/// Write-only accessor to _asmAllowedWithBitErrorsCount.
	void setAsmAllowedWithBitErrorsCount(const uint64_t& newVal) {
		_asmAllowedWithBitErrorsCount = newVal;
	}

	/// Increase _asmAllowedWithBitErrorsCount by the specified amount.
	void incAllowedWithBitErrorsCount(const uint16_t& inc = 1) {
		_asmAllowedWithBitErrorsCount += inc;
	}

	/// Read-only accessor to _asmAllowedWithBitErrorsCount.
	uint64_t getAsmAllowedWithBitErrorsCount() const {
		return _asmAllowedWithBitErrorsCount;
	}

	/// Write-only accessor to _asmRejectedWithBitErrorsCount.
	void setAsmRejectedWithBitErrorsCount(const uint64_t& newVal) {
		_asmRejectedWithBitErrorsCount = newVal;
	}

	/// Increase _asmRejectedWithBitErrorsCount by the specified amount.
	void incRejectedWithBitErrorsCount(const uint16_t& inc = 1) {
		_asmRejectedWithBitErrorsCount += inc;
	}

	/// Read-only accessor to _asmRejectedWithBitErrorsCount.
	uint64_t getAsmRejectedWithBitErrorsCount() const {
		return _asmRejectedWithBitErrorsCount;
	}

	/// Write-only accessor to _asmSearchCount.
	void setSearchCount(const uint64_t& newVal) {
		_asmSearchCount = newVal;
	}

	/// Increase _asmSearchCount by the specified amount.
	void incSearchCount(const uint16_t& inc = 1) {
		_asmSearchCount += inc;
	}

	/// Read-only accessor to _asmSearchCount.
	uint64_t getSearchCount() const {
		return _asmSearchCount;
	}

	/// Write-only accessor to _asmPartialMismatch.
	void setPartialMismatchCount(const uint64_t& newVal) {
		_asmPartialMismatch = newVal;
	}

	/// Increase _asmPartialMismatch by the specified amount.
	void incPartialMismatchCount(const uint16_t& inc = 1) {
		_asmPartialMismatch += inc;
	}

	/// Read-only accessor to _asmPartialMismatch.
	uint64_t getPartialMismatchCount() const {
		return _asmPartialMismatch;
	}

private:

	/// The type of ASM marker.
	Setting& _marker;

	/// The length in octets of a good data unit.
	int _expectedUnitLength;

	/// Config file reference to _expectedUnitLength.
	Setting& _expectedUnitLengthSetting;

	/// The number of bit errors allowed for each ASM.
	int _allowedMarkerBitErrors;

	/// Config file interface to _allowedMarkerBitErrors.
	Setting& _allowedMarkerBitErrorsSetting;

	/// Whether _marker has been updated or the ASM buffer otherwise needs to be (re)created.
	bool _rebuildMarker;

	/// Create a buffer with the contents of the new marker.
	/// @param buffer Deleted if not null. Then a new buffer is allocated and filled.
	/// @param bufferLen Receives the new marker length.
	void _updateMarker(ACE_UINT8*& buffer, size_t& bufferLen);

	void _send(NetworkData*& goodUnit);

	/// Determine if the specified area in the unit buffer matches the specified portion of
	/// the marker buffer. It doesn't have to be a perfect match if allowedMarkerBERs is
	/// greater than zero.
	/// @param ptrUnitBuf Area to compare in the unit buffer.
	/// @param ptrMarkerBuf Area to compare in the marker buffer.
	/// @param testLen Number of octets to compare.
	/// @return True if the match is acceptable enough.
	bool _markerMatch(const ACE_UINT8* ptrUnitBuf, const ACE_UINT8* ptrMarkerBuf, const size_t testLen);

	/// The total number of bit errors encountered for the current unit.
	int _currentBitErrors;

	/// The grand total of ASMs that were located.
	uint64_t _asmCount;

	/// Tally of ASMs found exactly where expected.
	uint64_t _asmValidCount;

	/// The tally of ASMs that were not exactly where they were supposed to be.
	uint64_t _asmMissedCount;

	/// The tally of failed searches for an ASM.
	uint64_t _asmSearchCount;

	/// Tally of ASMs found only after a search.
	uint64_t _asmDiscoveredCount;

	/// The number of bit errors that were allowed in ASMs.
	uint64_t _asmBitErrorsAllowed;

		/// The number of bit errors that were not allowed in ASMs.
	uint64_t _asmBitErrorsRejected;

	/// The number of allowed ASMs that had bit errors.
	uint64_t _asmAllowedWithBitErrorsCount;

	/// The number of disallowed ASMs that had bit errors.
	uint64_t _asmRejectedWithBitErrorsCount;

	/// The number of ASMs that matched at the head but not the tail.
	uint64_t _asmPartialMismatch;

}; // class modASM_Remove

} // namespace nASM_Remove

#endif // _MOD_ASM_REMOVE_HPP_
