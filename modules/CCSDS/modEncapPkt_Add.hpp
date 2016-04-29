/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modEncapPkt_Add.hpp
 * @author Tad Kollar  
 *
 * $Id: modEncapPkt_Add.hpp 1856 2013-01-30 14:16:24Z tkollar $ 
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _MOD_ENCAP_PKT_ADD_
#define _MOD_ENCAP_PKT_ADD_

#include "BaseTrafficHandler.hpp"
#include "IPv4Addr.hpp"

namespace nEncapPkt_Add {

//=============================================================================
/**
	@class modEncapPkt_Add
	@author Tad Kollar  
	@brief Wrap incoming data in an Encapsulation Packet.
*/
//=============================================================================
class modEncapPkt_Add: public BaseTrafficHandler {
public:
	/// An invalid IPE value to indicate that the IPE has not been overridden.
	static const ACE_UINT64 unsetIpeVal;
	
	/// An invalid protocol value to indicate that the IPE has not been overridden.
	static const ACE_UINT8 unsetProtocolVal;	

	/// Primary constructor.
	/// @param newName An identifier for the module that's unique within the channel.
	/// @param newChannelName An identifier for the containing channel.
	/// @param newDLLPtr Pointer to the DLL management structure.
    modEncapPkt_Add(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr);

	/// Destructor
    ~modEncapPkt_Add();

	/// Read in data, wraps it in an Encapsulation Packet, and sends it on.
	int svc();

	/// Write-only accessor to _ipeSupport and _ipeSupportSetting.
	void setIpeSupport(bool newVal) { _ipeSupport = newVal; _ipeSupportSetting = _ipeSupport; }
	
	/// Read-only accessor to _ipeSupport.
	bool getIpeSupport() const { return _ipeSupport; }

	/// Write-only accessor to _ipe and _ipeSetting.
	/// @param newVal The value to set all Encap IPE shims to. Will not be set if invalid (e.g. any even number).
	/// @throw BadVAlue If newVal is not a proper IPE.
	void setIpe(const ACE_UINT64& newVal);
	
	/// Read-only accessor to _ipe.
	ACE_UINT64 getIpe() const { return _ipe; }
	
	/// Stop using a user-specified IPE value; autodetect instead.
	void clearIpe() {
		if ( CEcfg::instance()->exists(cfgKey("IPE")) ) CEcfg::instance()->remove(cfgKey("IPE"));
		_ipe = unsetIpeVal;
	}

	/// Write-only accessor to _protocol and _protocolSetting.
	/// @throw ValueOutOfRange If newVal is not 0 - 7.
	void setProtocol(const ACE_UINT8 newVal) {
		if ( newVal > 7 && newVal != unsetProtocolVal )
			throw ValueOutOfRange(ValueOutOfRange::msg("Protocol ID", newVal, 0, 7));

		_protocol = newVal;
		
		if ( newVal != unsetProtocolVal ) CEcfg::instance()->getOrAddInt(cfgKey("protocol"), _protocol);
	}

	/// Read-only accessor to _protocol.
	ACE_UINT8 getProtocol() const { return _protocol; }
	
	/// Stop using a user-specified protocol value; autodetect instead.
	void clearProtocol() {
		if ( CEcfg::instance()->exists(cfgKey("protocol")) ) CEcfg::instance()->remove(cfgKey("protocol"));
		_protocol = unsetProtocolVal;
	}

	/// Write-only accessor to _lengthOfLength and _lengthOfLengthSetting.
	/// @throw ValueOutOfRange If newVal is not 0 - 3.
	void setLengthOfLength(const ACE_UINT8& newVal) {
		if ( newVal > 3 )
			throw ValueOutOfRange(ValueOutOfRange::msg("Length of Length", newVal, 0, 3));
			
		_lengthOfLength = newVal;
		_lengthOfLengthSetting = static_cast<int>(_lengthOfLength);
	}
	
	/// Read-only accessor to _lengthOfLength.
	ACE_UINT8 getLengthOfLength() const { return _lengthOfLength; }

	/// Write-only accessor to _userDefinedField and _userDefinedFieldSetting.
	/// @throw ValueOutOfRange If newVal is not 0 - 15.
	void setUserDefinedField(const ACE_UINT8& newVal) {
		if ( newVal > 0xF )
			throw ValueOutOfRange(ValueOutOfRange::msg("User Defined Field", newVal, 0, 15));
		
		_userDefinedField = newVal;
		_userDefinedFieldSetting = static_cast<int>(_userDefinedField);
	}
	
	/// Read-only accessor to _userDefinedField.
	ACE_UINT8 getUserDefinedField() const { return _userDefinedField; }

	/// Write-only accessor to _protocolIdExt and _protocolIdExtSetting.
	/// @throw ValueOutOfRange If newVal is not 0 - 15.
	void setProtocolIdExt(const ACE_UINT8& newVal) {
		if ( newVal > 0xF )
			throw ValueOutOfRange(ValueOutOfRange::msg("Protocol ID Extension", newVal, 0, 15));
			
		_protocolIdExt = newVal;
		_protocolIdExtSetting = static_cast<int>(_protocolIdExt);
	}	

	/// Read-only accessor to _protocolIdExt.
	ACE_UINT8 getProtocolIdExt() const { return _protocolIdExt;	}

	/// Write-only accessor to _ccsdsDefinedField and _ccsdsDefinedFieldSetting.
	void setCcsdsDefinedField(const ACE_UINT16& newVal) {
		_ccsdsDefinedField = newVal;
		_ccsdsDefinedFieldSetting = static_cast<int>(_ccsdsDefinedField);
	}

	/// Read-only accessor to _ccsdsDefinedField.
	ACE_UINT16 getCcsdsDefinedField() const {
		return _ccsdsDefinedField;
	}
	
private:
	/// Whether the Internet Protocol Extension is supported.
	bool _ipeSupport;
	
	/// Configuration file reference to _ipeSupport.
	Setting& _ipeSupportSetting;
	
	/// The value to set the IPE regardless of the type of wrapper.
	ACE_UINT64 _ipe;
	
	/// The value to set the protocol regardless of the type of wrapper.
	ACE_UINT8 _protocol;
	
	/// If _protocol is set, return that, otherwise return the provided value.
	/// @param autoVal A protocol value autodetected by the segment.
	ACE_UINT8 _pickProtocol(const ACE_UINT8 autoVal) const {
		return (( _protocol == unsetProtocolVal )? autoVal : _protocol);
	}
	
	/// User-specified 2-bit value to set the LoL field to, which corresponds to the length of the 'Packet Length' field.
	ACE_UINT8 _lengthOfLength;
	
	/// Configuration file reference to _lengthOfLength.
	Setting& _lengthOfLengthSetting;
	
	/// 4-bits that the user sets according to mission parameters.
	ACE_UINT8 _userDefinedField;
	
	/// Configuration file reference to _userDefinedField.
	Setting& _userDefinedFieldSetting;
	
	/// Can be used to identify the contents of the data field, but presently all values are reserved.
	ACE_UINT8 _protocolIdExt;
	
	/// Configuration file reference to _protocolIdExt.
	Setting& _protocolIdExtSetting;
	
	/// 16-bit field reserved for future use, normally set to 'all zeroes'.
	ACE_UINT16 _ccsdsDefinedField;

	/// Configuration file reference to _ccsdsDefinedField.
	Setting& _ccsdsDefinedFieldSetting;
	
}; // class modEncapPkt_Add

} // namespace nEncapPkt_Add

#endif // _MOD_ENCAP_PKT_ADD_
