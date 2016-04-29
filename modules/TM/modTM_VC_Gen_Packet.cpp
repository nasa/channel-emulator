/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modTM_VC_Gen_Packet.cpp
 * @author Tad Kollar  
 *
 * $Id: modTM_VC_Gen_Packet.cpp 2344 2013-08-30 20:24:16Z tkollar $
 * Copyright (c) 2012.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "modTM_VC_Gen_Packet.hpp"
#include "SpacePacket.hpp"

using namespace nasaCE;
using namespace nTM_VC_Gen_Packet;

namespace nTM_VC_Gen_Packet {

modTM_VC_Gen_Packet::modTM_VC_Gen_Packet(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr):
	TM_Mux_Base(newName, newChannelName, newDLLPtr),
	_maxUsecsForNextPacket(1000),
	_maxUsecsForNextPacketSetting(CEcfg::instance()->getOrAddInt(cfgKey("maxUsecsForNextPacket"), _maxUsecsForNextPacket)),
	_fillPattern(0),
	_fillPatternSetting(CEcfg::instance()->getOrAddArray(cfgKey("fillPattern"))),
	_multiPacketDataField(true),
	_multiPacketDataFieldSetting(CEcfg::instance()->getOrAddBool(cfgKey("multiPacketDataField"), _multiPacketDataField)),
	_unfilledFrame(0),
	_unfilledFrameIdx(0),
	_firstHeaderIndexSet(false) {

	ACE_Time_Value oneMillisec(0, 1000);

	_maxUsecsForNextPacket = static_cast<int>(_maxUsecsForNextPacketSetting);

	if ( _fillPatternSetting.getLength() == 0 ) {
		_fillPattern = new NetworkData(1);
		*(_fillPattern->ptrUnit()) = 0xE0;
		_fillPatternSetting.add(Setting::TypeInt);
		_fillPatternSetting[0] = 0xE0;
	}
	else {
		_fillPattern = new NetworkData(_fillPatternSetting.getLength());
		MOD_DEBUG("Setting up %d-octet fill pattern.", _fillPattern->getUnitLength());

		for ( int i = 0; i < _fillPatternSetting.getLength(); ++i )
			*(_fillPattern->ptrUnit() + i) = static_cast<int>(_fillPatternSetting[i]) & 0xff;
	}

	_multiPacketDataField = _multiPacketDataFieldSetting;

	rebuildIdleUnitTemplate_();
}

modTM_VC_Gen_Packet::~modTM_VC_Gen_Packet() {
	MOD_DEBUG("Running ~modTM_VC_Gen_Packet().");
}

int modTM_VC_Gen_Packet::svc() {
	svcStart_();


	while ( continueService() ) {
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

		NetworkData* data = queueTop.first;
		queueTop.first = 0;

		MOD_DEBUG("Received %d octets to frame into TM Virtual Channel with VCID %X.", data->getUnitLength(), getVCID());

		_processData(data); // Deletion of received data is performed in _processData.

		MOD_DEBUG("Finished processing incoming packets.");
	}

	return svcEnd_();
}

size_t modTM_VC_Gen_Packet::getMTU() const { return getFrameSize(); }

void modTM_VC_Gen_Packet::setMTU(const size_t newVal) { setFrameSize(newVal); }

size_t modTM_VC_Gen_Packet::getDataFieldLength() const {
	return getFrameSize() - TM_Transfer_Frame::getNonDataLength(getUseOperationalControl(),
		getUseFrameErrorControl(),
		getFSHSize());
}

int modTM_VC_Gen_Packet::getMaxUsecsForNextPacket() const {
	return _maxUsecsForNextPacket;
}

void modTM_VC_Gen_Packet::setMaxUsecsForNextPacket(const int& newVal) {
	_maxUsecsForNextPacket = newVal;
	_maxUsecsForNextPacketSetting = _maxUsecsForNextPacket;
}

ACE_Time_Value modTM_VC_Gen_Packet::getWaitForNextPacket() const {
	ACE_Time_Value timeWait;
	timeWait.sec(static_cast<int>(_maxUsecsForNextPacket)/1000000);
	timeWait.usec(static_cast<int>(_maxUsecsForNextPacket)%1000000);
	return timeWait;
}

void modTM_VC_Gen_Packet::setWaitForNextPacket(ACE_Time_Value& timeWait) {
	ACE_UINT64 totalUsecs;
	timeWait.to_usec(totalUsecs);

	_maxUsecsForNextPacket = static_cast<int>(totalUsecs);
	_maxUsecsForNextPacketSetting = _maxUsecsForNextPacket;
}

bool modTM_VC_Gen_Packet::_send(TM_Transfer_Frame* frame) {
	if ( ! frame ) {
		MOD_WARNING("_send() received a null pointer.");
		return false;
	}

	if ( links_[PrimaryOutputLink] ) {
		incValidFrameCount();
		incCurrentFrameNumber(TM_Transfer_Frame::maxVCFrameCount);

		MOD_DEBUG("Sending %d-octet TM Transfer Frame.", frame->getUnitLength());
		links_[PrimaryOutputLink]->send(frame);
	}
	else {
		MOD_NOTICE("No output target defined yet, dropping data.");
		return false;
	}

	return true;
}

void modTM_VC_Gen_Packet::_processData(NetworkData* netdata) {
	TM_Transfer_Frame* frame = 0;
	ACE_Message_Block* mb = 0;

	size_t frameIdx = 0;
	size_t netdataIdx = 0;
	size_t dataFieldLen = getDataFieldLength();
	int frameCount = 0;

	MOD_DEBUG("Distributing packets into TM Transfer Frames.");

	if (_unfilledFrame) {
		MOD_DEBUG("Starting with unfilled TM Transfer Frame (index %d).", _unfilledFrameIdx);
		frame = _unfilledFrame;
		frameIdx = _unfilledFrameIdx;
		++frameCount;

		_unfilledFrame = 0;
		_unfilledFrameIdx = 0;
	}

	while (netdata) {
		if (! frame) {
			frame = new TM_Transfer_Frame(
				getFrameSize(), // all frames in VC have the same size
				0, // we have the contents the data zone, but no TF header/trailer yet
				getUseOperationalControl(), // existance of OCF is VC dependent
				getUseFrameErrorControl(), // all frames in PC have frame CRC, or not
				getFSHSize() // length of TF frame secondary header
			);

			if ( ! frame ) {
				throw nd_error("Failed to allocate a TM_Transfer_Frame object!");
			}

			frame->initialize(getSCID(), getVCID(), getDataFieldLength(), 0, getCurrentFrameNumber());
			_firstHeaderIndexSet = false;
			frameIdx = 0;
			++frameCount;
		}

		// Figure out how much to take out of the current packet.
		size_t dataLen = 0;
		if ( netdata->getUnitLength() - netdataIdx <= dataFieldLen - frameIdx ) {
			// Copy everything that's left.
			dataLen = netdata->getUnitLength() - netdataIdx;
		}
		else {
			// Copy only as much as the Data Field has room for.
			dataLen = dataFieldLen - frameIdx;
		}

		try {
			frame->copyUnit(frame->ptrData() + frameIdx, netdata->ptrUnit() + netdataIdx, dataLen);
		}
		catch (const BufferOverflow& e) {
			MOD_DEBUG("Not enough room in target buffer (Target size: %d, Source size: %d)",
				frame->getDataLength() - frameIdx, dataLen);
			ndSafeRelease(netdata);
			ndSafeRelease(frame);
			return;
		}

		// If we just copied the middle of a packet and it takes up the whole
		// Data Field, mark it as headerless.
		if ( netdataIdx > 0 && (dataLen == dataFieldLen) ) {
			frame->setNoPacketStart();
		}
		// Otherwise if we just copied the start of a packet, and it's the
		// first one in the Data Field, set the first header index field.
		else if ( netdataIdx == 0 && ! _firstHeaderIndexSet ) {
			frame->setFirstHeaderPointer(frameIdx);
			_firstHeaderIndexSet = true;
		}

		frameIdx += dataLen;
		netdataIdx += dataLen;

		if (frameIdx == dataFieldLen) {
			// If these values are equal, make a new frame. If frameIdx is greater, we added wrong.
			_send(frame);
			frame = 0;
		}
		else if (frameIdx > dataFieldLen) throw LogicError("frameIdx > dataFieldLen");

		if ( netdataIdx == netdata->getUnitLength() ) {
			NetworkData* cur = netdata;
			// If these values are equal, go to the next packet. If netdataIdx is greater, we added wrong.
			netdata = dynamic_cast<NetworkData*>(netdata->getNextPart());
			ndSafeRelease(cur);
			netdataIdx = 0;

		}
		else if (netdataIdx > netdata->getUnitLength()) {
			MOD_DEBUG("netdataIdx == %d, netdata->getUnitLength == %d, dataLen == %d", netdataIdx, netdata->getUnitLength(), dataLen);
			throw LogicError("netdataIdx > netdata->getUnitLength()");
		}
	}

	// Handle processing and sending of the final frame, if not filled.
	if ( frame && frameCount ) {
		TM_Transfer_Frame* lastFrame = frame;

		ACE_Time_Value timeToWaitUntil(ACE_High_Res_Timer::gettimeofday_hr() + getWaitForNextPacket());
		_unfilledFrame = lastFrame;
		_unfilledFrameIdx = frameIdx;
		--frameCount;

		// If multiple packets in the Data Field are disallowed, or there's nothing left in the
		// message queue, fill with an Idle Packet.
		if ( ! _multiPacketDataField || msg_queue()->peek_dequeue_head(mb, &timeToWaitUntil) == -1 ) {

			// Now that we're out of packets to add, pad the last Data Field with an Idle Packet.
			if ( frameIdx < dataFieldLen) {
				if ( !lastFrame ) throw nd_error("Got a null packet for the final Transfer Frame!\n");

				const size_t emptyLen = dataFieldLen - frameIdx;
				const size_t idlePktLen = (emptyLen > 7)? emptyLen : 7;

				NetworkData* fill;

				// This is necessary because otherwise it could go into an inifinite recursive loop.
				if ( _unfilledFrame->getDataLength() < 14 ) {
					MOD_WARNING("Very small Data Field length (%d) detected! Filling with idle pattern instead of Idle Packet.",
						_unfilledFrame->getDataLength());

					fill = new NetworkData(emptyLen);
					_fillPatternLock.acquire();
					fill->setDataToPattern(_fillPattern);
					_fillPatternLock.release();
				}
				else {
					MOD_DEBUG("Adding a %d-octet Idle Packet at index %d.", idlePktLen, frameIdx);

					fill = new SpacePacket(idlePktLen);

					_fillPatternLock.acquire();
					dynamic_cast<SpacePacket*>(fill)->makeIdle(_fillPattern);
					_fillPatternLock.release();
				}

				_processData(fill);
			}
		}
		else {
			MOD_DEBUG("Receiving more traffic to fill last TM Transfer Frame (index %d).", frameIdx);
		}
	}

	MOD_DEBUG("Generated %d complete TM Transfer Frames and %d unfilled.",
		frameCount, (_unfilledFrame? 1 : 0));
}

void modTM_VC_Gen_Packet::setFillPattern(const NetworkData* fill) {
	_fillPatternLock.acquire();

	_fillPattern->setUnitLength(fill->getUnitLength());
	_fillPattern->copyUnit(0, fill);

	_fillPatternLock.release();

	for (unsigned i = 0; i < fill->getUnitLength(); ++i ) {
		// Add slots when needed
		if ( static_cast<unsigned>(_fillPatternSetting.getLength()) < i + 1 )
			_fillPatternSetting.add(Setting::TypeInt);
		_fillPatternSetting[i] = *(fill->ptrUnit() + i);
	}

	// Eliminate extra slots
	while (static_cast<unsigned>(_fillPatternSetting.getLength()) > fill->getUnitLength())
		_fillPatternSetting.remove(fill->getUnitLength());
}

void modTM_VC_Gen_Packet::setFillPattern(const std::vector<uint8_t>& fill) {

	_fillPattern->setUnitLength(fill.size());

	for (unsigned i = 0; i < fill.size(); ++i ) {
		// Add slots when needed
		if ( static_cast<unsigned>(_fillPatternSetting.getLength()) < i + 1 )
			_fillPatternSetting.add(Setting::TypeInt);

		*(_fillPattern->ptrUnit() + i) = fill[i];
		_fillPatternSetting[i] = fill[i];
	}

	_fillPatternLock.release();

	// Eliminate extra slots
	while (static_cast<unsigned>(_fillPatternSetting.getLength()) > fill.size())
		_fillPatternSetting.remove(fill.size());
}

void modTM_VC_Gen_Packet::getFillPattern(std::vector<uint8_t>& fillPattern) const {
	fillPattern.clear();

	for ( unsigned i = 0; i < _fillPattern->getUnitLength(); ++i )
		fillPattern.push_back(static_cast<int>(*(_fillPattern->ptrUnit() + i)));
}

void modTM_VC_Gen_Packet::setMultiPacketDataField(bool newVal) {
	_multiPacketDataField = newVal; _multiPacketDataFieldSetting = newVal;
}

bool modTM_VC_Gen_Packet::getMultiPacketDataField() const { return _multiPacketDataField; }


ACE_UINT8 modTM_VC_Gen_Packet::getIdentifier_(ACE_Message_Block* mb) const {
	return (*(mb->rd_ptr()) >> 5);
}

}
