/* -*- C++ -*- */

//=============================================================================
/**
 * @file   TM_PhysicalChannel_Service.cpp
 * @author Tad Kollar  
 *
 * $Id: TM_PhysicalChannel_Service.cpp 2489 2014-02-13 16:44:28Z tkollar $
 * Copyright (c) 2012.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "TM_PhysicalChannel_Service.hpp"

namespace nasaCE {

TM_PhysicalChannel_Service::TM_PhysicalChannel_Service(const std::string& newName,
	const std::string& newChannelName,
    const CE_DLL* newDLLPtr):
	PeriodicTrafficHandler(newName, newChannelName, newDLLPtr),
	idlePattern_(CEcfg::instance()->getOrAddArray(pcKey("idlePattern"))),
	_frameSize(CEcfg::instance()->getOrAddInt(pcKey("frameSize"), 256)),
	_useFrameErrorControl(CEcfg::instance()->getOrAddBool(pcKey("useFrameErrorControl"), false)),
	_dropBadFrames(true),
	_dropBadFramesSetting(CEcfg::instance()->getOrAddBool(cfgKey("dropBadFrames"), _dropBadFrames)),
	_validFrameCount(0), _badFrameChecksumCount(0), _badHeaderCount(0), _badLengthCount(0) {

	// Update if it was set in the config file.
	_dropBadFrames = _dropBadFramesSetting;

	if ( idlePattern_.getLength() == 0 ) {
		uint8_t defaultPattern[] = { 0xAA, 0xBB, 0xCC };
		setIdlePattern(defaultPattern, 3);
	}
}

TM_PhysicalChannel_Service::~TM_PhysicalChannel_Service() {

}

void TM_PhysicalChannel_Service::setIdlePattern(const uint8_t* idleBuf, const int bufLen) {
	for (int i = 0; i < bufLen; i++) {
		// Add slots when needed
		if ( idlePattern_.getLength() < i + 1 )	idlePattern_.add(Setting::TypeInt);
		idlePattern_[i] = idleBuf[i];
	}

	// Eliminate extra slots
	while (idlePattern_.getLength() > bufLen) idlePattern_.remove(bufLen);

	rebuildIdleUnitTemplate_();
}

void TM_PhysicalChannel_Service::setIdlePattern(const std::vector<uint8_t>& idlePattern) {
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

void TM_PhysicalChannel_Service::getIdlePattern(std::vector<uint8_t>& idlePattern) const {
	idlePattern.clear();

	for ( int i = 0; i < idlePattern_.getLength(); i++ )
		idlePattern.push_back(static_cast<int>(idlePattern_[i]) & 0xFF);
}

void TM_PhysicalChannel_Service::addErrorControlIfPossible(TM_Transfer_Frame* frame) {
	if ( getUseFrameErrorControl() && frame->hasFrameErrorControl() )
		frame->initializeCRC();
}

bool TM_PhysicalChannel_Service::validateFrame(TM_Transfer_Frame*& frame, const bool deleteBadFrame /* = false */) {
	// Test expected values.
	if (static_cast<int>(frame->getUnitLength()) != getFrameSize()) {
		MOD_WARNING("TM Transfer Frame bad length: received %d octets, needed exactly %d.",
			frame->getUnitLength(), getFrameSize());
		incBadLengthCount();
		if (deleteBadFrame) ndSafeRelease(frame);
		return false;
	}

	int valid = frame->isValid();

	if ( valid > 0 ) return true;

	switch (valid) {
		case -1: MOD_WARNING("TM Transfer Frame version number is not 00.");
			break;
		case -2: MOD_WARNING("Operational Control Field Flag must be set if there's an Operational Control Field.");
			break;
		case -3: MOD_WARNING("Operational Control Field Flag must be not set if there's no Operational Control Field.");
			break;
		case -4: MOD_WARNING("Secondary header flag must be set if there's a secondary header.");
			break;
		case -5: MOD_WARNING("Secondary header flag must not be set if there's no secondary header.");
			break;
		case -6: MOD_WARNING("If the Synchronization Flag is 0, the Packet Order Flag must be 0.");
			break;
		case -7: MOD_WARNING("If the Synchronization Flag is 0, the Segment Length Identifier must be 11 (binary).");
			break;
		case -8: MOD_WARNING("The secondary header data length stored in the buffer must be one less than the mission-phase secondary header length field.");
			break;
		case -9: MOD_WARNING("The first header pointer can't point beyond the end of the data field.");
			break;
		default: MOD_WARNING("TM Transfer Frame invalid for unknown reason.");
	}

	incBadHeaderCount();
	return false;
}

} // namespace nasaCE
