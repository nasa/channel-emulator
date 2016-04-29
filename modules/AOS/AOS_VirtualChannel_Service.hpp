/* -*- C++ -*- */

//=============================================================================
/**
 * @file   AOS_VirtualChannel_Service.hpp
 * @author Tad Kollar  
 *
 * $Id: AOS_VirtualChannel_Service.hpp 2405 2013-12-12 19:56:04Z tkollar $
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _AOS_VIRTUAL_CHANNEL_SERVICE_HPP_
#define _AOS_VIRTUAL_CHANNEL_SERVICE_HPP_

#include "AOS_MasterChannel_Service.hpp"

namespace nasaCE {
//=============================================================================
/**
  * @class AOS_VirtualChannel_Service
  * @brief Store all possible settings for an AOS Virtual Channel.
  *
  * Each datum is stored in a StatefulValue, which manages the real type as well
  * as maintaining whether and when it's been set.
  */
//=============================================================================
class AOS_VirtualChannel_Service: public AOS_MasterChannel_Service {
public:
	/// @enum ServiceTypes
	/// @brief The services that a virtual channel may be configured to carry.
	enum ServiceTypes {
		ServicePacket, /*!< The VC carries transfer frames containing M_PDUs */
		ServiceBitstream, /*!< The VC carries transfer frames containing B_PDUs */
		ServiceAccess, /*!< The VC carries transfer frames containing VCA_SDUs */
		ServiceIdle,  /*!< The VC carries Idle frames */
		ServiceInvalid /*!< An improper setting */
	};

	/// Primary constructor.
	/// @param newName An identifier for the module that's unique within the channel.
	/// @param newChannelName An identifier for the containing channel.
	/// @param newDLLPtr Pointer to the DLL management structure.
	AOS_VirtualChannel_Service(const std::string& newName,
		const std::string& newChannelName,
		const CE_DLL* newDLLPtr);

	/// Write-only accessor to _useOperationalControl.
	virtual void setUseOperationalControl(const bool newVal);

	/// Read-only accessor to _useOperationalControl.
	bool getUseOperationalControl() const;

	/// Write-only accessor to _VCID.
	void setVCID(const int32_t newVal);

	/// Read-only accessor to _VCID.
	int8_t getVCID() const;

	/// Write-only accessor to _vcFrameCount.
	void setFrameCount(const int64_t newVal);

	/// Read-only accessor to _vcFrameCount.
	int64_t getFrameCount() const;

	/// Increment _vcFrameCount by one.
	/// @return The new _vcFrameCount.
	int64_t incFrameCount();

	/// Write-only accessor to _useVCFrameCycle.
	void setUseVCFrameCycle(const bool newVal);

	/// Read-only accessor to _useVCFrameCycle.
	bool getUseVCFrameCycle() const;

	/// Write-only accessor to _vcFrameCountCycle.
	void setFrameCountCycle(const int32_t newVal);

	/// Read-only accessor to _vcFrameCountCycle.
	int8_t getFrameCountCycle() const;

	/// Increment _frameCountCycle by one.
	/// @return The new _frameCountCycle.
	int8_t incFrameCountCycle();

	/// Combine the MCID and _VCID into the GVCID.
	uint16_t getGVCID() const;

	/// Write-only accessor to _badGVCIDCount.
	void setBadGVCIDCount(const uint64_t& newVal);

	/// Read-only accessor to _badGVCIDCount.
	uint64_t getBadGVCIDCount() const;

	/// Increment _badGVCIDCount by one.
	/// @return The new _badGVCIDCount.
	uint64_t incBadGVCIDCount();

	/// Write-only accessor for _serviceType.
	void setServiceType(const ServiceTypes newVal);

	/// Read-only accessor for _serviceType.
	ServiceTypes getServiceType() const;

	/// Convert an enum value to a string.
	static std::string serviceType2Str(const ServiceTypes svc);

	/// Convert a string to a value in the ServiceTypes enum.
	static ServiceTypes str2ServiceType(const std::string svcStr);

	/// Write-only accessor for _serviceType.
	void setServiceType(const std::string& newVal);

	/// Read-only accessor for _serviceType.
	std::string getServiceTypeStr() const;

	/// Write-only accessor to _frameCountMisses.
	void setFrameCountMisses(const u_int64_t& newVal);

	/// Read-only accessor to _frameCountMisses.
	u_int64_t getFrameCountMisses() const;

	/// Increment _frameCountMisses by one.
	/// @return The new _frameCountMisses.
	u_int64_t incFrameCountMisses();

private:
	/* --- Operation Control Field --- */
	/// Whether to use the Operational Control Field, or not.
	Setting& _useOperationalControl;

	/* --- Identifiers --- */
	/// The Virtual Channel Identifier.
	Setting& _VCID;

	/// What type of data to expect as a payload.
	Setting& _serviceTypeStr;

	/* --- Frame counting --- */
	/// Whether to use the Virtual Channel frame cycle counter or not.
	Setting& _useVCFrameCycle;

	/// The number of the current frame.
	uint64_t _vcFrameCount;

	/// The number of times that the frame counter has flipped.
	uint8_t _vcFrameCountCycle;

	/// The tally of frames that have an incorrect GVCID.
	uint64_t _badGVCIDCount;

	/// Tally of mismatches between the tallied frame count and that in the frame.
	u_int64_t _frameCountMisses;
};


} // namespace nasaCE

#endif // _AOS_VIRTUAL_CHANNEL_SERVICE_HPP_
