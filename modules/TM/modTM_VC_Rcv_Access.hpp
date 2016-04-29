/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modTM_VC_Rcv_Access.hpp
 * @author Tad Kollar  
 *
 * $Id: modTM_VC_Rcv_Access.hpp 1564 2012-06-06 15:20:12Z tkollar $ 
 * Copyright (c) 2012.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _MOD_TM_VC_RCV_ACCESS_HPP_
#define _MOD_TM_VC_RCV_ACCESS_HPP_

#include "TM_VirtualChannel_Service.hpp"

namespace nTM_VC_Rcv_Access {

//=============================================================================
/**
 * @class modTM_VC_Rcv_Access
 * @author Tad Kollar  
 * @brief Extract data from incoming TM Transfer Frames.
*/
//=============================================================================
class modTM_VC_Rcv_Access: public TM_VirtualChannel_Service {
public:


	/// Primary constructor.
	/// @param newName An identifier for the module that's unique within the channel.
	/// @param newChannelName An identifier for the containing channel.
	/// @param newDLLPtr Pointer to the DLL management structure.
    modTM_VC_Rcv_Access(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr);

	/// Destructor.
    ~modTM_VC_Rcv_Access();

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
	
}; // class modTM_VC_Rcv_Access

} // namespace nTM_VC_Rcv_Access

#endif // _MOD_TM_VC_RCV_ACCESS_HPP_
