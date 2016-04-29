/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modTM_FSH_Insert.hpp
 * @author Tad Kollar  
 *
 * $Id: modTM_FSH_Insert.hpp 1613 2012-06-27 14:18:17Z tkollar $ 
 * Copyright (c) 2012.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _MOD_TM_FSH_INSERT_HPP_
#define _MOD_TM_FSH_INSERT_HPP_

#include "TM_MasterChannel_Service.hpp"

namespace nTM_FSH_Insert {

//=============================================================================
/**
 * @class modTM_FSH_Insert
 * @author Tad Kollar  
 * @brief Insert data into the TM Transfer Frame Secondary Header.
 */
//=============================================================================
class modTM_FSH_Insert: public TM_MasterChannel_Service {
public:
	/// Primary constructor.
	/// @param newName An identifier for the module that's unique within the channel.
	/// @param newChannelName An identifier for the containing channel.
	/// @param newDLLPtr Pointer to the DLL management structure.
    modTM_FSH_Insert(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr);

	/// Destructor
    virtual ~modTM_FSH_Insert();

	/// Stop processing.
	void stopTraffic(bool shutdown = true);
    
	/// Read in data from multiple source, and sends it on to a single output.
	int svc();

	/// Override BaseTrafficHandler::getMRU() to return frameSize_ instead of MRU_.
	size_t getMRU() const { return getFrameSize(); }

	/// Override BaseTrafficHandler::setMRU() to set frameSize_ instead of MRU_.
	void setMRU(const size_t newVal) { setFrameSize(newVal); }

	/// Override BaseTrafficHandler::getMTU() to return frameSize_ instead of MTU_.
	size_t getMTU() const { return getFrameSize(); }

	/// Override BaseTrafficHandler::setMTU() to set frameSize_ instead of MTU_.
	void setMTU(const size_t newVal) { setFrameSize(newVal); }
	
	/// Compute the size of the FSH Data Field.
	ssize_t getFSHDataFieldSize() const { return getFSHSize() - 1; }
	
	/// Read-only accessor to _receivedFshUnitCount.
	u_int64_t getReceivedFshUnitCount() { return _receivedFshUnitCount; }

	/// Write-only accessor to _receivedFshUnitCount.
	void setReceivedFshUnitCount(const u_int64_t& newVal) { _receivedFshUnitCount = newVal; }

	/// Read-only accessor to _receivedFshOctetCount.
	u_int64_t getReceivedFshOctetCount() { return _receivedFshOctetCount; }

	/// Write-only accessor to _receivedFshOctetCount.
	void setReceivedFshOctetCount(const u_int64_t& newVal) { _receivedFshOctetCount = newVal; }

	/// Read-only accessor to _receivedFrameUnitCount.
	u_int64_t getReceivedFrameUnitCount() { return _receivedFrameUnitCount; }

	/// Write-only accessor to _receivedFrameUnitCount.
	void setReceivedFrameUnitCount(const u_int64_t& newVal) { _receivedFrameUnitCount = newVal; }

	/// Read-only accessor to _receivedFrameOctetCount.
	u_int64_t getReceivedFrameOctetCount() { return _receivedFrameOctetCount; }

	/// Write-only accessor to _receivedFrameOctetCount.
	void setReceivedFrameOctetCount(const u_int64_t& newVal) { _receivedFrameOctetCount = newVal; }

private:
	/// The total number of Frame Secondary Header units received.
	u_int64_t _receivedFshUnitCount;
	
	/// Total size of received Frame Secondary Header units.
	u_int64_t _receivedFshOctetCount;
	
	/// Total number of TM Transfer Frames received.
	u_int64_t _receivedFrameUnitCount;
	
	/// Total size of received TM Transfer frames.
	u_int64_t _receivedFrameOctetCount;
	
	/// If an FSH Unit is waiting, insert it into the FSH field in the supplied frame.
	/// @param frame The TM Transfer Frame to insert the FSH unit into.
	/// @return True is the insertion was successful, false otherwise.
	bool _insertWaitingFSHUnit(TM_Transfer_Frame* frame);
	

}; // class modTM_FSH_Insert

} // namespace nTM_FSH_Insert

#endif // _MOD_TM_FSH_INSERT_HPP_
