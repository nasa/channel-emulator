/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modAOS_M_PDU_Add.hpp
 * @author Tad Kollar  
 *
 * $Id: modAOS_M_PDU_Add.hpp 2029 2013-06-05 15:06:31Z tkollar $
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _AOS_M_PDU_ADD_HPP_
#define _AOS_M_PDU_ADD_HPP_

#include "BaseTrafficHandler.hpp"
#include "AOS_Multiplexing_PDU.hpp"

using namespace nasaCE;

namespace nAOS_M_PDU_Add {

//=============================================================================
/**
 * @class modAOS_M_PDU_Add
 * @author Tad Kollar  
 * @brief Convert incoming data to AOS Multiplexing Protocol Data Units.
*/
//=============================================================================
class modAOS_M_PDU_Add: public BaseTrafficHandler {
public:
	/// Primary constructor.
	/// @param newName An identifier for the module that's unique within the channel.
	/// @param newChannelName An identifier for the containing channel.
	/// @param newDLLPtr Pointer to the DLL management structure.
    modAOS_M_PDU_Add(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr);

	/// Destructor
    ~modAOS_M_PDU_Add();

	/// Read in data, encapsulate in one or more M_PDUs, and send it on.
	int svc();

	/// Wrapper for getMTU().
	size_t get_M_PDU_Length() const { return _mpduLength; }

	/// Wrapper for setMTU().
	void set_M_PDU_Length(const size_t newLength) { _mpduLength = newLength; setMTU(newLength); }

	/// Read-only accessor to _sendImmediately.
	bool isSendingImmediately() const { return _sendImmediately; }

	/// Write-only accessor to _sendImmediately.
	void setSendImmediately(const bool newVal = true) { _sendImmediately = newVal; }

	/// Overloaded to attempt to autodetect our MTU (if unset) from the target.
	/// Create a HandlerLink to manage the existance of the connection from
	/// this handler's output to the next handler's input.
	/// @param target The Traffic Handler to output to.
	/// @param sourceOutputRank Whether to connect the source's primary or auxiliary output.
	/// @param targetInputRank Whether to connect to the target's primary or auxiliary input.
	/// @param postOpen True if the link is being created after the source is already active.
	void connectOutput(BaseTrafficHandler* target,
		const HandlerLink::OutputRank sourceOutputRank = HandlerLink::PrimaryOutput,
		const HandlerLink::InputRank targetInputRank = HandlerLink::PrimaryInput,
		const bool postOpen = false );

	/// Read-only accessor to _maxUsecsForNextPacket.
	int getMaxUsecsForNextPacket() const {
		return _maxUsecsForNextPacket;
	}

	/// Write-only accessor to _maxUsecsForNextPacket and _maxUsecsForNextPacketSetting.
	void setMaxUsecsForNextPacket(const int newVal) {
		_maxUsecsForNextPacket = newVal;
		_maxUsecsForNextPacketSetting = _maxUsecsForNextPacket;
	}

	/// Convert _maxUsecsForNextPacket to an ACE_Time_Value.
	ACE_Time_Value getWaitForNextPacket() const {
		ACE_Time_Value timeWait;
		timeWait.sec(static_cast<int>(_maxUsecsForNextPacket)/1000000);
		timeWait.usec(static_cast<int>(_maxUsecsForNextPacket)%1000000);
		return timeWait;
	}

	/// Convert an ACE_Time_Value to _maxUsecsForNextPacket.
	void setWaitForNextPacket(ACE_Time_Value& timeWait) {
		ACE_UINT64 totalUsecs;
		timeWait.to_usec(totalUsecs);

		_maxUsecsForNextPacket = static_cast<int>(totalUsecs);
		_maxUsecsForNextPacketSetting = _maxUsecsForNextPacket;
	}

	/// Set the contents of _fillPattern.
	/// @param fill The unit holding the new pattern.
	void setFillPattern(const NetworkData* fill);

	/// Set the contents of _fillPattern.
	/// @param fill A vector containing the new pattern.
	void setFillPattern(const std::vector<uint8_t>& fill);

	/// Return the contents of _fillPattern.
	/// @param fill A vector to copy _fillPattern into.
	void getFillPattern(std::vector<uint8_t>& fill) const;

	/// Write-only accessor to _multiPacketZone and _multiPacketZoneSetting.
	void setMultiPacketZone(bool newVal) { _multiPacketZone = newVal; _multiPacketZoneSetting = newVal; }

	/// Read-only accessor to _multiPacketZone.
	bool getMultiPacketZone() const { return _multiPacketZone; }

private:
	/// How long to wait for an incoming packet when the current M_PDU isn't full.
	int _maxUsecsForNextPacket;

	/// Config file reference to _maxUsecsForNextPacket.
	Setting& _maxUsecsForNextPacketSetting;

	/// The buffer to fill any leftover Packet Zone space with.
	NetworkData* _fillPattern;

	/// Config file reference to _fillPattern.
	Setting& _fillPatternSetting;

	/// Whether to expect more than one packet in the Packet Zone.
	bool _multiPacketZone;

	/// Config file reference to _multiPacketZone.
	Setting& _multiPacketZoneSetting;

	/// Restrict access to _fillPattern so it can be modified on the fly.
	ACE_Process_Mutex _fillPatternLock;

	/// Calculate the packet zone length from the total length.
	size_t _getPacketZoneLength() const { return get_M_PDU_Length() - AOS_Multiplexing_PDU::spanHeader; }

	/// Whether to send each M_PDU as it's created or wait to send them all together.
	bool _sendImmediately;

	/// Iterate over a list of NetworkData objects and convert them to M_PDUs.
	/// @return A list of the M_PDUs created, if _sendImmediately is false; otherwise 0.
	AOS_Multiplexing_PDU* _processData(NetworkData* netdata);

	/// Send a newly-created M_PDU to the next handler.
	bool _send(AOS_Multiplexing_PDU* mpdu);

	/// When there's more data waiting in the queue than has been passed to
	/// _processData(), this holds the M_PDU that is being worked on until
	/// _processData() is called again with the next data from the queue.
	AOS_Multiplexing_PDU* _unfilled_mpdu;

	/// The index to begin writing data to in the _unfilled_mpdu.
	size_t _unfilled_mpdu_idx;

	/// Preserve this state between calls to _processData() in case
	/// an _unfilled_mpdu is held.
	bool _firstHeaderIndexSet;

	/// How long each M_PDU will be.
	size_t _mpduLength;

	/// If true the MTU was specified manually instead of being obtained from the target.
	bool _mtuIsManual;
}; // class modAOS_M_PDU_Add

} // namespace nAOS_M_PDU_Add

#endif // _AOS_M_PDU_ADD_HPP_
