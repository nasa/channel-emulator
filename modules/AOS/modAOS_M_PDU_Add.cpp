/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modAOS_M_PDU_Add.cpp
 * @author Tad Kollar  
 *
 * $Id: modAOS_M_PDU_Add.cpp 2344 2013-08-30 20:24:16Z tkollar $
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "modAOS_M_PDU_Add.hpp"

using namespace nasaCE;

namespace nAOS_M_PDU_Add {

modAOS_M_PDU_Add::modAOS_M_PDU_Add(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr /* = 0 */):
	BaseTrafficHandler(newName, newChannelName, newDLLPtr),
	_maxUsecsForNextPacket(1000),
	_maxUsecsForNextPacketSetting(CEcfg::instance()->getOrAddInt(cfgKey("maxUsecsForNextPacket"), _maxUsecsForNextPacket)),
	_fillPattern(0),
	_fillPatternSetting(CEcfg::instance()->getOrAddArray(cfgKey("fillPattern"))),
	_multiPacketZone(true),
	_multiPacketZoneSetting(CEcfg::instance()->getOrAddBool(cfgKey("multiPacketZone"), _multiPacketZone)),
	_sendImmediately(true),
	_unfilled_mpdu(0),
	_unfilled_mpdu_idx(0),
	_firstHeaderIndexSet(false), _mpduLength(0), _mtuIsManual(false) {

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
		for ( int i = 0; i < _fillPatternSetting.getLength(); ++i )
			*(_fillPattern->ptrUnit() + i) = static_cast<int>(_fillPatternSetting[i]);
	}

	_multiPacketZone = _multiPacketZoneSetting;
}

modAOS_M_PDU_Add::~modAOS_M_PDU_Add() {
	MOD_DEBUG("Running ~modAOS_M_PDU_Add().");
}

int modAOS_M_PDU_Add::svc() {
	svcStart_();

	if ( ! _mpduLength) {
		if ( getMTU() > 0 ) {
			_mpduLength = getMTU();
			_mtuIsManual = true;
		}
		else if (links_[PrimaryOutputLink] ) {
			MOD_INFO("M_PDU length is unset, asking output target %s for MRU.", links_[PrimaryOutputLink]->getTarget()->getName().c_str());
			_mpduLength = links_[PrimaryOutputLink]->getTarget()->getMRU();
			_mtuIsManual = false;
		}
	}

	if ( _mpduLength ) {
		while ( continueService() ) {
			std::pair<NetworkData*, int> queueTop = getData_();

			// Check every 11th unit for a new MTU
			if ( ! _mtuIsManual && getReceivedUnitCount() % 11 == 0 && links_[PrimaryOutputLink] ) {
				_mpduLength = links_[PrimaryOutputLink]->getTarget()->getMRU();
			}

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

			MOD_DEBUG("Received %d bytes to convert into AOS M_PDUs.", data->getUnitLength());

			AOS_Multiplexing_PDU* mpdu_list = _processData(data);
			// Deletion of received data is performed in _processData.

			if ( ! _sendImmediately ) {
				while (mpdu_list) {
					AOS_Multiplexing_PDU* cur = mpdu_list;
					_send(cur);

					mpdu_list = dynamic_cast<AOS_Multiplexing_PDU*>(mpdu_list->getNextPart());
				}
			}
			MOD_DEBUG("Finished processing incoming packets.");
		}
	}
	else {
		MOD_ERROR("No MTU obtainable, exiting service loop (pausing).");
	}

	return svcEnd_();

}


bool modAOS_M_PDU_Add::_send(AOS_Multiplexing_PDU* mpdu) {
	if ( ! mpdu ) {
		MOD_WARNING("_send() received a null pointer.");
		return false;
	}

	if ( links_[PrimaryOutputLink] ) {
		MOD_DEBUG("Sending %d-octet M_PDU.", mpdu->getUnitLength());
		links_[PrimaryOutputLink]->send(mpdu);
	}
	else {
		MOD_NOTICE("No output target defined yet, dropping data.");
		return false;
	}

	return true;
}

AOS_Multiplexing_PDU* modAOS_M_PDU_Add::_processData(NetworkData* netdata) {
	AOS_Multiplexing_PDU* mpdu_list = 0;
	AOS_Multiplexing_PDU* mpdu = 0;
	ACE_Message_Block* mb = 0;

	size_t mpdu_idx = 0;
	size_t netdata_idx = 0;
	size_t packet_zone_len = _getPacketZoneLength();
	int mpdu_count = 0;

	MOD_DEBUG("Converting NetworkData objects to M_PDUs.");

	if (_unfilled_mpdu) {
		MOD_DEBUG("Starting with unfilled M_PDU (index %d).", _unfilled_mpdu_idx);
		mpdu = _unfilled_mpdu;
		mpdu_idx = _unfilled_mpdu_idx;
		++mpdu_count;

		_unfilled_mpdu = 0;
		_unfilled_mpdu_idx = 0;
	}

	while (netdata) {
		if (! mpdu) {

			if ( ! get_M_PDU_Length() ) {
				MOD_ERROR("The M_PDU length is unset, cannot continue.");
				return 0;
			}

			mpdu = new AOS_Multiplexing_PDU(get_M_PDU_Length());
			_firstHeaderIndexSet = false;
			mpdu_idx = 0;
			++mpdu_count;
		}

		// Figure out how much to take out of the current packet.
		size_t dataLen = 0;
		if ( netdata->getUnitLength() - netdata_idx <= packet_zone_len - mpdu_idx ) {
			// Copy everything that's left.
			dataLen = netdata->getUnitLength() - netdata_idx;
		}
		else {
			// Copy only as much as the Packet Zone has room for.
			dataLen = packet_zone_len - mpdu_idx;
		}

		try {
			mpdu->copyUnit(mpdu->ptrData() + mpdu_idx, netdata->ptrUnit() + netdata_idx, dataLen);
		}
		catch (const BufferOverflow& e) {
			MOD_DEBUG("Not enough room in target buffer (Target size: %d, Source size: %d)",
				mpdu->getDataLength() - mpdu_idx, dataLen);
			ndSafeRelease(netdata);
			ndSafeRelease(mpdu);
			return 0;
		}

		// If we just copied the middle of a packet and it takes up the whole
		// packet zone, mark it as headerless.
		if ( netdata_idx > 0 && (dataLen == packet_zone_len) ) {
			mpdu->setFirstHeaderIndex(AOS_Multiplexing_PDU::HasHeaderlessVal);
		}
		// Otherwise if we just copied the start of a packet, and it's the
		// first one in the M_PDU, set the first header index field.
		else if ( netdata_idx == 0 && ! _firstHeaderIndexSet ) {
			mpdu->setFirstHeaderIndex(mpdu_idx);
			_firstHeaderIndexSet = true;
		}

		mpdu_idx += dataLen;
		netdata_idx += dataLen;

		if (mpdu_idx == packet_zone_len) {
			// If these values are equal, make a new MPDU. If mpdu_idx is greater, we added wrong.
			if ( _sendImmediately ) {
				_send(mpdu);
			}
			else {
				if (mpdu_list) mpdu_list->tail()->setNextPart(mpdu);
				else mpdu_list = mpdu;
			}
			mpdu = 0;
		}
		else if (mpdu_idx > packet_zone_len) throw LogicError("mpdu_idx > packet_zone_len");

		if ( netdata_idx == netdata->getUnitLength() ) {
			NetworkData* cur = netdata;
			// If these values are equal, go to the next packet. If netdata_idx is greater, we added wrong.
			netdata = dynamic_cast<NetworkData*>(netdata->getNextPart());
			ndSafeRelease(cur);
			netdata_idx = 0;

			// Add this last M_PDU to the list.
			if ( !netdata && mpdu && ! _sendImmediately ) {
				if ( ! mpdu_list ) mpdu_list = mpdu; // Only got one short packet.
				else mpdu_list->tail()->setNextPart(mpdu);
			}
		}
		else if (netdata_idx > netdata->getUnitLength()) {
			MOD_DEBUG("netdata_idx == %d, netdata->getUnitLength == %d, dataLen == %d", netdata_idx, netdata->getUnitLength(), dataLen);
			throw LogicError("netdata_idx > netdata->getUnitLength()");
		}
	}

	// Handle processing and sending of the final M_PDU, if not filled.
	if ( (mpdu || mpdu_list) && mpdu_count) {
		AOS_Multiplexing_PDU* last_mpdu;

		if ( ! _sendImmediately ) last_mpdu = ((AOS_Multiplexing_PDU*) mpdu_list->tail());
		else last_mpdu = mpdu;

		ACE_Time_Value timeToWaitUntil(ACE_High_Res_Timer::gettimeofday_hr() + getWaitForNextPacket());

		// If multiple packets in the Packet Zone are disallowed, or we're not sending the M_PDUs in
		// "groups" (_sendImmediately is false), or there's nothing left in the message queue, fill
		// with idle pattern.
		if ( ! _multiPacketZone || msg_queue()->peek_dequeue_head(mb, &timeToWaitUntil) == -1 || ! _sendImmediately ) {

			// Now that we're out of packets to add, pad the last M_PDU with Fill.
			if ( mpdu_idx < packet_zone_len) {
				if ( !last_mpdu ) throw nd_error("Got a null packet for the final M_PDU!\n");

				MOD_DEBUG("Adding %d Fill packets at index %d.",
					packet_zone_len - mpdu_idx, mpdu_idx);

				_fillPatternLock.acquire();
				last_mpdu->addFill(mpdu_idx, _fillPattern);
				_fillPatternLock.release();

				// If the first header index was never set on the last M_PDU, set it to
				// point to the first Fill packet.
				if ( ! _firstHeaderIndexSet ) last_mpdu->setFirstHeaderIndex(mpdu_idx);

				// We should probably never have to fill the M_PDU with 100% idle data,
				// at least within this function.
				if (mpdu && mpdu_idx == 0) {
					MOD_DEBUG("Marking M_PDU as 100% fill.");
					last_mpdu->setFirstHeaderIndex(AOS_Multiplexing_PDU::HasIdleVal);
				}

				if ( _sendImmediately && ! _send(last_mpdu) )
					MOD_WARNING("Unable to send last M_PDU.");

				_firstHeaderIndexSet = false;
			}
		}
		else {
			MOD_DEBUG("Getting more traffic to fill last M_PDU (index %d).", mpdu_idx);
			_unfilled_mpdu = last_mpdu;
			_unfilled_mpdu_idx = mpdu_idx;
			--mpdu_count;
		}
	}

	MOD_DEBUG("Generated %d complete M_PDUs and %d unfilled.",
		mpdu_count, (_unfilled_mpdu? 1 : 0));
	return mpdu_list;
}

void modAOS_M_PDU_Add::connectOutput(BaseTrafficHandler* target,
		const HandlerLink::OutputRank sourceOutputRank /* = HandlerLink::PrimaryOutput */,
		const HandlerLink::InputRank targetInputRank /* = HandlerLink::PrimaryInput */,
		const bool postOpen /* = false */ ) {
	BaseTrafficHandler::connectOutput(target, sourceOutputRank, targetInputRank, postOpen);

	if (! get_M_PDU_Length() && links_[PrimaryOutputLink]) {
		set_M_PDU_Length(links_[PrimaryOutputLink]->getTarget()->getMRU());
	}
}

void modAOS_M_PDU_Add::setFillPattern(const NetworkData* fill) {
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

void modAOS_M_PDU_Add::setFillPattern(const std::vector<uint8_t>& fill) {

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

void modAOS_M_PDU_Add::getFillPattern(std::vector<uint8_t>& fillPattern) const {
	fillPattern.clear();

	for ( unsigned i = 0; i < _fillPattern->getUnitLength(); ++i )
		fillPattern.push_back(static_cast<int>(*(_fillPattern->ptrUnit() + i)));
}

}
