/* -*- C++ -*- */

//=============================================================================
/**
 * @file   devSource.cpp
 * @author Tad Kollar  
 *
 * $Id: devSource.cpp 2182 2013-06-20 16:39:13Z tkollar $
 * Copyright (c) 2011.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "devSource.hpp"
#include "SettingsManager.hpp"
#include <ace/OS_main.h>
#include <ace/Reactor.h>
#include <csignal>

namespace nasaCE {

devSource::devSource(const std::string& newName /* = "" */,
	const CE_DLL* newDLLPtr /* = 0 */ ):
	CE_Device(newName, newDLLPtr),
	_patternBuffer(0),
	_patternSetting(CEcfg::instance()->getOrAddArray(cfgKey("pattern"))),
	_patternBufferSize(CEcfg::instance()->getOrAddInt(cfgKey("patternBufferSize"), 0x2800)) {
	MOD_DEBUG("Creating devSource.");

	CEcfg::instance()->getOrAddString(cfgKey("devType")) = "Source";

	_patternBuffer = new NetworkData(static_cast<int>(_patternBufferSize));

	if ( _patternSetting.getLength() == 0 ) {
		uint8_t defaultPattern[] = {
			0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7,
			0x8, 0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf
		};
		setPattern(defaultPattern, 16, false);
	}
	else _rebuildPatternBuffer(false);

	signal(SIGPIPE, SIG_IGN);

	_openPipe();

	setInitialized(true);

	this->open();
}

devSource::~devSource() {
	MOD_DEBUG("Running ~devSource().");
	_closePipe();
	stopTraffic();
	setInitialized(false);

	MOD_DEBUG("Deleting pattern buffer.");
	delete _patternBuffer;

	reactor(0);
}

ACE_HANDLE devSource::getHandle() {
	return _pipe.read_handle();
}

void devSource::setPattern(const uint8_t* patternBuf, const int bufLen, const bool restart /* = true */) {
	for (int i = 0; i < bufLen; i++) {
		// Add slots when needed
		if ( _patternSetting.getLength() < i + 1 ) _patternSetting.add(Setting::TypeInt);
		_patternSetting[i] = patternBuf[i];
	}

	// Eliminate extra slots
	while (_patternSetting.getLength() > bufLen) _patternSetting.remove(bufLen);

	_rebuildPatternBuffer(restart);
}

void devSource::setPattern(const std::vector<uint8_t>& newPattern, const bool restart /* = true */) {
	for (unsigned i = 0; i < newPattern.size(); i++) {
		// Add slots when needed
		if ( static_cast<unsigned>(_patternSetting.getLength()) < i + 1 )
			_patternSetting.add(Setting::TypeInt);

		_patternSetting[i] = newPattern[i];
	}

	// Eliminate extra slots
	while (static_cast<unsigned>(_patternSetting.getLength()) > newPattern.size())
		_patternSetting.remove(newPattern.size());

	_rebuildPatternBuffer(restart);
}

void devSource::getPattern(std::vector<uint8_t>& pattern) const {
	pattern.clear();

	for ( int i = 0; i < _patternSetting.getLength(); i++ )
		pattern.push_back(static_cast<int>(_patternSetting[i]) & 0xff);
}

int devSource::svc() {
	svcStart_();

	int write_count = 0;

	while ( write_count >= 0 && ! (shutdown_ || exit_requested) && get_handle() != ACE_INVALID_HANDLE ) {
		write_count = _pipe.send(_patternBuffer->ptrUnit(), _patternBuffer->getUnitLength());
		if ( write_count < 0 ) {
			if ( errno != EWOULDBLOCK ) {
				MOD_ERROR("%p.", "Error writing to pipe");
			}
		}
	}

	#ifdef DEFINE_DEBUG
	if (get_handle() == ACE_INVALID_HANDLE ) {
		MOD_DEBUG("Exiting svc() due to invalid handle.");
	}
	else if (shutdown_ || exit_requested) {
		MOD_DEBUG("Exiting svc() due to shutdown/exit request.");
	}
	else if (write_count < 0) {
		MOD_DEBUG("Existing svc() due to error on write.");
	}
	#endif

	return svcEnd_();
}

void devSource::_rebuildPatternBuffer(const bool restart /* = true */) {
	if (restart && ! svcDone_) {
		MOD_INFO("Stopping traffic before source pattern buffer rebuild.");
		this->stopTraffic();
	}

	_patternBuffer->setUnitLength(getPatternBufferSize());
	for ( unsigned x = 0; x < _patternBuffer->getUnitLength(); ++x )
		*(_patternBuffer->ptrUnit() + x) =
			static_cast<int>(_patternSetting[x % _patternSetting.getLength()]) & 0xff;

	if (restart && ! svcDone_) {
		MOD_INFO("Starting traffic after source pattern buffer rebuild.");
		this->open();
	}
}

void devSource::_openPipe() {
	MOD_DEBUG("Opening pipe.");
	if ( _pipe.open() < 0 ) {
		MOD_CRITICAL("%p.", "Error opening pipe");
	}

	if ( ACE_OS::fcntl(_pipe.write_handle(), F_SETFD, O_NONBLOCK|O_CLOEXEC) < 0 ) {
		MOD_ERROR("%p.", "Could not set the writable end of the pipe non-blocking");
	}
	else {
		set_handle(_pipe.write_handle());
		if (get_handle() != _pipe.write_handle() || get_handle() == ACE_INVALID_HANDLE) {
			MOD_WARNING("Write handle not set correctly - _pipe.write_handle(): %d, get_handle(): %d.",
				_pipe.write_handle(), get_handle());
		}
		else {
			MOD_DEBUG("Write handle is now %d.", get_handle());
		}
		MOD_DEBUG("Pipe successfully opened.");
	}
}

void devSource::_closePipe() {
	MOD_DEBUG("Closing pipe.");

	if ( _pipe.close() < 0 ) {
		MOD_ERROR("%p.", "Error closing pipe");
	}
	unset_handle();
	MOD_DEBUG("Pipe successfully closed.");
}

}
