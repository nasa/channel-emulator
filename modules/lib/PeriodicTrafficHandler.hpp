/* -*- C++ -*- */

//=============================================================================
/**
 * @file   PeriodicTrafficHandler.hpp
 * @author Tad Kollar  
 *
 * $Id: PeriodicTrafficHandler.hpp 1975 2013-04-26 17:45:33Z tkollar $
 * Copyright (c) 2010.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _NASA_PERIODIC_TRAFFIC_HANDLER_HPP_
#define _NASA_PERIODIC_TRAFFIC_HANDLER_HPP_

#include "BaseTrafficHandler.hpp"

namespace nasaCE {

//=============================================================================
/**
 * @class PeriodicTrafficHandler
 * @author Tad Kollar  
 * @brief Provide a few extra methods for handling PeriodicLinks.
*/
//=============================================================================
class PeriodicTrafficHandler: public BaseTrafficHandler {
public:
	/// @class NoIdleUnit
	/// @brief Thrown when an idle unit is requested before one is available.
	struct NoIdleUnit: public Exception {
		NoIdleUnit(const std::string e): Exception(e) { }
	};

	/// Primary constructor.
	/// @param newName An identifier for the module that's unique within the channel.
	/// @param newChannelName An identifier for the containing channel.
	/// @param newDLLPtr If loaded in a DLL, a pointer to that management structure.
    PeriodicTrafficHandler(const std::string& newName, const std::string& newChannelName,
    	const CE_DLL* newDLLPtr = 0);

	/// Destructor.
    virtual ~PeriodicTrafficHandler();

	/// Called by a PeriodicLink object when an idle unit is required.
	virtual NetworkData* getIdleUnit() = 0;

	/// Shut down the PeriodicLinks as well as the message queue and wait for processing to stop.
	void stopTraffic(bool shutdown = true);

	/// Create a PeriodicLink to manage the existance of the connection from
	/// this handler's output to the next handler's input.
	/// @param target The Traffic Handler to output to.
	/// @param sourceOutputRank Whether to connect the source's primary or auxiliary output.
	/// @param targetInputRank Whether to connect to the target's primary or auxiliary input.
	/// @param postOpen True if the link is being created after the source is already active.
	void connectPeriodicOutput(BaseTrafficHandler* target,
		const HandlerLink::OutputRank sourceOutputRank = HandlerLink::PrimaryOutput,
		const HandlerLink::InputRank targetInputRank = HandlerLink::PrimaryInput,
		const bool postOpen = false );

	/// Write-only accessor to intervalUsec_.
	void setInterval(const suseconds_t newVal) {
		intervalUsec_ = static_cast<int>(newVal);
		updateIntervals_();
	}

	/// Write-only accessor to interval_.
	void setInterval(const ACE_Time_Value& newVal) {
		intervalUsec_ = static_cast<int>(newVal.usec());
		updateIntervals_();
	}

	/// Read-only accessor to interval_.
	suseconds_t getInterval() const { return static_cast<int>(intervalUsec_); }

	/// Make sure the periodic transmitter is activatived in addition to the object.
    int open (void* = 0);

	/// Write-only accessor to idleCount_.
	void setIdleCount(const uint64_t& newCount) { idleCount_ = newCount; }

	/// Add 1 to  idleCount_ and return it.
	uint64_t incIdleCount() { return ++idleCount_; }

	/// Read-only accessor to idleCount_.
	uint64_t getIdleCount() const { return idleCount_; }

protected:

	/// How often to send data. Value of zero means asynchronous.
	Setting& intervalUsec_;

	/// This unit is built during construction, rebuilt if idle parameters change, and copied
	/// whenever an idle unit is required.
	NetworkData* idleUnitTemplate_;

	/// Restrict access to _idleUnitTemplate so it can be modified on the fly.
	ACE_Process_Mutex idleUnitTemplateCreation_;

	/// Delete old (if exists), create and configure the idle frame template.
	virtual void rebuildIdleUnitTemplate_() = 0;

	/// Update the interval in the periodic transmitters if output links exist.
	void updateIntervals_();

	/// The tally of idle units that have been sent.
	uint64_t idleCount_;
};

} // namespace nasaCE

#endif
