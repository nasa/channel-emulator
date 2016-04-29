/* -*- C++ -*- */

//=============================================================================
/**
 * @file   AsyncSerialDevice.hpp
 * @author Tad Kollar  
 *
 * $Id: devAsyncSerial.hpp 1670 2012-08-28 13:25:28Z tkollar $ 
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _NASA_ASYNC_SERIAL_DEVICE_HPP_
#define _NASA_ASYNC_SERIAL_DEVICE_HPP_

#include <CE_Device.hpp>

namespace nasaCE {

//=============================================================================
/**
	@class AsyncSerialDevice
	@brief Manage the operations of a asynchronous serial port.
*/
//=============================================================================

class AsyncSerialDevice : public CE_Device {
public:
    AsyncSerialDevice();

    ~AsyncSerialDevice();

}; // class AsyncSerialDevice

} // namespace nasaCE

#endif // _NASA_ASYNC_SERIAL_DEVICE_HPP_
