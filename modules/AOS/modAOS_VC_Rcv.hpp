/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modAOS_VC_Rcv.hpp
 * @author Tad Kollar  
 *
 * $Id: modAOS_VC_Rcv.hpp 2018 2013-06-04 14:59:19Z tkollar $ 
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _MOD_AOS_VC_RCV_HPP_
#define _MOD_AOS_VC_RCV_HPP_

#include "AOS_VirtualChannel_Service.hpp"

namespace nAOS_VC_Rcv {

//=============================================================================
/**
 * @class modAOS_VC_Rcv
 * @author Tad Kollar  
 * @brief Extract data from incoming AOS Transfer Frames.
*/
//=============================================================================
class modAOS_VC_Rcv: public AOS_VirtualChannel_Service {
public:


	/// Primary constructor.
	/// @param newName An identifier for the module that's unique within the channel.
	/// @param newChannelName An identifier for the containing channel.
	/// @param newDLLPtr Pointer to the DLL management structure.
    modAOS_VC_Rcv(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr);

	/// Destructor.
    ~modAOS_VC_Rcv();

	/// Read in Transfer Frames, extract the packets or data, and send it on if not idle.
	int svc();
		
	/// Override BaseTrafficHandler::getMRU() to return frameSize_ instead of MRU_.
	size_t getMRU() const { return getFrameSize(); }
	
	/// Override BaseTrafficHandler::setMRU() to set frameSize_ instead of MRU_.
	void setMRU(const size_t newVal) { setFrameSize(newVal); }	
	
	/// Override BaseTrafficHandler::getMTU() with a calculation.
	size_t getMTU() const;
		
	/// Override BaseTrafficHandler::setMTU() to calculate frameSize_ instead of MTU_.
	/// @param newVal The new data zone length.
	void setMTU(const size_t newVal);
	
}; // class modAOS_VC_Rcv

} // namespace nAOS_VC_Rcv

#endif // _MOD_AOS_VC_RCV_HPP_
