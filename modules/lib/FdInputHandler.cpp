/* -*- C++ -*- */

//=============================================================================
/**
 * @file   FdInputHandler.cpp
 * @author Tad Kollar  
 *
 * $Id: FdInputHandler.cpp 2514 2014-02-14 20:03:58Z tkollar $
 * Copyright (c) 2008-2014.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "FdInputHandler.hpp"
#include <ace/Reactor.h>

FdInputHandler::FdInputHandler(const std::string& newName, const std::string& newChannelName,
	const CE_DLL* newDLLPtr /* = 0 */): BaseTrafficHandler(newName, newChannelName, newDLLPtr) {

	set_handle(ACE_INVALID_HANDLE);
}


FdInputHandler::~FdInputHandler() {
}

int FdInputHandler::open (void* /* = 0 */) {
	MOD_INFO("open(): Registering event handler.");
	if ( svcDone_ == true ) {
		if ( get_handle() != ACE_INVALID_HANDLE ) {
			reactor()->register_handler(get_handle(), this, ACE_Event_Handler::READ_MASK);
			svcDone_ = false;
		}
		else {
			MOD_WARNING("open(): Handle is invalid, cannot register new handler.");
		}
	}
	else {
		MOD_NOTICE("open(): Already registered event handler.");
	}

	return 0;
}

int FdInputHandler::close(u_long flags /* = 0 */) {
	MOD_INFO("close(): Removing event handler.");
	if ( svcDone_ == false) {
		if (  get_handle() != ACE_INVALID_HANDLE ) {
			reactor()->remove_handler(get_handle(), ACE_Event_Handler::READ_MASK|ACE_Event_Handler::DONT_CALL);
			svcDone_ = true;
			clearHandlerBits(ACE_Event_Handler::READ_MASK);
		}
		else {
			MOD_WARNING("close(): Handle is invalid, cannot remove event handler.");
		}
	}
	else {
		MOD_NOTICE("close(): Already removed event handler.");
	}

	return 0;
}

void FdInputHandler::stopTraffic(bool shutdown /* = true */) {

	MOD_DEBUG("Stopping traffic.");

	if (shutdown) shutdown_ = true;
	this->close();
}


int FdInputHandler::handle_close(ACE_HANDLE handle, ACE_Reactor_Mask close_mask) {
	MOD_INFO("Handling close of fd %d.", handle);
	return this->close();
}

int FdInputHandler::handle_exception(ACE_HANDLE fd /* = ACE_INVALID_HANDLE */) {
	MOD_NOTICE("Handling exception on fd %d.", fd);

	return 0;
}

