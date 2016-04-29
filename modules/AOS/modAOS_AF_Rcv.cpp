/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modAOS_AF_Rcv.cpp
 * @author Tad Kollar  
 *
 * $Id: modAOS_AF_Rcv.cpp 2489 2014-02-13 16:44:28Z tkollar $
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "modAOS_AF_Rcv.hpp"
#include "printers.hpp"

namespace nAOS_AF_Rcv {

modAOS_AF_Rcv::modAOS_AF_Rcv(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr):
	AOS_PhysicalChannel_Service(newName, newChannelName, newDLLPtr), _frameTemplate(0),
	_badFrameChecksumCount(0L),
	_correctedHeaderErrorCount(0L),
	_uncorrectedHeaderErrorCount(0L),
	_errorlessHeaderUnitCount(0L),
	_correctedHeaderUnitCount(0L),
	_uncorrectedHeaderUnitCount(0L),
	_aosHeaderDecoder(new RSDecoder(0x13, 4, 10, 4, 1, 6, 1, false)) {
	updateTemplate_();
}

modAOS_AF_Rcv::~modAOS_AF_Rcv() {
	MOD_DEBUG("Running ~modAOS_AF_Rcv().");
	stopTraffic();
	delete _aosHeaderDecoder;
}

int modAOS_AF_Rcv::svc() {
	svcStart_();

	AOS_Transfer_Frame* aos = 0;

	while ( continueService() ) {
		ndSafeRelease(aos);

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

		// Be tricky - if we received a pointer to an actual AOS_Transfer_Frame object,
		// continue using it. Otherwise, build a new wrapper around the received data.
		if ( ! (aos = dynamic_cast<AOS_Transfer_Frame*>(queueTop.first)) ) {
			MOD_DEBUG("Generating AOS_Transfer_Frame wrapper.");
			aos = new AOS_Transfer_Frame(_frameTemplate);
			aos->deepCopy(queueTop.first);
			ndSafeRelease(queueTop.first);
		}
		else {
			MOD_DEBUG("Retaining existing AOS_Transfer_Frame wrapper.");
		}

		aos->useHeaderErrorControl(getUseHeaderErrorControl());
		aos->setInSduLen(getInsertZoneSize());
		aos->useFrameErrorControl(getUseFrameErrorControl());

		queueTop.first = 0;

		MOD_DEBUG("Received %d-octet AOS Transfer Frame with GVCID %X.", aos->getUnitLength(), aos->getGVCID());

		if ( ! validateFrame(aos, getDropBadFrames()) ) {
			if (getDropBadFrames()) continue;
		}
		else incValidFrameCount();

		// If this is idle data, move along. Checking frame count is optional.
		if (aos->isIdle()) {
			// TODO: Increase idle frame count?
			continue;
		}

		if ( links_[PrimaryOutputLink] ) {
			links_[PrimaryOutputLink]->send(aos);
			aos = 0; // Important so that ndSafeRelease at top doesn't actually delete.
		}
		else {
			MOD_ERROR("No output target defined, dropping frame.");
		}

		MOD_DEBUG("Finished processing incoming frame.");
	}

	return svcEnd_();

}

void modAOS_AF_Rcv::updateTemplate_() {
	ndSafeRelease(_frameTemplate);

	_frameTemplate = new AOS_Transfer_Frame(
		getFrameSize(),
		0,
		getUseHeaderErrorControl(),
		getInsertZoneSize(),
		false, // existance of OCF is VC dependent
		getUseFrameErrorControl()
	);

	if (! _frameTemplate ) throw nd_error("Unable to allocate an AOS_Transfer_Frame!");
}

bool modAOS_AF_Rcv::validateFrame(AOS_Transfer_Frame*& aos, const bool deleteBadFrame /* = false */) {

	// Test length. If it's wrong then there's nothing to be done.
	if (static_cast<int>(aos->getUnitLength()) != getFrameSize()) {
		MOD_WARNING("AOS Transfer Frame bad length: received %d octets, needed exactly %d.",
			aos->getUnitLength(), getFrameSize());
		incBadLengthCount();
		if (deleteBadFrame) ndSafeRelease(aos);
		return false;
	}

	// Attempt error correction before checking other vakues.
	if ( getUseHeaderErrorControl() ) {
		if ( aos->hasHeaderErrorControl() ) {
			Sym headerData[5];

			// Extract the 4-bit symbols to check parity for.
			ACE_OS::memcpy(headerData, aos->ptrUnit(), 2);
			ACE_OS::memcpy(headerData + 2, aos->ptrSignalingField(), 3);
			SymVec headerDataUnpacked = _aosHeaderDecoder->unpack(headerData, 5, 10);

			try {
				SymVec corrected = _aosHeaderDecoder->correct(headerDataUnpacked);
				_aosHeaderDecoder->pack(corrected, 10, headerData, 5);

				std::size_t errs = _aosHeaderDecoder->getLastCorrectedErrors();

				if (errs > 0) {
					++_correctedHeaderUnitCount;
					_correctedHeaderErrorCount += errs;
					aos->copyUnit(aos->ptrUnit(), headerData, 2);
					aos->copyUnit(aos->ptrSignalingField(), headerData + 2, 3);
					// _correctedHeaderErrorCount += errs;
					MOD_DEBUG("Corrected %d header errors.", errs);
				}
				else {
					++_errorlessHeaderUnitCount;
				}
			}
			catch (RSDecoder::TooManyErrors& tme) {
				++_uncorrectedHeaderUnitCount;
				_uncorrectedHeaderErrorCount += tme.errors;
				MOD_NOTICE("Failed to decode: %s. Dropping data.", tme.what());
				if (deleteBadFrame) ndSafeRelease(aos);
				return false;
			}
			catch (std::logic_error &e) {
				MOD_WARNING("Failed to correct frame header errors: %s", e.what());
				if (deleteBadFrame) ndSafeRelease(aos);
				return false;
			}
		}
		else {
			MOD_WARNING("Received an AOS Transfer Frame without header parity symbols when they were required.");
		}
	}

	if ( aos->getTFVN() != 01 ) {
		MOD_WARNING("AOS Transfer Frame bad version number: should be 01, received %o.", aos->getTFVN());
		// print_transfer_frame_info(aos);
		incBadTFVNCount();
		if (deleteBadFrame) ndSafeRelease(aos);
		return false;
	}

	if ( getUseFrameErrorControl() && ! aos->hasValidCRC() ) {
		MOD_NOTICE("Transfer Frame has bad CRC.");
		incBadFrameChecksumCount();
		if (deleteBadFrame) ndSafeRelease(aos);
		return false;
	}

	return true;
}

}
