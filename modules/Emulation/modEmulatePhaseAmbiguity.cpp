/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modEmulatePhaseAmbiguity.cpp
 * @author Tad Kollar  
 *
 * $Id: modEmulatePhaseAmbiguity.cpp 2344 2013-08-30 20:24:16Z tkollar $
 * Copyright (c) 2011.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "modEmulatePhaseAmbiguity.hpp"

using namespace nasaCE;

namespace nEmulatePhaseAmbiguity {

modEmulatePhaseAmbiguity::modEmulatePhaseAmbiguity(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr /* = 0 */):
	BaseTrafficHandler(newName, newChannelName, newDLLPtr),
	_leftShiftBitsSetting(CEcfg::instance()->getOrAddInt(cfgKey("leftShiftBits"), 0)),
	_invertSetting(CEcfg::instance()->getOrAddBool(cfgKey("invert"), false)),
	_maxUsecsForNewData(CEcfg::instance()->getOrAddInt(cfgKey("maxUsecsForNewData"), -1)) {

	_leftShiftBits = _leftShiftBitsSetting;
	_invert = _invertSetting;

	ACE_Time_Value oneMillisec(0, 1000);
	if ( static_cast<int>(_maxUsecsForNewData) < 0) setWaitForNewDataInterval(oneMillisec);
	else {
		_waitForNewDataInterval.sec(getMaxUsecsForNewData()/1000000);
		_waitForNewDataInterval.usec(getMaxUsecsForNewData()%1000000);
	}
}

modEmulatePhaseAmbiguity::~modEmulatePhaseAmbiguity() {
	MOD_DEBUG("Running ~modEmulatePhaseAmbiguity().");
	stopTraffic();
}

int modEmulatePhaseAmbiguity::svc() {
	svcStart_();

	ACE_Time_Value timeToWaitUntil;
	ACE_Message_Block* mb = 0;
	ACE_UINT8 carry = 0; // Bits left over from the previous data unit.

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

		size_t incomingBlockLen = data->getUnitLength();
		MOD_DEBUG("Received %d octets to shift and/or invert.", incomingBlockLen);

		if ( incomingBlockLen && links_[PrimaryOutputLink] ) {
			NetworkData* shifted = new NetworkData(incomingBlockLen + 1);
			MOD_DEBUG("Created %d octet buffer to shift into.", shifted->getUnitLength());

			ACE_UINT8 rightShiftBits = 8 - _leftShiftBits;

			for (unsigned offset = 0; offset < incomingBlockLen; ++offset ) {
				if ( _leftShiftBits > 0 ) {

					if ( offset == 0 ) { // first octet
						*(shifted->ptrUnit()) = (*(data->ptrUnit()) >> rightShiftBits) | carry ;
						carry = 0;
					}
					else {
						*(shifted->ptrUnit() + offset) = (*(data->ptrUnit() + offset - 1) << _leftShiftBits) |
							(*(data->ptrUnit() + offset) >> rightShiftBits);
					}
				}
				else *(shifted->ptrUnit() + offset) = *(data->ptrUnit() + offset);

				if ( _invert ) *(shifted->ptrUnit() + offset) = ~ *(shifted->ptrUnit() + offset);
			}

			if ( _leftShiftBits > 0 ) carry = *(data->ptrUnit() + incomingBlockLen - 1 ) << _leftShiftBits;

			// Before sending, peek into the queue for up to the configured microseconds.
			// If nothing shows up, send the carry with the current block; otherwise
			// save it for the next.
			timeToWaitUntil = _getWaitUntilTime();
			if ( _leftShiftBits > 0 && msg_queue()->peek_dequeue_head(mb, &timeToWaitUntil) == -1 ) {
				MOD_DEBUG("Including carry on current block after not seeing a new block for %d microseconds.",
					getMaxUsecsForNewData());

				// Nothing else is coming, send the carry now.
				*(shifted->ptrUnit() + shifted->getUnitLength() - 1) = _invert? ~carry : carry;
				carry = 0;
			}
			else {
				// Either there's no left shift, or another block is waiting, send the carry with it.
				// Reduce the size of the unit by 1 octet (buffer size is unmodified).
				shifted->setUnitLength(incomingBlockLen, true);
			}

			MOD_DEBUG("Sending %d unit/%d buffer octets.", shifted->getUnitLength(),
				shifted->getBufferSize());
			links_[PrimaryOutputLink]->send(shifted);
		}
		else {
			if ( incomingBlockLen )
				MOD_NOTICE("No output target defined yet, dropping data.");
			else
				MOD_INFO("Received null data.");
		}

		ndSafeRelease(data);

		MOD_DEBUG("Finished processing incoming data block.");
	}

	return svcEnd_();

}

} // namespace nEmulatePhaseAmbiguity
