/* -*- C++ -*- */

//=============================================================================
/**
 * @file   BaseTask.cpp
 * @author Tad Kollar  
 *
 * $Id: BaseTask.cpp 2175 2013-06-20 14:28:11Z tkollar $
 * Copyright (c) 2010.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "BaseTask.hpp"

namespace nasaCE {

const int BaseTask::maxThreads = 1;
const timespec_t BaseTask::hundredMSec = { 0, 100000000 };

BaseTask::BaseTask(const std::string& newName /* = "" */):
    	name_(newName),
    	shutdown_(true),
    	svcThreadID_(0),
    	svcDone_(true),
    	mask_(0),
    	handle_(ACE_INVALID_HANDLE)
		#ifdef DEFINE_DEBUG
		,debugging_(globalCfg.debug)
		#endif
    	 {

}

BaseTask::~BaseTask() {
	if ( mask_ > 0 )
		MOD_WARNING("Still registered for reactor events during destruction! Mask is %X.", mask_);
}

int BaseTask::svc() { return 0; }


void BaseTask::stopTraffic(bool shutdown /* = true */) {

	if ( shutdown_ ) {
		while ( ! svcDone_ ) {
			MOD_DEBUG("stopTraffic: Waiting for previous stopTraffic call to complete.");
			ACE_OS::nanosleep(&hundredMSec, 0);
		}
		MOD_DEBUG("stopTraffic: Service already stopped.");
		return;
	}

	MOD_DEBUG("Stopping traffic.");

	if (shutdown) shutdown_ = true;

	if ( ! msg_queue()->deactivated() ) {
		MOD_DEBUG("Closing message queue.");
		msg_queue()->close();
	}

	if ( svcThreadID_ ) {

		// Due to the sleeping that can happen in svc() if the link is down,
		// we may have to manually cancel that thread from this one. This loop
		// gives svc() one second to finish on its own before being nuked.

		for ( int loopCount = 0; ! svcDone_ && loopCount < 10; ++loopCount ) {
			ACE_OS::nanosleep(&hundredMSec, 0);
		}

		if ( ! svcDone_ ) {
			MOD_WARNING("Service thread %Q did not exit on its own, trying cancel.", svcThreadID_);
			ACE_Thread_Manager::instance()->cancel(svcThreadID_);
		}
		else {
			MOD_INFO("Service thread %Q was able to exit on its own, joining.", svcThreadID_);
			ACE_Thread_Manager::instance()->join(svcThreadID_);
		}
	}

	svcDone_ = true; // make doubly sure this is set for modules that don't use svc()

}

std::string BaseTask::getName() const { return name_; }

bool BaseTask::acceptingData() { return ! ( shutdown_ || msg_queue()->deactivated() ); }

bool BaseTask::continueService() const {
	return ( ! shutdown_ && ! svcDone_ && ! exit_requested );
}

#ifdef DEFINE_DEBUG
void BaseTask::setDebugging(const bool newVal) { debugging_ = newVal; }

bool BaseTask::getDebugging() const { return debugging_; }
#endif

void BaseTask::svcStart_() {
	MOD_DEBUG("Entering svc thread.");
	svcThreadID_ = ACE_Thread::self();
	svcDone_ = false;
}

int BaseTask::svcEnd_() {
	MOD_DEBUG("Exiting svc thread.");
	svcDone_ = true;
	ACE_Thread::exit();

	return 0;
}

int BaseTask::open (void* /* = 0 */) {
	if ( ! svcDone_ ) {
		MOD_DEBUG("The service thread is already running (svcDone_ is false).",
			getName().c_str());
		return 0;
	}

	shutdown_ = false;
	// return activate(THR_JOINABLE, MAX_THREADS);
	msg_queue()->open(msg_queue()->high_water_mark(), msg_queue()->low_water_mark());
	return activate(THR_JOINABLE | THR_SCOPE_SYSTEM, maxThreads);
}

int BaseTask::handle_close(ACE_HANDLE fd, ACE_Reactor_Mask close_mask) {
	clearHandlerBits(close_mask);

	return -1;
}

void BaseTask::setHandlerBits(const ACE_Reactor_Mask maskBits) {
	ACE_SET_BITS(mask_, maskBits);

}

void BaseTask::clearHandlerBits(const ACE_Reactor_Mask maskBits) {
	ACE_CLR_BITS(mask_, maskBits);
}

ACE_HANDLE BaseTask::get_handle(void) const { return handle_; }

void BaseTask::set_handle(ACE_HANDLE newVal) { handle_ = newVal; }

void BaseTask::unset_handle() { handle_ = ACE_INVALID_HANDLE; }

}
