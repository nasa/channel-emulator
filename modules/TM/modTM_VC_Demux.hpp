/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modTM_VC_Demux.hpp
 * @author Tad Kollar  
 *
 * $Id: modTM_VC_Demux.hpp 1597 2012-06-20 19:36:39Z tkollar $ 
 * Copyright (c) 2012.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _MOD_TM_VC_DEMUX_HPP_
#define _MOD_TM_VC_DEMUX_HPP_

#include "TM_Demux_Base.hpp"

using namespace nTM_Demux;

namespace nTM_VC_Demux {

//=============================================================================
/**
 * @class modTM_VC_Demux
 * @author Tad Kollar  
 * @brief Input from an TM Master Channel is split to multiple Virtual Channel outputs.
 */
//=============================================================================

class modTM_VC_Demux : public TM_Demux_Base {
public:
	/// Primary constructor.
	/// @param newName An identifier for the module that's unique within the channel.
	/// @param newChannelName An identifier for the containing channel.
	/// @param newDLLPtr Pointer to the DLL management structure.
    modTM_VC_Demux(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr);

	/// Destructor
    virtual ~modTM_VC_Demux();
    
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

}; // class modTM_VC_Demux

} // namespace nTM_VC_Demux

#endif // _MOD_TM_VC_DEMUX_HPP_
