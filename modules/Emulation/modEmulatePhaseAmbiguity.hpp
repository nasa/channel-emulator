/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modEmulatePhaseAmbiguity.hpp
 * @author Tad Kollar  
 *
 * $Id: modEmulatePhaseAmbiguity.hpp 1241 2011-05-13 19:40:17Z tkollar $ 
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _MOD_EMULATE_PHASE_AMBIGUITY_HPP_
#define _MOD_EMULATE_PHASE_AMBIGUITY_HPP_

#include <BaseTrafficHandler.hpp>
#include <ace/Event_Handler.h>

namespace nEmulatePhaseAmbiguity {

//=============================================================================
/**
 * @class modEmulatePhaseAmbiguity
 * @author Tad Kollar  
 * @brief Bit shifts and/or inverts incoming traffic.
 */
//=============================================================================
class modEmulatePhaseAmbiguity : public BaseTrafficHandler {

public:

	/// Primary constructor.
	/// @param newName An identifier for the module that's unique within the channel.
	/// @param newChannelName An identifier for the containing channel.
	/// @param newDLLPtr Pointer to the DLL management structure.
    modEmulatePhaseAmbiguity(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr);

	/// Destructor.
    ~modEmulatePhaseAmbiguity();
	
	/// Reads in data, possibly introduces error(s), and sends it to the output handler.
	int svc();

	/// Write-only accessor to _leftShiftBits and _leftShiftBitsSetting.
	void setLeftShiftBits(const int& newVal) {
		_leftShiftBits = newVal;
		_leftShiftBitsSetting = newVal;
	}
	
	/// Read-only accessor to _leftShiftBits.
	int getLeftShiftBits() const { return _leftShiftBits; }

	/// Write-only accessor to _invert and _invertSetting.
	void setInversion(const bool newVal) {
		_invert = newVal;
		_invertSetting = newVal;
	}
	
	/// Read-only accessor to _invert.
	bool getInversion() const { return _invert; }
	
	/// Read-only accessor to _maxUsecsForNewData.
	int getMaxUsecsForNewData() const {
		return _maxUsecsForNewData;
	}
	
	/// Write-only accessor to _waitForNewDataTime and _maxUsecsForNewData.
	void setMaxUsecsForNewData(const int newVal) {
		_maxUsecsForNewData = newVal;
		_waitForNewDataInterval.sec(newVal/1000000);
		_waitForNewDataInterval.usec(newVal%1000000);
	}
	
	/// Read-only accessor to _waitForNewDataInterval.
	ACE_Time_Value getWaitForNewDataInterval() const {
		return _waitForNewDataInterval;
	}
	
	/// Write-only accessor to _waitForNewDataTime and _maxUsecsForNewData.
	void setWaitForNewDataInterval(ACE_Time_Value& timeWait) {
		ACE_UINT64 totalUsecs;
		timeWait.to_usec(totalUsecs);
		
		_maxUsecsForNewData = static_cast<int>(totalUsecs);
		_waitForNewDataInterval = timeWait;
	}	
	
private:
	/// The number of bits to left shift by, 0 - 7.
	int _leftShiftBits;
	
	/// Representation of _leftShiftBits in the config file.
	Setting& _leftShiftBitsSetting;
	
	/// Whether or not to invert all the bits.
	bool _invert;
	
	/// Representation of _invert in the config file.
	Setting& _invertSetting;
	
	/// Time to wait for incoming data before sending what is already stored.
	ACE_Time_Value _waitForNewDataInterval;
	
	/// Representation of _waitForNewDataInterval in the config file; useds microseconds instead of ACE_Time_Value.
	Setting& _maxUsecsForNewData;
	
	/// Add _waitForNewDataInterval to the current time.
	ACE_Time_Value _getWaitUntilTime() const {
		return ACE_High_Res_Timer::gettimeofday_hr() + _waitForNewDataInterval;
	}

}; // class modEmulatePhaseAmbiguity

} // namespace nEmulatePhaseAmbiguity

#endif // _MOD_EMULATE_PHASE_AMBIGUITY_HPP_
