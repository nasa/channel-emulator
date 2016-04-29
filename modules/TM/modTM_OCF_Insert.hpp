/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modTM_OCF_Insert.hpp
 * @author Tad Kollar  
 *
 * $Id: modTM_OCF_Insert.hpp 1630 2012-07-03 17:47:09Z tkollar $ 
 * Copyright (c) 2012.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _MOD_TM_OCF_INSERT_HPP_
#define _MOD_TM_OCF_INSERT_HPP_

#include "TM_MasterChannel_Service.hpp"

namespace nTM_OCF_Insert {

//=============================================================================
/**
 * @class modTM_OCF_Insert
 * @author Tad Kollar  
 * @brief Insert data into the TM Transfer Frame Operational Control Field.
 */
//=============================================================================
class modTM_OCF_Insert: public TM_MasterChannel_Service {
public:
	/// Primary constructor.
	/// @param newName An identifier for the module that's unique within the channel.
	/// @param newChannelName An identifier for the containing channel.
	/// @param newDLLPtr Pointer to the DLL management structure.
    modTM_OCF_Insert(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr);

	/// Destructor
    virtual ~modTM_OCF_Insert();

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
	
	/// Read-only accessor to _receivedReportUnitCount.
	u_int64_t getReceivedReportUnitCount() { return _receivedReportUnitCount; }

	/// Write-only accessor to _receivedReportUnitCount.
	void setReceivedReportUnitCount(const u_int64_t& newVal) { _receivedReportUnitCount = newVal; }

	/// Read-only accessor to _receivedReportOctetCount.
	u_int64_t getReceivedReportOctetCount() { return _receivedReportOctetCount; }

	/// Write-only accessor to _receivedReportOctetCount.
	void setReceivedReportOctetCount(const u_int64_t& newVal) { _receivedReportOctetCount = newVal; }

	/// Read-only accessor to _receivedFrameUnitCount.
	u_int64_t getReceivedFrameUnitCount() { return _receivedFrameUnitCount; }

	/// Write-only accessor to _receivedFrameUnitCount.
	void setReceivedFrameUnitCount(const u_int64_t& newVal) { _receivedFrameUnitCount = newVal; }

	/// Read-only accessor to _receivedFrameOctetCount.
	u_int64_t getReceivedFrameOctetCount() { return _receivedFrameOctetCount; }

	/// Write-only accessor to _receivedFrameOctetCount.
	void setReceivedFrameOctetCount(const u_int64_t& newVal) { _receivedFrameOctetCount = newVal; }

private:
	/// The total number of Operational Control units received.
	u_int64_t _receivedReportUnitCount;
	
	/// Total size of received Operational Control units.
	u_int64_t _receivedReportOctetCount;
	
	/// Total number of TM Transfer Frames received.
	u_int64_t _receivedFrameUnitCount;
	
	/// Total size of received TM Transfer frames.
	u_int64_t _receivedFrameOctetCount;
	
	/// If a Report is waiting, insert it into the OCF field in the supplied frame.
	/// @param frame The TM Transfer Frame to insert the Report into.
	/// @return True is the insertion was successful, false otherwise.
	bool _insertWaitingReport(TM_Transfer_Frame* frame);
	

}; // class modTM_OCF_Insert

} // namespace nTM_OCF_Insert

#endif // _MOD_TM_OCF_INSERT_HPP_
