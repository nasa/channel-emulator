/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modTM_VC_Rcv_Packet.cpp
 * @author Tad Kollar  
 *
 * $Id: modTM_VC_Rcv_Packet.cpp 2405 2013-12-12 19:56:04Z tkollar $
 * Copyright (c) 2012.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "modTM_VC_Rcv_Packet.hpp"
#include "EncapsulationPacket.hpp"
#include "SpacePacket.hpp"

namespace nTM_VC_Rcv_Packet {

using namespace nasaCE;
using namespace nTM_VC_Rcv_Packet;
using namespace nTM_Demux;

modTM_VC_Rcv_Packet::modTM_VC_Rcv_Packet(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr):
	TM_Demux_Base(newName, newChannelName, newDLLPtr),
	_packet(0),
	_ipeSupport(true),
	_ipeSupportSetting(CEcfg::instance()->getOrAddBool(cfgKey("supportIPE"), _ipeSupport)),
	_multiDataField(true),
	_multiDataFieldSetting(CEcfg::instance()->getOrAddBool(cfgKey("multiDataField"), _multiDataField)),
	_allowPacketsAfterFill(true),
	_allowPacketsAfterFillSetting(CEcfg::instance()->getOrAddBool(cfgKey("allowPacketsAfterFill"), _allowPacketsAfterFill)),
	_badPacketTally(0) {

	_ipeSupport = _ipeSupportSetting;
	_multiDataField = _multiDataFieldSetting;
	_allowPacketsAfterFill = _allowPacketsAfterFillSetting;
}

modTM_VC_Rcv_Packet::~modTM_VC_Rcv_Packet() {
	MOD_DEBUG("Running ~modTM_VC_Rcv_Packet().");
}

int modTM_VC_Rcv_Packet::svc() {
	svcStart_();

	TM_Transfer_Frame* frame = 0;

	while ( continueService() ) {
		ndSafeRelease(frame);
		bool frameIsValid = true;

		std::pair<NetworkData*, int> queueTop = getData_();

		if ( msg_queue()->deactivated() ) break;

		if ( queueTop.second < 0 ) {
			MOD_ERROR("getData_() call failed.");
			continue;
		}
		else if ( ! queueTop.first ) {
			MOD_ERROR("getData_() returned with null data.");
			continue;
		}

		frame = dynamic_cast<TM_Transfer_Frame*>(queueTop.first);

		if ( !frame ) {
			MOD_ERROR("Received %d-octet buffer in %s wrapper (not TM_Transfer_Frame)! Must discard.",
				queueTop.first->getUnitLength(), queueTop.first->typeStr().c_str());
			ndSafeRelease(queueTop.first);
			continue;
		}

		frame->useOperationalControl(getUseOperationalControl());
		frame->setSecondaryHeaderLen(getFSHSize());
		frame->useFrameErrorControl(getUseFrameErrorControl());

		queueTop.first = 0;

		MOD_DEBUG("Received %d octet frame in VC# %d.", frame->getUnitLength(), getVCID());

		// Test expected values.
		if (frame->getTotalUnitLength() != getFrameSizeU()) {
			MOD_WARNING("TM Transfer Frame bad length: received %d octets, needed %d.", frame->getUnitLength(), getFrameSize());
			incBadLengthCount();
			frameIsValid = false;
			if ( getDropBadFrames() ) continue;
		}

		if ( ! validateFrame(frame) ) {
			frameIsValid = false;
			if ( getDropBadFrames() ) continue;
		}

		if ( frame->getMCID() != getMCID() ) {
			MOD_WARNING("Received Frame with wrong MCID, 0x%X instead of 0x%X.", frame->getMCID(), getMCID());
			incBadMCIDCount();
			frameIsValid = false;
			if ( getDropBadFrames() ) continue;
		}

		if ( frame->getVCID() != getVCID() ) {
			MOD_WARNING("Received Frame with wrong VCID, 0x%X instead of 0x%X.", frame->getVCID(), getVCID());
			incBadVCIDCount();
			frameIsValid = false;
			if ( getDropBadFrames() ) continue;
		}

		if ( frame->onlyIdleData() ) continue;

		// If we get an incorrect frame count here, reset the frame count and keep going.
		if (frame->getVCFrameCount() != ( getCurrentFrameNumber()) ) {
			incFrameCountMisses();
			MOD_WARNING("Bad VC frame count: Got %d, expected %d.", frame->getVCFrameCount(),
				getCurrentFrameNumber());
			setCurrentFrameNumber(frame->getVCFrameCount());

			// Get rid of the current packet if one exists because it can't be valid now.
			ndSafeRelease(_packet);
		}

		if (frameIsValid) incValidFrameCount();
		incCurrentFrameNumber(TM_Transfer_Frame::maxVCFrameCount);

		MOD_DEBUG("Received a %d-octest TM Transfer Frame", frame->getUnitLength());

		if ( frame->getSynchronizationFlag() ) {
			MOD_WARNING("Synchronization flag is set indicating a VCA_SDU, cannot process as a packet. Dropping data.");
		}
		else {
			_assemblePacket(frame);
			frame = 0; // important
		}
	}

	return svcEnd_();
}

void modTM_VC_Rcv_Packet::_send(NetworkData* data) {
	HandlerLinkMap::iterator outputLink;
	ACE_UINT8 pvn = getPVN(data->ptrUnit());

	if ( ( outputLink = primaryOutputLinks_.find(pvn)) != primaryOutputLinks_.end() ) {
		MOD_DEBUG("Sending %d octets to %s.", data->getTotalUnitLength(), outputLink->second->getTarget()->getName().c_str());
		outputLink->second->send(data);
	}
	else {
		MOD_NOTICE("No output target defined for PVN %d, dropping data.", pvn);

		ndSafeRelease(data);
	}
}

void modTM_VC_Rcv_Packet::_assemblePacket(TM_Transfer_Frame* frame) {
	ACE_UINT8* pktPtr = 0;
	size_t headerPointer = 0;
	bool morePacketsComing = true;

	while ( morePacketsComing && headerPointer < frame->getDataLength() ) {
		MOD_DEBUG("Iterating over Data Field from index %d.", headerPointer);
		pktPtr = (_packet) ? _packet->ptrUnit() : frame->wrapInnerPDU<NetworkData>()->ptrUnit() + headerPointer;

		int pvn = getPVN(pktPtr);
		MOD_DEBUG("Packet Version Number is %d.", pvn);

		switch ( pvn ) {
			case pvnSpacePacket:
				morePacketsComing = _assembleSpacePkt(frame, headerPointer);
				break;
			case pvnSCPS_NP:
				morePacketsComing = _assembleSCPS_NP(frame, headerPointer);
				break;
			case pvnEncapsulationPacket:
				morePacketsComing = _assembleEncap(frame, headerPointer);
				break;
			default:
				MOD_WARNING("Unknown CCSDS Packet Version Number %d found, discarding remainder of frame.\n", pvn );
				return;
		}
		/*
		if ( headerPointer == 0 ) {
			MOD_WARNING("modTM_VC_Rcv_Packet::_assemble_packet: No operation performed, discarding remainder of frame.\n");
			return;
		}
		*/
	}
}

bool modTM_VC_Rcv_Packet::_assembleSpacePkt(TM_Transfer_Frame* frame, size_t& idx) {

	if (frame->onlyIdleData()) return false;

	return ( _packet )? _continueSpacePkt(frame, idx) : _startNewSpacePkt(frame, idx);
}

bool modTM_VC_Rcv_Packet::_startNewSpacePkt(TM_Transfer_Frame* frame, size_t& idx) {
	if ( idx == 0 && frame->getFirstHeaderPointer() > 0) {

		MOD_NOTICE("Continued packet data in Data Field with nothing to append to! Ignoring.");
		if (! frame->noPacketStart()) { // Nothing else to process, get out.
			idx += frame->getFirstHeaderPointer(); // Just ignore the first chunk and move on.
		}
		return getMultiDataField();
	}

	size_t remainingOctets = frame->getDataLength() - idx;
	ACE_UINT8 firstOctet = *(frame->ptrData() + idx);
	size_t minOctets = SpacePacket::minBytesToDetermineLength(firstOctet);

	MOD_DEBUG("Starting fresh Space Packet, %d of %d octets remain in the Data Field.",
		remainingOctets, frame->getDataLength());

	if ( minOctets > remainingOctets ) {
		// We're at the end of the data field and there's not enough here
		// to figure out if there's a complete packet, so copy what's left
		// and get out.
		_packet = new SpacePacket(remainingOctets, frame->ptrData() + idx);
		return false;
	}
	else {
		_packet = new SpacePacket(minOctets, frame->ptrData() + idx);
	}

	idx += minOctets;
	remainingOctets -= minOctets;

	// Decide whether to use all remaining data or just part.
	size_t expectedLen = dynamic_cast<SpacePacket*>(_packet)->expectedLength();
	size_t dataLen = (expectedLen - minOctets >= remainingOctets)? remainingOctets : expectedLen - minOctets;

	_packet->append(frame->ptrData() + idx, dataLen);
	idx += dataLen;

	MOD_DEBUG("Space Packet front end has %d octets; needs %d more (want %d total).", _packet->getTotalUnitLength(),
		expectedLen - _packet->getTotalUnitLength(), expectedLen);

	// The expected length matches the collected data length. All's well as far as we know.
	if (_packet->hasAllParts()) {
		_packet->flatten();

		if (dynamic_cast<SpacePacket*>(_packet)->isIdle()) {
			MOD_DEBUG("Completed an Idle Space Packet.");
			ndSafeRelease(_packet);
		}
		else {
			MOD_DEBUG("Completed a Space Packet.");
			_send(_packet);
		}

		_packet = 0;
	}

	return getMultiDataField();
}

bool modTM_VC_Rcv_Packet::_continueSpacePkt(TM_Transfer_Frame* frame, size_t& idx) {
	// Get the tail of the last truncated packet if there is one.
	// Data field is already allocated.

	// Should be part of the current packet. It's possible, if a frame was
	// lost, that this is a continuation of a different packet.

	// Check length here as an initial (weak) test, check CRC later.
	// Should be able to tell in advance from the Transfer Frame if
	// a packet was lost/corrupted.
	MOD_DEBUG("Attempting to continue packet from earlier TM Transfer Frame Data Field.");

	ACE_UINT8 firstOctet = *(_packet->ptrUnit());

	if ( idx == 0 && frame->getFirstHeaderPointer() > 0) {
		MOD_DEBUG("Found continued data at the start of the Data Field.");
		size_t dataLen;
		bool foundTail = false;

		if (frame->noPacketStart()) {
			// There are no additional packets in this Data Field.
			MOD_DEBUG("Entire Data Field continues the Encapsulation Packet.");
			dataLen = frame->getDataLength();
		}
		else {
			// There are more packets inside, but the tail of this one is here.
			MOD_DEBUG("Front part of Data Field continues the Space Packet.");
			dataLen = frame->getFirstHeaderPointer();
			foundTail = true;
		}

		idx += dataLen;

		if (_packet->getUnitLength() >= SpacePacket::minBytesToDetermineLength(firstOctet)) {
			// If we have more than a few bytes, add the new part to our message list
			MOD_DEBUG("Appending data to list.");
			_packet->tail()->setNextPart(frame->wrapInnerPDU<NetworkData>(dataLen, frame->ptrData()));
		}
		else {
			// Otherwise append to the head buffer instead of to the list.
			MOD_DEBUG("Appending data to buffer (current size %d).", _packet->getUnitLength());
			_packet->append(frame->ptrData(), dataLen);
		}

		if (dynamic_cast<SpacePacket*>(_packet)->hasAllParts()) {
			_packet->flatten();

			if (dynamic_cast<SpacePacket*>(_packet)->isIdle()) {
				MOD_DEBUG("Found the end of an Idle Space Packet.");
				ndSafeRelease(_packet);
			}
			else {
				MOD_DEBUG("Found the end of a split Space Packet.");
				_send(_packet);
			}

			_packet = 0;
		}
		else if ( foundTail ) {
			// Found the end, but there's a length mismatch. Dump the bad packet,
			// but keep processing the remaining data.
			MOD_NOTICE("Space Packet length mismatch (need %d, has %d)! Dropping.",
				_packet->expectedLength(), _packet->getTotalUnitLength());
			incBadPacketTally();
			ndSafeRelease(_packet);
		}
		else if ( _packet->getUnitLength() >= SpacePacket::minBytesToDetermineLength(firstOctet) &&
			_packet->getTotalUnitLength() > _packet->expectedLength() ) {
			// Did NOT find the end, but the packet is already too long.
			MOD_NOTICE("Space Packet has too much data (need %d, have %d)! Dropping.",
				_packet->expectedLength(), _packet->getTotalUnitLength());
			incBadPacketTally();
			ndSafeRelease(_packet);
			return false;
		}
	}
	else {
		MOD_NOTICE("Lost the tail of the previous Space Packet! Dropping the head portion.");
		incBadPacketTally();
		ndSafeRelease(_packet);
		return true; // Report more packets coming because nothing was done.
	}

	return getMultiDataField();
}

bool modTM_VC_Rcv_Packet::_assembleSCPS_NP(TM_Transfer_Frame* frame, size_t& idx) {
	MOD_WARNING("Encountered an unsupported SCPS-NP Packet. Dropping.");
	return false;
}

bool modTM_VC_Rcv_Packet::_assembleEncap(TM_Transfer_Frame* frame, size_t& idx) {

	if (frame->onlyIdleData()) return false;

	return ( _packet )? _continueEncap(frame, idx) : _startNewEncap(frame, idx);
}

bool modTM_VC_Rcv_Packet::_startNewEncap(TM_Transfer_Frame* frame, size_t& idx) {
	if ( idx == 0 && frame->getFirstHeaderPointer() > 0) {

		MOD_NOTICE("Continued packet data in Data Field with nothing to append to! Ignoring.");
		if (! frame->noPacketStart()) { // Nothing else to process, get out.
			idx += frame->getFirstHeaderPointer(); // Just ignore the first chunk and move on.
		}
		return getMultiDataField();
	}

	size_t remainingOctets = frame->getDataLength() - idx;
	ACE_UINT8 firstOctet = *(frame->ptrData() + idx);
	size_t minOctets = EncapsulationPacketBase::minBytesToDetermineLength(firstOctet);

	// Check for a one-byte "fill" packet.
	if (minOctets == 1 && EncapsulationPacketBase::isFill(firstOctet) ) {
		MOD_DEBUG("Ignoring fill.");

		if (getAllowPacketsAfterFill() ) {
			++idx;
			return getMultiDataField();
		}
		else {
			return false; // Assumes only fill remains in Data Field
		}
	}

	MOD_DEBUG("Starting fresh Encapsulation Packet, %d of %d octets remain in the Data Field.",
		remainingOctets, frame->getDataLength());

	if ( minOctets > remainingOctets ) {
		// We're at the end of the data field and there's not enough here
		// to figure out if there's a complete packet, so copy what's left
		// and get out.
		if (getIpeSupport())
			_packet = new EncapsulationPacketWithIPE(remainingOctets, frame->ptrData() + idx);
		else
			_packet = new EncapsulationPacketPreIPE(remainingOctets, frame->ptrData() + idx);

		return false;
	}
	else {
		if (getIpeSupport())
			_packet = new EncapsulationPacketWithIPE(minOctets, frame->ptrData() + idx);
		else
			_packet = new EncapsulationPacketPreIPE(minOctets, frame->ptrData() + idx);
	}

	idx += minOctets;
	remainingOctets -= minOctets;

	// Decide whether to use all remaining data or just part.
	size_t expectedLen = dynamic_cast<EncapsulationPacketBase*>(_packet)->expectedLength();
	size_t dataLen = (expectedLen - minOctets >= remainingOctets)? remainingOctets : expectedLen - minOctets;

	_packet->append(frame->ptrData() + idx, dataLen);
	idx += dataLen;

	MOD_DEBUG("Encapsulation Packet front end has %d octets; needs %d more (want %d total).", _packet->getTotalUnitLength(),
		expectedLen - _packet->getTotalUnitLength(), expectedLen);

	if (_packet->hasAllParts()) {
		MOD_DEBUG("Completed an Encapsulation Packet.");

		// The expected length matches the collected data length. All's well as far as we know.
		_packet->flatten();
		_send(_packet);
		_packet = 0;
	}

	return getMultiDataField();
}

bool modTM_VC_Rcv_Packet::_continueEncap(TM_Transfer_Frame* frame, size_t& idx) {
	// Get the tail of the last truncated packet if there is one.
	// Data field is already allocated.

	// Should be part of the current packet. It's possible, if a frame was
	// lost, that this is a continuation of a different packet.

	// Check length here as an initial (weak) test, check CRC later.
	// Should be able to tell in advance from the Transfer Frame if
	// a packet was lost/corrupted.
	MOD_DEBUG("Attempting to continue packet from earlier TM Transfer Frame Data Field.");

	ACE_UINT8 firstOctet = *(_packet->ptrUnit());

	if ( idx == 0 && frame->getFirstHeaderPointer() > 0) {
		MOD_DEBUG("Found continued data at the start of the Data Field.");
		size_t dataLen;
		bool foundTail = false;

		if (frame->noPacketStart()) {
			// There are no additional packets in this Data Field.
			MOD_DEBUG("Entire Data Field continues the Encapsulation Packet.");
			dataLen = frame->getDataLength();
		}
		else {
			// There are more packets inside, but the tail of this one is here.
			MOD_DEBUG("Front part of Data Field continues the Encapsulation Packet.");
			dataLen = frame->getFirstHeaderPointer();
			foundTail = true;
		}

		idx += dataLen;

		if (_packet->getUnitLength() >= EncapsulationPacketBase::minBytesToDetermineLength(firstOctet)) {
			// If we have more than a few bytes, add the new part to our message list
			MOD_DEBUG("Appending data to list.");
			_packet->tail()->setNextPart(frame->wrapInnerPDU<NetworkData>(dataLen, frame->ptrData()));
		}
		else {
			// Otherwise append to the head buffer instead of to the list.
			MOD_DEBUG("Appending data to buffer (current size %d).", _packet->getUnitLength());
			_packet->append(frame->ptrData(), dataLen);
		}

		if (dynamic_cast<EncapsulationPacketBase*>(_packet)->hasAllParts()) {
			// The expected length matches the collected data length.
			// All's well as far as we know.
			MOD_DEBUG("Found the end of a split Encapsulation Packet.");
			_packet->flatten();
			_send(_packet);
			_packet = 0;
		}
		else if ( foundTail ) {
			// Found the end, but there's a length mismatch. Dump the bad packet,
			// but keep processing the remaining data.
			MOD_NOTICE("Encapsulation Packet length mismatch (need %d, has %d)! Dropping.", _packet->expectedLength(), _packet->getTotalUnitLength());
			incBadPacketTally();
			ndSafeRelease(_packet);
		}
		else if ( _packet->getUnitLength() >= EncapsulationPacketBase::minBytesToDetermineLength(firstOctet) &&
			_packet->getTotalUnitLength() > _packet->expectedLength() ) {
			// Did NOT find the end, but the packet is already too long.
			MOD_NOTICE("Encapsulation Packet has too much data (need %d, have %d)! Dropping.", _packet->expectedLength(), _packet->getTotalUnitLength());
			incBadPacketTally();
			ndSafeRelease(_packet);
			return false;
		}
	}
	else {
		MOD_NOTICE("Lost the tail of the previous Encapsulation Packet! Dropping the head portion.");
		incBadPacketTally();
		ndSafeRelease(_packet);
		return true; // Report more packets coming because nothing was done.
	}

	return getMultiDataField();
}

HandlerLink* modTM_VC_Rcv_Packet::getLink(const LinkType ltype, const ACE_UINT8 pvn /* =0 */) {
	if ( ltype != BaseTrafficHandler::PrimaryOutputLink )
		return BaseTrafficHandler::getLink(ltype);

	HandlerLinkMap::iterator outputLink;
	if ( ( outputLink = primaryOutputLinks_.find(pvn)) != primaryOutputLinks_.end() ) {
		return outputLink->second;
	}
	else return 0;
}

}
