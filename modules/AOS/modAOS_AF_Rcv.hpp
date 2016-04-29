/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modAOS_AF_Rcv.hpp
 * @author Tad Kollar  
 *
 * $Id: modAOS_AF_Rcv.hpp 2489 2014-02-13 16:44:28Z tkollar $
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _MOD_AOS_AF_RCV_HPP_
#define _MOD_AOS_AF_RCV_HPP_

#include "AOS_PhysicalChannel_Service.hpp"
#include "AOS_Transfer_Frame.hpp"
#include "BaseTrafficHandler.hpp"
#include "RSDecoder.hpp"

namespace nAOS_AF_Rcv {

//=============================================================================
/**
 * @class modAOS_AF_Rcv
 * @author Tad Kollar  
 * @brief Receive AOS Transfer Frames, perform error control, and extract IN_SDUs.
 */
//=============================================================================
class modAOS_AF_Rcv : public AOS_PhysicalChannel_Service {
public:

	/// Primary constructor.
	/// @param newName An identifier for the module that's unique within the channel.
	/// @param newChannelName An identifier for the containing channel.
	/// @param newDLLPtr Pointer to the DLL management structure.
    modAOS_AF_Rcv(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr);

	/// Destructor.
    ~modAOS_AF_Rcv();

	/// Read in Transfer Frames, perform error control, extract the IN_SDU,
	/// send frame to Primary Output (if not idle) and IN_SDU to Aux. Output.
	int svc();

	/// Override BaseTrafficHandler::getMRU() to return frameSize_ instead of MRU_.
	size_t getMRU() const { return getFrameSize(); }

	/// Override BaseTrafficHandler::setMRU() to set frameSize_ instead of MRU_.
	void setMRU(const size_t newVal) { setFrameSize(newVal); }

	/// Override BaseTrafficHandler::getMTU() to return frameSize_ instead of MTU_.
	size_t getMTU() const { return getFrameSize(); }

	/// Override BaseTrafficHandler::setMTU() to set frameSize_ instead of MTU_.
	void setMTU(const size_t newVal) { setFrameSize(newVal); }

	/// Write-only accessor to _badFrameChecksumCount.
	void setBadFrameChecksumCount(const uint64_t newVal) { _badFrameChecksumCount = newVal; }

	/// Read-only accessor to _badFrameChecksumCount.
	uint64_t getBadFrameChecksumCount() const { return _badFrameChecksumCount; }

	/// Increment _badFrameChecksumCount by one.
	/// @return The new _badFrameChecksumCount.
	uint64_t incBadFrameChecksumCount() { return ( _badFrameChecksumCount = getBadFrameChecksumCount() + 1); }

	/// Write-only accessor to _correctedHeaderErrorCount.
	void setCorrectedHeaderErrorCount(const uint64_t& newVal) {
		_correctedHeaderErrorCount = newVal;
	}

	/// Read-only accessor to _correctedHeaderErrorCount.
	uint64_t getCorrectedHeaderErrorCount() const {
		return _correctedHeaderErrorCount;
	}

	/// Write-only accessor to _uncorrectedHeaderErrorCount.
	void setUncorrectedHeaderErrorCount(const uint64_t& newVal) {
		_uncorrectedHeaderErrorCount = newVal;
	}

	/// Read-only accessor to _uncorrectedHeaderErrorCount.
	uint64_t getUncorrectedHeaderErrorCount() const {
		return _uncorrectedHeaderErrorCount;
	}

	/// Write-only accessor to _errorlessHeaderUnitCount.
	void setErrorlessHeaderUnitCount(const uint64_t& newVal) {
		_errorlessHeaderUnitCount = newVal;
	}

	/// Read-only accessor to _errorlessHeaderUnitCount.
	uint64_t getErrorlessHeaderUnitCount() const {
		return _errorlessHeaderUnitCount;
	}

	/// Write-only accessor to _correctedHeaderUnitCount.
	void setCorrectedHeaderUnitCount(const uint64_t& newVal) {
		_correctedHeaderUnitCount = newVal;
	}

	/// Read-only accessor to _correctedHeaderUnitCount.
	uint64_t getCorrectedHeaderUnitCount() const {
		return _correctedHeaderUnitCount;
	}

	/// Write-only accessor to _uncorrectedHeaderUnitCount.
	void setUncorrectedHeaderUnitCount(const uint64_t& newVal) {
		_uncorrectedHeaderUnitCount = newVal;
	}

	/// Read-only accessor to _uncorrectedHeaderUnitCount.
	uint64_t getUncorrectedHeaderUnitCount() const {
		return _uncorrectedHeaderUnitCount;
	}

	/// Make sure the Transfer Frame is valid. Also performs error checking and correction.
	/// @param aos Pointer to the Transfer Frame to test.
	/// @param deleteBadFrame If true, delete an invalid frame.
	/// @return True if the frame checks out, false if unusable.
	bool validateFrame(AOS_Transfer_Frame*& aos, const bool deleteBadFrame = false);

protected:
	void updateTemplate_(); /// Rebuild _frameHeader.

private:
	AOS_Transfer_Frame* _frameTemplate; /// An empty, preconfigured frame to duplicate and make new frames from.

	/// Tally of frames rejected for having an invalid frame checksum.
	uint64_t _badFrameChecksumCount;

	/// The total number of errors fixed in the header.
	uint64_t _correctedHeaderErrorCount;

	/// The total number of errors in the header that could not be fixed..
	uint64_t _uncorrectedHeaderErrorCount;

	/// The total number of received units that had no errors in the header.
	uint64_t _errorlessHeaderUnitCount;

	/// The total number of received units with repaired headers.
	uint64_t _correctedHeaderUnitCount;

	/// The total number of received units with unrepaired headers.
	uint64_t _uncorrectedHeaderUnitCount;

	/// Object that handles error correction for the TF header.
	RSDecoder* _aosHeaderDecoder;

}; // class modAOS_AF_Rcv

} // namespace nAOS_AF_Rcv

#endif // _MOD_AOS_AF_RCV_HPP_
