/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modAOS_AF_Gen.hpp
 * @author Tad Kollar  
 *
 * $Id: modAOS_AF_Gen.hpp 2033 2013-06-05 18:28:52Z tkollar $
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _MOD_AOS_AF_GEN_HPP_
#define _MOD_AOS_AF_GEN_HPP_

#include "AOS_PhysicalChannel_Service.hpp"
#include "BaseTrafficHandler.hpp"
#include "RSEncoder.hpp"


namespace nAOS_AF_Gen {

//=============================================================================
/**
 * @class modAOS_AF_Gen
 * @author Tad Kollar  
 * @brief Add IN_SDUs to incoming Transfer Frames and perform error control.
*/
//=============================================================================
class modAOS_AF_Gen: public AOS_PhysicalChannel_Service {
public:
	/// Primary constructor.
	/// @param newName An identifier for the module that's unique within the channel.
	/// @param newChannelName An identifier for the containing channel.
	/// @param newDLLPtr Pointer to the DLL management structure.
    modAOS_AF_Gen(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr);

	/// Destructor
    ~modAOS_AF_Gen();

	/// Read in data, perform EC and IN_SDU addition, and sends it on.
	int svc();

	/// Override BaseTrafficHandler::getMRU() to return frameSize_ instead of MRU_.
	size_t getMRU() const { return getFrameSize(); }

	/// Override BaseTrafficHandler::setMRU() to set frameSize_ instead of MRU_.
	void setMRU(const size_t newVal) { setFrameSize(newVal); }

	/// Override BaseTrafficHandler::getMTU() to return frameSize_ instead of MTU_.
	size_t getMTU() const { return getFrameSize(); }

	/// Override BaseTrafficHandler::setMTU() to set frameSize_ instead of MTU_.
	void setMTU(const size_t newVal) { setFrameSize(newVal); }

	/// If configured to do so, add RS(10,6) header parity and/or checksum symbols.
	/// @param aos Pointer to the Transfer Frame to modify.
	void addErrorControlIfPossible(AOS_Transfer_Frame* aos);

private:

	/// Object that handles generation of parity symbols for the TF header.
	RSEncoder* _aosHeaderEncoder;

}; // class modAOS_AF_Gen

} // namespace nAOS_AF_Gen

#endif // _MOD_AOS_AF_GEN_HPP_
