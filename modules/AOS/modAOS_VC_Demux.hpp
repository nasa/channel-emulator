/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modAOS_VC_Demux.hpp
 * @author Tad Kollar  
 *
 * $Id: modAOS_VC_Demux.hpp 2018 2013-06-04 14:59:19Z tkollar $ 
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _MOD_AOS_VC_DEMUX_HPP_
#define _MOD_AOS_VC_DEMUX_HPP_

#include "AOS_Demux_Base.hpp"

using namespace nAOS_Demux;

namespace nAOS_VC_Demux {

//=============================================================================
/**
 * @class modAOS_VC_Demux
 * @author Tad Kollar  
 * @brief Input from an AOS Master Channel is split to multiple Virtual Channel outputs.
 */
//=============================================================================

class modAOS_VC_Demux : public AOS_Demux_Base {
public:
	/// Primary constructor.
	/// @param newName An identifier for the module that's unique within the channel.
	/// @param newChannelName An identifier for the containing channel.
	/// @param newDLLPtr Pointer to the DLL management structure.
    modAOS_VC_Demux(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr);

	/// Destructor
    virtual ~modAOS_VC_Demux();
    
	/// Read in data from a single multiple source, and split it
	/// to multiple outputs based on VCID.
	int svc();
	
	/// Override BaseTrafficHandler::getMRU() to return frameSize_ instead of MRU_.
	size_t getMRU() const { return getFrameSize(); }
	
	/// Override BaseTrafficHandler::setMRU() to set frameSize_ instead of MRU_.
	void setMRU(const size_t newVal) { setFrameSize(newVal); }	
	
	/// Override BaseTrafficHandler::getMTU() to return frameSize_ instead of MTU_.
	size_t getMTU() const { return getFrameSize(); }
	
	/// Override BaseTrafficHandler::setMTU() to set frameSize_ instead of MTU_.
	void setMTU(const size_t newVal) { setFrameSize(newVal); }
	
	/// Get a pointer to the specified output link.
	/// @return 0 if link is not found.
	HandlerLink* getLink(const LinkType ltype, const ACE_UINT8 VCID = 0);	

}; // class modAOS_VC_Demux

} // namespace nAOS_VC_Demux

#endif // _MOD_AOS_VC_DEMUX_HPP_
