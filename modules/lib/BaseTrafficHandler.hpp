/* -*- C++ -*- */

//=============================================================================
/**
 * @file   BaseTrafficHandler.hpp
 * @author Tad Kollar  
 *
 * $Id: BaseTrafficHandler.hpp 2365 2013-09-24 17:55:34Z tkollar $
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _BASE_TRAFFIC_HANDLER_HPP_
#define _BASE_TRAFFIC_HANDLER_HPP_

#include "BaseTask.hpp"
#include "CE_Device.hpp"
#include "CE_DLL.hpp"
#include "HandlerLink.hpp"
#include "SettingsManager.hpp"

#include <string>
#include <ace/Message_Queue_T.h>

typedef ACE_Message_Queue<ACE_MT_SYNCH> AuxMessageQueue;

using namespace nasaCE;

//=============================================================================
/**
 * @class BaseTrafficHandler
 * @brief Provides the basic methods that modules will need to communicate.
 *
 * Most modules will override these methods with functions that actually move data.
 */
//=============================================================================
class BaseTrafficHandler: public BaseTask {

public:

	static const int defaultLowWaterMark;
	static const int defaultHighWaterMark;

	enum LinkType {
		PrimaryInputLink,
		AuxInputLink,
		PrimaryOutputLink,
		AuxOutputLink
	};

	/// @class CircularConnection
	/// @brief Thrown when the handler may deliver traffic to itself.
	struct CircularConnection: public Exception {
		CircularConnection(const std::string e): Exception(e) { }
	};

	/// Primary constructor.
	/// @param newName An identifier for the module that's unique within the channel.
	/// @param newChannelName An identifier for the containing channel.
	/// @param newDLLPtr If loaded in a DLL, a pointer to that management structure.
    BaseTrafficHandler(const std::string& newName, const std::string& newChannelName,
    	const CE_DLL* newDLLPtr = 0);

	/// Destructor.
	virtual ~BaseTrafficHandler();

    /// The functional loop that makes this an active object.
    int svc();

	// ----- Traffic flow management -----

	/// Test for a possible circular connection anywhere in the chain.
	/// Must be overloaded by subclasses with multiple output targets.
	/// @param testTarget Pointer to compare against existing targets.
	/// @return True if testTarget already exists as an output target.
	virtual bool isCircularConnection(BaseTrafficHandler* testTarget);

    /// Remove ourselves from the flow of traffic. Leave deletion to the Channel container.
    /// @param stopTraffic Shut down the message queue? Should only set false if already done.
    virtual void withdraw(const bool doStopTraffic = true);

    /// Read-only accessor to refCount_.
    virtual unsigned getRefCount() const { return refCount_; }

    /// Increase refCount_ by one.
    virtual unsigned incRefCount() { return (++refCount_); }

    /// Decreate refCount_ by one.
    virtual unsigned decRefCount() { return ((refCount_)? --refCount_: 0); }

	/// Write-only accessor to MRU_ and mruSetting_, the Maximum Receive Unit.
	virtual void setMRU(const size_t newVal) {
		MRU_ = newVal;
		mruSetting_ = static_cast<int>(MRU_);
	}

	/// Read-only accessor to MRU_, the Maximum Receive Unit.
	virtual size_t getMRU() const { return MRU_; }

	/// Write-only accessor to MTU_ and mtuSetting_, the Maximum Transmit Unit.
	virtual void setMTU(const size_t newVal) {
		MTU_ = newVal;
		mtuSetting_ = static_cast<int>(MTU_);
	}

	/// Read-only accessor to MTU_, the Maximum Transmit Unit.
	virtual size_t getMTU() const { return MTU_; }

	/// Can be called by targets when their MRU changes, so that we
	/// can update our MTU if it's important to the target.
	virtual void updateMTU(const size_t newMTU) { }

	/// Read-only accessor to the message queue low water mark.
	size_t getLowWaterMark() { return msg_queue()->low_water_mark(); }

	/// Write-only accessor to the message queue low water mark.
	void setLowWaterMark(const size_t newVal) {
		msg_queue()->low_water_mark(newVal);
		lowWaterMarkSetting_ = static_cast<int>(newVal);
	}

	/// Read-only accessor to the message queue high water mark.
	size_t getHighWaterMark() { return msg_queue()->high_water_mark(); }

	/// Write-only accessor to the message queue high water mark.
	void setHighWaterMark(const size_t newVal) {
		msg_queue()->high_water_mark(newVal);
		highWaterMarkSetting_ = static_cast<int>(newVal);
	}

	/// Write-only accessor to dumpToLog_ and dumpToLogSetting_;
	void setDumpToLog(bool newVal) { dumpToLog_ = newVal; dumpToLogSetting_ = newVal; }

	/// Read-only accessor to dumpToLog_.
	bool getDumpToLog() const { return dumpToLog_; }

	/// Read-only accessor to ptrDLL_.
	CE_DLL* ptrDLL() { return ptrDLL_; }

	/// Write-only accessor to ptrDLL_.
	void setDLL(const CE_DLL* newPtr) { ptrDLL_ = const_cast<CE_DLL*>(newPtr); }

    /// Subclass defines whether to connect the specified device as input or output.
    virtual void connectDevice(CE_Device* device) { }

    /// Subclass defines how to return a pointer to its connected device.
    virtual CE_Device* getDevice() const { return 0; }

    /// Accessor to auxInputQueue_.
	AuxMessageQueue* getAuxQueue() { return auxInputQueue_; }

	/// Create a HandlerLink to manage the existance of the connection from
	/// this handler's output to the next handler's input.
	/// @param target The Traffic Handler to output to.
	/// @param sourceOutputRank Whether to connect the source's primary or auxiliary output.
	/// @param targetInputRank Whether to connect to the target's primary or auxiliary input.
	/// @param postOpen True if this link is being added while the source is already active.
	virtual void connectOutput(BaseTrafficHandler* target,
		const HandlerLink::OutputRank sourceOutputRank = HandlerLink::PrimaryOutput,
		const HandlerLink::InputRank targetInputRank = HandlerLink::PrimaryInput,
		const bool postOpen = false );

	/// Destroy an existing output connection, represented by a HandlerLink object.
	/// @param sourceOutputRank Which output to disconnect (primary or auxiliary).
	virtual void disconnectOutput(const HandlerLink::OutputRank sourceOutputRank =
		 HandlerLink::PrimaryOutput, HandlerLink* = 0);

	virtual HandlerLink* getLink(const LinkType ltype, const ACE_UINT8 = 0) { return links_[ltype]; }

	std::string channelKey() const { return "Channels." + channelName_; }
	std::string handlerKey() const { return channelKey() + "." + name_; }
	std::string cfgKey(const std::string& key) { return handlerKey() + "." + key; }

	/// Insert a message block into the specified message queue.
	/// @param data The message to insert.
	virtual int putData(NetworkData* data, const HandlerLink::InputRank inputRank = HandlerLink::PrimaryInput);

	/// Write-only accessor to receivedUnitCount_.
	void setReceivedUnitCount(const ACE_UINT32& newVal,
		const HandlerLink::InputRank inputRank = HandlerLink::PrimaryInput);

	/// Increase receivedUnitCount_ by the specified step value.
	uint64_t incReceivedUnitCount(const ACE_UINT32& step = 1,
		const HandlerLink::InputRank inputRank = HandlerLink::PrimaryInput);

	/// Read-only accessor to receivedUnitCount_.
	uint64_t getReceivedUnitCount(const HandlerLink::InputRank inputRank = HandlerLink::PrimaryInput) const;

	/// Write-only accessor to receivedOctetCount_.
	void setReceivedOctetCount(const ACE_UINT32& newVal,
		const HandlerLink::InputRank inputRank = HandlerLink::PrimaryInput);

	/// Increase receivedOctetCount_ by the specified step value.
	uint64_t incReceivedOctetCount(const ACE_UINT32& step = 1,
		const HandlerLink::InputRank inputRank = HandlerLink::PrimaryInput);

	/// Read-only accessor to receivedOctetCount_.
	uint64_t getReceivedOctetCount(const HandlerLink::InputRank inputRank = HandlerLink::PrimaryInput) const;

	/// Return the total number of units held in the message queue.
	/// @param useAuxQueue If true, return the size of the auxiliary queue instead of the primary.
	size_t getQueuedUnitCount(bool useAuxQueue = false);

	/// Return the total number of octets held in the message queue.
	/// @param useAuxQueue If true, return the size of the auxiliary queue instead of the primary.
	size_t getQueuedOctetCount(bool useAuxQueue = false);

	/// Read-only accessor to channelName_.
	std::string getChannelName() const { return channelName_; }

	/// If dumpToLog_ is true, dump the contents of the supplied unit to the log.
	void dumpUnit(NetworkData* unit) const;

protected:
	/// The number of units that have been received.
	uint64_t receivedUnitCount_[2];

	/// The number of octets that have been received.
	uint64_t receivedOctetCount_[2];

	/// An identifier for containing channel
	std::string channelName_;

	/// The number of handlers that use this for their output.
	unsigned refCount_;

	/// The (optional) maximum size incoming unit we can handle.
	/// Zero means no maximum is enforced by the handler. If it's greater than zero,
	/// units larger than that will probably be dropped.
	size_t MRU_;

	/// Configuration file reference to MRU_.
	Setting& mruSetting_;

	/// The (optional) maximum size outgoing unit we can create.
	/// Zero means no maximum is enforced by the handler.
	size_t MTU_;

	/// Configuration file reference to MTU_;
	Setting& mtuSetting_;

	/// Configuration file reference to the message queue low water mark.
	Setting& lowWaterMarkSetting_;

	/// Configuration file reference to the message queue high water mark.
	Setting& highWaterMarkSetting_;

	/// If true, hex dump every received unit to the log file.
	bool dumpToLog_;

	/// Configuration file reference to dumpToLog_;
	Setting& dumpToLogSetting_;

	/// Pointer to the DLL where this class is defined.
	CE_DLL* ptrDLL_;

	/// @brief Retrieve a message from an input queue.
	/// @param inputRank Whether to dequeue from the primary or auxiliary queue.
	/// @param timeout The amount of time to wait for a message.
	/// @return A std::pair, where .first is a pointer to the message data and
	/// .second is the number of messages left in the queue.
	virtual std::pair<NetworkData*, int> getData_(
		const HandlerLink::InputRank inputRank = HandlerLink::PrimaryInput,
		ACE_Time_Value* timeout = 0);

	/// @brief Retrieve a message from the auxiliary input queue.
	/// @param timeout The amount of time to wait for a message.
	/// @return A std::pair, where .first is a pointer to the message data and
	/// .second is the number of messages left in the queue.
	virtual std::pair<NetworkData*, int> getAuxData_(ACE_Time_Value* timeout = 0) {
		return getData_(HandlerLink::AuxInput, timeout);
	}

	/// A auxiliary queue managed as necessary by a derived class.
	AuxMessageQueue* auxInputQueue_;

	/// Pointers to the two inputs (primary, auxiliary) and two outputs
	/// (primary, auxiliary).
	HandlerLink* links_[4];

	/// Delete the link but let the HandlerLink object call unsetLink to
	/// nullify the pointer.
	virtual void deleteLink_(const LinkType linkType, HandlerLink*);

	/// Stores the pointer in the appropriate index in the link array.
	/// If that slot is not null, delete the link that's already there.
	virtual void setLink_(const LinkType linkType, HandlerLink* newLink);

	/// Nullify the pointer only if deleteLink has been called.
	/// Only intended to be called by a HandlerLink object.
	virtual void unsetLink_(const LinkType linkType, HandlerLink*);

	/// HandlerLink (and only HandlerLink) must call _setLink and _unsetLink.
	friend class HandlerLink;


}; // class BaseTrafficHandler

#endif // _BASE_TRAFFIC_HANDLER_HPP_
