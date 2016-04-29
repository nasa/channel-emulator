/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modAOS_M_PDU_Remove.hpp
 * @author Tad Kollar  
 *
 * $Id: modAOS_M_PDU_Remove.hpp 2018 2013-06-04 14:59:19Z tkollar $ 
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _MOD_AOS_M_PDU_REMOVE_HPP_
#define _MOD_AOS_M_PDU_REMOVE_HPP_

#include "BaseTrafficHandler.hpp"
#include "AOS_Multiplexing_PDU.hpp"
#include "EncapsulationPacket.hpp"

namespace nAOS_M_PDU_Remove {

//=============================================================================
/**
	@class modAOS_M_PDU_Remove
	@author Tad Kollar  
	@brief Extract data from incoming AOS Multiplexing Protocol Data Units.
*/
//=============================================================================
class modAOS_M_PDU_Remove: public BaseTrafficHandler {
public:
	enum ContainedPacketTypes {
		mpduEncapsulationPacket,
		mpduSpacePacket
	};

	/// Primary constructor.
	/// @param newName An identifier for the module that's unique within the channel.
	/// @param newChannelName An identifier for the containing channel.
	/// @param newDLLPtr Pointer to the DLL management structure.
    modAOS_M_PDU_Remove(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr);

	/// Destructor
    ~modAOS_M_PDU_Remove();

	/// Read in data, encapsulate in one or more M_PDUs, and send it on.
	int svc();
	
	/// Read-only accessor to _contained_type.
	unsigned getContainedType() const { return _contained_type; }
	
	/// Return a string representation of _contained_type.
	std::string getContainedTypeStr() const {
		switch (_contained_type) {
			case mpduEncapsulationPacket: return "EncapsulationPacket";
				break;
			case mpduSpacePacket: return "SpacePacket";
				break;
			default: return "Unrecognized";
		}
	};
	
	/// Write-only accessor to _contained_type.
	/// @param newType Either mpduEncapsulationPacket or mpduSpacePacket.
	/// @throw LogicError If newType is unrecognized.
	void setContainedType(const ContainedPacketTypes newType) {
		if ( newType != mpduEncapsulationPacket && newType != mpduSpacePacket )
			throw LogicError("Unrecognized type provided to modAOS_M_PDU_Remove as contained type.");
			
		_contained_type = newType;
	}
	
	/// Derive _contained_type from the provided string.
	/// @throw LogicError If newTypeStr is unrecognized.
	void setContainedType(const std::string newTypeStr) {
		switch(tolower(newTypeStr[0])) {
			case 'e': _contained_type = mpduEncapsulationPacket;
				break;
			case 's': _contained_type = mpduSpacePacket;
				break;
			default:
				throw LogicError("Unrecognized type provided to modAOS_M_PDU_Remove as contained type.");
		}
	}
	
	/// Write-only accessor to _ipeSupport and _ipeSupportSetting.
	void setIpeSupport(bool newVal) { _ipeSupport = newVal; _ipeSupportSetting = newVal; }
	
	/// Read-only accessor to _ipeSupport.
	bool getIpeSupport() const { return _ipeSupport; }

	/// Write-only accessor to _multiPacketZone and _multiPacketZoneSetting.
	void setMultiPacketZone(bool newVal) { _multiPacketZone = newVal; _multiPacketZoneSetting = newVal; }

	/// Read-only accessor to _multiPacketZone.
	bool getMultiPacketZone() const { return _multiPacketZone; }

	/// Write-only accessor to _allowPacketsAfterFill and _allowPacketsAfterFillSetting.
	void setAllowPacketsAfterFill(bool newVal) {
		_allowPacketsAfterFill = newVal;
		_allowPacketsAfterFillSetting = newVal;
	}
	
	/// Read-only accessor to _allowPacketsAfterFill.
	bool getAllowPacketsAfterFill() const { return _allowPacketsAfterFill; }

	/// Write-only accessor to _badEncapTally.
	void setBadEncapTally(const ACE_UINT64& newVal) { _badEncapTally = newVal; }
	
	/// Read-only accessor to _badEncapTally.
	ACE_UINT64 getBadEncapTally() const { return _badEncapTally; }
	
private:
	/// Need to know which type is inside so the length field can be located.
	unsigned _contained_type;
	
	/// When a packet is fully assembled, transfer it to the next handler.
	void _send(NetworkData* data);
	
	/// Assemble Encapsulation Packets from the received M_PDU(s).
	/// @param m_pdu The M_PDU or list of M_PDUs to process.
	int _assemble_encap(AOS_Multiplexing_PDU* m_pdu);
	
	/// The current Encapsulation Packet being worked on.
	EncapsulationPacketBase* _encap;
	
	/// Whether the Internet Protocol Extension in the generated Encap Packets is supported.
	bool _ipeSupport;
	
	/// Config file reference to _ipeSupport.
	Setting& _ipeSupportSetting;
	
	/// Whether to expect more than one packet in the Packet Zone.
	bool _multiPacketZone;
	
	/// Config file reference to _multiPacketZone.
	Setting& _multiPacketZoneSetting;
	
	/// If true, continue to look for packets after encap fill (0xe0) is found.
	bool _allowPacketsAfterFill;
	
	/// Config file reference to _allowPacketsAfterFill.
	Setting& _allowPacketsAfterFillSetting;
	
	/// The number of badly formed Encap Packets.
	ACE_UINT64 _badEncapTally;
	
	/// Increment _badEncapTally by one.
	void incBadEncapTally() { _badEncapTally++; }	
	
}; // class modAOS_M_PDU_Remove

} // namespace nAOS_M_PDU_Remove

#endif // _MOD_AOS_M_PDU_REMOVE_HPP_
