/* -*- C++ -*- */

//=============================================================================
/**
 * @file   BaseTask.hpp
 * @author Tad Kollar  
 *
 * $Id: BaseTask.hpp 2366 2013-09-24 18:45:54Z tkollar $
 * Copyright (c) 2010.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _NASA_BASE_TASK_HPP_
#define _NASA_BASE_TASK_HPP_

#include "CE_Config.hpp"
#include "utilityMacros.hpp"
#include <ace/OS_main.h>
#include <ace/Task.h>
#include <ace/Message_Queue_T.h>
#include <ace/OS_NS_time.h>

typedef ACE_Task<ACE_MT_SYNCH> taskType;

namespace nasaCE {
//=============================================================================
/**
 * @class BaseTask
 * @author Tad Kollar  
 * @brief Basis of all modular segments and devices.
*/
//=============================================================================
class BaseTask: public taskType {

public:
	/// @class DeviceInUse
	/// @brief Thrown when the specified device cannot be acquired because it is used elsewhere.
	struct DeviceInUse: public Exception {
		DeviceInUse(const std::string e): Exception(e) { }
	};

	static const int maxThreads;
	static const timespec_t hundredMSec;

	/// Default constructor.
    BaseTask(const std::string& newName = "");

	/// Destructor.
    virtual ~BaseTask();

    /// Activate the object.
    int open (void* = 0);

    /// The functional loop that makes this an active object.
    virtual int svc();

	/// Shut down the message queue and wait for processing to stop.
	/// @param shutdown True indicates the whole system is coming down; false means it may come back.
	virtual void stopTraffic(bool shutdown = true);

   	/// Read-only accessor to name_.
	std::string getName() const;

    /// Allow others to check if they can add to our queue.
    virtual bool acceptingData();

	/// Return true if svc() should keep processing.
	virtual bool continueService() const;

	#ifdef DEFINE_DEBUG
	/// Write-only accessor to debugging_.
	void setDebugging(const bool newVal);

	/// Read-only accessor to debugging_.
	bool getDebugging() const;
	#endif

    /// Subclass defines how to break the link with the captured device.
    /// This is found in BaseTask rather than BaseTrafficHandler so a CE_Device can
    /// disconnect itself from its reader/writer when destructing.
	/// @param updateDevice If true, call setReader(0) or setWriter(0) on the associated device.
	/// @param updateCfg If true, remove the deviceName setting from the config file.
	/// @throw DeviceInUse When the device cannot be disconnected.
    virtual void disconnectDevice(const bool updateDevice = true, const bool updateCfg = false) { }

	/// Keep track of reactor event registrations.
	int handle_close(ACE_HANDLE fd, ACE_Reactor_Mask close_mask);

	/// Add some new bits to mask_.
	void setHandlerBits(const ACE_Reactor_Mask maskBits);

	/// Clear some bits in mask_.
	void clearHandlerBits(const ACE_Reactor_Mask maskBits);

	/// Gets the file descriptor associated with this task.
	ACE_HANDLE get_handle(void) const;

	/// Sets the file descriptor associated with this task.
	void set_handle(ACE_HANDLE newVal);

	/// Make the stored handle invalid.
	void unset_handle();

protected:
	/// An identifier for the task.
	std::string name_;

	/// When false, the svc() thread keeps running; when true, svc() knows to stop.
	bool shutdown_;

	/// The ID of the thread running the svc() method.
	ACE_thread_t svcThreadID_;

	/// Whether svc() is active or not.
	bool svcDone_;

	/// Set svcThreadID_ to the correct thread ID and set svcDone_ to false.
	void svcStart_();

	/// Set svcDone_ to true and exit the thread.
	int svcEnd_();

	/// Keep track of the events the module is registered for.
	ACE_Reactor_Mask mask_;

	/// The default handle that the reactor will care about.
	ACE_HANDLE handle_;

	#ifdef DEFINE_DEBUG
	/// If true, enable debug info printing for this segment.
	bool debugging_;
	#endif

}; // class BaseTask

} // namespace nasaCE

#endif // _NASA_BASE_TASK_HPP_
