/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modTM_VC_Gen_Packet.hpp
 * @author Tad Kollar  
 *
 * $Id: modTM_VC_Gen_Packet.hpp 2033 2013-06-05 18:28:52Z tkollar $
 * Copyright (c) 2012.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _MOD_TM_VC_GEN_PACKET_HPP_
#define _MOD_TM_VC_GEN_PACKET_HPP_

#include "TM_Transfer_Frame.hpp"
#include "TM_Mux_Base.hpp"

using namespace nTM_Mux;

namespace nTM_VC_Gen_Packet {

//=============================================================================
/**
 * @class modTM_VC_Gen_Packet
 * @author Tad Kollar  
 * @brief Encapsulate incoming packets into a TM Transfer Frame.
 */
//=============================================================================
class modTM_VC_Gen_Packet: public TM_Mux_Base {
public:
	/// Primary constructor.
	/// @param newName An identifier for the module that's unique within the channel.
	/// @param newChannelName An identifier for the containing channel.
	/// @param newDLLPtr Pointer to the DLL management structure.
    modTM_VC_Gen_Packet(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr);

	/// Destructor
    ~modTM_VC_Gen_Packet();

	/// Read in data, encapsulate in one or more M_PDUs, and send it on.
	int svc();

	/// Override BaseTrafficHandler::getMTU() to return _frameSize instead of _MTU.
	size_t getMTU() const;

	/// Override BaseTrafficHandler::setMTU() to set _frameSize instead of MTU_.
	void setMTU(const size_t newVal);

	/// Calculate the Data Field length from the total length.
	size_t getDataFieldLength() const;

	/// Read-only accessor to _maxUsecsForNextPacket.
	int getMaxUsecsForNextPacket() const;

	/// Write-only accessor to _maxUsecsForNextPacket and _maxUsecsForNextPacketSetting.
	void setMaxUsecsForNextPacket(const int& newVal);

	/// Convert _maxUsecsForNextPacket to an ACE_Time_Value.
	ACE_Time_Value getWaitForNextPacket() const;

	/// Convert an ACE_Time_Value to _maxUsecsForNextPacket.
	void setWaitForNextPacket(ACE_Time_Value& timeWait);

	/// Set the contents of _fillPattern.
	/// @param fill The unit holding the new pattern.
	void setFillPattern(const NetworkData* fill);

	/// Set the contents of _fillPattern.
	/// @param fill A vector containing the new pattern.
	void setFillPattern(const std::vector<uint8_t>& fill);

	/// Return the contents of _fillPattern.
	/// @param fill A vector to copy _fillPattern into.
	void getFillPattern(std::vector<uint8_t>& fill) const;

	/// Write-only accessor to _multiPacketDataField and _multiPacketDataFieldSetting.
	void setMultiPacketDataField(bool newVal);

	/// Read-only accessor to _multiPacketDataField.
	bool getMultiPacketDataField() const;

protected:

	/// Return the PVN (first 3 bits) extracted from the provided message pointer.
	/// @param mb Polymorphed pointer to an CCSDS-approved packet type.
	ACE_UINT8 getIdentifier_(ACE_Message_Block* mb) const;

private:
	/// How long to wait for an incoming packet when the current Data Field isn't full.
	int _maxUsecsForNextPacket;

	/// Config file reference to _maxUsecsForNextPacket.
	Setting& _maxUsecsForNextPacketSetting;

	/// The buffer to fill any leftover Data Field space with.
	NetworkData* _fillPattern;

	/// Config file reference to _fillPattern.
	Setting& _fillPatternSetting;

	/// Whether to expect more than one packet in the Data Field.
	bool _multiPacketDataField;

	/// Config file reference to _multiPacketDataField.
	Setting& _multiPacketDataFieldSetting;

	/// Restrict access to _fillPattern so it can be modified on the fly.
	ACE_Process_Mutex _fillPatternLock;

	/// Iterate over a list of NetworkData objects and convert them to Transfer Frames.
	void _processData(NetworkData* netdata);

	/// Send a newly-created Transfer Frame to the next handler.
	bool _send(TM_Transfer_Frame* frame);

	/// When there's more data waiting in the queue than has been passed to
	/// _processData(), this holds the Transfer Frame that is being worked on until
	/// _processData() is called again with the next data from the queue.
	TM_Transfer_Frame* _unfilledFrame;

	/// The index to begin writing data to in the _unfilledFrame.
	size_t _unfilledFrameIdx;

	/// Preserve this state between calls to _processData() in case
	/// an _unfilledFrame is held.
	bool _firstHeaderIndexSet;

}; // class modTM_VC_Gen_Packet

} // namespace nTM_VC_Gen_Packet

#endif // _MOD_TM_VC_GEN_PACKET_HPP_
