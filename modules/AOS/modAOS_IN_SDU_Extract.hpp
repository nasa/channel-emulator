/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modAOS_IN_SDU_Extract.hpp
 * @author Tad Kollar  
 *
 * $Id: modAOS_IN_SDU_Extract.hpp 2027 2013-06-04 19:28:10Z tkollar $
 * Copyright (c) 2012.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _MOD_AOS_IN_SDU_EXTRACT_HPP_
#define _MOD_AOS_IN_SDU_EXTRACT_HPP_

#include "AOS_PhysicalChannel_Service.hpp"

namespace nAOS_IN_SDU_Extract {

//=============================================================================
/**
 * @class modAOS_IN_SDU_Extract
 * @author Tad Kollar  
 * @brief Extract the Insert Zone payload from incoming AOS Transfer Frames.
*/
//=============================================================================
class modAOS_IN_SDU_Extract: public AOS_PhysicalChannel_Service {
public:
	/// Primary constructor.
	/// @param newName An identifier for the module that's unique within the channel.
	/// @param newChannelName An identifier for the containing channel.
	/// @param newDLLPtr Pointer to the DLL management structure.
    modAOS_IN_SDU_Extract(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr);

	/// Destructor.
    ~modAOS_IN_SDU_Extract();

	/// Read in Transfer Frames, extract the packets or data, and send it on if not idle.
	int svc();

	/// Override BaseTrafficHandler::getMRU() to return frameSize_ instead of MRU_.
	size_t getMRU() const { return getFrameSize(); }

	/// Override BaseTrafficHandler::setMRU() to set frameSize_ instead of MRU_.
	void setMRU(const size_t newVal) { setFrameSize(newVal); }

	/// Override BaseTrafficHandler::getMTU() to return frameSize_ instead of MTU_.
	size_t getMTU() const { return getFrameSize(); }

	/// Override BaseTrafficHandler::setMTU() to set frameSize_ instead of MTU_.
	/// @param newVal The new data zone length.
	void setMTU(const size_t newVal) { setFrameSize(newVal); }

private:
	/// Copy the IN_SDU field of the supplied frame and send it via the auxiliary output.
	void _extractAndSendInSdu(AOS_Transfer_Frame* frame);

}; // class modAOS_IN_SDU_Extract

} // namespace nAOS_IN_SDU_Extract

#endif // _MOD_AOS_IN_SDU_EXTRACT_HPP_
