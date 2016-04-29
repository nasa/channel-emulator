/* -*- C++ -*- */

//=============================================================================
/**
 * @file   PeriodicTrafficHandler.cpp
 * @author Tad Kollar  
 *
 * $Id: PeriodicTrafficHandler.cpp 2336 2013-08-30 17:50:59Z tkollar $
 * Copyright (c) 2010.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "PeriodicTrafficHandler.hpp"
#include "PeriodicLink.hpp"

PeriodicTrafficHandler::PeriodicTrafficHandler(const std::string& newName, const std::string& newChannelName,
	const CE_DLL* newDLLPtr /* = 0 */):
	BaseTrafficHandler(newName, newChannelName, newDLLPtr),
	intervalUsec_(CEcfg::instance()->getOrAddInt(cfgKey("sendIntervalUsec"), 0)),
	idleUnitTemplate_(0),
	idleUnitTemplateCreation_(getName().c_str()), idleCount_(0) {

}

PeriodicTrafficHandler::~PeriodicTrafficHandler() { }

void PeriodicTrafficHandler::stopTraffic(bool shutdown) {
	PeriodicLink* link = 0;

	if ((link = dynamic_cast<PeriodicLink*>(links_[PrimaryOutputLink]))) link->getPeriodicTX()->stopTraffic();
	if ((link = dynamic_cast<PeriodicLink*>(links_[AuxOutputLink]))) link->getPeriodicTX()->stopTraffic();

	BaseTask::stopTraffic(shutdown);
}

void PeriodicTrafficHandler::connectPeriodicOutput(BaseTrafficHandler* target,
	const HandlerLink::OutputRank sourceOutputRank /* = HandlerLink::PrimaryOutput */,
	const HandlerLink::InputRank targetInputRank /* = HandlerLink::PrimaryInput */,
	const bool postOpen /* = false */ ) {

	// Disconnect first
	disconnectOutput(sourceOutputRank);
	ACE_Time_Value interval;
	interval.usec(getInterval());

	// if this is called but open() is not called later, nothing will be transmitted
	PeriodicLink* link = new PeriodicLink(this, target, interval, sourceOutputRank, targetInputRank, postOpen);
	if ( ! link ) throw OutOfMemory("PeriodicTrafficHandler::stopTraffic() failed to allocate memory for new PeriodicLink.");

	const std::string outputSettingName = (sourceOutputRank == HandlerLink::PrimaryOutput)? "primaryOutput" : "auxOutput";
	Setting& outputSetting = CEcfg::instance()->getOrAddArray(cfgKey(outputSettingName));

	if ( outputSetting.getLength() == 0 ) {
		outputSetting.add(Setting::TypeString);
		outputSetting.add(Setting::TypeString);
	}

	outputSetting[0] = target->getName();
	outputSetting[1] = (targetInputRank == HandlerLink::PrimaryInput)? "PrimaryInput" : "AuxInput";
}

void PeriodicTrafficHandler::updateIntervals_() {
	PeriodicLink* link = 0;

	if ((link = dynamic_cast<PeriodicLink*>(links_[PrimaryOutputLink])))
		link->getPeriodicTX()->setInterval(getInterval());

	if ((link = dynamic_cast<PeriodicLink*>(links_[AuxOutputLink])))
		link->getPeriodicTX()->setInterval(getInterval());
}

int PeriodicTrafficHandler::open (void* /* = 0 */) {
	PeriodicLink* plinks[2] = { 0 , 0 };
	int i;

	plinks[0] = dynamic_cast<PeriodicLink*>(links_[PrimaryOutputLink]);
	plinks[1] = dynamic_cast<PeriodicLink*>(links_[AuxOutputLink]);

	// open the transmitter queues first so we can start sending even
	// though they'll be servicing slightly afterwards
	for ( i = 0; i < 2; ++i ) {
		if (plinks[i]) {
			MOD_INFO("Opening transmitter message queue #%d.", i);
			plinks[i]->getPeriodicTX()->msg_queue()->open(msg_queue()->high_water_mark(), msg_queue()->low_water_mark());
		}
	}

	// start our own svc() up
	int ret = BaseTask::open();

	// start svc() in each transmitter
	for ( i = 0; i < 2; ++i ) {
		if (plinks[i]) {
			MOD_INFO("Activating transmitter #%d.", i);
			plinks[i]->getPeriodicTX()->open();
		}
	}

	return ret;

}