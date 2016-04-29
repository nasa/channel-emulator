/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modEthFrame_Remove.hpp
 * @author Tad Kollar  
 *
 * $Id: modEthFrame_Remove.hpp 1205 2011-04-07 17:34:14Z tkollar $ 
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _MOD_ETH_FRAME_REMOVE_HPP_
#define _MOD_ETH_FRAME_REMOVE_HPP_

#include "BaseTrafficHandler.hpp"
#include "EthernetFrame.hpp"

namespace nEthFrame_Remove {

/**
	@author Tad Kollar  
*/
class modEthFrame_Remove : public BaseTrafficHandler {
public:

	/// Primary constructor.
	/// @param newName An identifier for the module that's unique within the channel.
	/// @param newChannelName An identifier for the containing channel.
	/// @param newDLLPtr Pointer to the DLL management structure.
    modEthFrame_Remove(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr);

	/// Destructor
    ~modEthFrame_Remove();

	/// Read in Ethernet Frames, extract the payload, and send it on.
	int svc();
	
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

	/// Write-only accessor to _frameType.
	void setFrameType(const EthernetFrame::SupportedFrameTypes newVal) {
		_frameType = newVal;
	}

	/// Read-only accessor to _frameType.
	EthernetFrame::SupportedFrameTypes getFrameType() const {
		return _frameType;
	}
	
	/// Read-only accessor to _frameType.
	std::string getFrameTypeStr() const {
		return frameType2Str(_frameType);
	}	

private:
	/// The type of EthernetFrame we should expect to receive.
	EthernetFrame::SupportedFrameTypes _frameType;

}; // class modEthFrame_Remove

} // namespace nEthFrame_Remove

#endif // _MOD_ETH_FRAME_REMOVE_HPP_
