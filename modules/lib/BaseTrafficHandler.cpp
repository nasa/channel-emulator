/* -*- C++ -*- */

//=============================================================================
/**
 * @file   BaseTrafficHandler.cpp
 * @author Tad Kollar  
 *
 * $Id: BaseTrafficHandler.cpp 2350 2013-09-03 14:29:09Z tkollar $
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "BaseTrafficHandler.hpp"
#include "ace/Thread_Manager.h"

const int BaseTrafficHandler::defaultLowWaterMark = 0x300000;
const int BaseTrafficHandler::defaultHighWaterMark = 0x400000;

BaseTrafficHandler::BaseTrafficHandler(const std::string& newName,
	const std::string& newChannelName,
	const CE_DLL* newDLLPtr /* = 0 */):
	BaseTask(newName), // Base class constructor
	channelName_(newChannelName),
	refCount_(0),
	MRU_(0),
	mruSetting_(CEcfg::instance()->getOrAddInt(cfgKey("MRU"), MRU_)),
	MTU_(0),
	mtuSetting_(CEcfg::instance()->getOrAddInt(cfgKey("MTU"), MTU_)),
	lowWaterMarkSetting_(CEcfg::instance()->getOrAddInt(cfgKey("lowWaterMark"), defaultLowWaterMark)),
	highWaterMarkSetting_(CEcfg::instance()->getOrAddInt(cfgKey("highWaterMark"), defaultHighWaterMark)),
	dumpToLog_(false),
	dumpToLogSetting_(CEcfg::instance()->getOrAddBool(cfgKey("dumpToLog"), dumpToLog_)),
	ptrDLL_(const_cast<CE_DLL*>(newDLLPtr)),
	auxInputQueue_(0) {

	ND_DEBUG("Constructing a BaseTrafficHandler named %s.\n", name_.c_str());

	receivedUnitCount_[0] = 0;
	receivedUnitCount_[1] = 0;

	receivedOctetCount_[0] = 0;
	receivedOctetCount_[1] = 0;

	links_[0] = 0; links_[1] = 0; links_[2] = 0; links_[3] = 0;

	MRU_ = static_cast<int>(mruSetting_);
	MTU_ = static_cast<int>(mtuSetting_);
	dumpToLog_ = dumpToLogSetting_;

	if (ptrDLL_) ptrDLL_->incReference();

	msg_queue()->low_water_mark(static_cast<int>(lowWaterMarkSetting_));
	msg_queue()->high_water_mark(static_cast<int>(highWaterMarkSetting_));

	#ifdef DEFINE_DEBUG
	Setting& debugSetting = CEcfg::instance()->getOrAddBool(cfgKey("debug"), globalCfg.debug);
	setDebugging(debugSetting);
	#endif
}

BaseTrafficHandler::~BaseTrafficHandler() {
	MOD_DEBUG("Running ~BaseTrafficHandler().\n");
	stopTraffic();

	if (ptrDLL_) ptrDLL_->decReference();
}

int BaseTrafficHandler::svc() {
	svcStart_();
	ACE_Message_Block* data;

	while ( ! shutdown_ ) {
		getq(data);
		if (msg_queue()->deactivated()) break;

		if (next()) {
			next()->putq(data);
			MOD_DEBUG("BaseTrafficHandler data has been send on to next target.");
		}
		else {
			MOD_DEBUG("No output target defined, dropping %d bytes.", data->size());
		}
	}

	return svcEnd_();
}

bool BaseTrafficHandler::isCircularConnection(BaseTrafficHandler* testTarget) {
	// Don't output directly to ourselves!
	if (testTarget == this) return true;

	// Test primary output target, if it exists.
	if (links_[PrimaryOutputLink] &&
		links_[PrimaryOutputLink]->getTarget()->isCircularConnection(testTarget) )
		return true;

	// Test auxiliary output target, if it exists.
	if (links_[AuxOutputLink] &&
		links_[AuxOutputLink]->getTarget()->isCircularConnection(testTarget) )
		return true;

	// Must be safe.
	return false;
}

void BaseTrafficHandler::connectOutput(BaseTrafficHandler* target,
	const HandlerLink::OutputRank sourceOutputRank /* = HandlerLink::PrimaryOutput */,
	const HandlerLink::InputRank targetInputRank /* = HandlerLink::PrimaryInput */,
	const bool postOpen /* = false */ ) {

	// Disconnect first
	disconnectOutput(sourceOutputRank);
	HandlerLink* handlerPtr = new HandlerLink(this, target, sourceOutputRank, targetInputRank);
	if ( handlerPtr == 0 ) throw OutOfMemory("BaseTrafficHandler::connectOutput: Unable to allocate memory for new HandlerLink.");

	const std::string outputSettingName = (sourceOutputRank == HandlerLink::PrimaryOutput)? "primaryOutput" : "auxOutput";
	Setting& outputSetting = CEcfg::instance()->getOrAddArray(cfgKey(outputSettingName));

	if ( outputSetting.getLength() == 0 ) {
		outputSetting.add(Setting::TypeString);
		outputSetting.add(Setting::TypeString);
	}

	outputSetting[0] = target->getName();
	outputSetting[1] = (targetInputRank == HandlerLink::PrimaryInput)? "PrimaryInput" : "AuxInput";
}

void BaseTrafficHandler::disconnectOutput(const HandlerLink::OutputRank sourceOutputRank
	/* = HandlerLink::PrimaryOutput */, HandlerLink* ) {

	switch (sourceOutputRank) {
	case HandlerLink::PrimaryOutput:
		deleteLink_(PrimaryOutputLink, links_[PrimaryOutputLink]);
		break;
	case HandlerLink::AuxOutput:
		deleteLink_(AuxOutputLink, links_[AuxOutputLink]);
		break;
	}

	const std::string outputSettingName = (sourceOutputRank == HandlerLink::PrimaryOutput)? "primaryOutput" : "auxOutput";
	if ( CEcfg::instance()->exists(cfgKey(outputSettingName))) {
		CEcfg::instance()->remove(cfgKey(outputSettingName));
	}
}

void BaseTrafficHandler::withdraw(const bool doStopTraffic /* = true */) {
	MOD_DEBUG("Withdrawing from channel output linkages.");
	if (doStopTraffic) stopTraffic();

	for (int link = 0; link < 4; ++link ) delete links_[link];
}

int BaseTrafficHandler::putData(NetworkData* data,
	const HandlerLink::InputRank inputRank /* = HandlerLink::PrimaryInput */ ) {

	if ( ! data ) {
		MOD_ERROR("BaseTrafficHandler::putData() received a null data pointer.");
		return -1;
	}

	ACE_Message_Block* mblk = dynamic_cast<ACE_Message_Block*>(data);

	if ( ! mblk ) {
		MOD_ERROR("BaseTrafficHandler::putData() failed to cast from NetworkData* to ACE_Message_Block*.");
		return -1;
	}

	switch (inputRank) {
		case HandlerLink::PrimaryInput:
			return putq(mblk);
			break;
		case HandlerLink::AuxInput:
			return getAuxQueue()->enqueue_tail(mblk);
			break;
	}

	return -1;
}

void BaseTrafficHandler::setReceivedUnitCount(const ACE_UINT32& newVal,
	const HandlerLink::InputRank inputRank /* = HandlerLink::PrimaryInput */ ) {
	receivedUnitCount_[inputRank] = newVal;
}

uint64_t BaseTrafficHandler::incReceivedUnitCount(const ACE_UINT32& step /* = 1 */,
	const HandlerLink::InputRank inputRank /* = HandlerLink::PrimaryInput */ ) {
	return (receivedUnitCount_[inputRank] += step);
}

uint64_t BaseTrafficHandler::getReceivedUnitCount(
	const HandlerLink::InputRank inputRank /* = HandlerLink::PrimaryInput */ ) const {
	return receivedUnitCount_[inputRank];
}

void BaseTrafficHandler::setReceivedOctetCount(const ACE_UINT32& newVal,
	const HandlerLink::InputRank inputRank /* = HandlerLink::PrimaryInput */ ) {
	receivedOctetCount_[inputRank] = newVal;
}

uint64_t BaseTrafficHandler::incReceivedOctetCount(const ACE_UINT32& step /* = 1 */,
	const HandlerLink::InputRank inputRank /* = HandlerLink::PrimaryInput */ ) {
	return (receivedOctetCount_[inputRank] += step);
}

uint64_t BaseTrafficHandler::getReceivedOctetCount(
	const HandlerLink::InputRank inputRank /* = HandlerLink::PrimaryInput */ ) const {
	return receivedOctetCount_[inputRank];
}

size_t BaseTrafficHandler::getQueuedUnitCount(bool useAuxQueue /* = false */ ) {
	if ( useAuxQueue ) return getAuxQueue()->message_count();
	return msg_queue()->message_count();
}

size_t BaseTrafficHandler::getQueuedOctetCount(bool useAuxQueue /* = false */ ) {
	if ( useAuxQueue ) return getAuxQueue()->message_length();
	return msg_queue()->message_length();
}

void BaseTrafficHandler::dumpUnit(NetworkData* unit) const {
	if (dumpToLog_) unit->dump(0, false);
}

std::pair<NetworkData*, int> BaseTrafficHandler::getData_(
	const HandlerLink::InputRank inputRank /* = HandlerLink::PrimaryInput */,
	ACE_Time_Value* timeout /* = 0 */) {

	ACE_Message_Block* mblk = 0;
	NetworkData* data = 0;
	int messages = 0;

	switch (inputRank) {
		case HandlerLink::PrimaryInput:
			messages = taskType::getq(mblk, timeout);
			break;
		case HandlerLink::AuxInput:
			messages = getAuxQueue()->dequeue_head(mblk, timeout);
			break;
	}

	if (mblk) data = dynamic_cast<NetworkData*>(mblk);

	if (data) {
		incReceivedUnitCount(1, inputRank);
		incReceivedOctetCount(data->getUnitLength(), inputRank);
	}

	return std::make_pair(data, messages);
}

void BaseTrafficHandler::deleteLink_(const LinkType linkType, HandlerLink*) {
	if ( links_[linkType] ) {
		delete links_[linkType];
		links_[linkType] = 0;
	}
}

void BaseTrafficHandler::setLink_(const LinkType linkType, HandlerLink* newLink) {
	if ( links_[linkType] ) deleteLink_(linkType, links_[linkType]);
	links_[linkType] = newLink;
}

void BaseTrafficHandler::unsetLink_(const LinkType linkType, HandlerLink*) {
	if (! links_[linkType] ) {
		MOD_WARNING("unsetLink() called on null link pointer.");
		return;
	}

	links_[linkType] = 0;
}
