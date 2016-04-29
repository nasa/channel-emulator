/* -*- C++ -*- */

//=============================================================================
/**
 * @file   HandlerLink.hpp
 * @author Tad Kollar  
 *
 * $Id: HandlerLink.hpp 1989 2013-05-29 15:17:04Z tkollar $ 
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _HANDLERLINK_HPP_
#define _HANDLERLINK_HPP_

#include "CE_Config.hpp"
#include "NetworkData.hpp"

using namespace nasaCE;

class BaseTrafficHandler;

//=============================================================================
/**
 * @class HandlerLink
 * @author Tad Kollar  
 * @brief Manage the state of the connection between two BaseTrafficHandlers.
 */
//=============================================================================
class HandlerLink {

public:
	/// @class CircularConnection
	/// @brief Thrown when the handler would deliver traffic to itself.
	struct CircularConnection: public Exception {
		CircularConnection(const std::string e): Exception(e) { }
	};
	
	/// @class NoAuxiliaryQueue
	/// @brief There was an attempt to use an auxiliary input queue in a target without one.
	struct NoAuxiliaryQueue: public Exception {
		NoAuxiliaryQueue(const std::string e): Exception(e) { }
	};
	
	/// @class DisconnectedLink
	/// @brief There was an attempt to use the link before it was configured.
	struct DisconnectedLink: public Exception {
		DisconnectedLink(const std::string e): Exception(e) { }
	};		
		
	enum InputRank { PrimaryInput, AuxInput };
	enum OutputRank { PrimaryOutput, AuxOutput };
	
	/// Default constructor. If possible, use the preferred constructor instead.
	/// Not restricted because there are some cases where the object must be
	/// created and then connected in two steps instead of one.
	HandlerLink();
	
	/// Preferred constructor, calls _connectSourceToTarget() automatically.
	HandlerLink( BaseTrafficHandler* source, BaseTrafficHandler* target,
		const OutputRank sourceOutputRank = PrimaryOutput,
		const InputRank targetInputRank = PrimaryInput );
		
	/// Destructor.
	~HandlerLink();
	
	/// Read-only accessor for _target.
	BaseTrafficHandler* getTarget() const { return _target; }
	
	/// True if the target input is primary, false if auxiliary.
	bool targetInputIsPrimary() const { return _targetInputRank == PrimaryInput; }
	
	/// Read-only accessor for _source.
	BaseTrafficHandler* getSource() const { return _source; }
	
	/// True if the source output is primary, false if auxiliary.
	bool sourceOutputIsPrimary() const { return _sourceOutputRank == PrimaryOutput; }

	/// @brief Configure the connection that this object manages.
	/// @param source The Traffic Handler that data originates from.
	/// @param target The Traffic Handler that will receive the data.
	/// @param sourceOutputRank Selects the source's primary or auxiliary output.
	/// @param targetInputRank Selects the target's primary or auxiliary input.
	/// @throw CircularConnection If the attempted connection would form a loop.
	/// @throw NoAuxiliaryQueue If an attempt is made to connect to a non-existant aux input queue.
	void connectSourceToTarget(BaseTrafficHandler* source,
		BaseTrafficHandler* target,
		const OutputRank sourceOutputRank,
		const InputRank targetInputRank);

	/// Calls ACE_TASK::putq() for data heading to a primary input, or
	/// ACE_Message_Queue::enqueue_prio() for data heading to an auxiliary input.
	/// @param data The information to pass from the source to the target.
	/// @throw DisconnectedLink If the connection has not been initialized.
	virtual int send(NetworkData* data);
	
private:
	BaseTrafficHandler* _source;
	OutputRank _sourceOutputRank;
	
	BaseTrafficHandler* _target;
	InputRank _targetInputRank;

}; // class HandlerLink

#endif // _HANDLERLINK_HPP_
