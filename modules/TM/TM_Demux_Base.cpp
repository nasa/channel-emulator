/* -*- C++ -*- */

//=============================================================================
/**
 * @file   TM_Demux_Base.cpp
 * @author Tad Kollar  
 *
 * $Id: TM_Demux_Base.cpp 2372 2013-09-25 18:27:42Z tkollar $
 * Copyright (c) 2012.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "TM_Demux_Base.hpp"
#include "ChannelRegistry.hpp"

namespace nTM_Demux {

using namespace nasaCE;

TM_Demux_Base::TM_Demux_Base(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr /* = 0 */):
	TM_VirtualChannel_Service(newName, newChannelName, newDLLPtr),
	outputNames_(CEcfg::instance()->getOrAddList(cfgKey("primaryOutputs"))) {

	for ( int idx = 0; idx < outputNames_.getLength(); ++idx ) {
		std::string targetName = outputNames_[idx][0];
		std::string inputType = outputNames_[idx][1];
		int identifier = outputNames_[idx][2];

		BaseTrafficHandler* target = channels::instance()->channel(newChannelName)->
			find(targetName);

		if ( !target ) {
			MOD_ERROR("Couldn't find target '%s' in channel '%s' to connect to.", static_cast<const char*>(outputNames_[idx][0]), newChannelName.c_str());
		}
		else if ( primaryOutputLinks_.count(identifier)  ) {
			MOD_ERROR("Duplicate identifer '%d' specified, cannot connect.", identifier);
		}
		else {
			HandlerLink *newLink = new HandlerLink();

			primaryOutputLinks_[identifier] = newLink;

			HandlerLink::InputRank targetInputRank = ( inputType == "PrimaryInput" ) ?
				HandlerLink::PrimaryInput : HandlerLink::AuxInput;

			newLink->connectSourceToTarget(this, target, HandlerLink::PrimaryOutput, targetInputRank);
		}
	}
}

TM_Demux_Base::~TM_Demux_Base() {
	MOD_DEBUG("Running ~TM_Demux_Base().");
}

void TM_Demux_Base::withdraw(const bool doStopTraffic /* = true */) {
	if (doStopTraffic) stopTraffic();

	// Test primary output targets.
	HandlerLinkMap::iterator pos;

	for ( pos = primaryOutputLinks_.begin(); pos != primaryOutputLinks_.end(); ++pos ) {
		delete pos->second;
	}

	primaryOutputLinks_.clear();

	BaseTrafficHandler::withdraw(false);
}

bool TM_Demux_Base::isCircularConnection(BaseTrafficHandler* testTarget) {
	// Don't output directly to ourselves!
	if (testTarget == this) return true;

	// Test primary output targets.
	HandlerLinkMap::iterator pos;

	for ( pos = primaryOutputLinks_.begin(); pos != primaryOutputLinks_.end(); ++pos ) {
		if (pos->second->getTarget() &&
			pos->second->getTarget()->isCircularConnection(testTarget)) return true;
	}

	// Test auxiliary output target, if it exists.
	if (links_[AuxOutputLink] &&
		links_[AuxOutputLink]->getTarget()->isCircularConnection(testTarget) )
		return true;

	// Must be safe.
	return false;
}

void TM_Demux_Base::connectOutput(BaseTrafficHandler* target,
	const HandlerLink::OutputRank sourceOutputRank /* = HandlerLink::PrimaryOutput */,
	const HandlerLink::InputRank targetInputRank /* = HandlerLink::PrimaryInput */,
	const bool postOpen /* = false */ ) {

	// Not allowed here for sourceOutputRank == HandlerLink::PrimaryOutput.
	if (sourceOutputRank == HandlerLink::PrimaryOutput)
		throw nd_error("TM_Demux_Base::connectOutput() without channel identifier used for PrimaryOutput.");

	// Pass through for sourceOutputRank == HandlerLink::AuxOutput.
	BaseTrafficHandler::connectOutput(target, sourceOutputRank, targetInputRank, postOpen);
}

void TM_Demux_Base::connectPrimaryOutput(const ACE_UINT8 identifier,
	BaseTrafficHandler* target,
	const HandlerLink::InputRank targetInputRank /* = HandlerLink::PrimaryInput */ ) {

	// Determine whether the identifier has already been mapped:
	HandlerLinkMap::iterator pos = primaryOutputLinks_.find(identifier);

	if ( pos != primaryOutputLinks_.end() ) {
		// Already mapped to the requested target
		if ( pos->second->getTarget() == target ) return;

		// Otherwise, disconnect the current output before creating the new one:
		disconnectOutput(HandlerLink::PrimaryOutput, pos->second);
	}

	HandlerLink *newLink = new HandlerLink();
	primaryOutputLinks_[identifier] = newLink;

	newLink->connectSourceToTarget(this, target, HandlerLink::PrimaryOutput, targetInputRank);

	// Determine if the new target already exists in a sublist.
	for ( int idx = 0; idx < outputNames_.getLength(); ++idx ) {
		std::string testName = outputNames_[idx][0];
		if ( testName == target->getName() ) {
			outputNames_[idx][1] = ( (targetInputRank == HandlerLink::PrimaryInput)?
				"PrimaryInput" : "AuxInput" );
			outputNames_[idx][2] = static_cast<int>(identifier);
			return;
		}
	}

	// The new target was not found, so add a new sublist.
	Setting& newOutput = outputNames_.add(Setting::TypeList);
	newOutput.add(Setting::TypeString) = target->getName();
	newOutput.add(Setting::TypeString) = ( (targetInputRank == HandlerLink::PrimaryInput)?
		"PrimaryInput" : "AuxInput" );
	newOutput.add(Setting::TypeInt) = static_cast<int>(identifier);
}

void TM_Demux_Base::disconnectOutput(const HandlerLink::OutputRank sourceOutputRank
		/* = HandlerLink::PrimaryOutput */, HandlerLink* oldLink /* = 0 */) {

	for ( int idx = 0; idx < outputNames_.getLength(); ++idx ) {
		std::string testName = outputNames_[idx][0];
		if ( testName == oldLink->getTarget()->getName() ) {
			outputNames_.remove(idx);
		}
	}

	if ( sourceOutputRank == HandlerLink::PrimaryOutput && oldLink )
		deleteLink_(PrimaryOutputLink, oldLink);
	else BaseTrafficHandler::disconnectOutput(sourceOutputRank, oldLink);
}

HandlerLinkMap::iterator TM_Demux_Base::findLink_(HandlerLink* link) {
	HandlerLinkMap::iterator pos;

	for ( pos = primaryOutputLinks_.begin(); pos != primaryOutputLinks_.end() &&
			pos->second != link; ++pos );

	return pos;
}

void TM_Demux_Base::getIdentifiers(std::list<ACE_UINT8>& idList) {
	idList.clear();

	HandlerLinkMap::iterator pos;
	for ( pos = primaryOutputLinks_.begin(); pos != primaryOutputLinks_.end(); ++pos )
		idList.push_back(pos->first);

	idList.sort();
}


void TM_Demux_Base::deleteLink_(const LinkType linkType, HandlerLink* oldLink) {
	if ( linkType == PrimaryOutputLink) {
		HandlerLinkMap::iterator pos = findLink_(oldLink);

		if ( pos == primaryOutputLinks_.end() ) {
			MOD_ERROR("deleteLink_() could not locate pointer to %s; cannot delete.", oldLink->getTarget()->getName().c_str());
		}
		else {
			delete pos->second;
		}
	}
	else BaseTrafficHandler::deleteLink_(linkType, oldLink);
}

void TM_Demux_Base::setLink_(const LinkType linkType, HandlerLink* newLink) {

	// This should have already been set by connectPrimaryOutput(), but double check.
	if ( linkType == PrimaryOutputLink) {
		HandlerLinkMap::iterator pos = findLink_(newLink);

		if ( pos == primaryOutputLinks_.end() ) {
			MOD_ERROR("setLink_(): HanderLink not found. Expected to be set by connectPrimaryOutput().");
		}
	}
	else BaseTrafficHandler::setLink_(linkType, newLink);
}

void TM_Demux_Base::unsetLink_(const LinkType linkType, HandlerLink* oldLink) {
	if ( linkType == PrimaryOutputLink ) {
		HandlerLinkMap::iterator pos = findLink_(oldLink);

		if ( pos == primaryOutputLinks_.end() ) {
			MOD_ERROR("unsetLink_() could not locate pointer to %s; cannot unset.", oldLink->getTarget()->getName().c_str());
		}
		else {
			primaryOutputLinks_.erase(pos);
		}
	}
	else BaseTrafficHandler::unsetLink_(linkType, oldLink);
}

}
