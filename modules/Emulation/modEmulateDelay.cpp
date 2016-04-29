/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modEmulateDelay.cpp
 * @author Tad Kollar  
 *
 * $Id: modEmulateDelay.cpp 2344 2013-08-30 20:24:16Z tkollar $
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "modEmulateDelay.hpp"
#include "SettingsManager.hpp"
#include <ace/Reactor.h>
#include <cmath>
#include <cstdlib>

namespace nEmulateDelay {



modEmulateDelay::modEmulateDelay(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr):
	BaseTrafficHandler(newName, newChannelName, newDLLPtr),
	_delaySeconds(0.0),
	_delaySecondsSetting(CEcfg::instance()->getOrAddFloat(cfgKey("delaySeconds"), _delaySeconds)),
	_jitterSeconds(0.0),
	_jitterSecondsSetting(CEcfg::instance()->getOrAddFloat(cfgKey("jitterSeconds"), _jitterSeconds)),
	_allowJitterReorder(false),
	_allowJitterReorderSetting(CEcfg::instance()->getOrAddBool(cfgKey("allowJitterReorder"), _allowJitterReorder)),
	_expectedKbits(0),
	_expectedKbitsSetting(CEcfg::instance()->getOrAddInt(cfgKey("expectedKbits"), _expectedKbits)),
	_useTimeStamp(false),
	_useTimeStampSetting(CEcfg::instance()->getOrAddBool(cfgKey("useTimeStamp"), _useTimeStamp)),
	_timeLine(0),
	_timeLineCycleSecs(0.0),
	_timeLineCycleSecsSetting(CEcfg::instance()->getOrAddFloat(cfgKey("timeLineCycleSecs"), _timeLineCycleSecs)) {

	_delaySeconds = ( static_cast<double>(_delaySecondsSetting) < 0.0 )? 0.0 : _delaySecondsSetting;
	_jitterSeconds = ( static_cast<double>(_jitterSecondsSetting) < 0.0 )? 0.0 : _jitterSecondsSetting;
	setAllowJitterReorder(_allowJitterReorderSetting);
	setExpectedKbits(_expectedKbitsSetting);
	setUseTimeStamp(_useTimeStampSetting);

	try { setTimeLineCycleSecs(_timeLineCycleSecsSetting); }
	catch (const ValueTooSmall& e) { setTimeLineCycleSecs(_timeLineCycleSecs); }

	if ( getExpectedKbits() > 0 && (getDelaySeconds() > 0.0 || getJitterSeconds() > 0.0 )) {
		int maxDelaySec = (int) ceil(getDelaySeconds() + getJitterSeconds());
		int BytesPerSec = (static_cast<int>(getExpectedKbits()) / 8) * 1000;

		int BDProduct = BytesPerSec * maxDelaySec;
		MOD_INFO("Computed Bandwidth Delay Product to be %d MB.", BDProduct / (1024*1024));

		if ( static_cast<int>(CEcfg::instance()->get(cfgKey("lowWaterMark"))) == defaultLowWaterMark )
			msg_queue()->low_water_mark(BDProduct);

		if ( static_cast<int>(CEcfg::instance()->get(cfgKey("highWaterMark"))) == defaultHighWaterMark )
			msg_queue()->high_water_mark(BDProduct);
	}

	if (CEcfg::instance()->exists(cfgKey("timeLine"))) {
		Setting& timeLineSetting = CEcfg::instance()->get(cfgKey("timeLine"));
		DelayAndJitterT defaultVals(_delaySeconds, _jitterSeconds);
		_timeLine = new TimeLine<DelayAndJitterT>(defaultVals);

		for ( int idx = 0; idx < timeLineSetting.getLength(); ++idx ) {
			double startTime = timeLineSetting[idx][0];
			double delay = timeLineSetting[idx][1];
			double jitter = timeLineSetting[idx][2];

			DelayAndJitterT newVals(delay, jitter);

			try { _timeLine->append(ACE_Time_Value(startTime), newVals); }
			catch (const AppendFailed& e) {
				continue;
			}
		}
	}
}

modEmulateDelay::~modEmulateDelay()
{
	// ACE_Reactor::instance()->cancel_timer(this);
}

int modEmulateDelay::svc() {
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

		if ( links_[PrimaryOutputLink] ) {
			MOD_DEBUG("Sending %d bytes, delayed %fs, with up to %fs jitter .",
				data->getUnitLength(), getDelaySeconds(), getJitterSeconds());

			links_[PrimaryOutputLink]->send(data);
		}
		else {
			MOD_NOTICE("No output target defined yet, dropping data.");
			ndSafeRelease(data);
		}

	}

	return svcEnd_();

}

int modEmulateDelay::handle_timeout(const ACE_Time_Value &currentTime, const void *dataPtr) {
	NetworkData* data = static_cast<NetworkData*>(const_cast<void*>(dataPtr));
	MOD_DEBUG("Sending %d delayed octets.", getName().c_str(), data->getUnitLength());

	if ( shutdown_ ) {
		MOD_INFO("Module is shut down, dropping delayed data unit.");
		ndSafeRelease(data);
		return -1;
	}
	else if ( links_[PrimaryOutputLink] ) {
		MOD_DEBUG("Sending %d delayed octets.", data->getUnitLength());
		links_[PrimaryOutputLink]->send(data);
	}
	else {
		MOD_NOTICE("No output target defined yet, dropping data.");
		ndSafeRelease(data);
	}

	return 0;
}

int modEmulateDelay::putData(NetworkData* data,
	const HandlerLink::InputRank inputRank /* = HandlerLink::PrimaryInput */ ) {

	if ( ! data ) {
		MOD_ERROR("modEmulateDelay::putData() received a null data pointer.");
		return -1;
	}

	ACE_Message_Block* mblk = dynamic_cast<ACE_Message_Block*>(data);

	if ( ! mblk ) {
		MOD_ERROR("modEmulateDelay::putData() failed to cast from NetworkData* to ACE_Message_Block*.");
		return -1;
	}

	ACE_Time_Value delaySecs(0);

	delaySecs.set(getDelaySeconds() + (getJitterSeconds() * drand48()));

	MOD_DEBUG("Generating delay of %d.%ds for %d-octet %s.",
		delaySecs.sec(), delaySecs.usec(), data->getUnitLength(), data->typeStr().c_str());

	ACE_Time_Value sendTime((getUseTimeStamp()? data->getTimeStamp() : ACE_High_Res_Timer::gettimeofday_hr()) +
		delaySecs);

	mblk->msg_deadline_time(sendTime);

	ACE_Message_Queue<ACE_MT_SYNCH>* msgq =
		( inputRank == HandlerLink::PrimaryInput )? msg_queue() : getAuxQueue();

	if (getAllowJitterReorder()) return msgq->enqueue_deadline(mblk);

	return msgq->enqueue_tail(mblk);
}

std::pair<NetworkData*, int> modEmulateDelay::getData_(
	const HandlerLink::InputRank inputRank /* = HandlerLink::PrimaryInput */,
	ACE_Time_Value* timeout /* = 0 */) {

	ACE_Message_Block* mblk = 0;
	NetworkData* data = 0;
	int messages = 0;

	ACE_Message_Queue<ACE_MT_SYNCH>* msgq =
		( inputRank == HandlerLink::PrimaryInput )? msg_queue() : getAuxQueue();

	messages = getAllowJitterReorder() ? msgq->dequeue_deadline(mblk, timeout) :
		msgq->dequeue_head(mblk, timeout);

	if (mblk) {
		data = dynamic_cast<NetworkData*>(mblk);

		ACE_Time_Value sleepSecs = mblk->msg_deadline_time() - ACE_High_Res_Timer::gettimeofday_hr();
		MOD_DEBUG("Sleeping %d.%ds before sending this %s unit (deadline is %d.%d).",
			sleepSecs.sec(), sleepSecs.usec(), data->typeStr().c_str(),
			mblk->msg_deadline_time().sec(), mblk->msg_deadline_time().usec());

		_nanoSleepSecs = sleepSecs;
		if ( ACE_OS::nanosleep(&_nanoSleepSecs, &_remainingSleepSecs) < 0 && errno != EINVAL) {
			MOD_NOTICE("nanosleep() interrupted with %d.%Q remaining.",
				_remainingSleepSecs.tv_sec, _remainingSleepSecs.tv_nsec);
		}

	}

	if (data) {
		incReceivedUnitCount(1, inputRank);
		incReceivedOctetCount(data->getUnitLength(), inputRank);
	}

	return std::make_pair(data, messages);
}

void modEmulateDelay::setDelayAndJitter(const double& newDelay,
	const double& newJitter /* = 0.0 */, const double& startTime /* = -1.0 */ ) {

	if ( newDelay < 0 ) throw ValueTooSmall(ValueTooSmall::msg("Delay Seconds", newDelay, 0));
	if ( newJitter < 0 ) throw ValueTooSmall(ValueTooSmall::msg("Jitter Seconds", newJitter, 0));

	// If a start time was given, modify the time line instead of _delaySeconds/_jitterSeconds.
	if ( startTime > 0.0 ) {
		Setting& timeLineSetting = CEcfg::instance()->getOrAddList(cfgKey("timeLine"));

		// If necessary allocate a new time line with the existing _delaySeconds as a default.
		if ( ! _timeLine ) {
			DelayAndJitterT defaultVals(_delaySeconds, _jitterSeconds);
			_timeLine = new TimeLine<DelayAndJitterT>(defaultVals);

			// Add the default value to the config file.
			/*
			Setting& defaultPair = timeLineSetting.add(Setting::TypeList);
			defaultPair.add(Setting::TypeFloat) = 0.0;
			defaultPair.add(Setting::TypeFloat) = _delaySeconds;
			defaultPair.add(Setting::TypeFloat) = _jitterSeconds;
			*/
		}

		DelayAndJitterT newVals(newDelay, newJitter);
		_timeLine->append(ACE_Time_Value(startTime), newVals);

		// Add the new values to the config file.
		Setting& newPair = timeLineSetting.add(Setting::TypeList);
		newPair.add(Setting::TypeFloat) = startTime;
		newPair.add(Setting::TypeFloat) = newDelay;
		newPair.add(Setting::TypeFloat) = newJitter;
	}
	else {
		_delaySeconds = newDelay;
		_delaySecondsSetting = static_cast<double>(_delaySeconds);
		_jitterSeconds = newJitter;
		_jitterSecondsSetting = static_cast<double>(_jitterSeconds);
	}
}

void modEmulateDelay::clearTimeLine() {
	if ( ! _timeLine ) return;

	TimeLine<DelayAndJitterT>* temp = _timeLine;
	_timeLine = 0;

	temp->reset();
	delete temp;

	CEcfg::instance()->remove(cfgKey("timeLine"));
}

}
