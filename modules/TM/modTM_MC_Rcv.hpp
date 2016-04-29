/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modTM_MC_Rcv.hpp
 * @author Tad Kollar  
 *
 * $Id: modTM_MC_Rcv.hpp 1555 2012-06-05 18:48:48Z tkollar $ 
 * Copyright (c) 2012.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _MOD_TM_MC_RCV_HPP_
#define _MOD_TM_MC_RCV_HPP_

#include "TM_VirtualChannel_Service.hpp"

namespace nTM_MC_Rcv {

//=============================================================================
/**
 * @class modTM_MC_Rcv
 * @author Tad Kollar  
 * @brief Process received TM Transfer Frames for a Master Channel.
*/
//=============================================================================
class modTM_MC_Rcv: public TM_VirtualChannel_Service {
public:


	/// Primary constructor.
	/// @param newName An identifier for the module that's unique within the channel.
	/// @param newChannelName An identifier for the containing channel.
	/// @param newDLLPtr Pointer to the DLL management structure.
    modTM_MC_Rcv(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr);

	/// Destructor.
    ~modTM_MC_Rcv();

	/// Read in Transfer Frames, extract the packets or data, and send it on if not idle.
	int svc();
		
	/// Override BaseTrafficHandler::getMRU() to return frameSize_ instead of MRU_.
	size_t getMRU() const { return getFrameSize(); }
	
	/// Override BaseTrafficHandler::setMRU() to set frameSize_ instead of MRU_.
	void setMRU(const size_t newVal) { setFrameSize(newVal); }	
	
	/// Override BaseTrafficHandler::getMTU() to return frameSize_ instead of MTU_.
	size_t getMTU() const { return getFrameSize(); }
	
	/// Override BaseTrafficHandler::setMTU() to set frameSize_ instead of MTU_.
	void setMTU(const size_t newVal) { setFrameSize(newVal); }
	
}; // class modTM_MC_Rcv

} // namespace nTM_MC_Rcv

#endif // _MOD_TM_MC_RCV_HPP_
