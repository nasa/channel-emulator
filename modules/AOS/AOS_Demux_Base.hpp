/* -*- C++ -*- */

//=============================================================================
/**
 * @file   AOS_Demux_Base.hpp
 * @author Tad Kollar  
 *
 * $Id: AOS_Demux_Base.hpp 2018 2013-06-04 14:59:19Z tkollar $ 
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _AOS_DEMUX_BASE_HPP_
#define _AOS_DEMUX_BASE_HPP_

#include "AOS_MasterChannel_Service.hpp"

namespace nAOS_Demux {

typedef std::map<ACE_UINT8, HandlerLink*> HandlerLinkMap;

//=============================================================================
/**
 * @class AOS_Demux_Base
 * @author Tad Kollar  
 * @brief Basic functionality to direct incoming AOS frames into multiple output streams.
*/
//=============================================================================
class AOS_Demux_Base: public AOS_MasterChannel_Service {
public:
	/// Primary constructor.
    AOS_Demux_Base(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr = 0);

	/// Destructor
    virtual ~AOS_Demux_Base();

    /// Remove ourselves from the flow of traffic. Leave deletion to the Channel container.
    /// @param stopTraffic Shut down the message queue? Should only set false if already done.
    void withdraw(const bool doStopTraffic = true);

	/// Test for a possible circular connection anywhere in the chain.
	/// Overloads the base class version to check the chain under
	/// potentially several output targets.
	/// @param testTarget Pointer to compare against existing targets.
	/// @return True if testTarget already exists as an output target.
	virtual bool isCircularConnection(BaseTrafficHandler* testTarget);
	
	/// Create a HandlerLink to manage the existance of the connection from
	/// this handler's output to the next handler's input. Invalid in AOS_Demux_Base
	/// for a primary output target; the connectOutput() with an identifier parameter
	/// should be used instead.
    /// @param target A Link to one of likely several Traffic Handlers that will receive demuxed frames.
	/// @param sourceOutputRank Whether to connect the source's primary or auxiliary output.
	/// @param targetInputRank Whether to connect to the target's primary or auxiliary input.
	/// @param postOpen True if the link is being created after the source is already active.
	virtual void connectOutput(BaseTrafficHandler* target,
		const HandlerLink::OutputRank sourceOutputRank = HandlerLink::PrimaryOutput,
		const HandlerLink::InputRank targetInputRank = HandlerLink::PrimaryInput,
		const bool postOpen = false );	
	
	/// Create a HandlerLink to manage the existance of the connection from
	/// this handler's output to the next handler's input.
	/// @param identifier The channel identifier to associate with the output target.
    /// @param target A Link to one of likely several Traffic Handlers that will receive demuxed frames.
	/// @param targetInputRank Whether to connect to the target's primary or auxiliary input.
	virtual void connectPrimaryOutput(const ACE_UINT8 identifier,
		BaseTrafficHandler* target,
		const HandlerLink::InputRank targetInputRank = HandlerLink::PrimaryInput );
		
	/// Destroy an existing output connection, represented by a HandlerLink object.
	/// @param sourceOutputRank Which output to disconnect (primary or auxiliary).
	/// @param target A Link to one of likely several Traffic Handlers that was receiving demuxed frames.
	virtual void disconnectOutput(const HandlerLink::OutputRank sourceOutputRank =
		 HandlerLink::PrimaryOutput, HandlerLink* oldLink = 0);

	/// Fill idList with the identifiers of each output link and sort.
	void getIdentifiers(std::list<ACE_UINT8>& idList);

protected:
    /// The group of Traffic Handlers that may receive frames from this module.
    HandlerLinkMap primaryOutputLinks_;
    
    /// Position an iterator to the key, value pair that has link as the value.
    /// @param link The HandlerLink pointer to find.
    HandlerLinkMap::iterator findLink_(HandlerLink* link);
    
	/// Delete the link but let the HandlerLink object call unsetLink to
	/// nullify the pointer.
	virtual void deleteLink_(const LinkType linkType, HandlerLink* oldLink);
		
	/// Stores the pointer in the appropriate index in the link array.
	/// If that slot is not null, delete the link that's already there.
	virtual void setLink_(const LinkType linkType, HandlerLink* newLink);
		
	/// Nullify the pointer only if deleteLink has been called.
	/// Only intended to be called by a HandlerLink object.
	virtual void unsetLink_(const LinkType linkType, HandlerLink* oldLink);
	
	Setting& outputNames_;
}; // class AOS_Demux_Base

} // namespace nAOS_Demux

#endif // _AOS_DEMUX_BASE_HPP_
