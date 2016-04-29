/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modEthTransmitter.cpp
 * @author Tad Kollar  
 *
 * $Id: modEthTransmitter.cpp 2524 2014-02-19 20:46:51Z tkollar $
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "modEthTransmitter.hpp"
#include "httpd/string_nocase.hpp"

namespace nEthTransmitter {

using namespace nasaCE;

modEthTransmitter::modEthTransmitter(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr):
	BaseTrafficHandler(newName, newChannelName, newDLLPtr),
	_etherDev(0),
	_linkCheckFreq(LinkCheckNever),
	_linkCheckFreqStr(CEcfg::instance()->getOrAddString(cfgKey("linkCheckFreq"), "Never")),
	_linkCheckMaxIterations(CEcfg::instance()->getOrAddInt(cfgKey("linkCheckMaxIterations"), 1000)),
	_linkCheckSleepMSec(CEcfg::instance()->getOrAddInt(cfgKey("linkCheckSleepMsec"), 1)) {

	setLinkCheckFreqStr(_linkCheckFreqStr); // To force _linkCheckFreq to be set as well.
}

modEthTransmitter::~modEthTransmitter() {
	MOD_DEBUG("Running ~EthTransmitter().");
	disconnectDevice();
}

int modEthTransmitter::open (void* /* = 0 */ ) {
	if ( ! shutdown_ ) {
		MOD_DEBUG("Variable shutdown_ is already false, not activating twice.");
		return 0;
	}

	if ( ! _etherDev ) {
		MOD_ERROR("Cannot activate, ethernet device is undefined.");
		return 0;
	}

	shutdown_ = false;
	// return activate(THR_JOINABLE, 1);
	return activate(THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED, maxThreads);

}

int modEthTransmitter::svc() {
	svcStart_();

	if ( getLinkCheckFreq() == LinkCheckOnce ) _waitForLink();

	while ( continueService() ) {
		std::pair<NetworkData*, int> queueTop = getData_();

		if (msg_queue()->deactivated()) break;

		// If LinkCheckAlways has been specified, don't send anything
		// until a link is achieved.
		if ( getLinkCheckFreq() == LinkCheckAlways ) _waitForLink();

		EthernetFrame* eFrame = dynamic_cast<EthernetFrame*>(queueTop.first);

		if ( ! eFrame ) {
			MOD_WARNING("Received non-Ethernet data (%s), dropping.", queueTop.first->typeStr().c_str());
			ndSafeRelease(queueTop.first);
			continue;
		}

		#ifdef DEFINE_DEBUG
		MacAddress src(eFrame->ptrSrcAddr());
		MacAddress dst(eFrame->ptrDstAddr());
		MOD_DEBUG("Received frame to send out device %s (addr %s -> %s).", _etherDev->getName().c_str(),
			src.addr2str().c_str(), dst.addr2str().c_str());
		#endif

		if (!_etherDev || _etherDev->getSocket() == ACE_INVALID_HANDLE) {
			MOD_ERROR("Missing ethernet device or socket, cannot send.");
		}
		else {
			size_t len = eFrame->getUnitLength();

			ssize_t sent = send(_etherDev->getSocket(),
				reinterpret_cast<const char*>(eFrame->ptrUnit()),
				len, MSG_CONFIRM);

			if ( static_cast<size_t>(sent) != len ) {
				MOD_ERROR("Error during send: %s.", ACE_OS::strerror(errno));
			}
			else {
				MOD_DEBUG("Successfully sent %d octets.", len);
			}
		}

		// eFrame->dump();
		eFrame->release();
	}

	return svcEnd_();

}

void modEthTransmitter::connectDevice(CE_Device* device) {
	MOD_DEBUG("Connecting to Ethernet device %s.", device->getName().c_str());

	device->setWriter(this);
	_etherDev = dynamic_cast<devEthernet*>(device);
	_etherDev->disableARP();
}

void modEthTransmitter::disconnectDevice(const bool updateDevice /* = true */, const bool updateCfg /* = false */) {
	stopTraffic();

	if ( !_etherDev ) return;
	if ( updateDevice) _etherDev->setWriter(0);
	_etherDev = 0;

	if ( updateCfg ) CEcfg::instance()->remove(cfgKey("deviceName"));
}

void modEthTransmitter::setLinkCheckFreqStr(const std::string& newVal) {
	string_nocase checkType(newVal.c_str());

	if ( checkType == "never" ) { _linkCheckFreq = LinkCheckNever; _linkCheckFreqStr = "Never"; }
	else if ( checkType == "once" ) { _linkCheckFreq = LinkCheckOnce; _linkCheckFreqStr = "Once"; }
	else if ( checkType == "always" ) { _linkCheckFreq = LinkCheckAlways; _linkCheckFreqStr = "Always"; }
	else throw BadValue(BadValue::msg("Link Check Frequency", newVal));

	MOD_INFO("Will now check for link: %s.", _linkCheckFreqStr.c_str());
}

bool modEthTransmitter::_waitForLink() {
	int linkCheckCount = 0;

	timespec_t timeToWait = { 0, 0 };

	while ( continueService() ) {
		timeToWait.tv_nsec = 1000000 * getLinkCheckSleepMSec();

		if (_etherDev->hasLink()) {
			MOD_DEBUG("Detected link.");
			return true;
		}

		++linkCheckCount;

		if ( getLinkCheckMaxIterations() >= 0 &&
			linkCheckCount >= getLinkCheckMaxIterations() ) {
			MOD_INFO("Failed to detect link for %s after %d attempts, giving up.",
				_etherDev->getName().c_str(), linkCheckCount);
			return false;
		}

		MOD_DEBUG("Failed to detect link for %s (attempt %d); sleeping %dms.",
			_etherDev->getName().c_str(), linkCheckCount, getLinkCheckSleepMSec());

		ACE_OS::nanosleep(&timeToWait, 0);
	}

	// If this point is reached, a shutdown must have been requested.
	svcEnd_();
	return false;
}

} // namespace nEthTransmitter
