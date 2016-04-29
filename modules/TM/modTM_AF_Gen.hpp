/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modTM_AF_Gen.hpp
 * @author Tad Kollar  
 *
 * $Id: modTM_AF_Gen.hpp 2033 2013-06-05 18:28:52Z tkollar $
 * Copyright (c) 2012.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _MOD_TM_AF_GEN_HPP_
#define _MOD_TM_AF_GEN_HPP_

#include "TM_PhysicalChannel_Service.hpp"
#include "BaseTrafficHandler.hpp"

namespace nTM_AF_Gen {

//=============================================================================
/**
 * @class modTM_AF_Gen
 * @author Tad Kollar  
 * @brief Accept incoming Transfer Frames and perform error control.
*/
//=============================================================================
class modTM_AF_Gen: public TM_PhysicalChannel_Service {
public:
	/// Primary constructor.
	/// @param newName An identifier for the module that's unique within the channel.
	/// @param newChannelName An identifier for the containing channel.
	/// @param newDLLPtr Pointer to the DLL management structure.
    modTM_AF_Gen(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr);

	/// Destructor
    ~modTM_AF_Gen();

	/// Read in data, perform EC addition, and sends it on.
	int svc();

	/// Override BaseTrafficHandler::getMRU() to return frameSize_ instead of MRU_.
	size_t getMRU() const { return getFrameSize(); }

	/// Override BaseTrafficHandler::setMRU() to set frameSize_ instead of MRU_.
	void setMRU(const size_t newVal) { setFrameSize(newVal); }

	/// Override BaseTrafficHandler::getMTU() to return frameSize_ instead of MTU_.
	size_t getMTU() const { return getFrameSize(); }

	/// Override BaseTrafficHandler::setMTU() to set frameSize_ instead of MTU_.
	void setMTU(const size_t newVal) { setFrameSize(newVal); }

}; // class modTM_AF_Gen

} // namespace nTM_AF_Gen

#endif // _MOD_TM_AF_GEN_HPP_
