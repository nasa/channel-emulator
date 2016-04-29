/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modTM_OCF_Insert.cpp
 * @author Tad Kollar  
 *
 * $Id: modTM_OCF_Insert.cpp 2403 2013-12-12 19:08:39Z tkollar $
 * Copyright (c) 2012.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "modTM_OCF_Insert.hpp"
#include "TM_Transfer_Frame.hpp"

using namespace nasaCE;
using namespace nTM_OCF_Insert;

namespace nTM_OCF_Insert {

modTM_OCF_Insert::modTM_OCF_Insert(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr):
	TM_MasterChannel_Service(newName, newChannelName, newDLLPtr),
	_receivedReportUnitCount(0),
	_receivedReportOctetCount(0),
	_receivedFrameUnitCount(0),
	_receivedFrameOctetCount(0) {

	auxInputQueue_ = new AuxMessageQueue();

	MOD_DEBUG("Opening auxiliary input queue to receive OCF units.");
	getAuxQueue()->open(); // can set high/low water mark here, too.

}

modTM_OCF_Insert::~modTM_OCF_Insert() {
	MOD_DEBUG("Running ~modTM_OCF_Insert().");
	stopTraffic();

	delete auxInputQueue_;
	auxInputQueue_ = 0;
}

void modTM_OCF_Insert::stopTraffic(bool shutdown) {
	MOD_DEBUG("Closing auxiliary message queue.");
	getAuxQueue()->close();

	PeriodicTrafficHandler::stopTraffic(shutdown);
}

bool modTM_OCF_Insert::_insertWaitingReport(TM_Transfer_Frame* frame) {
	bool success = false;

	if ( ! getAuxQueue()->is_empty() ) {
		std::pair<NetworkData*, int> auxQueueTop = getAuxData_();

		if ( auxQueueTop.second < 0 ) {
			MOD_ERROR("getAuxData_() call failed.");
		}
		else if ( ! auxQueueTop.first ) {
			MOD_ERROR("getAuxData_() returned with null data.");
		}
		else {
			ACE_Message_Block* mb = auxQueueTop.first;

			TC_Comm_Link_Control_Word* clcw = dynamic_cast<TC_Comm_Link_Control_Word*>(mb);

			if (clcw) {
				MOD_DEBUG("Received a Communications Link Control Word.");

				_receivedReportUnitCount += 1;
				_receivedReportOctetCount += clcw->getUnitLength();

				try {
					frame->setCLCW(clcw);
					success = true;
				}
				catch (const MissingField& e) {
					MOD_WARNING("Attempt to insert CLCW into frame without an OCF, dropping the CLCW.");
				}

				ndSafeRelease(clcw);
			}
			else {
				NetworkData* ocf = dynamic_cast<NetworkData*>(mb);

				if ( ! ocf || mb->length() != TM_Transfer_Frame::spanOperationalControlField ) {
					MOD_WARNING("Unrecognized %d-octet, non-Report data unit received on auxiliary queue.", mb->length());

					ndSafeRelease(mb);
				}
				else {
					MOD_DEBUG("Type-2 Report is waiting, attempting to copy to the Operational Control Field.");

					_receivedReportUnitCount += 1;
					_receivedReportOctetCount += ocf->getUnitLength();

					try {
						frame->setType2ReportData(ocf->ptrUnit());
						success = true;
					}
					catch (const MissingField& e) {
						MOD_WARNING("Attempt to insert Type-2 Report into TM Transfer Frame without an OCF, dropping the report.");
					}
					catch (const NetworkData::MalformedPayload& e) {
						MOD_WARNING("Attempt to insert malformed Type-2 Report into TM Transfer Frame, dropping the report.");
					}

					ndSafeRelease(ocf);
				}
			}
		}
	}

	return success;
}


int modTM_OCF_Insert::svc() {
	svcStart_();

	TM_Transfer_Frame* frame = 0;

	while ( continueService() ) {
		ndSafeRelease(frame);

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

		if ( (frame = dynamic_cast<TM_Transfer_Frame*>(queueTop.first)) ) {
			MOD_DEBUG("Received %d-octet TM Transfer Frame to add a Secondary Header to.", frame->getUnitLength());

			// Test expected values.
			if (static_cast<int>(frame->getUnitLength()) != getFrameSize()) {
				MOD_WARNING("TM Transfer Frame bad length: received %d octets, needed %d.", frame->getUnitLength(), getFrameSize());
				incBadLengthCount();
				if (getDropBadFrames()) continue;
			}
			else incValidFrameCount();

			_receivedFrameUnitCount += 1;
			_receivedFrameOctetCount += frame->getUnitLength();

			_insertWaitingReport(frame);

			if ( links_[PrimaryOutputLink] ) {
				MOD_DEBUG("Sending %d octets.", frame->getUnitLength());

				links_[PrimaryOutputLink]->send(frame);
				frame = 0; // important
			}
			else {
				MOD_NOTICE("No output target defined yet, dropping data.");
			}
		}
		else {
			MOD_ERROR("Received %d-octet buffer in %s wrapper (not TM_Transfer_Frame)! Must discard.",
				queueTop.first->getUnitLength(), queueTop.first->typeStr().c_str());
			ndSafeRelease(queueTop.first);
			continue;
		}
	}

	return svcEnd_();

}

}
