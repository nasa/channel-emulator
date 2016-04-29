/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modAOS_OCF_Insert.hpp
 * @author Tad Kollar  
 *
 * $Id: modAOS_OCF_Insert.hpp 2006 2013-05-31 19:59:14Z tkollar $
 * Copyright (c) 2012.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _MOD_AOS_OCF_INSERT_HPP_
#define _MOD_AOS_OCF_INSERT_HPP_

#include "AOS_MasterChannel_Service.hpp"

namespace nAOS_OCF_Insert {

//=============================================================================
/**
 * @class modAOS_OCF_Insert
 * @author Tad Kollar  
 * @brief Insert data into the AOS Transfer Frame Operational Control Field.
 */
//=============================================================================
class modAOS_OCF_Insert: public AOS_MasterChannel_Service {
public:
	/// Primary constructor.
	/// @param newName An identifier for the module that's unique within the channel.
	/// @param newChannelName An identifier for the containing channel.
	/// @param newDLLPtr Pointer to the DLL management structure.
    modAOS_OCF_Insert(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr);

	/// Destructor
    virtual ~modAOS_OCF_Insert();

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

	/// Total number of AOS Transfer Frames received.
	u_int64_t _receivedFrameUnitCount;

	/// Total size of received AOS Transfer frames.
	u_int64_t _receivedFrameOctetCount;

	/// If a Report is waiting, insert it into the OCF field in the supplied frame.
	/// @param frame The AOS Transfer Frame to insert the Report into.
	/// @return True is the insertion was successful, false otherwise.
	bool _insertWaitingReport(AOS_Transfer_Frame* frame);


}; // class modAOS_OCF_Insert

} // namespace nAOS_OCF_Insert

#endif // _MOD_AOS_OCF_INSERT_HPP_
