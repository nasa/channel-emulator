/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modTM_OCF_Extract.hpp
 * @author Tad Kollar  
 *
 * $Id: modTM_OCF_Extract.hpp 1633 2012-07-03 19:07:00Z tkollar $
 * Copyright (c) 2012.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _MOD_TM_OCF_EXTRACT_HPP_
#define _MOD_TM_OCF_EXTRACT_HPP_

#include "TM_MasterChannel_Service.hpp"

namespace nTM_OCF_Extract {

//=============================================================================
/**
 * @class modTM_OCF_Extract
 * @author Tad Kollar  
 * @brief Extract a Report from the OCF of incoming TM Transfer Frames.
*/
//=============================================================================
class modTM_OCF_Extract: public TM_MasterChannel_Service {
public:
	/// Primary constructor.
	/// @param newName An identifier for the module that's unique within the channel.
	/// @param newChannelName An identifier for the containing channel.
	/// @param newDLLPtr Pointer to the DLL management structure.
    modTM_OCF_Extract(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr);

	/// Destructor.
    ~modTM_OCF_Extract();

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
	/// Copy the OCF field of the supplied frame and send it via the auxiliary output.
	void _extractAndSendOCF(TM_Transfer_Frame* frame);

}; // class modTM_OCF_Extract

} // namespace nTM_OCF_Extract

#endif // _MOD_TM_OCF_EXTRACT_HPP_
