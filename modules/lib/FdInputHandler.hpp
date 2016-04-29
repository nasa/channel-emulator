/* -*- C++ -*- */

//=============================================================================
/**
 * @file   FdInputHandler.hpp
 * @author Tad Kollar  
 *
 * $Id: FdInputHandler.hpp 2512 2014-02-14 19:36:40Z tkollar $
 * Copyright (c) 2008-2014.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _FD_INPUT_HANDLER_HPP_
#define _FD_INPUT_HANDLER_HPP_

#include "BaseTrafficHandler.hpp"

using namespace nasaCE;

//=============================================================================
/**
 * @class FdInputHandler
 * @brief Provides the common methods for handlers that read from file descriptors.
 *
 */
//=============================================================================
class FdInputHandler: public BaseTrafficHandler {

public:
	/// Primary constructor.
	/// @param newName An identifier for the module that's unique within the channel.
	/// @param newChannelName An identifier for the containing channel.
	/// @param newDLLPtr If loaded in a DLL, a pointer to that management structure.
    FdInputHandler(const std::string& newName, const std::string& newChannelName,
		const CE_DLL* newDLLPtr = 0);

	/// Destructor.
	virtual ~FdInputHandler();

	/// Activate the object.
    int open (void* = 0);

	/// Deactivate the object.
	int close(u_long flags = 0);

	/// Remove the event handler and stop processing traffic.
	void stopTraffic(bool shutdown = true);

	/// Called when handle_input returns -1.
	int handle_close(ACE_HANDLE handle, ACE_Reactor_Mask close_mask);

	/// Called when exceptional events occur.
	int handle_exception(ACE_HANDLE fd = ACE_INVALID_HANDLE);

    /// This becomes a stub because traffic comes from event handling.
    int svc() { }

}; // Class FdInputHandler

#endif // _FD_INPUT_HANDLER_HPP_