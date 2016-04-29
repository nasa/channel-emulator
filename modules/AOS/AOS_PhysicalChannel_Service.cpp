/* -*- C++ -*- */

//=============================================================================
/**
 * @file   AOS_PhysicalChannel_Service.cpp
 * @author Tad Kollar  
 *
 * $Id: AOS_PhysicalChannel_Service.cpp 2403 2013-12-12 19:08:39Z tkollar $
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "AOS_PhysicalChannel_Service.hpp"

namespace nasaCE {

AOS_PhysicalChannel_Service::AOS_PhysicalChannel_Service(const std::string& newName,
	const std::string& newChannelName,
    const CE_DLL* newDLLPtr):
	PeriodicTrafficHandler(newName, newChannelName, newDLLPtr),
	idlePattern_(CEcfg::instance()->getOrAddArray(pcKey("idlePattern"))),
	_frameSize(CEcfg::instance()->getOrAddInt(pcKey("frameSize"), 256)),
	_useHeaderErrorControl(CEcfg::instance()->getOrAddBool(pcKey("useHeaderErrorControl"), false)),
	_useFrameErrorControl(CEcfg::instance()->getOrAddBool(pcKey("useFrameErrorControl"), false)),
	_pcInsertZoneSize(CEcfg::instance()->getOrAddInt(pcKey("insertZoneSize"), 0)),
	_dropBadFrames(true),
	_dropBadFramesSetting(CEcfg::instance()->getOrAddBool(cfgKey("dropBadFrames"), _dropBadFrames)),
	_validFrameCount(0L), _badLengthCount(0L), _badTFVNCount(0L) {

	// Update if it was set in the config file.
	_dropBadFrames = _dropBadFramesSetting;

	if ( idlePattern_.getLength() == 0 ) {
		uint8_t defaultPattern[] = { 0xAA, 0xBB, 0xCC };
		setIdlePattern(defaultPattern, 3);
	}
}

AOS_PhysicalChannel_Service::~AOS_PhysicalChannel_Service() {

}

void AOS_PhysicalChannel_Service::setIdlePattern(const uint8_t* idleBuf, const int bufLen) {
	for (int i = 0; i < bufLen; i++) {
		// Add slots when needed
		if ( idlePattern_.getLength() < i + 1 )	idlePattern_.add(Setting::TypeInt);
		idlePattern_[i] = idleBuf[i];
	}

	// Eliminate extra slots
	while (idlePattern_.getLength() > bufLen) idlePattern_.remove(bufLen);

	rebuildIdleUnitTemplate_();
}

void AOS_PhysicalChannel_Service::setIdlePattern(const std::vector<uint8_t>& idlePattern) {
	for (unsigned i = 0; i < idlePattern.size(); i++) {
		// Add slots when needed
		if ( static_cast<unsigned>(idlePattern_.getLength()) < i + 1 )
			idlePattern_.add(Setting::TypeInt);

		idlePattern_[i] = idlePattern[i];
	}

	// Eliminate extra slots
	while (static_cast<unsigned>(idlePattern_.getLength()) > idlePattern.size())
		idlePattern_.remove(idlePattern.size());

	rebuildIdleUnitTemplate_();
}

void AOS_PhysicalChannel_Service::getIdlePattern(std::vector<uint8_t>& idlePattern) const {
	idlePattern.clear();

	for ( int i = 0; i < idlePattern_.getLength(); i++ )
		idlePattern.push_back(static_cast<int>(idlePattern_[i]) & 0xFF);
}

bool AOS_PhysicalChannel_Service::validateFrame(AOS_Transfer_Frame* aos, const bool deleteBadFrame /* = false */) {

	// Test expected values.
	if (static_cast<int>(aos->getUnitLength()) != getFrameSize()) {
		MOD_WARNING("AOS Transfer Frame bad length: received %d octets, needed exactly %d.",
			aos->getUnitLength(), getFrameSize());
		incBadLengthCount();
		if (deleteBadFrame) ndSafeRelease(aos);
		return false;
	}

	if ( aos->getTFVN() != 01 ) {
		MOD_WARNING("AOS Transfer Frame bad version number: should be 01, received %o.", aos->getTFVN());
		// print_transfer_frame_info(aos);
		incBadTFVNCount();
		if (deleteBadFrame) ndSafeRelease(aos);
		return false;
	}

	return true;
}

} // namespace nasaCE
