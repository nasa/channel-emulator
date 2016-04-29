/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modTM_MC_Gen.hpp
 * @author Tad Kollar  
 *
 * $Id: modTM_MC_Gen.hpp 2033 2013-06-05 18:28:52Z tkollar $
 * Copyright (c) 2012.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _MOD_TM_MC_GEN_HPP_
#define _MOD_TM_MC_GEN_HPP_

#include "TM_VirtualChannel_Service.hpp"

namespace nTM_MC_Gen {

//=============================================================================
/**
 * @class modTM_MC_Gen
 * @author Tad Kollar  
 * @brief Perform Master Channel send operations on a TM Transfer Frame.
 */
//=============================================================================
class modTM_MC_Gen: public TM_VirtualChannel_Service {
public:
	/// Primary constructor.
	/// @param newName An identifier for the module that's unique within the channel.
	/// @param newChannelName An identifier for the containing channel.
	/// @param newDLLPtr Pointer to the DLL management structure.
    modTM_MC_Gen(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr);

	/// Destructor
    ~modTM_MC_Gen();

	/// Read in data, encapsulate in one or more M_PDUs, and send it on.
	int svc();

	/// Override BaseTrafficHandler::getMRU() to return frameSize_ instead of MRU_.
	size_t getMRU() const { return getFrameSize(); }

	/// Override BaseTrafficHandler::setMRU() to set frameSize_ instead of MRU_.
	void setMRU(const size_t newVal) { setFrameSize(newVal); }

	/// Override BaseTrafficHandler::getMTU() to return frameSize_ instead of MTU_.
	size_t getMTU() const { return getFrameSize(); }

	/// Override BaseTrafficHandler::setMTU() to set frameSize_ instead of MTU_.
	void setMTU(const size_t newVal) { setFrameSize(newVal); }

}; // class modTM_MC_Gen

} // namespace nTM_MC_Gen

#endif // _MOD_TM_MC_GEN_HPP_
