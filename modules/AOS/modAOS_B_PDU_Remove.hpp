/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modAOS_B_PDU_Remove.hpp
 * @author Tad Kollar  
 *
 * $Id: modAOS_B_PDU_Remove.hpp 918 2009-12-21 18:49:14Z tkollar $ 
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _MOD_AOS_B_PDU_REMOVE_HPP_
#define _MOD_AOS_B_PDU_REMOVE_HPP_

#include "BaseTrafficHandler.hpp"
#include "AOS_Bitstream_PDU.hpp"
#include "EncapsulationPacket.hpp"

namespace nAOS_B_PDU_Remove {

//=============================================================================
/**
	@class modAOS_B_PDU_Remove
	@author Tad Kollar  
	@brief Extract data from incoming AOS Bitstream Protocol Data Units.
*/
//=============================================================================
class modAOS_B_PDU_Remove: public BaseTrafficHandler {
public:
	/// Primary constructor.
	/// @param newName An identifier for the module that's unique within the channel.
	/// @param newChannelName An identifier for the containing channel.
	/// @param newDLLPtr Pointer to the DLL management structure.
    modAOS_B_PDU_Remove(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr);

	/// Destructor
    ~modAOS_B_PDU_Remove();

	/// Read in data, encapsulate in one or more B_PDUs, and send it on.
	int svc();

}; // class modAOS_B_PDU_Remove

} // namespace nAOS_B_PDU_Remove

#endif // _MOD_AOS_B_PDU_REMOVE_HPP_
