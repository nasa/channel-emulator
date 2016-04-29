/* -*- C++ -*- */

//=============================================================================
/**
 * @file   devFile.cpp
 * @author Tad Kollar  
 *
 * $Id: devFile.cpp 2413 2013-12-13 17:10:47Z tkollar $
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "devFile.hpp"
#include "SettingsManager.hpp"
#include <ace/OS_main.h>
#include <ace/Basic_Types.h>

namespace nasaCE {

std::string devFile::fileDeviceSection = "Devices.FileDevice_Common";

devFile::devFile(const std::string& newName /* = "" */,
	const CE_DLL* newDLLPtr /* = 0 */ ):
	CE_Device(newName, newDLLPtr),
	_filename(CEcfg::instance()->getOrAddString(cfgKey("fileName"))),
	_directoryName(globalCfg.fileIOPath),
	_isInput(CEcfg::instance()->getOrAddBool(cfgKey("isInput"), true)),
	_fd(ACE_INVALID_HANDLE) {
	ND_DEBUG("[%s] Creating devFile.\n", newName.c_str());

	CEcfg::instance()->getOrAddString(cfgKey("devType")) = "File";

	if ( _filename != "" ) {
		setInitialized();
		if ( static_cast<bool>(_isInput) == true ) {
			try { openForReading(); }
			catch (const OperationFailed& e) { }
		}
		else {
			try { openForWriting(); }
			catch (const OperationFailed& e) { }
		}
	}
}

devFile::~devFile() {
	ND_DEBUG("[%s] Running ~devFile().\n", getName().c_str());
	if ( isOpen() ) this->close();
	setInitialized(false);
}

void devFile::setFilename(const std::string& newFilename) {
	if ( isOpen() ) throw AlreadyOpen(std::string("The file named ") + getFilename() +
		std::string(" managed by devFile ") + getName() +
		std::string(" is already open so its filename cannot be changed."));
	_filename = newFilename;
	setInitialized();
}

std::string devFile::getFilename() const {
	return _filename;
}

ACE_HANDLE devFile::openForWriting(const bool append /* = false */) {
	int flags = O_CREAT|O_WRONLY;
	if (append) flags |= O_APPEND;

	if (isOpen()) throw AlreadyOpen(std::string("The file named ") + getFilename() +
		std::string(" managed by devFile ") + getName() + std::string(" is already open."));

	std::string pathname = _pathname();
	ND_DEBUG("[%s] Opening file %s for writing.\n", getName().c_str(), pathname.c_str());

	_fd = ACE_OS::open(pathname.c_str(), flags);

	if ( _fd == -1 ) {
		_fd = ACE_INVALID_HANDLE;
		throw OperationFailed(std::string("The file ") + pathname +
		std::string(" managed by devFile ") + getName() +
		std::string(" could not be opened for writing: ") +
		std::string(ACE_OS::strerror(errno)));
	}

	CEcfg::instance()->getOrAddBool(cfgKey("isInput")) = false;

	return _fd;
}

ACE_HANDLE devFile::openForReading() {

	if (isOpen()) throw AlreadyOpen(std::string("The file named ") + getFilename() +
		std::string(" managed by devFile ") + getName() + std::string(" is already open."));

	std::string pathname = _pathname();
	ND_DEBUG("[%s] Opening file %s for reading.\n", getName().c_str(), pathname.c_str());

	// O_RDWR instead of O_RDONLY to prevent named pipes from blocking
	_fd = ACE_OS::open(pathname.c_str(), O_RDWR);

	if ( _fd == -1 ) {
		_fd = ACE_INVALID_HANDLE;
		throw OperationFailed(std::string("The file named ") + pathname +
		std::string(" managed by devFile ") + getName() +
		std::string(" could not be opened for reading: ") +
		std::string(ACE_OS::strerror(errno)));
	}

	CEcfg::instance()->getOrAddBool(cfgKey("isInput"), true) = true;

	return _fd;
}

ACE_HANDLE devFile::getHandle() {
	return _fd;
}

int devFile::close() {
	if (isOpen()) {
		int ret = ACE_OS::close(_fd);
		_fd = ACE_INVALID_HANDLE;
		return ret;
	}
	return 0;
}

bool devFile::isOpen() {
	return ( _fd != ACE_INVALID_HANDLE );
}

std::string devFile::_pathname() {
	std::string fname = _filename, dname = _directoryName;

	if ( fname.empty() )
		throw FilenameNotSet(std::string("Operation on devFile ") + getName() +
			" could not be performed because it does not have a filename set.");

	if (*dname.rbegin() != '/') dname += "/";
	return (dname + fname);
}

}
