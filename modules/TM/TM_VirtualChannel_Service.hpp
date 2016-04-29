/* -*- C++ -*- */

//=============================================================================
/**
 * @file   TM_VirtualChannel_Service.hpp
 * @author Tad Kollar  
 *
 * $Id: TM_VirtualChannel_Service.hpp 1670 2012-08-28 13:25:28Z tkollar $
 * Copyright (c) 2012.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _TM_VIRTUAL_CHANNEL_SERVICE_HPP_
#define _TM_VIRTUAL_CHANNEL_SERVICE_HPP_

#include "TM_MasterChannel_Service.hpp"

namespace nasaCE {
//=============================================================================
/**
  * @class TM_VirtualChannel_Service
  * @brief Store all possible settings for a TM Virtual Channel.
  *
  * Each datum is stored in a StatefulValue, which manages the real type as well
  * as maintaining whether and when it's been set.
  */
//=============================================================================
class TM_VirtualChannel_Service: public TM_MasterChannel_Service {
public:
	/// Primary constructor.
	/// @param newName An identifier for the module that's unique within the channel.
	/// @param newChannelName An identifier for the containing channel.
	/// @param newDLLPtr Pointer to the DLL management structure.
	TM_VirtualChannel_Service(const std::string& newName,
		const std::string& newChannelName,
		const CE_DLL* newDLLPtr);

	/// Write-only accessor to _VCID.
	/// @throw ValueTooLarge If the provided VCID is larger than maxVirtualChannelID.
	void setVCID(const u_int8_t& newVal) {
		if ( newVal > TM_Transfer_Frame::maxVirtualChannelID )
			throw ValueTooLarge(ValueTooLarge::msg("VCID", newVal, TM_Transfer_Frame::maxVirtualChannelID));
		_VCID = newVal;
		_VCIDSetting = newVal;
	}

	/// Read-only accessor to _VCID.
	int8_t getVCID() const { return _VCID & 7; }
	
	/// Combine the MCID and VCID into the GVCID.
	u_int16_t getGVCID() const { return ((getMCID() << 3) | getVCID()) & 0xffff; }

	/// Write-only accessor to _badGVCIDCount.
	void setBadVCIDCount(const u_int64_t& newVal) { _badVCIDCount = newVal; }

	/// Read-only accessor to _badGVCIDCount.
	u_int64_t getBadVCIDCount() const { return _badVCIDCount; }

	/// Increment _badGVCIDCount by one.
	/// @return The new _badGVCIDCount.
	u_int64_t incBadVCIDCount() { return ++_badVCIDCount; }

private:
	/// The Virtual Channel Identifier.
	u_int8_t _VCID;

	/// Config file reference to _VCID.
	Setting& _VCIDSetting;

	/// The tally of frames that have an incorrect VCID.
	u_int64_t _badVCIDCount;

};
} // namespace nasaCE

#endif // _TM_VIRTUAL_CHANNEL_SERVICE_HPP_
