/* -*- C++ -*- */

//=============================================================================
/**
 * @file   AOS_Mux_Base.hpp
 * @author Tad Kollar  
 *
 * $Id: AOS_Mux_Base.hpp 2018 2013-06-04 14:59:19Z tkollar $
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _AOS_MUX_BASE_HPP_
#define _AOS_MUX_BASE_HPP_

#include "AOS_MasterChannel_Service.hpp"
#include "AOS_Transfer_Frame.hpp"
#include "SettingsManager.hpp"
#include <set>

namespace nAOS_Mux {

typedef std::set<HandlerLink*> HandlerLinkSet;
typedef std::map<uint8_t, int> PriorityMapType;

//=============================================================================
/**
 * @class AOS_Mux_Base
 * @brief Define common functionality for AOS multiplexing services.
*/
//=============================================================================
class AOS_Mux_Base : public AOS_MasterChannel_Service {
public:
	/// Primary constructor.
    AOS_Mux_Base(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr = 0);

	/// Destructor
    virtual ~AOS_Mux_Base();

    /// Should never be called.
    /// @throw nd_error Any time it's called, because it's meaningless for this subclass.
    BaseTrafficHandler* getInputSource() const;

    /// Remove ourselves from the flow of traffic. Leave deletion to the Channel container.
    /// @param stopTraffic Shut down the message queue? Should only set false if already done.
    void withdraw(const bool doStopTraffic = true);

    /// Write-only accessor to _inputSource.
    /// @param newPrev One (of possibly several) Traffic Handlers that will be sending data to us.
    void setInputSource(BaseTrafficHandler* newPrev);

    /// Set _inputSource to zero.
    /// @param oldPrev One (of possibly several) Traffic Handlers that was sending data to us.
    void unsetInputSource(BaseTrafficHandler* oldPrev);

    /// Build a list of all segments that send output to this one.
    /// @param inputList Segment name list. cleared before filling, sorted alphabetically after.
    void getInputNames(std::list<std::string>& inputList);

	/// For the provided Identifier, set a new priority.
	/// @param identifier Channel identifier (VCID or MCID depending on mux type).
	/// @param newPriority The new priority to set (higher number means higher priority).
	void setPriority(const uint8_t identifier, const int newPriority);

	/// Return the priority of the specified identifier, or defaultPriority_ if one isn't set.
	/// @param identifier Channel identifier (VCID or MCID depending on mux type).
	int getPriority(const uint8_t identifier);

	/// Overload putq with one that will prioritize virtual channels.
	int putq(ACE_Message_Block *mb, ACE_Time_Value *timeout = 0);

	/// Write-only accessor to defaultPriority_.
	void setDefaultPriority(const int newVal) { defaultPriority_ = newVal; }

	/// Read-only accessor to defaultPriority_.
	int getDefaultPriority() const { return defaultPriority_; }

	/* For periodic functionality */

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
		const bool postOpen = false  ) {
			connectPeriodicOutput(target, sourceOutputRank, targetInputRank, postOpen);
	}

protected:
    /// The group of Handler Links that uses this as a primary output target.
    HandlerLinkSet primaryInputLinks_;

	/// The default priority to give frames if one isn't stored in Priorities_ for that channel.
	int defaultPriority_;

	/// Config file reference to defaultPriority_.
	Setting& defaultPrioritySetting_;

	/// An identifier to priority mapping that determines what priority the frames go into the queue with.
	PriorityMapType priorities_;

	/// Config file reference to priorities_;
	Setting& prioritiesSetting_;

	/// Define in subclass to return either the VCID or MCID from the provided message pointer.
	/// @param mb Polymorphed pointer to an AOS_Transfer_Frame.
	virtual uint8_t getIdentifier_(ACE_Message_Block* mb) const = 0;

	/// Delete the link but let the HandlerLink object call unsetLink to
	/// nullify the pointer.
	void deleteLink_(const LinkType linkType, HandlerLink* oldLink);

	/// Stores the pointer in the appropriate index in the link array.
	/// If that slot is not null, delete the link that's already there.
	void setLink_(const LinkType linkType, HandlerLink* newLink);

	/// Nullify the pointer only if deleteLink has been called.
	/// Only intended to be called by a HandlerLink object.
	void unsetLink_(const LinkType linkType, HandlerLink* oldLink);

	/// Delete old (if exists), create and configure the idle frame template.
	void rebuildIdleUnitTemplate_();

}; // class AOS_Mux_Base

} // namespace nAOS_Mux

#endif // _AOS_MUX_BASE_HPP_
