/* -*- C++ -*- */

//=============================================================================
/**
 * @file   CE_Device.hpp
 * @author Tad Kollar  
 *
 * $Id: CE_Device.hpp 2366 2013-09-24 18:45:54Z tkollar $
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _NASA_CE_DEVICE_HPP_
#define _NASA_CE_DEVICE_HPP_

#include "BaseTask.hpp"
#include "CE_Config.hpp"
#include "CE_DLL.hpp"
#include "SettingsManager.hpp"
#include "utilityMacros.hpp"
#include <ace/Event_Handler.h>
#include <string>
#include <ace/OS_main.h>

namespace nasaCE {

//=============================================================================
/**
 * @class CE_Device
 * @brief Manage generic information about a hardware I/O interface.
 */
//=============================================================================
class CE_Device: public BaseTask {
public:
	/// @class IsUnitialized
	/// @brief Thrown when a non-initializing method is called on an unconfigured device.
	struct IsUnitialized: public Exception {
		IsUnitialized(const std::string e): Exception(e) { }
	};

	/// @class OperationFailed
	/// @brief Thrown when a non-specific failure is detected.
	struct OperationFailed: public Exception {
		OperationFailed(const std::string e): Exception(e) { }
	};

	/// Default constructor.
	/// @param newName A unique name for the device. Auto-generated if empty.
	CE_Device(const std::string& newName = "", const CE_DLL* newDLLPtr = 0): BaseTask(newName),
		reader_(0), writer_(0),
		ptrDLL_(const_cast<CE_DLL*>(newDLLPtr)) {
    	if ( name_ == "" ) {
    		ACE_TCHAR uniq_name[13];
    		ACE_OS::unique_name(this, uniq_name, 12);
    		name_ = std::string("CE_Device_") + std::string(uniq_name);
    	}
		if (ptrDLL_) ptrDLL_->incReference();

		#ifdef DEFINE_DEBUG
		Setting& debugSetting = CEcfg::instance()->getOrAddBool(cfgKey("debug"), globalCfg.debug);
		setDebugging(debugSetting);
		#endif
  	}

	/// Destructor.
	virtual ~CE_Device() {
		if ( reader_ ) reader_->disconnectDevice(false);
		reader_ = 0;

		if ( writer_ ) writer_->disconnectDevice(false);
		writer_ = 0;

		if (ptrDLL_) ptrDLL_->decReference();
	}

	/// Sets the name of the device and returns a copy of it.
	virtual std::string setName(const std::string& newName) { return ( name_ = newName); }

	/// Read-only accessor to reader_.
	virtual BaseTask* getReader() const { return reader_; }

	/// Read-write accessor to reader_.
	/// @throw DeviceInUse When the existing reader cannot be disconnected.
	virtual BaseTask* setReader(BaseTask* newReader) {
		if (reader_ != 0) reader_->disconnectDevice(false, true);

		return (reader_ = newReader);
	}

	/// Read-only accessor to writer_.
	virtual BaseTask* getWriter() const { return writer_; }

	/// Read-write accessor to writer_.
	/// @throw DeviceInUse When the existing writer cannot be disconnected.
	virtual BaseTask* setWriter(BaseTask* newWriter) {
		if (writer_ != 0) writer_->disconnectDevice(false, true);

		return (writer_ = newWriter);
	}

	/// Return the value of isInitialized_.
	virtual bool isInitialized() const { return isInitialized_; }

	/// Read-write accessor to isInitialized_.
	virtual bool setInitialized(const bool newVal = true) { return ( isInitialized_ = newVal ); }

	/// Check the value of isInitialized_ and throw if false.
	/// @throw IsUnitialized If isInitialized_ is false.
	virtual void throwIfUnitialized() const {
		if ( ! isInitialized_ )
			throw IsUnitialized(std::string("Device named ") + name_ +
				std::string(" has not been initialized."));
	}

	virtual std::string cfgKey(const std::string& key) const {
		return "Devices." + getName() + (key.empty()? "" : "." + key);
	}

	virtual std::string getType() const = 0;

	virtual ACE_HANDLE getHandle() { return ACE_INVALID_HANDLE; }

protected:
	BaseTask* reader_; /// Who, if anyone, receives input from us.
	BaseTask* writer_; /// Who, if anyone, provides output to us.
	bool isInitialized_; /// Whether the device has been properly configured or not.

	/// Pointer to the DLL where this class is defined.
	CE_DLL* ptrDLL_;
};

} // namespace nasaCE

#endif // _NASA_CE_DEVICE_HPP_