/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modTM_MC_Mux.hpp
 * @author Tad Kollar  
 *
 * $Id: modTM_MC_Mux.hpp 1600 2012-06-21 13:40:38Z tkollar $ 
 * Copyright (c) 2012.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _MOD_TM_MC_MUX_HPP_
#define _MOD_TM_MC_MUX_HPP_

#include "TM_Mux_Base.hpp"

using namespace nTM_Mux;

namespace nTM_MC_Mux {

//=============================================================================
/**
 * @class modTM_MC_Mux
 * @author Tad Kollar  
 * @brief Merge frames from multiple TM Master Channels into one Physical Channel.
 */
//=============================================================================
class modTM_MC_Mux: public TM_Mux_Base {
public:
	/// Primary constructor.
	/// @param newName An identifier for the module that's unique within the channel.
	/// @param newChannelName An identifier for the containing channel.
	/// @param newDLLPtr Pointer to the DLL management structure.
    modTM_MC_Mux(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr);

	/// Destructor
    virtual ~modTM_MC_Mux();
    
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

protected:
	/// Return either the MCID extracted from the provided message pointer.
	/// @param mb Polymorphed pointer to an TM_Transfer_Frame.
	virtual ACE_UINT8 getIdentifier_(ACE_Message_Block* mb) const;
}; // class modTM_MC_Mux

} // namespace nTM_MC_Mux

#endif // _MOD_TM_MC_MUX_HPP_
