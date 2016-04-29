/* -*- C++ -*- */

//=============================================================================
/**
 * @file   devSink.cpp
 * @author Tad Kollar  
 *
 * $Id: devSink.cpp 2182 2013-06-20 16:39:13Z tkollar $
 * Copyright (c) 2011.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "devSink.hpp"
#include "SettingsManager.hpp"
#include <ace/OS_main.h>
#include <ace/Basic_Types.h>
#include <ace/Reactor.h>
#include <csignal>

namespace nasaCE {

devSink::devSink(const std::string& newName /* = "" */,
	const CE_DLL* newDLLPtr /* = 0 */ ): CE_Device(newName, newDLLPtr) {
	MOD_DEBUG("Creating devSink.");

	CEcfg::instance()->getOrAddString(cfgKey("devType")) = getType();

	signal(SIGPIPE, SIG_IGN);
	reactor(ACE_Reactor::instance());

	setInitialized(true);
	_openPipe();
	this->open();
}

devSink::~devSink() {
	MOD_DEBUG("Running ~devSink().");
	_closePipe();
	stopTraffic();
	setInitialized(false);
}

int devSink::svc() {
	svcStart_();

	int read_count = 0;

	while ( read_count >= 0 && ! (shutdown_ || exit_requested) && get_handle() != ACE_INVALID_HANDLE ) {
		read_count = _pipe.recv(_buf, 16384);
		if ( read_count < 0 ) {
			if ( errno != EWOULDBLOCK ) {
				MOD_ERROR("%p.", "Error reading from pipe");
			}
		}
		else {
			MOD_DEBUG("Received %d octets to discard.", read_count);
		}
	}

	#ifdef DEFINE_DEBUG
	if (get_handle() == ACE_INVALID_HANDLE ) {
		MOD_DEBUG("Exiting svc() due to invalid handle.");
	}
	else if (shutdown_ || exit_requested) {
		MOD_DEBUG("Exiting svc() due to shutdown/exit request.");
	}
	else if (read_count < 0) {
		MOD_DEBUG("Existing svc() due to error on read.");
	}
	#endif

	return svcEnd_();
}

ACE_HANDLE devSink::getHandle() {
	return _pipe.write_handle();
}

void devSink::_openPipe() {
	MOD_DEBUG("Opening pipe.");
	if ( _pipe.open() < 0 ) {
		MOD_CRITICAL("%p.", "Error opening pipe");
	}

	if ( ACE_OS::fcntl(_pipe.read_handle(), F_SETFD, O_NONBLOCK|O_CLOEXEC) < 0 ) {
		MOD_ERROR("%p.", "Could not set the readable end of the pipe non-blocking");
	}
	else {
		set_handle(_pipe.read_handle());
		if (get_handle() != _pipe.read_handle() || get_handle() == ACE_INVALID_HANDLE) {
			MOD_WARNING("Read handle not set correctly - _pipe.read_handle(): %d, get_handle(): %d.",
				_pipe.read_handle(), get_handle());
		}
		else {
			MOD_DEBUG("Read handle is now %d.", get_handle());
		}

		MOD_DEBUG("Pipe successfully opened.");
	}
}

void devSink::_closePipe() {
	MOD_DEBUG("Closing pipe.");

	if ( _pipe.close() < 0 ) {
		MOD_ERROR("%p.", "Error closing pipe");
	}
	unset_handle();
	MOD_DEBUG("Pipe successfully closed.");
}

}