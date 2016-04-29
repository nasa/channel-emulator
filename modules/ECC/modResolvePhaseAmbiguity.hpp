/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modResolvePhaseAmbiguity.hpp
 * @author Tad Kollar  
 *
 * $Id: modResolvePhaseAmbiguity.hpp 2323 2013-08-07 16:22:35Z tkollar $
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _MOD_EMULATE_PHASE_AMBIGUITY_HPP_
#define _MOD_EMULATE_PHASE_AMBIGUITY_HPP_

#include <BaseTrafficHandler.hpp>
#include <ace/Event_Handler.h>

namespace nResolvePhaseAmbiguity {

//=============================================================================
/**
 * @class modResolvePhaseAmbiguity
 * @author Tad Kollar  
 * @brief Detect phase ambiguity by ASM and bit shifts and/or inverts to correct.
 */
//=============================================================================
class modResolvePhaseAmbiguity : public BaseTrafficHandler {

public:

	/// Primary constructor.
	/// @param newName An identifier for the module that's unique within the channel.
	/// @param newChannelName An identifier for the containing channel.
	/// @param newDLLPtr Pointer to the DLL management structure.
    modResolvePhaseAmbiguity(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr);

	/// Destructor.
    ~modResolvePhaseAmbiguity();

	/// Reads in data, possibly introduces error(s), and sends it to the output handler.
	int svc();

	/// Write-only accessor to _leftShiftBits and _leftShiftBitsSetting.
	void setLeftShiftBits(const int& newVal) {
		_leftShiftBits = newVal;
		if ( _leftShiftBits < 0 ) _leftShiftBits = 0;
		else if ( _leftShiftBits > 7 ) _leftShiftBits %= 8;

		_leftShiftBitsSetting = _leftShiftBits;
	}

	/// Read-only accessor to _leftShiftBits.
	int getLeftShiftBits() const { return _leftShiftBits; }

	/// Write-only accessor to _invert and _invertSetting.
	void setInversion(const bool newVal) {
		_invert = newVal;
		_invertSetting = newVal;
	}

	/// Read-only accessor to _invert.
	bool getInversion() const { return _invert; }

	/// Read-only accessor to _maxUsecsForNewData.
	int getMaxUsecsForNewData() const {
		return _maxUsecsForNewData;
	}

	/// Write-only accessor to _waitForNewDataTime and _maxUsecsForNewData.
	void setMaxUsecsForNewData(const int newVal) {
		_maxUsecsForNewData = newVal;
		_waitForNewDataInterval.sec(newVal/1000000);
		_waitForNewDataInterval.usec(newVal%1000000);
	}

	/// Read-only accessor to _waitForNewDataInterval.
	ACE_Time_Value getWaitForNewDataInterval() const {
		return _waitForNewDataInterval;
	}

	/// Write-only accessor to _waitForNewDataTime and _maxUsecsForNewData.
	void setWaitForNewDataInterval(ACE_Time_Value& timeWait) {
		ACE_UINT64 totalUsecs;
		timeWait.to_usec(totalUsecs);

		_maxUsecsForNewData = static_cast<int>(totalUsecs);
		_waitForNewDataInterval = timeWait;
	}

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

	/// Write-only accessor to _detectShift and _detectShiftSetting.
	void setDetectShift(bool newVal) {
		_detectShift = newVal;
		_detectShiftSetting = newVal;
	}

	/// Read-only accessor to _detectShift.
	bool getDetectShift() const { return _detectShift; }

	/// Write-only accessor to _allowedMarkerBitErrors and _allowedMarkerBitErrorsSetting.
	void setAllowedMarkerBitErrors(const int newVal) {
		_allowedMarkerBitErrors = newVal;
		_allowedMarkerBitErrorsSetting = newVal;
	}

	/// Read-only accessor to _allowedMarkerBitErrors.
	int getAllowedMarkerBitErrors() const {
		return _allowedMarkerBitErrors;
	}

private:
	/// The number of bits the data has been shifted by, 0 - 7.
	int _leftShiftBits;

	/// Representation of _leftShiftBits in the config file.
	Setting& _leftShiftBitsSetting;

	/// Whether or not the bits have been inverted.
	bool _invert;

	/// Representation of _invert in the config file.
	Setting& _invertSetting;

	/// Time to wait for incoming data before sending what is already stored.
	ACE_Time_Value _waitForNewDataInterval;

	/// Representation of _waitForNewDataInterval in the config file; useds microseconds instead of ACE_Time_Value.
	Setting& _maxUsecsForNewData;

	/// Add _waitForNewDataInterval to the current time.
	ACE_Time_Value _getWaitUntilTime() const {
		return ACE_High_Res_Timer::gettimeofday_hr() + _waitForNewDataInterval;
	}

	/// The type of ASM marker.
	Setting& _marker;

	/// Whether _marker has been updated or the ASM buffer otherwise needs to be (re)created.
	bool _rebuildMarkerTable;

	static const size_t _markerTableRows;

	/// The two-dimensional array that holds the ASM in row 0, the ASM left-shifted 1 bit
	/// in row 1, etc, up to row 7. Row 8 is row 0 inverted, row 9 is row 1 inverted, etc.
	ACE_UINT8** _markerTable;

	/// The number of octets in the ASM.
	size_t _markerLen;

	/// Create a buffer with the contents of the new marker.
	void _updateMarkerTable();

	/// Determine whether the contents of the supplied buffer match the contents
	/// of the marker table row that's been subjected to the specified shifting
	/// and inversion.
	/// @param buffer The data to compare to the table.
	/// @param leftShiftBits Number of bits to shift, 0-7.
	/// @param isInverted Whether all the bits are flipped.
	/// @return True if the buffer matches.
	bool _inTableAt(const ACE_UINT8* buffer, const uint8_t& leftShiftBits, const bool isInverted);

	/// Check through each row in the ASM table to determine the shift/version values.
	/// @param buffer The data to compare to the table.
	/// @param leftShiftBits Returned with the number of bits in the shift, 0-7. Value supplied by the call is tried first.
	/// @param isInverted Returned with the value of the inversion. Value supplied by the call is tried first.
	/// @return True if the buffer was found in the table.
	bool _findInTable(const ACE_UINT8* buffer, uint8_t& leftShiftBits, uint8_t& isInverted);

	/// Iterate through the data block looking for shifted/inverted ASMs and count them.
	/// Whichever left shift/inversion count is the highest indicates the way the entire
	/// block will be right shifted and/or inverted to correct the problem.
	/// @param data The block to be searched for ASMs (not modified at this time).
	void _findShift(NetworkData*& data);

	/// If true, try to detect the shift/inversion ourselves via finding ASMs.
	bool _detectShift;

	/// Representation of _detectShift in the config file.
	Setting& _detectShiftSetting;

	/// The number of bit errors allowed for each ASM.
	int _allowedMarkerBitErrors;

	/// Config file interface to _allowedMarkerBitErrors.
	Setting& _allowedMarkerBitErrorsSetting;

}; // class modResolvePhaseAmbiguity

} // namespace nResolvePhaseAmbiguity

#endif // _MOD_EMULATE_PHASE_AMBIGUITY_HPP_
