/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modAOS_IN_SDU_Insert.hpp
 * @author Tad Kollar  
 *
 * $Id: modAOS_IN_SDU_Insert.hpp 2027 2013-06-04 19:28:10Z tkollar $
 * Copyright (c) 2012.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _MOD_AOS_IN_SDU_INSERT_HPP_
#define _MOD_AOS_IN_SDU_INSERT_HPP_

#include "AOS_PhysicalChannel_Service.hpp"

namespace nAOS_IN_SDU_Insert {

//=============================================================================
/**
 * @class modAOS_IN_SDU_Insert
 * @author Tad Kollar  
 * @brief Insert data into the AOS Transfer Frame Insert Zone.
 */
//=============================================================================
class modAOS_IN_SDU_Insert: public AOS_PhysicalChannel_Service {
public:
	/// Primary constructor.
	/// @param newName An identifier for the module that's unique within the channel.
	/// @param newChannelName An identifier for the containing channel.
	/// @param newDLLPtr Pointer to the DLL management structure.
    modAOS_IN_SDU_Insert(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr);

	/// Destructor
    virtual ~modAOS_IN_SDU_Insert();

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

	/// Read-only accessor to _receivedInSduUnitCount.
	u_int64_t getReceivedInSduUnitCount() { return _receivedInSduUnitCount; }

	/// Write-only accessor to _receivedInSduUnitCount.
	void setReceivedInSduUnitCount(const u_int64_t& newVal) { _receivedInSduUnitCount = newVal; }

	/// Read-only accessor to _receivedInSduOctetCount.
	u_int64_t getReceivedInSduOctetCount() { return _receivedInSduOctetCount; }

	/// Write-only accessor to _receivedInSduOctetCount.
	void setReceivedInSduOctetCount(const u_int64_t& newVal) { _receivedInSduOctetCount = newVal; }

	/// Read-only accessor to _receivedFrameUnitCount.
	u_int64_t getReceivedFrameUnitCount() { return _receivedFrameUnitCount; }

	/// Write-only accessor to _receivedFrameUnitCount.
	void setReceivedFrameUnitCount(const u_int64_t& newVal) { _receivedFrameUnitCount = newVal; }

	/// Read-only accessor to _receivedFrameOctetCount.
	u_int64_t getReceivedFrameOctetCount() { return _receivedFrameOctetCount; }

	/// Write-only accessor to _receivedFrameOctetCount.
	void setReceivedFrameOctetCount(const u_int64_t& newVal) { _receivedFrameOctetCount = newVal; }

private:
	/// The total number of IN_SDU units received.
	u_int64_t _receivedInSduUnitCount;

	/// Total size of received IN_SDU units.
	u_int64_t _receivedInSduOctetCount;

	/// Total number of AOS Transfer Frames received.
	u_int64_t _receivedFrameUnitCount;

	/// Total size of received AOS Transfer frames.
	u_int64_t _receivedFrameOctetCount;

	/// If an IN_SDU Unit is waiting, insert it into the Insert Zone of the supplied frame.
	/// @param frame The AOS Transfer Frame to insert the IN_SDU unit into.
	/// @return True is the insertion was successful, false otherwise.
	bool _insertWaitingInSduUnit(AOS_Transfer_Frame* frame);


}; // class modAOS_IN_SDU_Insert

} // namespace nAOS_IN_SDU_Insert

#endif // _MOD_AOS_IN_SDU_INSERT_HPP_
