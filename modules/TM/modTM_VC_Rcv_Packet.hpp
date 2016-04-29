/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modTM_VC_Rcv_Packet.hpp
 * @author Tad Kollar  
 *
 * $Id: modTM_VC_Rcv_Packet.hpp 2386 2013-11-26 15:27:46Z tkollar $
 * Copyright (c) 2012.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _MOD_TM_VC_RCV_PACKET_HPP_
#define _MOD_TM_VC_RCV_PACKET_HPP_

#include "TM_Transfer_Frame.hpp"
#include "TM_Demux_Base.hpp"

using namespace nTM_Demux;

namespace nTM_VC_Rcv_Packet {

//=============================================================================
/**
	@class modTM_VC_Rcv_Packet
	@author Tad Kollar  
	@brief Extract packets from incoming TM Protocol transfer frames.
*/
//=============================================================================
class modTM_VC_Rcv_Packet: public TM_Demux_Base {
public:
	enum PacketVersionNumbers {
		pvnSpacePacket = 0,
		pvnSCPS_NP = 1,
		pvnEncapsulationPacket = 7
	};

	/// Primary constructor.
	/// @param newName An identifier for the module that's unique within the channel.
	/// @param newChannelName An identifier for the containing channel.
	/// @param newDLLPtr Pointer to the DLL management structure.
    modTM_VC_Rcv_Packet(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr);

	/// Destructor
    ~modTM_VC_Rcv_Packet();

	/// Read in data, extract and assemble packets, and send them on.
	int svc();

	/// Write-only accessor to _ipeSupport and _ipeSupportSetting.
	void setIpeSupport(bool newVal) { _ipeSupport = newVal; _ipeSupportSetting = newVal; }

	/// Read-only accessor to _ipeSupport.
	bool getIpeSupport() const { return _ipeSupport; }

	/// Write-only accessor to _multiDataField and _multiDataFieldSetting.
	void setMultiDataField(bool newVal) { _multiDataField = newVal; _multiDataFieldSetting = newVal; }

	/// Read-only accessor to _multiDataField.
	bool getMultiDataField() const { return _multiDataField; }

	/// Write-only accessor to _allowPacketsAfterFill and _allowPacketsAfterFillSetting.
	void setAllowPacketsAfterFill(bool newVal) {
		_allowPacketsAfterFill = newVal;
		_allowPacketsAfterFillSetting = newVal;
	}

	/// Read-only accessor to _allowPacketsAfterFill.
	bool getAllowPacketsAfterFill() const { return _allowPacketsAfterFill; }

	/// Write-only accessor to _badPacketTally.
	void setBadPacketTally(const ACE_UINT64& newVal) { _badPacketTally = newVal; }

	/// Read-only accessor to _badPacketTally.
	ACE_UINT64 getBadPacketTally() const { return _badPacketTally; }

	/// Get a pointer to the specified output link.
	/// @return 0 if link is not found.
	HandlerLink* getLink(const LinkType ltype, const ACE_UINT8 pvn = 0);

	/// Return the Packet Version Number of a data unit in a buffer.
	/// @param buf Pointer to the buffer.
	static ACE_UINT8 getPVN(const ACE_UINT8* buf) { return (*buf) >> 5; }

private:
	/// Need to know which type is inside so the length field can be located.
	unsigned _contained_type;

	/// When a packet is fully assembled, transfer it to the next handler.
	void _send(NetworkData* data);

	/// Determine which packet type is in the frame and apply the correct method.
	/// @param frame The frame or list of frames to process.
	void _assemblePacket(TM_Transfer_Frame* frame);

	/// Assemble Space Packets from the received frame(s) (Not implemented).
	/// @param frame The frame or list of frames to process.
	/// @param idx Where to begin processing in the TM Transfer Frame Data Field
	/// @return True if there may be more packets in the Data Field.
	bool _assembleSpacePkt(TM_Transfer_Frame* frame, size_t& idx);

	/// If _packet points to nothing, create a new Space Packet to fill with the
	/// contents of the Data Field.
	/// @param frame The frame or list of frames to process.
	/// @param idx Where to begin processing in the TM Transfer Frame Data Field
	/// @return True if there may be more packets in the Data Field.
	bool _startNewSpacePkt(TM_Transfer_Frame* frame, size_t& idx);

	/// If _packet contains the head of a Space Packet, continue it with the
	/// contents of the Data Field.
	/// @param frame The frame or list of frames to process.
	/// @param idx Where to begin processing in the TM Transfer Frame Data Field
	/// @return True if there may be more packets in the Data Field.
	bool _continueSpacePkt(TM_Transfer_Frame* frame, size_t& idx);

	/// Assemble SCPS-NP Packets from the received frame(s) (Not implemented).
	/// @param frame The frame or list of frames to process.
	/// @param idx Where to begin processing in the TM Transfer Frame Data Field
	/// @return True if there may be more packets in the Data Field.
	bool _assembleSCPS_NP(TM_Transfer_Frame* frame, size_t& idx);

	/// Assemble Encapsulation Packets from the received frame(s).
	/// @param frame The frame or list of frames to process.
	/// @param idx Where to begin processing in the TM Transfer Frame Data Field
	/// @return True if there may be more packets in the Data Field.
	bool _assembleEncap(TM_Transfer_Frame* frame, size_t& idx);

	/// If _packet points to nothing, create a new Encapsulation Packet to fill with the
	/// contents of the Data Field.
	/// @param frame The frame or list of frames to process.
	/// @param idx Where to begin processing in the TM Transfer Frame Data Field
	/// @return True if there may be more packets in the Data Field.
	bool _startNewEncap(TM_Transfer_Frame* frame, size_t& idx);

	/// If _packet contains the head of an Encapsulation Packet, continue it with the
	/// contents of the Data Field.
	/// @param frame The frame or list of frames to process.
	/// @param idx Where to begin processing in the TM Transfer Frame Data Field
	/// @return True if there may be more packets in the Data Field.
	bool _continueEncap(TM_Transfer_Frame* frame, size_t& idx);

	/// The current packet being worked on.
	NetworkData* _packet;

	/// Whether the Internet Protocol Extension in the generated Encap Packets is supported.
	bool _ipeSupport;

	/// Config file reference to _ipeSupport.
	Setting& _ipeSupportSetting;

	/// Whether to expect more than one packet in the Packet Zone.
	bool _multiDataField;

	/// Config file reference to _multiDataField.
	Setting& _multiDataFieldSetting;

	/// If true, continue to look for packets after encap fill (0xe0) is found.
	bool _allowPacketsAfterFill;

	/// Config file reference to _allowPacketsAfterFill.
	Setting& _allowPacketsAfterFillSetting;

	/// The number of badly formed Encap Packets.
	ACE_UINT64 _badPacketTally;

	/// Increment _badPacketTally by one.
	void incBadPacketTally() { _badPacketTally++; }


}; // class modTM_VC_Rcv_Packet

} // namespace nTM_VC_Rcv_Packet

#endif // _MOD_TM_VC_RCV_PACKET_HPP_
