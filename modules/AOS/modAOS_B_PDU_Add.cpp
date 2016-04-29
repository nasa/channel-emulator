/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modAOS_B_PDU_Add.cpp
 * @author Tad Kollar  
 *
 * $Id: modAOS_B_PDU_Add.cpp 2344 2013-08-30 20:24:16Z tkollar $
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "modAOS_B_PDU_Add.hpp"

using namespace nasaCE;

namespace nAOS_B_PDU_Add {

modAOS_B_PDU_Add::modAOS_B_PDU_Add(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr):
	PeriodicTrafficHandler(newName, newChannelName, newDLLPtr),
		_sendImmediately(CEcfg::instance()->getOrAddBool(cfgKey("immediateSend"), true)),
		_unfilled_bpdu(0), _unfilled_bpdu_idx(0), _bpduLength(0), _mtuIsManual(false) {

}

modAOS_B_PDU_Add::~modAOS_B_PDU_Add() {
	MOD_DEBUG("Running ~modAOS_B_PDU_Add().");
}

int modAOS_B_PDU_Add::svc() {
	svcStart_();

	if ( ! _bpduLength) {
		if ( getMTU() > 0 ) {
			_bpduLength = getMTU();
			_mtuIsManual = true;
		}
		else if (links_[PrimaryOutputLink] ) {
			MOD_INFO("B_PDU length is unset, asking output target %s for MRU.", links_[PrimaryOutputLink]->getTarget()->getName().c_str());
			_bpduLength = links_[PrimaryOutputLink]->getTarget()->getMRU();
			_mtuIsManual = false;
		}
	}

	if (_bpduLength > 0) {
		rebuildIdleUnitTemplate_();

		while ( continueService() ) {
			std::pair<NetworkData*, int> queueTop = getData_();

			// Check every 11th unit for a new MTU
			if ( ! _mtuIsManual && getReceivedUnitCount() % 11 == 0 && links_[PrimaryOutputLink] ) {
				_bpduLength = links_[PrimaryOutputLink]->getTarget()->getMRU();
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

			MOD_DEBUG("Received %d bytes to convert into AOS B_PDUs.", queueTop.first->getUnitLength());

			AOS_Bitstream_PDU* bpdu_list = _process_data(queueTop.first);
			// Deletion of received data is performed in _process_data.

			if ( ! getSendImmediately() ) {
				while (bpdu_list) {
					AOS_Bitstream_PDU* cur = bpdu_list;
					_send(cur);

					bpdu_list = (AOS_Bitstream_PDU*) bpdu_list->getNextPart();
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

bool modAOS_B_PDU_Add::_send(AOS_Bitstream_PDU* bpdu) {
	if ( ! bpdu ) {
		MOD_WARNING("_send() received a null pointer.");
		return false;
	}

	if ( links_[PrimaryOutputLink] ) {
		MOD_DEBUG("Sending %d octets.", bpdu->getUnitLength());
		links_[PrimaryOutputLink]->send(bpdu);
	}
	else {
		MOD_NOTICE("No output target defined yet, dropping data.");
		return false;
	}

	return true;
}

AOS_Bitstream_PDU* modAOS_B_PDU_Add::_process_data(NetworkData* netdata) {
	AOS_Bitstream_PDU* bpdu_list = 0;
	AOS_Bitstream_PDU* bpdu = 0;
	size_t bpdu_idx = 0;
	size_t netdata_idx = 0;
	size_t data_zone_len = _getDataZoneLength();
	int bpdu_count = 0;
	size_t dataLen = 0;
	size_t netdata_length = netdata->getUnitLength();

	MOD_DEBUG("Converting NetworkData objects to B_PDUs.");

	if (_unfilled_bpdu) {
		MOD_DEBUG("Starting with unfilled B_PDU (index %d).", _unfilled_bpdu_idx);
		bpdu = _unfilled_bpdu;
		bpdu_idx = _unfilled_bpdu_idx;
		++bpdu_count;

		_unfilled_bpdu = 0;
		_unfilled_bpdu_idx = 0;
	}

	while (netdata) {
		if (! bpdu) {

			if ( ! get_B_PDU_Length() ) {
				MOD_ERROR("The B_PDU length is unset, cannot continue.");
				return 0;
			}

			bpdu = new AOS_Bitstream_PDU(get_B_PDU_Length());
			bpdu_idx = 0;
			++bpdu_count;
		}

		// Figure out how much to take out of the current data buffer.
		if ( netdata_length - netdata_idx <= data_zone_len - bpdu_idx ) {
			// Copy everything that's left.
			dataLen = netdata_length - netdata_idx;
		}
		else {
			// Copy only as much as the Packet Zone has room for.
			dataLen = data_zone_len - bpdu_idx;
		}

		try {
			bpdu->copyUnit(bpdu->ptrData() + bpdu_idx, netdata->ptrUnit() + netdata_idx, dataLen);
		}
		catch (const BufferOverflow& e) {
			MOD_ERROR("_process_data(): Target size: %d, Source size: %d",
				bpdu->getDataLength() - bpdu_idx, dataLen);
			break;
		}

		bpdu_idx += dataLen;
		netdata_idx += dataLen;

		if (bpdu_idx == data_zone_len) {
			bpdu->setAllData();
			// If these values are equal, make a new B_PDU. If bpdu_idx is greater, we added wrong.
			if ( getSendImmediately() ) {
				if ( ! _send(bpdu) )
					MOD_WARNING("Unable to send B_PDU after Data Zone filled.");
			}
			else {
				if (bpdu_list) bpdu_list->tail()->setNextPart(bpdu);
				else bpdu_list = bpdu;
			}
			bpdu = 0;
		}
		else if (bpdu_idx > data_zone_len) throw LogicError("bpdu_idx > data_zone_len");

		if ( netdata_idx == netdata_length ) {
			NetworkData* cur = netdata;
			// If these values are equal, go to the next packet. If netdata_idx is greater, we added wrong.
			netdata = netdata->getNextPart();
			ndSafeRelease(cur);
			netdata_idx = 0;

			if (netdata) netdata_length = netdata->getUnitLength();

			// Add this last B_PDU to the list.
			if ( !netdata && bpdu && ! getSendImmediately() ) {
				if ( ! bpdu_list ) bpdu_list = bpdu; // Only got one short packet.
				else bpdu_list->tail()->setNextPart(bpdu);
			}
		}
		else if (netdata_idx > netdata_length) {
			MOD_DEBUG("netdata_idx == %d, netdata->getUnitLength == %d, dataLen == %d", netdata_idx, netdata->getUnitLength(), dataLen);
			throw LogicError("netdata_idx > netdata->getUnitLength()");
		}
	}

	// Handle processing and sending of the final B_PDU, if not filled.
	if ((bpdu || bpdu_list) && bpdu_count) {
		AOS_Bitstream_PDU* last_bpdu;

		if ( ! getSendImmediately() ) last_bpdu = ((AOS_Bitstream_PDU*) bpdu_list->tail());
		else last_bpdu = bpdu;

		// If we're not sending the B_PDUs in "groups" (getSendImmediately() is false),
		// or there's nothing left in the message queue, fill with idle pattern.
		//
		// NOTE: Checking is_empty() below will result in varying totals of octets sent!!!
		//       This is because the feeder module occasionally will not be able to
		//       insert the next unit into the queue fast enough. This will NOT cause
		//       errors, but you cannot rely on the exact received octet count as a
		//       measure of repeatability.
		if ( msg_queue()->is_empty() || ! getSendImmediately() ) {

			// Now that we're out of packets to add, pad the last B_PDU with Fill.
			if ( bpdu_idx < data_zone_len) {
				if ( !last_bpdu ) throw nd_error("Got a null packet for the final B_PDU!\n");

				MOD_DEBUG("Adding %d Fill packets at index %d.",
					data_zone_len - bpdu_idx, bpdu_idx);
				last_bpdu->addFill(bpdu_idx);

				if (bpdu_idx > 0) {
					last_bpdu->setBitstreamDataEnd(bpdu_idx - 1);
				}
			}

			// We should probably never have to fill the B_PDU with 100% idle data,
			// at least within this function.
			if (last_bpdu && bpdu_idx == 0) {
				MOD_DEBUG("Marking B_PDU as 100% idle.");
				last_bpdu->setAllIdle();
			}

			if ( getSendImmediately() )
				if ( ! _send(last_bpdu) )
					MOD_WARNING("Unable to send last B_PDU.");
		}
		else {
			MOD_DEBUG("Waiting for more traffic to fill last B_PDU (index %d).", bpdu_idx);
			_unfilled_bpdu = last_bpdu;
			_unfilled_bpdu_idx = bpdu_idx;
			--bpdu_count;
		}
	}

	MOD_DEBUG("Generated %d complete B_PDUs and %d unfilled.", bpdu_count, (_unfilled_bpdu? 1 : 0));
	return bpdu_list;
}

void modAOS_B_PDU_Add::connectOutput(BaseTrafficHandler* target,
		const HandlerLink::OutputRank sourceOutputRank /* = HandlerLink::PrimaryOutput */,
		const HandlerLink::InputRank targetInputRank /* = HandlerLink::PrimaryInput */,
		const bool postOpen /* = false */ ) {
	connectPeriodicOutput(target, sourceOutputRank, targetInputRank, postOpen);

	if (! get_B_PDU_Length() && links_[PrimaryOutputLink]) {
		set_B_PDU_Length(links_[PrimaryOutputLink]->getTarget()->getMRU());
		MOD_DEBUG("Obtained B_PDU length of %d octets from target.", get_B_PDU_Length());
	}
}

NetworkData* modAOS_B_PDU_Add::getIdleUnit() {
	if (!idleUnitTemplate_)
		throw NoIdleUnit("The bitstream PDU idle template has not been constructed yet.");

	idleUnitTemplateCreation_.acquire();
	AOS_Bitstream_PDU* idleDup = new AOS_Bitstream_PDU(dynamic_cast<AOS_Bitstream_PDU*>(idleUnitTemplate_));
	idleUnitTemplateCreation_.release();
	return idleDup;
}

void modAOS_B_PDU_Add::rebuildIdleUnitTemplate_() {
	if ( get_B_PDU_Length() ) {
		MOD_DEBUG("Rebuilding %d-octet Idle Bitstream PDU template.", get_B_PDU_Length());
		idleUnitTemplateCreation_.acquire();

		AOS_Bitstream_PDU* newBPDU = new AOS_Bitstream_PDU(get_B_PDU_Length());
		newBPDU->setAllIdle();
		newBPDU->addFill(0);

		ndSafeRelease(idleUnitTemplate_);

		idleUnitTemplateCreation_.release();
	}
}

}
