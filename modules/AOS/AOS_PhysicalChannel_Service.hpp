/* -*- C++ -*- */

//=============================================================================
/**
 * @file   AOS_PhysicalChannel_Service.hpp
 * @author Tad Kollar  
 *
 * $Id: AOS_PhysicalChannel_Service.hpp 2331 2013-08-26 18:42:47Z tkollar $
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _AOS_PHYSICAL_CHANNEL_SETTINGS_
#define _AOS_PHYSICAL_CHANNEL_SETTINGS_

#include "AOS_Transfer_Frame.hpp"
#include "CE_Config.hpp"
#include "nd_macros.hpp"
#include "PeriodicTrafficHandler.hpp"

#include <ace/OS_main.h>
#include <ace/Basic_Types.h>
#include <string>

namespace nasaCE {

//=============================================================================
/**
  * @class AOS_PhysicalChannel_Service
  * @brief A BaseTrafficHandler with all possible settings for an AOS Physical Channel.
  */
//=============================================================================
class AOS_PhysicalChannel_Service: public PeriodicTrafficHandler {
public:
	/// Primary constructor.
	/// @param newName An identifier for the module that's unique within the channel.
	/// @param newChannelName An identifier for the containing channel.
	/// @param newDLLPtr Pointer to the DLL management structure.
    AOS_PhysicalChannel_Service(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr);

	/// Destructor
    virtual ~AOS_PhysicalChannel_Service();

	/// Write-only accessor to _frameSize.
	void setFrameSize(const int newVal) { _frameSize = newVal; rebuildPersistentUnits_(); }

	/// Read-only accessor to _frameSize.
	int getFrameSize() const { return _frameSize; }

	/// Read-only accessor to _frameSize returned as an unsigned value.
	size_t getFrameSizeU() const { return static_cast<size_t>(getFrameSize()); }

	/// Write-only accessor to _useHeaderErrorControl.
	void setUseHeaderErrorControl(const bool newVal) { _useHeaderErrorControl = newVal; rebuildPersistentUnits_(); }

	/// Read-only accessor to _useHeaderErrorControl.
	bool getUseHeaderErrorControl() const { return _useHeaderErrorControl;	}

	/// Write-only accessor to _useFrameErrorControl.
	void setUseFrameErrorControl(const bool newVal) { _useFrameErrorControl = newVal; rebuildPersistentUnits_(); }

	/// Read-only accessor to _useFrameErrorControl.
	bool getUseFrameErrorControl() const { return _useFrameErrorControl; }

	/// Write-only accessor to _pcInsertZoneSize.
	virtual void setInsertZoneSize(const int32_t newVal) {
		_pcInsertZoneSize = newVal;
		updateTemplate_();
	}

	/// Read-only accessor to _pcInsertZoneSize.
	/// @return Value of _pcInsertZoneSize if _usePCInsertZone is true; otherwise, 0.
	virtual ssize_t getInsertZoneSize() const {
		return static_cast<int32_t>(_pcInsertZoneSize);
	}

	/// Write-only accessor to _dropBadFrames.
	void setDropBadFrames(const bool newVal) { _dropBadFrames = newVal; _dropBadFramesSetting = newVal; }

	/// Read-only accessor to _dropBadFrames.
	bool getDropBadFrames() const { return _dropBadFrames; }

	/// Write-only accessor to _validFrameCount.
	void setValidFrameCount(const uint64_t newVal) { _validFrameCount = newVal; }

	/// Read-only accessor to _validFrameCount.
	uint64_t getValidFrameCount() const { return _validFrameCount; }

	/// Increment _validFrameCount by one.
	/// @return The new _validFrameCount.
	uint64_t incValidFrameCount() { return (_validFrameCount = getValidFrameCount() + 1); }

	/// Write-only accessor to _badLengthCount.
	void setBadLengthCount(const uint64_t newVal) { _badLengthCount = newVal; }

	/// Read-only accessor to _badLengthCount.
	uint64_t getBadLengthCount() const { return _badLengthCount; }

	/// Increment _badLengthCount by one.
	/// @return The new _badLengthCount.
	uint64_t incBadLengthCount() { return (_badLengthCount = getBadLengthCount() + 1); }

	/// Write-only accessor to _badTFVNCount.
	void setBadTFVNCount(const uint64_t newVal) { _badTFVNCount = newVal; }

	/// Read-only accessor to _badTFVNCount.
	uint64_t getBadTFVNCount() const { return _badTFVNCount; }

	/// Increment _badTFVNCount by one.
	/// @return The new _badTFVNCount.
	uint64_t incBadTFVNCount() { return (_badTFVNCount = getBadTFVNCount() + 1); }

	/// Set the contents of idlePattern_.
	/// @param idleBuf The buffer holding the new pattern.
	/// @param bufLen The length of the pattern in the buffer.
	void setIdlePattern(const uint8_t* idleBuf, const int bufLen);

	/// Set the contents of idlePattern_.
	/// @param idlePattern A vector containing the new pattern.
	void setIdlePattern(const std::vector<uint8_t>& idlePattern);

	/// Return the contents of idlePattern_.
	/// @param idlePattern A vector to copy idlePattern_ into.
	void getIdlePattern(std::vector<uint8_t>& idlePattern) const;

	/// Return a key to the AOS_PhysicalChannel section in the channel configuration.
	std::string pcKey(const std::string& key) const { return channelKey() + ".AOS_PhysicalChannel." + key; }

	/// Redefined by subclasses that need periodicity.
	NetworkData* getIdleUnit() { return 0; }

	/// Make sure the Transfer Frame is valid.
	/// @param aos Pointer to the Transfer Frame to test.
	/// @param deleteBadFrame If true, delete an invalid frame.
	/// @return True if the frame checks out, false if unusable.
	virtual bool validateFrame(AOS_Transfer_Frame* aos, const bool deleteBadFrame = false);

protected:

	/// When settings change that affect the structure of the transfer frame, rebuild
	/// the templates that can be stored by subclasses.
	virtual void rebuildPersistentUnits_() { updateTemplate_(); rebuildIdleUnitTemplate_(); }

	virtual void updateTemplate_() { }

	/// Redefined by subclasses that need periodicity.
	void rebuildIdleUnitTemplate_() { }

	/// The project-specific pattern to put into the data field for idle frames.
	Setting& idlePattern_;


private:
	/// Every transfer frame in the Physical Channel is the same length.
	Setting& _frameSize;

	/// All frames in the Physical Channel use error control in the header or not.
	Setting& _useHeaderErrorControl;

	/// All frames in the Physical Channel use the frame error control field, or not.
	Setting& _useFrameErrorControl;

	/// All frames in the Physical Channel have the same size Insert Zone.
	Setting& _pcInsertZoneSize;

	/// If true, stop processing frames that are found to be invalid for any reason.
	bool _dropBadFrames;

	/// Config file reference to _dropBadFrames.
	Setting& _dropBadFramesSetting;

	/* --- Counters --- */
	/// Tally of good frames that have passed through this service.
	uint64_t _validFrameCount;

	/// Tally of frames rejected for having an invalid length.
	uint64_t _badLengthCount;

	/// Tally of frames with incorrect Transfer Frame Version Number.
	uint64_t _badTFVNCount;
};

} // namespace nasaCE

#endif // _AOS_PHYSICAL_CHANNEL_SETTINGS_
