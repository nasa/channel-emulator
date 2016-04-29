/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modEmulateBitErrors.cpp
 * @author Tad Kollar  
 *
 * $Id: modEmulateBitErrors.cpp 2344 2013-08-30 20:24:16Z tkollar $
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "modEmulateBitErrors.hpp"
#include <limits>
#include <cstdlib>

namespace nEmulateBitErrors {

modEmulateBitErrors::modEmulateBitErrors(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr):
	BaseTrafficHandler(newName, newChannelName, newDLLPtr),
	_errorProbability(0.0),
	_errorProbabilitySetting(CEcfg::instance()->getOrAddFloat(cfgKey("errorProbability"), _errorProbability)),
	_maxErrorsPerUnit(1),
	_maxErrorsPerUnitSetting(CEcfg::instance()->getOrAddInt(cfgKey("maxErrorsPerUnit"), _maxErrorsPerUnit)),
	_protectedHeaderBits(0),
	_protectedHeaderBitsSetting(CEcfg::instance()->getOrAddInt(cfgKey("protectedHeaderBits"), _protectedHeaderBits)),
	_protectedTrailerBits(0),
	_protectedTrailerBitsSetting(CEcfg::instance()->getOrAddInt(cfgKey("protectedTrailerBits"), _protectedTrailerBits)),
	_unitsWithErrors(0), _totalErrors(0), _timeLine(0),
	_timeLineCycleSecs(0.0),
	_timeLineCycleSecsSetting(CEcfg::instance()->getOrAddFloat(cfgKey("timeLineCycleSecs"), _timeLineCycleSecs)) {

	if ( getErrorProbability() > 1.0 ) setErrorProbability(1.0);
	else if ( getErrorProbability() < 0.0 ) setErrorProbability(0.0);
	else _errorProbability = _errorProbabilitySetting;

	_maxErrorsPerUnit = _maxErrorsPerUnitSetting;
	_protectedHeaderBits = _protectedHeaderBitsSetting;
	_protectedTrailerBits = _protectedTrailerBitsSetting;

	if (CEcfg::instance()->exists(cfgKey("timeLine"))) {
		Setting& timeLineSetting = CEcfg::instance()->get(cfgKey("timeLine"));
		ErrRateAndMaxT defaultVals(_errorProbability, _maxErrorsPerUnit);
		_timeLine = new TimeLine<ErrRateAndMaxT>(defaultVals);

		for ( int idx = 0; idx < timeLineSetting.getLength(); ++idx ) {
			double startTime = timeLineSetting[idx][0];
			double ber = timeLineSetting[idx][1];
			int max = timeLineSetting[idx][2];

			ErrRateAndMaxT newVals(ber, max);

			try { _timeLine->append(ACE_Time_Value(startTime), newVals); }
			catch (const AppendFailed& e) { continue; }
		}
	}

	try { setTimeLineCycleSecs(_timeLineCycleSecsSetting); }
	catch (const ValueTooSmall& e) { setTimeLineCycleSecs(_timeLineCycleSecs); }

}


modEmulateBitErrors::~modEmulateBitErrors()
{
}

int modEmulateBitErrors::svc() {
	svcStart_();

	while ( continueService() ) {
		std::pair<NetworkData*, int> queueTop = getData_();

		if ( msg_queue()->deactivated() ) break;

		if ( queueTop.second < 0 ) {
			MOD_ERROR("getData_() call failed.");
			continue;
		}
		else if ( ! queueTop.first ) {
			MOD_ERROR("getData_() returned with null data.");
			continue;
		}

		NetworkData* data = queueTop.first;

		MOD_DEBUG("Received %d bytes to possibly introduce bit errors into.", data->getUnitLength());

		if ( links_[PrimaryOutputLink] ) {

			bool madeError = false;

			for ( int error_tests = 0; error_tests < getMaxErrorsPerUnit(); ++error_tests ) {

				if ( drand48() <= getErrorProbability() ) {
					int errorZoneBitLen = ((data->getUnitLength() * 8) - getProtectedHeaderBits()) -
						getProtectedTrailerBits();

					// Randomly select which bit in the buffer to flip.
					unsigned long bitIdx = (lrand48() % errorZoneBitLen) + getProtectedHeaderBits();

					// Calculate the octet to place the bit error in.
					unsigned long byteIdx = bitIdx / 8;

					// Calculate the bit within the octet to flip.
					ACE_UINT8 bitWithinByteIdx = bitIdx % 8;

					MOD_INFO("Flipping bit %d of %d (#%d at octet index %d of %d).", bitIdx,
						data->getUnitLength()*8, bitWithinByteIdx, byteIdx, data->getUnitLength());

					*(data->ptrUnit() + byteIdx) ^=  (0x80 >> bitWithinByteIdx);

					if ( ! madeError ) {
						madeError = true;
						incUnitsWithErrors();
					}

					incTotalErrors();
				}
			}

			links_[PrimaryOutputLink]->send(data);
		}
		else {
			MOD_NOTICE("No output target defined yet, dropping data.");
			ndSafeRelease(data);
		}
	}

	return svcEnd_();

}

void modEmulateBitErrors::setErrorProbability(const double& newBer,
	const int& newMax /* = 1 */, const double& startTime /* = -1.0 */ ) {

	if ( newBer < 0 ) throw ValueTooSmall(ValueTooSmall::msg("Error Probability", newBer, 0));
	if ( newMax < 1 ) throw ValueTooSmall(ValueTooSmall::msg("Max Errors/Unit", newMax, 1));

	// If a start time was given, modify the time line instead of _delaySeconds/_jitterSeconds.
	if ( startTime > 0.0 ) {
		Setting& timeLineSetting = CEcfg::instance()->getOrAddList(cfgKey("timeLine"));

		// If necessary allocate a new time line with the existing _delaySeconds as a default.
		if ( ! _timeLine ) {
			ErrRateAndMaxT defaultVals(_errorProbability, _maxErrorsPerUnit);
			_timeLine = new TimeLine<ErrRateAndMaxT>(defaultVals);

			// Add the default value to the config file.
			/*
			Setting& defaultPair = timeLineSetting.add(Setting::TypeList);
			defaultPair.add(Setting::TypeFloat) = 0.0;
			defaultPair.add(Setting::TypeFloat) = _errorProbability;
			defaultPair.add(Setting::TypeInt) = _maxErrorsPerUnit;
			*/
		}

		ErrRateAndMaxT newVals(newBer, newMax);
		_timeLine->append(ACE_Time_Value(startTime), newVals);

		// Add the new values to the config file.
		Setting& newPair = timeLineSetting.add(Setting::TypeList);
		newPair.add(Setting::TypeFloat) = startTime;
		newPair.add(Setting::TypeFloat) = newBer;
		newPair.add(Setting::TypeInt) = newMax;
	}
	else {
		_errorProbability = newBer;
		_errorProbabilitySetting = static_cast<double>(_errorProbability);
		_maxErrorsPerUnit = newMax;
		_maxErrorsPerUnitSetting = static_cast<int>(_maxErrorsPerUnit);
	}
}


void modEmulateBitErrors::clearTimeLine() {
	if ( ! _timeLine ) return;

	TimeLine<ErrRateAndMaxT>* temp = _timeLine;
	_timeLine = 0;

	temp->reset();
	delete temp;

	CEcfg::instance()->remove(cfgKey("timeLine"));
}

}
