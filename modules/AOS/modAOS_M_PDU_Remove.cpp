/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modAOS_M_PDU_Remove.cpp
 * @author Tad Kollar  
 *
 * $Id: modAOS_M_PDU_Remove.cpp 2344 2013-08-30 20:24:16Z tkollar $
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "modAOS_M_PDU_Remove.hpp"

using namespace nasaCE;

namespace nAOS_M_PDU_Remove {

modAOS_M_PDU_Remove::modAOS_M_PDU_Remove(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr):
	BaseTrafficHandler(newName, newChannelName, newDLLPtr),
	_contained_type(mpduEncapsulationPacket), _encap(0),
	_ipeSupport(true),
	_ipeSupportSetting(CEcfg::instance()->getOrAddBool(cfgKey("supportIPE"), _ipeSupport)),
	_multiPacketZone(true),
	_multiPacketZoneSetting(CEcfg::instance()->getOrAddBool(cfgKey("multiPacketZone"), _multiPacketZone)),
	_allowPacketsAfterFill(true),
	_allowPacketsAfterFillSetting(CEcfg::instance()->getOrAddBool(cfgKey("allowPacketsAfterFill"), _allowPacketsAfterFill)),
	_badEncapTally(0) {

	_ipeSupport = _ipeSupportSetting;
	_multiPacketZone = _multiPacketZoneSetting;
	_allowPacketsAfterFill = _allowPacketsAfterFillSetting;
}

modAOS_M_PDU_Remove::~modAOS_M_PDU_Remove() {
	MOD_DEBUG("Running ~modAOS_M_PDU_Remove().");
}

int modAOS_M_PDU_Remove::svc() {
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

		AOS_Multiplexing_PDU* mpdu = dynamic_cast<AOS_Multiplexing_PDU*>(queueTop.first);
		if ( msg_queue()->deactivated() ) break;

		if ( ! mpdu ) {
			MOD_INFO("Received %d octets of non-M_PDU data! Dropping.", queueTop.first->getUnitLength());
			ndSafeRelease(queueTop.first);
			continue;
		}

		MOD_DEBUG("Received a %d byte AOS M_PDU.", mpdu->getUnitLength());

		switch (_contained_type) {
			case mpduEncapsulationPacket:
				_assemble_encap(mpdu);
				break;
			case mpduSpacePacket:
				MOD_NOTICE("Contained type is a Space Packet which is not implemented.");
				break;
		}

		ndSafeRelease(mpdu);
	}

	return svcEnd_();
}

void modAOS_M_PDU_Remove::_send(NetworkData* data) {
	if ( links_[PrimaryOutputLink] ) {
		MOD_DEBUG("Sending %d octets.", data->getUnitLength());
		links_[PrimaryOutputLink]->send(data);
	}
	else {
		MOD_NOTICE("No output target defined yet, dropping data.");
	}
}

int modAOS_M_PDU_Remove::_assemble_encap(AOS_Multiplexing_PDU* m_pdu) {
	uint64_t idx = 0;
	int count = 0;

	if (m_pdu->isIdle()) return 0;

	while (idx < m_pdu->getDataLength()) {

		if ( ! _encap ) { // Start a new packet
			if ( idx == 0 && m_pdu->hasContinuedData()) {
				MOD_NOTICE("Continued data in M_PDU with nothing to append to! Ignoring.");
				if (m_pdu->hasHeaderlessData()) break; // Nothing else to process, get out.

				idx += m_pdu->getFirstHeaderIndex(); // Just ignore the first chunk and move on.
				continue;
			}

			size_t remainingOctets = m_pdu->getDataLength() - idx;
			ACE_UINT8 firstOctet = *(m_pdu->ptrData() + idx);
			size_t minOctets = EncapsulationPacketBase::minBytesToDetermineLength(firstOctet);

			// Check for a one-byte "fill" packet.
			if (minOctets == 1 && EncapsulationPacketBase::isFill(firstOctet) ) {
				MOD_DEBUG("Ignoring fill.");

				if (getAllowPacketsAfterFill() && getMultiPacketZone()) {
					++idx; continue; // Allows non-fill to follow fill:
				}
				else {
					break; // Assumes only fill remains in Packet Zone
				}
			}

			MOD_DEBUG("Starting fresh packet, %d of %d bytes remaining in Packet Zone.", remainingOctets, m_pdu->getDataLength());

			if ( minOctets > remainingOctets ) {
				// We're at the end of the data field and there's not enough here
				// to figure out if there's a complete packet, so copy what's left
				// and get out.
				if (getIpeSupport())
					_encap = new EncapsulationPacketWithIPE(remainingOctets, m_pdu->ptrData() + idx);
				else
					_encap = new EncapsulationPacketPreIPE(remainingOctets, m_pdu->ptrData() + idx);

				break;
			}
			else {
				if (getIpeSupport())
					_encap = new EncapsulationPacketWithIPE(minOctets, m_pdu->ptrData() + idx);
				else
					_encap = new EncapsulationPacketPreIPE(minOctets, m_pdu->ptrData() + idx);
			}

			idx += minOctets;
			remainingOctets -= minOctets;

			// Decide whether to use all remaining data or just part.
			size_t dataLen = (_encap->expectedLength() - minOctets >= remainingOctets)?
				remainingOctets : _encap->expectedLength() - minOctets;

			_encap->append(m_pdu->ptrData() + idx, dataLen);
			idx += dataLen;

			MOD_DEBUG("Packet front end has %d octets; needs %d more (want %d total).", _encap->getTotalUnitLength(),
				_encap->expectedLength() - _encap->getTotalUnitLength(),
				_encap->expectedLength());

			if (_encap->hasAllParts()) {
				MOD_DEBUG("Completed a packet.");

				// The expected length matches the collected data length.
				// All's well as far as we know.
				_encap->flatten();
				_send(_encap);
				_encap = 0;
				++count;

				// Finish if the mission allows only one whole packet per Packet Zone.
				if (! getMultiPacketZone()) break;
			}
		}
		else {
			// Get the tail of the last truncated packet if there is one.
			// Data field is already allocated.

			// Should be part of the current packet. It's possible, if a frame was
			// lost, that this is a continuation of a different packet.

			// Check length here as an initial (weak) test, check CRC later.
			// Should be able to tell in advance from the AOS Transfer Frame if
			// a packet was lost/corrupted.
			MOD_DEBUG("Continuing packet from earlier M_PDU.");

			ACE_UINT8 firstOctet = *(_encap->ptrUnit());

			if ( idx == 0 && m_pdu->hasContinuedData()) {
				MOD_DEBUG("Found continued data at the start of the Packet Zone.");
				size_t dataLen;
				bool foundTail = false;

				if (m_pdu->hasHeaderlessData()) {
					// There are no additional packets in this M_PDU.
					MOD_DEBUG("Entire Packet Zone continues the packet.");
					dataLen = m_pdu->getDataLength();
				}
				else {
					// There are more packets inside, but the tail of this one is here.
					MOD_DEBUG("Front part of Packet Zone continues the packet.");
					dataLen = m_pdu->getFirstHeaderIndex();
					foundTail = true;
				}

				idx += dataLen;

				if (_encap->getUnitLength() >= EncapsulationPacketBase::minBytesToDetermineLength(firstOctet)) {
					// If we have more than a few bytes, add the new part to our message list
					MOD_DEBUG("Appending data to list.");
					_encap->tail()->setNextPart(m_pdu->wrapInnerPDU<NetworkData>(dataLen, m_pdu->ptrData()));
				}
				else {
					// Otherwise append to the head buffer instead of to the list.
					MOD_DEBUG("Appending data to buffer (current size %d).",
						_encap->getUnitLength());
					_encap->append(m_pdu->ptrData(), dataLen);
				}

				if (_encap->hasAllParts()) {
					// The expected length matches the collected data length.
					// All's well as far as we know.
					MOD_DEBUG("Found the end of a split packet.");
					_encap->flatten();
					_send(_encap);
					_encap = 0;
					++count;
					if (! getMultiPacketZone()) break; // Done because only one packet per Zone allowed
				}
				else if ( foundTail ) {
					// Found the end, but there's a length mismatch. Dump the bad packet,
					// but keep processing the remaining M_PDU.
					MOD_NOTICE("Encapsulation Packet length mismatch (need %d, has %d)! Dropping.", _encap->expectedLength(), _encap->getTotalUnitLength());
					incBadEncapTally();
					ndSafeRelease(_encap);
				}
				else if ( _encap->getUnitLength() >= EncapsulationPacketBase::minBytesToDetermineLength(firstOctet) &&
					_encap->getTotalUnitLength() > _encap->expectedLength() ) {
					// Did NOT find the end, but the packet is already too long.
					MOD_NOTICE("Packet has too much data (need %d, have %d)! Dropping.", _encap->expectedLength(), _encap->getTotalUnitLength());
					incBadEncapTally();
					ndSafeRelease(_encap);
				}
			}
			else {
				MOD_NOTICE("Lost the end of the packet! Dropping.");
				incBadEncapTally();
				ndSafeRelease(_encap);
			}
		}
	}

	MOD_DEBUG("Finished processing one M_PDU.");
	return count; // The number of complete EncapsulationPackets found.
}

}
