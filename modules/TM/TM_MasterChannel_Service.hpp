/* -*- C++ -*- */

//=============================================================================
/**
 * @file   TM_MasterChannel_Service.hpp
 * @author Tad Kollar  
 *
 * $Id: TM_MasterChannel_Service.hpp 2405 2013-12-12 19:56:04Z tkollar $
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _TM_MASTER_CHANNEL_SERVICE_HPP_
#define _TM_MASTER_CHANNEL_SERVICE_HPP_

#include "TM_PhysicalChannel_Service.hpp"

namespace nasaCE {
//=============================================================================
/**
 * @class TM_MasterChannel_Service
 * @brief A BaseTrafficHandler with all possible settings for an TM Master Channel.
 */
//=============================================================================
class TM_MasterChannel_Service: public TM_PhysicalChannel_Service {
public:
	static const int32_t TFVN; /*!< Transfer Frame Version Number */

	/// Primary constructor.
	/// @param newName An identifier for the module that's unique within the channel.
	/// @param newChannelName An identifier for the containing channel.
	/// @param newDLLPtr Pointer to the DLL management structure.
	TM_MasterChannel_Service(const std::string& newName,
		const std::string& newChannelName,
		const CE_DLL* newDLLPtr);

	/// Write-only accessor to _SCID.
	/// @throw ValueTooLarge If newVal is larger than maxSpacecraftID.
	void setSCID(const int16_t& newVal) {
		if ( newVal > TM_Transfer_Frame::maxSpacecraftID )
			throw ValueTooLarge(ValueTooLarge::msg("SCID", newVal, TM_Transfer_Frame::maxSpacecraftID));
		_SCID = newVal;
		_SCIDSetting = newVal;
		updateTemplate_();
	}

	/// Read-only accessor to _SCID.
	u_int16_t getSCID() const { return _SCID; }

	/// Combine the TFVN and SCID into an Master Channel Identifier.
	u_int16_t getMCID() const { return ((TFVN << 10) | getSCID()); }

	/// Write-only accessor to _badMCIDCount.
	void setBadMCIDCount(const u_int64_t& newVal) { _badMCIDCount = newVal; }

	/// Read-only accessor to _badMCIDCount.
	u_int64_t getBadMCIDCount() const { return _badMCIDCount; }

	/// Increment _badMCIDCount by one.
	/// @return The new _badMCIDCount.
	u_int64_t incBadMCIDCount() { return ++_badMCIDCount; }

	/// Write-only accessor to _useFSH.
	void setUseFSH(const bool newVal) { _useFSH = newVal; _useFSHSetting = newVal; updateTemplate_(); }

	/// Read-only accessor to _useFSH.
	bool getUseFSH() const { return _useFSH; }

	/// Write-only accessor to _fshSize.
	/// @throw ValueTooLarge If newVal is larger than maxFSHLen.
	void setFSHSize(const int& newVal) {
		if ( newVal > TM_Transfer_Frame::maxFSHLen )
			throw ValueTooLarge(ValueTooLarge::msg("Frame Secondary Header Length", newVal, TM_Transfer_Frame::maxFSHLen));
		_fshSize = newVal;
		_fshSizeSetting = newVal;
	}

	/// Read-only accessor to _fshSize.
	ssize_t getFSHSize() const { return _fshSize; }

	/// Write-only accessor to _useOperationalControl.
	virtual void setUseOperationalControl(const bool newVal) {
		_useOperationalControl = newVal; _useOperationalControlSetting = newVal; updateTemplate_();
	}

	/// Read-only accessor to _useOperationalControl.
	bool getUseOperationalControl() const { return _useOperationalControl; }

	/// Write-only accessor to _syncFlag.
	virtual void setSyncFlag(const bool newVal) {
		_syncFlag = newVal; _syncFlagSetting = newVal; updateTemplate_();
	}

	/// Read-only accessor to _syncFlag.
	bool getSyncFlag() const { return _syncFlag; }

	/// Write-only accessor to _currentFrameNumber.
	void setCurrentFrameNumber(const u_int8_t& newVal) { _currentFrameNumber = newVal; }

	/// Read-only accessor to _currentFrameNumber.
	u_int8_t getCurrentFrameNumber() const { return _currentFrameNumber; }

	/// Increase the _currentFrameNumber by one unless it's maxed out; then set to zero.
	/// @param max The limit for _currentFrameNumber.
	u_int8_t incCurrentFrameNumber (const u_int8_t max = TM_Transfer_Frame::maxMCFrameCount) {
		if ( _currentFrameNumber == max ) _currentFrameNumber = 0;
		else ++_currentFrameNumber;
		return _currentFrameNumber;
	}

	/// Write-only accessor to _frameCountMisses.
	void setFrameCountMisses(const u_int64_t& newVal) { _frameCountMisses = newVal; }

	/// Read-only accessor to _frameCountMisses.
	u_int64_t getFrameCountMisses() const { return _frameCountMisses; }

	/// Increment _frameCountMisses by one.
	/// @return The new _frameCountMisses.
	u_int64_t incFrameCountMisses() { return ++_frameCountMisses; }

private:
	/// The Spacecraft Identifier.
	u_int8_t _SCID;

	/// Config file reference to _SCID.
	Setting& _SCIDSetting;

	/// The tally of frames that have an incorrect MCID.
	u_int64_t _badMCIDCount;

	/* --- Frame Secondary Header --- */
	/// Whether to use the Frame Secondary Header (FSH), or not.
	bool _useFSH;

	/// Config file reference to _useFSH.
	Setting& _useFSHSetting;

	/// The size of the Frame Secondary Header (FSH) if in use.
	int8_t _fshSize;

	/// Config file reference to _fshSize;
	Setting& _fshSizeSetting;

	/* --- Operation Control Field --- */
	/// Whether to use the Operational Control Field, or not.
	bool _useOperationalControl;

	/// Config file reference to _useOperationalControl.
	Setting& _useOperationalControlSetting;

	/* --- Other --- */
	/// Whether the Synchronization Flag is to be set or not.
	bool _syncFlag;

	/// Config file reference to _syncFlag.
	Setting& _syncFlagSetting;

	/// Corresponds to the 8-bit field in the TM Transfer Frame.
	u_int8_t _currentFrameNumber;

	/// Tally of mismatches between the tallied frame count and that in the frame.
	u_int64_t _frameCountMisses;
};

}

#endif // _TM_MASTER_CHANNEL_SERVICE_HPP_
