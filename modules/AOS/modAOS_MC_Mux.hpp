/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modAOS_MC_Mux.hpp
 * @author Tad Kollar  
 *
 * $Id: modAOS_MC_Mux.hpp 1205 2011-04-07 17:34:14Z tkollar $ 
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _MOD_AOS_MC_MUX_HPP_
#define _MOD_AOS_MC_MUX_HPP_

#include "AOS_Mux_Base.hpp"

using namespace nAOS_Mux;

namespace nAOS_MC_Mux {
//=============================================================================
/**
 * @class modAOS_MC_Mux
 * @author Tad Kollar   
 * @brief Merge frames from multiple Master Channels into one Physical Channel.
*/
//=============================================================================
class modAOS_MC_Mux : public AOS_Mux_Base {
public:
	/// Primary constructor.
	/// @param newName An identifier for the module that's unique within the channel.
	/// @param newChannelName An identifier for the containing channel.
	/// @param newDLLPtr Pointer to the DLL management structure.
    modAOS_MC_Mux(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr);

	/// Destructor
    virtual ~modAOS_MC_Mux();
    
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
	/// Return the MCID extracted from the provided message pointer.
	/// @param mb Polymorphed pointer to an AOS_Transfer_Frame.
	virtual ACE_UINT8 getIdentifier_(ACE_Message_Block* mb) const;
	
private:
	/// Disabled; not needed for this service.
	void setBadMCIDCount(const unsigned long long&) {}

	/// Disabled; not needed for this service.
	unsigned long long getBadMCIDCount() const { return 0; }
	
	/// Disabled; not needed for this service.
	unsigned long long incBadMCIDCount() { return 0; }

}; // class modAOS_MC_Mux

} // namespace nAOS_MC_Mux

#endif // _MOD_AOS_MC_MUX_HPP_
