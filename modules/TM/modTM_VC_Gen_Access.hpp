/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modTM_VC_Gen_Access.hpp
 * @author Tad Kollar  
 *
 * $Id: modTM_VC_Gen_Access.hpp 2033 2013-06-05 18:28:52Z tkollar $
 * Copyright (c) 2012.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _MOD_TM_VC_GEN_ACCESS_HPP_
#define _MOD_TM_VC_GEN_ACCESS_HPP_

#include "TM_VirtualChannel_Service.hpp"

namespace nTM_VC_Gen_Access {

//=============================================================================
/**
 * @class modTM_VC_Gen_Access
 * @author Tad Kollar  
 * @brief Encapsulate incoming data in a TM Transfer Frame.
 */
//=============================================================================
class modTM_VC_Gen_Access: public TM_VirtualChannel_Service {
public:
	/// Primary constructor.
	/// @param newName An identifier for the module that's unique within the channel.
	/// @param newChannelName An identifier for the containing channel.
	/// @param newDLLPtr Pointer to the DLL management structure.
    modTM_VC_Gen_Access(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr);

	/// Destructor
    ~modTM_VC_Gen_Access();

	/// Read in data, encapsulate in one or more M_PDUs, and send it on.
	int svc();

	/// Override BaseTrafficHandler::getMRU() with a calculation.
	size_t getMRU() const;

	/// Override BaseTrafficHandler::setMRU() to calculate frameSize_ instead of MRU_.
	/// @param newVal The new data zone length.
	void setMRU(const size_t newVal);

	/// Override BaseTrafficHandler::getMTU() to return _frameSize instead of _MTU.
	size_t getMTU() const { return getFrameSize(); }

	/// Override BaseTrafficHandler::setMTU() to set _frameSize instead of MTU_.
	void setMTU(const size_t newVal) { setFrameSize(newVal); }

	/* --- For periodic functionality --- */

	/// Return a pointer to a duplicate of _idleFrameTemplate.
	NetworkData* getIdleUnit();

	/// Create a PeriodicLink to manage the existance of the connection from
	/// this handler's output to the next handler's input.
	/// @param target The Traffic Handler to output to.
	/// @param sourceOutputRank Whether to connect the source's primary or auxiliary output.
	/// @param targetInputRank Whether to connect to the target's primary or auxiliary input.
	/// @param postOpen True if the link is being created after the source is already active.
	void connectOutput(BaseTrafficHandler* target,
		const HandlerLink::OutputRank sourceOutputRank = HandlerLink::PrimaryOutput,
		const HandlerLink::InputRank targetInputRank = HandlerLink::PrimaryInput,
		const bool postOpen = false ) {
			connectPeriodicOutput(target, sourceOutputRank, targetInputRank);
	}

protected:
	/// Delete old (if exists), create and configure the idle frame template.
	void rebuildIdleUnitTemplate_();
}; // class modTM_VC_Gen_Access

} // namespace nTM_VC_Gen_Access

#endif // _MOD_TM_VC_GEN_ACCESS_HPP_
