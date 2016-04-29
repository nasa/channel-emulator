/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modEmulateRate.cpp
 * @author Tad Kollar  
 *
 * $Id: modEmulateRate.cpp 2344 2013-08-30 20:24:16Z tkollar $
 * Copyright (c) 2010.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================


#include "modEmulateRate.hpp"
#include "SettingsManager.hpp"
#include <ace/Reactor.h>
#include <cmath>
#include <cstdlib>

namespace nEmulateRate {

modEmulateRate::modEmulateRate(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr):
	BaseTrafficHandler(newName, newChannelName, newDLLPtr),
	_rateLimit(0),
	_rateLimitSetting(CEcfg::instance()->getOrAddInt(cfgKey("rateLimit"), _rateLimit)),
	_unlimitedThroughput(0),
	_unlimitedThroughputSetting(CEcfg::instance()->getOrAddInt(cfgKey("unlimitedThroughput"), _unlimitedThroughput)),
	_rateTimeLine(0),
	_timeLineCycleSecs(0.0),
	_timeLineCycleSecsSetting(CEcfg::instance()->getOrAddFloat(cfgKey("timeLineCycleSecs"), _timeLineCycleSecs)) {

	try { setRateLimit(_rateLimitSetting); }
	catch (const ValueTooSmall& e) { setRateLimit(_rateLimit); }

	try { setUnlimitedThroughput(_unlimitedThroughputSetting); }
	catch (const ValueTooSmall& e) { setUnlimitedThroughput(_unlimitedThroughput); }

	if (CEcfg::instance()->exists(cfgKey("timeLine"))) {
		Setting& timeLineSetting = CEcfg::instance()->get(cfgKey("timeLine"));
		_rateTimeLine = new TimeLine<int>(_rateLimit);
		ACE_Time_Value timePoint;

		for ( int idx = 0; idx < timeLineSetting.getLength(); ++idx ) {
			double startTime = timeLineSetting[idx][0];
			int rate = timeLineSetting[idx][1];

			timePoint.set(startTime);

			try { _rateTimeLine->append(timePoint, rate); }
			catch (const AppendFailed& e) {
				continue;
			}
		}
	}

	try { setTimeLineCycleSecs(_timeLineCycleSecsSetting); }
	catch (const ValueTooSmall& e) { setTimeLineCycleSecs(_timeLineCycleSecs); }
}

modEmulateRate::~modEmulateRate() {
	delete _rateTimeLine;
}

void modEmulateRate::setRateLimit(const int& newVal, const double& startTime /* = -1.0 */) {
	if ( newVal < 0 ) throw ValueTooSmall(ValueTooSmall::msg("Rate Limit", newVal, 0));

	// A start time was given, modify the time line instead of _rateLimit.
	if ( startTime > 0.0 ) {
		Setting& timeLineSetting = CEcfg::instance()->getOrAddList(cfgKey("timeLine"));

		// If necessary allocate a new time line with the existing _rateLimit as a default.
		if ( ! _rateTimeLine ) {
			_rateTimeLine = new TimeLine<int>(_rateLimit);

			// Add the default value to the config file.
			/*
			Setting& defaultPair = timeLineSetting.add(Setting::TypeList);
			defaultPair.add(Setting::TypeFloat) = 0.0;
			defaultPair.add(Setting::TypeInt) = _rateLimit;
			*/
		}

		_rateTimeLine->append(ACE_Time_Value(startTime), newVal);

		// Add the new values to the config file.
		Setting& newPair = timeLineSetting.add(Setting::TypeList);
		newPair.add(Setting::TypeFloat) = startTime;
		newPair.add(Setting::TypeInt) = newVal;
	}
	else {
		_rateLimit = newVal;
		_rateLimitSetting = static_cast<int>(_rateLimit);
	}
}

int modEmulateRate::svc() {
	svcStart_();
	timespec_t remainingTime;

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

		const double bitLen = data->getUnitLength() * 8.0;

		if ( links_[PrimaryOutputLink] ) {
			MOD_DEBUG("Sending %d octets.", data->getUnitLength());
			links_[PrimaryOutputLink]->send(data);
		}
		else {
			MOD_NOTICE("No output target defined yet, dropping data.");
			ndSafeRelease(data);
		}

		if ( getRateLimit() > 0 ) {
			const double throughputBits = getUnlimitedThroughput() * 1024.0;

			// Estimate how long the unit would take to transmit without intervention
			const double naturalSeconds = (throughputBits > 0.0)? bitLen / throughputBits : 0.0;

			// Now figure out how long to wait in addition to that
			const double sleepSeconds = ( bitLen / ( 1024.0 * getRateLimit()) ) - naturalSeconds;

			// Put it in a timespec_t for nanosleep()'s benefit
			const time_t wholeSeconds = floor(sleepSeconds);
			const long nanoSeconds = (sleepSeconds - wholeSeconds) * 1000000000;
			timespec_t sleepTime = { wholeSeconds, nanoSeconds };
			MOD_DEBUG("Sleeping %d.%Q seconds (%.6e due to unlimitedThroughput setting).",
				wholeSeconds, nanoSeconds, naturalSeconds);

			if ( ACE_OS::nanosleep(&sleepTime, &remainingTime) < 0 ) {
				MOD_WARNING("nanosleep interrupted with %d.%Qs remaining.", remainingTime.tv_sec,
					remainingTime.tv_nsec);
			}
		}
	}

	return svcEnd_();

}

void modEmulateRate::clearTimeLine() {
	if ( ! _rateTimeLine ) return;

	TimeLine<int>* temp = _rateTimeLine;
	_rateTimeLine = 0;

	temp->reset();
	delete temp;

	CEcfg::instance()->remove(cfgKey("timeLine"));
}

}

