/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modEthFrame_Add.hpp
 * @author Tad Kollar  
 *
 * $Id: modEthFrame_Add.hpp 1205 2011-04-07 17:34:14Z tkollar $ 
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _MOD_ETH_FRAME_ADD_HPP_
#define _MOD_ETH_FRAME_ADD_HPP_

#include "BaseTrafficHandler.hpp"
#include "EthernetFrame.hpp"
#include "MacAddress.hpp"
#include <cctype>

namespace nEthFrame_Add {

/**
	@author Tad Kollar  
*/
class modEthFrame_Add : public BaseTrafficHandler
{
public:

	/// Primary constructor.
	/// @param newName An identifier for the module that's unique within the channel.
	/// @param newChannelName An identifier for the containing channel.
	/// @param newDLLPtr Pointer to the DLL management structure.
    modEthFrame_Add(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr);

	/// Destructor.
    ~modEthFrame_Add();

	/// Read in Ethernet Frames, extract the payload, and send it on.
	int svc();
	
	/// Write-only accessor to _frameType.
	void setFrameType(const EthernetFrame::SupportedFrameTypes newVal) {
		_frameType = frameType2Str(newVal);
	}
	
	/// Write-only accessor to _frameType.	
	void setFrameType(const std::string frameTypeStr) {
		switch (tolower(frameTypeStr[0])) {
			case 'p': _frameType = "PCap"; break;
			case 'l': _frameType = "LinuxTap"; break;
			case 'i': _frameType = "IEEE"; break;
			default: throw BadValue("Unrecognized value '" + frameTypeStr + "' provided for frame type.");
		}
	}

	/// Read-only accessor to _frameType.
	EthernetFrame::SupportedFrameTypes getFrameType() const {
		return str2FrameType(_frameType);
	}
	
	/// Read-only accessor to _frameType.
	std::string getFrameTypeStr() const { return _frameType;	}	
	
	/// Return the EthernetFrame::SupportedFrameTypes element that corresponds to the given string.
	/// @param frameTypeStr Either @c PCap, @c LinuxTap, or @c IEEE. Only the first letter is significant.
	static EthernetFrame::SupportedFrameTypes str2FrameType(const std::string frameTypeStr) {
		switch (tolower(frameTypeStr[0])) {
			case 'p': return EthernetFrame::PCap;
			case 'l': return EthernetFrame::LinuxTap;
			case 'i': return EthernetFrame::IEEE;
		}
		
		return EthernetFrame::PCap;
	}
	
	/// Return the string version of the provided frame type.
	/// @param frameType One of PCap, LinuxTap, IEEE.
	static std::string frameType2Str(const EthernetFrame::SupportedFrameTypes frameType) {
		switch (frameType) {
			case EthernetFrame::PCap: return "PCap";
			case EthernetFrame::LinuxTap: return "LinuxTap";
			case EthernetFrame::IEEE: return "IEEE";
		}
		
		return "PCap";
	}

	/// Write-only accessor to _dstMac.
	void setDstMAC(const MacAddress& newVal) { _dstMAC = newVal.addr2str();	_updateTemplate(); }
	
	/// Write-only string-based accessor to _dstMac.
	void setDstMACStr(const std::string& newVal) { _dstMAC = newVal; _updateTemplate(); }	
	
	/// Read-only accessor to _dstMac.
	MacAddress getDstMAC() const { std::string macStr = _dstMAC; return MacAddress(macStr); }
	
	/// Read-only string-based accessor to _dstMAC.
	std::string getDstMACStr() const { return _dstMAC; }

	/// Write-only accessor to _srcMac.
	void setSrcMAC(const MacAddress& newVal) { _srcMAC = newVal.addr2str(); _updateTemplate(); }

	/// Write-only string-based accessor to _srcMac.
	void setSrcMACStr(const std::string& newVal) { _srcMAC = newVal; _updateTemplate(); }	

	/// Read-only accessor to _srcMac.
	MacAddress getSrcMAC() const { std::string macStr = _srcMAC; return MacAddress(macStr); }
	
	/// Read-only string-based accessor to _srcMAC.
	std::string getSrcMACStr() const { return _srcMAC; }

	/// Write-only accessor to _defaultPayloadType.
	void setDefaultPayloadType(const ACE_UINT16 newVal) { _defaultPayloadType = static_cast<int>(newVal); }

	/// Read-only accessor to _defaultPayloadType.
	ACE_UINT16 getDefaultPayloadType() const { return static_cast<int>(_defaultPayloadType) & 0xFFFF; }
	
private:
	/// The type of EthernetFrame to generate.
	Setting& _frameType;
	
	/// The destination address to set on all outgoing frames.
	Setting& _dstMAC;
		
	/// The source address to set on all outgoing frames.
	Setting& _srcMAC;
	
	/// If the payload is unrecognized, use this type.
	Setting& _defaultPayloadType;
	
	/// Rebuild _frameHeader.
	void _updateTemplate(); 

	/// A preconfigured header to duplicate and make new frames from.
	EthernetFrame* _frameHeader;
	
	/// Restrict access to _frameHeader so it can be modified on the fly.
	ACE_Process_Mutex _frameHeaderCreation;
};

} // namespace nEthFrame_Add

#endif
