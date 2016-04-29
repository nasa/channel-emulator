/* -*- C++ -*- */

//=============================================================================
/**
 * @file   AOS_Mux_Base.cpp
 * @author Tad Kollar  
 *
 * $Id: AOS_Mux_Base.cpp 2034 2013-06-05 19:32:24Z tkollar $
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "AOS_Mux_Base.hpp"
#include "AOS_Transfer_Frame.hpp"
#include "PeriodicLink.hpp"
#include "SettingsManager.hpp"

namespace nAOS_Mux {

AOS_Mux_Base::AOS_Mux_Base(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr /* = 0 */):
	AOS_MasterChannel_Service(newName, newChannelName, newDLLPtr),
		defaultPriority_(1000),
		defaultPrioritySetting_(CEcfg::instance()->getOrAddInt(cfgKey("defaultMuxPriority"), defaultPriority_)),
		prioritiesSetting_(CEcfg::instance()->getOrAddList(cfgKey("channelID_Priorities"))) {

	defaultPriority_ = defaultPrioritySetting_;

	for ( int idx = 0; idx < prioritiesSetting_.getLength(); ++idx ) {
		priorities_[static_cast<uint8_t>(static_cast<unsigned int>(prioritiesSetting_[idx][0]))] =
			static_cast<int>(prioritiesSetting_[idx][1]);
	}

	rebuildIdleUnitTemplate_();
}

AOS_Mux_Base::~AOS_Mux_Base() {
	MOD_DEBUG("Running ~AOS_Mux_Base().");

	stopTraffic();
	delete idleUnitTemplate_;
}

void AOS_Mux_Base::withdraw(const bool doStopTraffic /* = true */) {
	if (doStopTraffic) stopTraffic();

	HandlerLinkSet::iterator pos;

	for ( pos = primaryInputLinks_.begin(); pos != primaryInputLinks_.end(); pos = primaryInputLinks_.begin() ) {
		deleteLink_(PrimaryInputLink, *pos);
	}

	primaryInputLinks_.clear(); // Really shouldn't be necessary

	BaseTrafficHandler::withdraw(false);
}

void AOS_Mux_Base::deleteLink_(const LinkType linkType, HandlerLink* oldLink) {
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

void AOS_Mux_Base::unsetLink_(const LinkType linkType, HandlerLink* oldLink) {
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

int AOS_Mux_Base::getPriority(const uint8_t identifier) {
	PriorityMapType::iterator pos = priorities_.find(identifier);
	if ( pos != priorities_.end() ) return pos->second;

	return defaultPriority_;
}

void AOS_Mux_Base::setPriority(const uint8_t identifier, const int newPriority) {
	if ( priorities_.find(identifier) != priorities_.end() ) {
		priorities_[identifier] = newPriority;

		for ( int idx = 0; idx < prioritiesSetting_.getLength(); ++idx ) {
			if ( static_cast<unsigned int>(prioritiesSetting_[idx][0]) == static_cast<unsigned int>(identifier) ) {
				prioritiesSetting_[idx][1] = static_cast<int>(identifier);
			}
		}
	}

	Setting& newItem = prioritiesSetting_.add(Setting::TypeArray);
	newItem.add(Setting::TypeInt);
	newItem.add(Setting::TypeInt);
	newItem[0] = static_cast<int>(identifier);
	newItem[1] = newPriority;
}

int AOS_Mux_Base::putq(ACE_Message_Block *mb, ACE_Time_Value *timeout /* = 0 */) {
	mb->msg_priority(getPriority(getIdentifier_(mb)));

	return taskType::putq(mb, timeout);
}

void AOS_Mux_Base::setLink_(const LinkType linkType, HandlerLink* newLink) {
	if ( linkType == PrimaryInputLink) {
		if ( ! primaryInputLinks_.insert(newLink).second ) {
			MOD_ERROR("Handler link %s not added because it already exists!",
				name_.c_str(), newLink->getTarget()->getName().c_str());
		}
	}
	else BaseTrafficHandler::setLink_(linkType, newLink);
}


NetworkData* AOS_Mux_Base::getIdleUnit() {
	if (!idleUnitTemplate_)
		throw NoIdleUnit("The AOS transfer frame idle template has not been constructed yet.");

	idleUnitTemplateCreation_.acquire();
	AOS_Transfer_Frame* idleDup = new AOS_Transfer_Frame(dynamic_cast<AOS_Transfer_Frame*>(idleUnitTemplate_));
	idleUnitTemplateCreation_.release();
	return idleDup;
}

void AOS_Mux_Base::rebuildIdleUnitTemplate_() {
	ND_DEBUG("[%s] Rebuilding Idle AOS Transfer Frame template.");
	idleUnitTemplateCreation_.acquire();

	AOS_Transfer_Frame* newFrame = new AOS_Transfer_Frame(static_cast<size_t>(getFrameSize()));
	newFrame->makeIdle(getSCID());

	NetworkData* idleData = new NetworkData(idlePattern_.getLength());
	for (int i = 0; i < idlePattern_.getLength(); i++ ) {
		*(idleData->ptrUnit() + i) = static_cast<int>(idlePattern_[i]) & 0xFF;
	}

	newFrame->setDataToPattern(idleData);
	delete idleData;
	delete idleUnitTemplate_;
	idleUnitTemplate_ = newFrame;

	idleUnitTemplateCreation_.release();
}

void AOS_Mux_Base::getInputNames(std::list<std::string>& inputList) {
	HandlerLinkSet::iterator pos;

	inputList.clear();

	for ( pos = primaryInputLinks_.begin(); pos != primaryInputLinks_.end(); ++pos ) {
		if ( (*pos)->getSource() )
			inputList.push_back((*pos)->getSource()->getName());
	}

	inputList.sort();
}

}
