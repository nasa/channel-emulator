/* -*- C++ -*- */

//=============================================================================
/**
 * @file   TM_Mux_Base.cpp
 * @author Tad Kollar  
 *
 * $Id: TM_Mux_Base.cpp 2034 2013-06-05 19:32:24Z tkollar $
 * Copyright (c) 2012.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "TM_Mux_Base.hpp"
#include "TM_Transfer_Frame.hpp"
#include "PeriodicLink.hpp"
#include "SettingsManager.hpp"

namespace nTM_Mux {

TM_Mux_Base::TM_Mux_Base(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr /* = 0 */):
	TM_VirtualChannel_Service(newName, newChannelName, newDLLPtr),
		defaultPriority_(CEcfg::instance()->getOrAddInt(cfgKey("defaultMuxPriority"), 1000)),
		priorities_(CEcfg::instance()->getOrAddList(cfgKey("channelID_Priorities"))) {

	rebuildIdleUnitTemplate_();
}

TM_Mux_Base::~TM_Mux_Base() {
	MOD_DEBUG("Running ~TM_Mux_Base().");

	stopTraffic();
	delete idleUnitTemplate_;
}

void TM_Mux_Base::withdraw(const bool doStopTraffic /* = true */) {
	if (doStopTraffic) stopTraffic();

	HandlerLinkSet::iterator pos;

	for ( pos = primaryInputLinks_.begin(); pos != primaryInputLinks_.end(); pos = primaryInputLinks_.begin() ) {
		deleteLink_(PrimaryInputLink, *pos);
	}

	primaryInputLinks_.clear(); // This really shouldn't be needed

	BaseTrafficHandler::withdraw(false);
}

void TM_Mux_Base::deleteLink_(const LinkType linkType, HandlerLink* oldLink) {
	if ( linkType == PrimaryInputLink) {
		HandlerLinkSet::iterator pos = primaryInputLinks_.find(oldLink);

		if ( pos == primaryInputLinks_.end() ) {
			MOD_ERROR("deleteLink_() could not locate pointer to %s -> %s; cannot delete.",
					oldLink->getSource()->getName().c_str(), oldLink->getTarget()->getName().c_str());
		}
		else {
			MOD_INFO("Running deleteLink_() on input link from %s -> %s.",
					oldLink->getSource()->getName().c_str(), oldLink->getTarget()->getName().c_str());
			delete *pos;
		}
	}
	else BaseTrafficHandler::deleteLink_(linkType, oldLink);
}

void TM_Mux_Base::unsetLink_(const LinkType linkType, HandlerLink* oldLink) {
	if ( linkType == PrimaryInputLink ) {
		HandlerLinkSet::iterator pos = primaryInputLinks_.find(oldLink);

		if ( pos == primaryInputLinks_.end() ) {
			MOD_ERROR("unsetLink_() could not locate pointer to %s; cannot unset.", oldLink->getTarget()->getName().c_str());
		}
		else {
			MOD_INFO("Deleting link from set.");
			primaryInputLinks_.erase(pos);
		}
	}
	else BaseTrafficHandler::unsetLink_(linkType, oldLink);
}

int TM_Mux_Base::getPriority(const ACE_UINT8 identifier) {
	for ( int idx = 0; idx < priorities_.getLength(); ++idx ) {
		if ( (unsigned int) priorities_[idx][0] == (unsigned int) identifier )
			return priorities_[idx][1];
	}

	return defaultPriority_;
}

void TM_Mux_Base::setPriority(const ACE_UINT8 identifier, const int newPriority) {
	int idx;

	for ( idx = 0; idx < priorities_.getLength(); ++idx ) {
		if ( (unsigned int) priorities_[idx][0] == (unsigned int) identifier ) {
			priorities_[idx][1] = (int) identifier;
			return;
		}
	}

	priorities_.add(Setting::TypeArray);
	priorities_[idx].add(Setting::TypeInt);
	priorities_[idx].add(Setting::TypeInt);
	priorities_[idx][0] = (int) identifier;
	priorities_[idx][1] = newPriority;
}

int TM_Mux_Base::putq(ACE_Message_Block *mb, ACE_Time_Value *timeout /* = 0 */) {
	mb->msg_priority(getPriority(getIdentifier_(mb)));

	return taskType::putq(mb, timeout);
}

void TM_Mux_Base::setLink_(const LinkType linkType, HandlerLink* newLink) {
	if ( linkType == PrimaryInputLink) {
		if ( ! primaryInputLinks_.insert(newLink).second ) {
			MOD_ERROR("Handler link %s not added because it already exists!",
				name_.c_str(), newLink->getTarget()->getName().c_str());
		}
	}
	else BaseTrafficHandler::setLink_(linkType, newLink);
}


NetworkData* TM_Mux_Base::getIdleUnit() {
	if (!idleUnitTemplate_)
		throw NoIdleUnit("The TM transfer frame idle template has not been constructed yet.");

	idleUnitTemplateCreation_.acquire();
	TM_Transfer_Frame* idleDup = new TM_Transfer_Frame(dynamic_cast<TM_Transfer_Frame*>(idleUnitTemplate_));
	idleUnitTemplateCreation_.release();
	return idleDup;
}

void TM_Mux_Base::rebuildIdleUnitTemplate_() {
	ND_DEBUG("[%s] Rebuilding Idle TM Transfer Frame template.");
	idleUnitTemplateCreation_.acquire();

	TM_Transfer_Frame* newFrame = new TM_Transfer_Frame(static_cast<size_t>(getFrameSize()));

	NetworkData* idleData = new NetworkData(idlePattern_.getLength());
	for (int i = 0; i < idlePattern_.getLength(); i++ ) {
		*(idleData->ptrUnit() + i) = static_cast<int>(idlePattern_[i]) & 0xFF;
	}

	newFrame->makeIdle(getSCID(), getVCID(), idleData);

	delete idleData;
	delete idleUnitTemplate_;
	idleUnitTemplate_ = newFrame;

	idleUnitTemplateCreation_.release();
}

void TM_Mux_Base::getInputNames(std::list<std::string>& inputList) {
	HandlerLinkSet::iterator pos;

	inputList.clear();

	for ( pos = primaryInputLinks_.begin(); pos != primaryInputLinks_.end(); ++pos ) {
		if ( (*pos)->getSource() )
			inputList.push_back((*pos)->getSource()->getName());
	}

	inputList.sort();
}

}
