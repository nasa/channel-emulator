/* -*- C++ -*- */

//=============================================================================
/**
 * @file   XML_RPC_Server.hpp
 * @author Tad Kollar  
 *
 * $Id: XML_RPC_Server.hpp 2082 2013-06-13 13:22:28Z tkollar $
 * Copyright (c) 2008.
 *      NASA Glenn Research Center.  All rights reserved.
 */
//=============================================================================

#ifndef _XML_RPC_SERVER_HPP_
#define _XML_RPC_SERVER_HPP_

#include "CE_DLL_Registry.hpp"
#include "httpd/ClientAcceptor.hpp"
#include "DelayedResults.hpp"

#include <xmlrpc-c/base.hpp>
#include <xmlrpc-c/registry.hpp>

#include <ace/Timer_Heap_T.h>
#include <ace/INET_Addr.h>
#include <ace/SOCK_Acceptor.h>

typedef ACE_Timer_Heap_T<ACE_Event_Handler *,
	ACE_Event_Handler_Handle_Timeout_Upcall<ACE_SYNCH_RECURSIVE_MUTEX>,
	ACE_SYNCH_RECURSIVE_MUTEX> ACE_Timer_Heap;

namespace nasaCE {

//=============================================================================
/**
 * @class XML_RPC_Server
 * @brief Brings the entire Channel Emulator project together.
 *
 * Handles reading the configuration file, setting up the server
 * thread, holding the DLL registry, and providing the XML-RPC
 * mod.* and time.* methods.
 */
//=============================================================================
class XML_RPC_Server {

public:
	/// The default constructor.
	XML_RPC_Server();

	/// The destructor.
	~XML_RPC_Server() {
		// delete _server;
		// delete _shutdown;
		delete _timer_heap;
	}

	/// The entry point.
	void initialize();

	/// A way to deactivate before being destroyed.
	void shutdown();

	/// All of the XML-RPC methods that are provided by the server.
	xmlrpc_c::registryPtr rpcRegistry;

	/// @brief Add a timer (alarm) to the module's map.
	/// @param timer_name A unique ID that will be the map key.
	/// @param timer_id The ID that the Reactor knows the timer as.
	void addTimer(const std::string& timer_name, const long timer_id);

	/// @brief Remove a timer from the map.
	/// @param timer_name A unique identifier for the timer, previously generated
	/// by the system.
	/// @param erase_only If false (as when a timer  has expired), the timer is
	/// only deleted from the map; if true, also tell the Reactor to cancel it.
	void clearTimer(const std::string& timer_name, bool erase_only = false);

	/// @brief Iterate over the timer map and run <b>clear_timer()</b> on each pair.
	void clearAllTimers();

	/// @brief Generate a unique timer ID based on the module name and a UUID.
	std::string newTimerId(const std::string& name);

	/// @brief Invoke an XML-RPC method.
	/// @param callName The full name of the method.
	/// @param paramList Array of parameters as XML-RPC value objects.
	/// @param resultP Where the result of the call is stored.
	void doCall(const std::string& callName, xmlrpc_c::paramList const& paramList,
		xmlrpc_c::value* resultP);

	/// Register the built-in XML-RPC interfaces (e.g. dll.*, time.*, channel.*) with the XML-RPC server.
	void registerBuiltins();

	/// Prevent the HTTP server from accepting new connections.
	void pauseHttpServer();

	/// Allow the HTTP server to accept new connections.
	void unpauseHttpServer();

	/// @brief Load a new device/channel configuration.
	/// In the process, unload all existing segments, channels, devices,
	/// and DLLs. After loading the new config file, load all the new
	/// DLLs, devices, channels, and segments.
	/// @param cfgFile The new filename, existing within the config file directory.
	void switchToConfigFile(const std::string& cfgFile);

private:
	/// A different timer queue for the Reactor.
	ACE_Timer_Heap* _timer_heap;

	/// Load DLLs and devices specified in the configuration file.
	void _autoLoad();

	/// Perform one-time initialization methods.
	void _setup();

	/// @brief The local association of timer names to timer IDs.
	/// One long timer id is created whenever the ACE Reactor schedules a timer.
	/// They're stored here so that if the module is removed, unresolved timers
	/// can be cancelled in the Reactor before they cause seg faults or
	/// undefined symbol errors.
	std::map<std::string, long> _timers;

	/// The HTTP service that accepts XML-RPC calls.
	ClientAcceptor* _acceptor;
};

typedef ACE_Singleton<XML_RPC_Server, ACE_Recursive_Thread_Mutex> xmlrpc_server;

} // namespace nasaCE

#endif // _XML_RPC_SERVER_HPP_
