/* -*- C++ -*- */

//=============================================================================
/**
 * @file   AOS_MasterChannel_Service.hpp
 * @author Tad Kollar  
 *
 * $Id: AOS_MasterChannel_Service.hpp 2002 2013-05-31 18:03:06Z tkollar $
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _AOS_MASTER_CHANNEL_SERVICE_HPP_
#define _AOS_MASTER_CHANNEL_SERVICE_HPP_

#include "AOS_PhysicalChannel_Service.hpp"

namespace nasaCE {
//=============================================================================
/**
 * @class AOS_MasterChannel_Service
 * @brief A BaseTrafficHandler with all possible settings for an AOS Master Channel.
 */
//=============================================================================
class AOS_MasterChannel_Service: public AOS_PhysicalChannel_Service {
public:
	static int32_t TFVN; /*!< Transfer Frame Version Number */

	/// Primary constructor.
	/// @param newName An identifier for the module that's unique within the channel.
	/// @param newChannelName An identifier for the containing channel.
	/// @param newDLLPtr Pointer to the DLL management structure.
	AOS_MasterChannel_Service(const std::string& newName,
		const std::string& newChannelName,
		const CE_DLL* newDLLPtr);

	/// Write-only accessor to _SCID.
	void setSCID(const int8_t& newVal) { _SCID = newVal; updateTemplate_();	}

	/// Read-only accessor to _SCID.
	ACE_UINT8 getSCID() const { return static_cast<int>(_SCID) & 0XFF; }

	/// Combine the TFVN and SCID into an Master Channel Identifier.
	ACE_UINT16 getMCID() const { return ((TFVN << 8) | getSCID()) & 0x3FF; }

	/// Write-only accessor to _badMCIDCount.
	void setBadMCIDCount(const long long& newVal) { _badMCIDCount = newVal; }

	/// Read-only accessor to _badMCIDCount.
	long long getBadMCIDCount() const { return _badMCIDCount; }

	/// Increment _badMCIDCount by one.
	/// @return The new _badMCIDCount.
	long long incBadMCIDCount() { return (_badMCIDCount = getBadMCIDCount() + 1); }


private:
	/// The Spacecraft Identifier.
	Setting& _SCID;

	/// The tally of frames that have an incorrect MCID.
	uint64_t _badMCIDCount;
};

}

#endif // _AOS_MASTER_CHANNEL_SERVICE_HPP_
