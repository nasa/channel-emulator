/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modFdReceiver.cpp
 * @author Tad Kollar  
 *
 * $Id: modFdReceiver.cpp 2366 2013-09-24 18:45:54Z tkollar $
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "modFdReceiver.hpp"

namespace nFdReceiver {

using namespace nasaCE;

modFdReceiver::modFdReceiver(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr):
	BaseTrafficHandler(newName, newChannelName, newDLLPtr),
	_device(0),
	_repeatMax(CEcfg::instance()->getOrAddInt(cfgKey("repeatMax"))),
	_repeatCount(0) {
	msg_queue()->close(); // Don't use the message queue.

	// Set the default read size to 4k.
	if ( getMaxRead() == 0 ) setMaxRead(4096);
}

modFdReceiver::~modFdReceiver() {
	MOD_DEBUG("Shutting down modFdReceiver.");
	disconnectDevice();
}

int modFdReceiver::svc() {
	if ( ! _device ) return svcEnd_();

	svcStart_();

	ACE_UINT8 buffer[getMaxRead()];
	struct stat fileStats;
	bool isRegularFile = false;

	if ( ! ACE_OS::fstat(_device->getHandle(), &fileStats) )
		isRegularFile = ( (fileStats.st_mode & S_IFREG) > 0);

	while  ( _device && continueService() ) {
		int recv_cnt = 0;

		if ((recv_cnt = ACE_OS::read(_device->getHandle(), buffer, getMaxRead()))) {
			MOD_DEBUG("Returned from read() with a non-zero value.");

			if ( recv_cnt < 0 ) {
				MOD_ERROR("Emergency shut down! Error reading from the descriptor: %s", ACE_OS::strerror(errno));
				shutdown_ = true;
				continue;
			}

			MOD_DEBUG("Successfully read %d bytes from the descriptor.", recv_cnt);

			incReceivedUnitCount(1, HandlerLink::PrimaryInput);
			incReceivedOctetCount(recv_cnt, HandlerLink::PrimaryInput);

			if ( links_[PrimaryOutputLink] ) {
				NetworkData *data = new NetworkData(recv_cnt, buffer);
				links_[PrimaryOutputLink]->send(data);
			}
			else {
				MOD_NOTICE("No output target defined yet, dropping data.");
			}
		}
		else if ( isRegularFile) { // Reached the end of input; determine if we keep going

			if ( static_cast<int>(_repeatMax) >= 0 && _repeatCount >= static_cast<int>(_repeatMax) ) {
				MOD_INFO("Finished reading regular file, leaving active loop.");
				shutdown_ = true;
				continue;
			}

			MOD_DEBUG("Rewinding file to read/send for iteration #%d.",	_repeatCount + 2);

			if ( ACE_OS::lseek(_device->getHandle(), 0, SEEK_SET) < 0 ) {
				MOD_ERROR("Rewinding file failed: %s", ACE_OS::strerror(errno));
				shutdown_ = true;
				continue;
			}

			++_repeatCount;
		}
	}

	return svcEnd_();

}

void modFdReceiver::connectDevice(CE_Device* device) {
	ND_DEBUG("[%s] Connecting to File Device %s.\n",
		getName().c_str(), device->getName().c_str());

	device->setReader(this);
	_device = device;
}

void modFdReceiver::disconnectDevice(const bool updateDevice /* = true */, const bool updateCfg /* = false */) {
	stopTraffic();
	if ( ! _device ) return;
	if ( updateDevice ) _device->setReader(0);
	_device = 0;

	if ( updateCfg ) CEcfg::instance()->remove(cfgKey("deviceName"));

}

}
