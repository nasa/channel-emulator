/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modTM_AF_Rcv.hpp
 * @author Tad Kollar  
 *
 * $Id: modTM_AF_Rcv.hpp 1551 2012-06-04 18:03:01Z tkollar $ 
 * Copyright (c) 2012.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _MOD_TM_AF_RCV_HPP_
#define _MOD_TM_AF_RCV_HPP_

#include "TM_PhysicalChannel_Service.hpp"
#include "TM_Transfer_Frame.hpp"
#include "BaseTrafficHandler.hpp"

namespace nTM_AF_Rcv {

//=============================================================================
/**
 * @class modTM_AF_Rcv
 * @author Tad Kollar  
 * @brief Receive TM Transfer Frames and perform error control.
 */
//=============================================================================
class modTM_AF_Rcv : public TM_PhysicalChannel_Service {
public:

	/// Primary constructor.
	/// @param newName An identifier for the module that's unique within the channel.
	/// @param newChannelName An identifier for the containing channel.
	/// @param newDLLPtr Pointer to the DLL management structure.
    modTM_AF_Rcv(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr);

	/// Destructor.
    ~modTM_AF_Rcv();

	/// Read in Transfer Frames, perform error control,
	/// send frame to Primary Output (if not idle).
	int svc();

	/// Override BaseTrafficHandler::getMRU() to return frameSize_ instead of MRU_.
	size_t getMRU() const { return getFrameSize(); }
	
	/// Override BaseTrafficHandler::setMRU() to set frameSize_ instead of MRU_.
	void setMRU(const size_t newVal) { setFrameSize(newVal); }	
	
	/// Override BaseTrafficHandler::getMTU() to return frameSize_ instead of MTU_.
	size_t getMTU() const { return getFrameSize(); }
	
	/// Override BaseTrafficHandler::setMTU() to set frameSize_ instead of MTU_.
	void setMTU(const size_t newVal) { setFrameSize(newVal); }
	
protected:
	void updateTemplate_(); /// Rebuild _frameHeader.
	
private:

	TM_Transfer_Frame* _frameTemplate; /// An empty, preconfigured frame to duplicate and make new frames from.
	
	
}; // class modTM_AF_Rcv

} // namespace nTM_AF_Rcv

#endif // _MOD_TM_AF_RCV_HPP_
