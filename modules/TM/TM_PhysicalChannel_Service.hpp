/* -*- C++ -*- */

//=============================================================================
/**
 * @file   TM_PhysicalChannel_Service.hpp
 * @author Tad Kollar  
 *
 * $Id: TM_PhysicalChannel_Service.hpp 2489 2014-02-13 16:44:28Z tkollar $
 * Copyright (c) 2012.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _TM_PHYSICAL_CHANNEL_SERVICE_
#define _TM_PHYSICAL_CHANNEL_SERVICE_

#include "TM_Transfer_Frame.hpp"
#include "CE_Config.hpp"
#include "nd_macros.hpp"
#include "PeriodicTrafficHandler.hpp"

#include <ace/OS_main.h>
#include <ace/Basic_Types.h>
#include <string>

namespace nasaCE {

//=============================================================================
/**
  * @class TM_PhysicalChannel_Service
  * @brief A BaseTrafficHandler with all possible settings for a TM Physical Channel.
  */
//=============================================================================
class TM_PhysicalChannel_Service: public PeriodicTrafficHandler {
public:
	/// Primary constructor.
	/// @param newName An identifier for the module that's unique within the channel.
	/// @param newChannelName An identifier for the containing channel.
	/// @param newDLLPtr Pointer to the DLL management structure.
    TM_PhysicalChannel_Service(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr);

	/// Destructor
    virtual ~TM_PhysicalChannel_Service();

	/// Write-only accessor to _frameSize.
	void setFrameSize(const int newVal) { _frameSize = newVal; rebuildPersistentUnits_(); }

	/// Read-only accessor to _frameSize.
	ssize_t getFrameSize() const { return _frameSize; }

	/// Read-only accessor to _frameSize returned as an unsigned value.
	size_t getFrameSizeU() const { return static_cast<size_t>(getFrameSize()); }

	/// Write-only accessor to _useFrameErrorControl.
	void setUseFrameErrorControl(const bool newVal) {
		_useFrameErrorControl = newVal;
		rebuildPersistentUnits_();
	}

	/// Read-only accessor to _useFrameErrorControl.
	bool getUseFrameErrorControl() const { return _useFrameErrorControl; }

	/// Write-only accessor to _dropBadFrames.
	void setDropBadFrames(const bool newVal) { _dropBadFrames = newVal; _dropBadFramesSetting = newVal; }

	/// Read-only accessor to _dropBadFrames.
	bool getDropBadFrames() const { return _dropBadFrames; }

	/// Write-only accessor to _validFrameCount.
	void setValidFrameCount(const u_int64_t newVal) { _validFrameCount = newVal; }

	/// Read-only accessor to _validFrameCount.
	u_int64_t getValidFrameCount() const { return _validFrameCount; }

	/// Increment _validFrameCount by one.
	/// @return The new _validFrameCount.
	u_int64_t incValidFrameCount() { return ++_validFrameCount; }

	/// Write-only accessor to _badFrameChecksumCount.
	void setBadFrameChecksumCount(const u_int64_t newVal) { _badFrameChecksumCount = newVal; }

	/// Read-only accessor to _badFrameChecksumCount.
	u_int64_t getBadFrameChecksumCount() const { return _badFrameChecksumCount; }

	/// Increment _badFrameChecksumCount by one.
	/// @return The new _badFrameChecksumCount.
	u_int64_t incBadFrameChecksumCount() { return ++_badFrameChecksumCount; }

	/// Write-only accessor to _badHeaderCount.
	void setBadHeaderCount(const u_int64_t newVal) { _badHeaderCount = newVal; }

	/// Read-only accessor to _badHeaderCount.
	u_int64_t getBadHeaderCount() const { return _badHeaderCount; }

	/// Increment _badHeaderCount by one.
	/// @return The new _badHeaderCount.
	u_int64_t incBadHeaderCount() { return ++_badHeaderCount; }

	/// Write-only accessor to _badLengthCount.
	void setBadLengthCount(const u_int64_t newVal) { _badLengthCount = newVal; }

	/// Read-only accessor to _badLengthCount.
	u_int64_t getBadLengthCount() const { return _badLengthCount; }

	/// Increment _badLengthCount by one.
	/// @return The new _badLengthCount.
	u_int64_t incBadLengthCount() { return ++_badLengthCount; }

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
	std::string pcKey(const std::string& key) const { return channelKey() + ".TM_PhysicalChannel." + key; }

	/// Redefined by subclasses that need periodicity.
	NetworkData* getIdleUnit() { return 0; }

	/// If configured to do so, add checksum symbols.
	/// @param frame Pointer to the Transfer Frame to modify.
	void addErrorControlIfPossible(TM_Transfer_Frame* frame);

	/// Make sure the Transfer Frame is valid, including performing error checking.
	/// All tests are skipped if _validateFrameHere is false.
	/// @param frame Pointer to the Transfer Frame to test.
	/// @param deleteBadFrame If true, delete an invalid frame.
	/// @return True if the frame checks out, false if unusable.
	bool validateFrame(TM_Transfer_Frame*& frame, const bool deleteBadFrame = false);

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

	/// All frames in the Physical Channel use the frame error control field, or not.
	Setting& _useFrameErrorControl;

	/// If true, stop processing frames that are found to be invalid for any reason.
	bool _dropBadFrames;

	/// Config file reference to _dropBadFrames.
	Setting& _dropBadFramesSetting;

	/* --- Counters --- */
	/// Tally of good frames that have passed through this service.
	u_int64_t _validFrameCount;

	/// Tally of frames rejected for having an invalid frame checksum.
	u_int64_t _badFrameChecksumCount;

	/// Tally of frames rejected for having uncorrectable errors in the header.
	u_int64_t _badHeaderCount;

	/// Tally of frames rejected for having an invalid length.
	u_int64_t _badLengthCount;

};

} // namespace nasaCE

#endif // _TM_PHYSICAL_CHANNEL_SERVICE_
