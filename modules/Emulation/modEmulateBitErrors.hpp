/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modEmulateBitErrors.hpp
 * @author Tad Kollar  
 *
 * $Id: modEmulateBitErrors.hpp 1578 2012-06-08 19:19:18Z tkollar $
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _MOD_EMULATE_BIT_ERRORS_HPP_
#define _MOD_EMULATE_BIT_ERRORS_HPP_

#include "BaseTrafficHandler.hpp"
#include "TimeLine.hpp"
#include "Time_Handler.hpp"

namespace nEmulateBitErrors {

typedef std::pair<double, int> ErrRateAndMaxT;

//=============================================================================
/**
 * @class modEmulateBitErrors
 * @author Tad Kollar  
 * @brief Potentially inject random errors into incoming data.
*/
//=============================================================================
class modEmulateBitErrors : public BaseTrafficHandler {

public:
	/// Primary constructor.
	/// @param newName An identifier for the module that's unique within the channel.
	/// @param newChannelName An identifier for the containing channel.
	/// @param newDLLPtr Pointer to the DLL management structure.
    modEmulateBitErrors(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr);

	/// Destructor.
    ~modEmulateBitErrors();

	/// Reads in data, possibly introduces error(s), and sends it to the output handler.
	int svc();

	/// Read-only accessor to _errorProbability.
	double getErrorProbability() const {
		return ( _timeLine ) ?
			_timeLine->get(scenarioTimeKeeper::instance()->elapsed(), _timeLineCycleSecs).first :
			_errorProbability;
	}

	/// Return the error probability at the specified time (relative to the start of the simulation).
	/// @param atTime The time value to check the BER for.
	/// @return The BER at the specified time.
	double getErrorProbabilityAtTime(const double& atTime) {
		return ( _timeLine) ?
			_timeLine->get(ACE_Time_Value(atTime), _timeLineCycleSecs).first :
			_errorProbability;
	}

	/// Read-only accessor to _maxErrorsPerUnit.
	double getMaxErrorsPerUnit() const {
		return ( _timeLine ) ?
			_timeLine->get(scenarioTimeKeeper::instance()->elapsed(), _timeLineCycleSecs).second :
			_maxErrorsPerUnit;
	}

	/// Return the max errors/unit at the specified time (relative to the start of the simulation).
	/// @param atTime The time value to check max errors for.
	/// @return Maximum allowed errors at the specified time.
	double getMaxErrorsPerUnitAtTime(const double& atTime) {
		return ( _timeLine) ?
			_timeLine->get(ACE_Time_Value(atTime), _timeLineCycleSecs).second :
			_maxErrorsPerUnit;
	}

	/// Write-only accessor to _errorProbability, _errorProbability, or _timeLine.
	/// @param newBer The new bit error rate.
	/// @param newMax The new maximum number of errors per unit.
	/// @param startTime The scenario time in seconds when the new BER should go into effect. If not provided or not > 0.0, the default BER is set instead.
	/// @throw ValueTooSmall If newBer is negative or newMax is less than 1.
	void setErrorProbability(const double& newBer, const int& newMax = 1, const double& startTime = -1.0 );

	/// Write-only accessor to _unitsWithErrors.
	void setUnitsWithErrors(int newVal) { _unitsWithErrors = newVal; }

	/// Increment _unitsWithErrors by one.
	void incUnitsWithErrors() { ++_unitsWithErrors; }

	/// Read-only accessor to _unitsWithErrors.
	int getUnitsWithErrors() const { return _unitsWithErrors; }

	/// Write-only accessor to _totalErrors.
	void setTotalErrors(int newVal) { _totalErrors = newVal; }

	/// Increment _totalErrors by one.
	void incTotalErrors() { ++_totalErrors; }

	/// Read-only accessor to _totalErrors.
	int getTotalErrors() const { return _totalErrors; }

	/// Write-only accessor to _protectedHeaderBits.
	void setProtectedHeaderBits(int newVal) {
		_protectedHeaderBits = newVal;
		_protectedHeaderBitsSetting = _protectedHeaderBits;
	}

	/// Read-only accessor to _protectedHeaderBits.
	int getProtectedHeaderBits() const { return _protectedHeaderBits; }

	/// Write-only accessor to _protectedTrailerBits.
	void setProtectedTrailerBits(int newVal) {
		_protectedTrailerBits = newVal;
		_protectedTrailerBitsSetting = _protectedTrailerBits;
	}

	/// Read-only accessor to _protectedTrailerBits.
	int getProtectedTrailerBits() const { return _protectedTrailerBits; }

	/// Read-only accessor to _timeLineCycleSecs.
	double getTimeLineCycleSecs() const { return _timeLineCycleSecs; }

	/// Write-only accessor to _timeLineCycleSecs and _timeLineCycleSecsSetting.
	/// @throw ValueTooSmall If newVal is negative.
	void setTimeLineCycleSecs(const double& newVal) {
		if ( newVal < 0 ) throw ValueTooSmall(ValueTooSmall::msg("Time Line Cycle Seconds", newVal, 0));

		_timeLineCycleSecs = newVal;
		_timeLineCycleSecsSetting = newVal;
	}

	/// Erase the delay time line and only use _delaySeconds.
	void clearTimeLine();

private:

	/// Double-precision floating point value between 0.0 and 1.0 that determines the chance
	/// for each data unit to have errors.
	double _errorProbability;

	/// Config file reference to _errorProbability.
	Setting& _errorProbabilitySetting;

	/// Maximum errors per unit; useful to provide limit for when _errorProbability is high.
	int _maxErrorsPerUnit;

	/// Config file reference to _maxErrorsPerUnit.
	Setting& _maxErrorsPerUnitSetting;

	/// The number of bits at the start of the unit that will not have errors introduced.
	int _protectedHeaderBits;

	/// Config file reference to _protectedHeaderBits.
	Setting& _protectedHeaderBitsSetting;

	/// The number of bits at the end of the unit that will not have errors introduced.
	int _protectedTrailerBits;

	/// Config file reference to _protectedTrailerBits.
	Setting& _protectedTrailerBitsSetting;

	/// The tally of units that have at least one error.
	int _unitsWithErrors;

	/// The tally of all errors introduced, including multiple per unit.
	int _totalErrors;

	/// List of time, BER/max per unit pairs that defines what BER applies starting at what time.
	TimeLine<ErrRateAndMaxT>* _timeLine;

	/// How often does the time line repeat.
	double _timeLineCycleSecs;

	/// Config file reference to _timeLineCycleLen.
	Setting& _timeLineCycleSecsSetting;
}; // class modEmulateBitErrors

} // namespace nEmulateBitErrors

#endif // _MOD_EMULATE_BIT_ERRORS_HPP_
