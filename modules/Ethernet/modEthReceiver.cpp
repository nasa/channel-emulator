/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modEthReceiver.cpp
 * @author Tad Kollar  
 *
 * $Id: modEthReceiver.cpp 2514 2014-02-14 20:03:58Z tkollar $
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "modEthReceiver.hpp"
#include "SettingsManager.hpp"
#include <ace/Reactor.h>

namespace nEthReceiver {

using namespace nasaCE;

modEthReceiver::modEthReceiver(const std::string& newName,
	const std::string& newChannelName,
    const CE_DLL* newDLLPtr):
	FdInputHandler(newName, newChannelName, newDLLPtr),
	_etherDev(0),
	_buffer(0) {
	reactor(ACE_Reactor::instance());
	msg_queue()->deactivate(); // Message queue is not used here.
}

modEthReceiver::~modEthReceiver() {
	MOD_DEBUG("Running ~EthReceiver().");
	disconnectDevice();
	delete _buffer;
}


void modEthReceiver::process_frame(EthernetFrame* eFrame) {
/*	if ( eFrame->extractPayloadLength() != eFrame->calculatePayloadLength() ) {
		MOD_DEBUG("Received a corrupt Ethernet frame (actual payload length %d, expected %d).", eFrame->calculatePayloadLength(), eFrame->extractPayloadLength());
	}
*/

	MOD_DEBUG("Received a %d-octet Ethernet frame.", eFrame->getUnitLength());

	if ( links_[PrimaryOutputLink] ) {
		MOD_DEBUG("Sending frame to target %s.",
			links_[PrimaryOutputLink]->getTarget()->getName().c_str());

		try {
			if ( links_[PrimaryOutputLink]->send(eFrame) < 0 )
				MOD_WARNING("Could not add frame to target's queue: %s", ACE_OS::strerror(errno));
		}
		catch (...) {
			MOD_ERROR("Caught exception and failed to send.");
		}
	}
	else {
		MOD_NOTICE("No output target defined, dropping frame.");
	}
}

void modEthReceiver::connectDevice(CE_Device* device) {
	ND_DEBUG("Connecting EthReceiver %s to Ethernet device %s.\n",
		getName().c_str(), device->getName().c_str());

	device->setReader(this);
	_etherDev = dynamic_cast<devEthernet*>(device);

	_etherDev->enablePromiscuous();
	_etherDev->disableARP();
	setBufferSize();

	set_handle(_etherDev->getSocket());
	this->open();
}

void modEthReceiver::disconnectDevice(const bool updateDevice /* = true */, const bool updateCfg /* = false */ ) {
	stopTraffic();

	if ( !_etherDev ) return;
	this->close();
	set_handle(ACE_INVALID_HANDLE);

	if ( updateDevice ) _etherDev->setReader(0);
	_etherDev = 0;

	if ( updateCfg ) CEcfg::instance()->remove(cfgKey("deviceName"));
}

void modEthReceiver::setBufferSize(const size_t newVal /* = 0 */ ) {
	int bufferSize;

	if ( newVal ) {
		bufferSize = newVal;
	}
	else if ( _etherDev ) {
		bufferSize = _etherDev->getSnapLen();
	}
	else {
		bufferSize = getMRU();
	}

	if ( ! bufferSize ) {
		MOD_WARNING("Unable to set input buffer size intelligently, using 1530.");
		bufferSize = 1530;
	}

	if (_buffer) delete _buffer;
	_buffer = new ACE_UINT8[bufferSize];
	setMRU(bufferSize);
}

int modEthReceiver::handle_input(ACE_HANDLE fd) {
	MOD_DEBUG("Received input event on descriptor #%d.", fd);
	if ( svcDone_ ) return -1;

	size_t mru = getMRU();

	const size_t rcvCount = ACE_OS::recv(fd, reinterpret_cast<char*>(_buffer), mru, MSG_DONTWAIT|MSG_TRUNC);

	if ( rcvCount < 1) {
		MOD_ERROR("%p.", "handle_input recv() error");
		return 0;
	}
	else if ( rcvCount > mru ) {
		MOD_NOTICE("handle_input truncated Ethernet frame: Buffer too short for %d octet message.", rcvCount);
		return 0;
	}

	MOD_DEBUG("handle_input received %d octets of Ethernet frame.", rcvCount);

	EthernetFrame* eFrame = new EthernetFrame(rcvCount, EthernetFrame::PCap,
		EthernetFrame::typeNormalLen, _buffer);

	incReceivedUnitCount(1, HandlerLink::PrimaryInput);
	incReceivedOctetCount(rcvCount, HandlerLink::PrimaryInput);

	timeval tvStamp;
	if ( ACE_OS::ioctl(fd, SIOCGSTAMP, &tvStamp) < 0 ) {
		MOD_NOTICE("%p.", "SIOCGSTAMP failed");
	}
	else {
		ACE_Time_Value aceStamp(tvStamp);
		eFrame->setTimeStamp(aceStamp);
	}

	process_frame(eFrame);

	return 0;
}

} // namespace nEthReceiver
