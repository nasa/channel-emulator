/* -*- C++ -*- */

//=============================================================================
/**
 * @file   devSink.hpp
 * @author Tad Kollar  
 *
 * $Id: devSink.hpp 2181 2013-06-20 15:50:27Z tkollar $
 * Copyright (c) 2011.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================


#ifndef _NASA_DEV_SINK_HPP_
#define _NASA_DEV_SINK_HPP_

#include "CE_Device.hpp"
#include "Device_Interface_Templ.hpp"
#include "SettingsManager.hpp"
#include "NetworkData.hpp"
#include <ace/Pipe.h>
#include <string>

namespace nasaCE {

//=============================================================================
/**
 * @class devSink
 * @author Tad Kollar  
 * @brief Discard any traffic received.
*/
//=============================================================================
class devSink: public CE_Device {
public:
	/// Default constructor.
    devSink(const std::string& newName = "", const CE_DLL* newDLLPtr = 0);

	/// Destructor.
    ~devSink();

    /// Read-only accessor to write end of the pipe file descriptor.
    ACE_HANDLE getHandle();

	/// What kind of device this is.
	std::string getType() const { return "Sink"; }

	/// The functional loop that makes this an active object.
    int svc();

private:
	/// Descriptors for the sink pipe.
	ACE_Pipe _pipe;

	/// The buffer to read into.
	ACE_UINT8 _buf[16384];

	/// Create the pipe.
	void _openPipe();

	/// Close both ends of the pipe.
	void _closePipe();

}; // class devSink

} // namespace nasaCE

#endif // _NASA_DEV_SINK_HPP_
