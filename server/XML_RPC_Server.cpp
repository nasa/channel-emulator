/* -*- C++ -*- */

//=============================================================================
/**
 * @file   XML_RPC_Server.cpp
 * @author Tad Kollar  
 *
 * $Id: XML_RPC_Server.cpp 2097 2013-06-13 18:18:57Z tkollar $
 * Copyright (c) 2008.
 *      NASA Glenn Research Center.  All rights reserved.
 */
//=============================================================================

#include "CE_DLL_Interface.hpp"
#include "CE_Macros.hpp"
#include "Channel_Interface.hpp"
#include "Device_Interface.hpp"
#include "DeviceRegistry.hpp"
#include "SettingsManager.hpp"
#include "Timer_Interface.hpp"
#include "XML_RPC_Server.hpp"
#include "Settings_Interface.hpp"

#include <xmlrpc-c/xml.hpp>
#include <xmlrpc-c/girerr.hpp>

#include <ace/Thread_Manager.h>
#include <ace/Reactor.h>
#include <ace/Event_Handler.h>
#include <ace/SOCK_Stream.h>
#include <ace/UUID.h>

using namespace std;

namespace nasaCE {

XML_RPC_Server::XML_RPC_Server(): _acceptor(0) /*, rpcRegistry(new xmlrpc_c::registry) */ {
	_timer_heap = new ACE_Timer_Heap;
	ACE_Reactor::instance()->timer_queue(_timer_heap);
}

void XML_RPC_Server::initialize() {
	_setup();

	CE_DLL_InterfaceP = new CE_DLL_Interface(rpcRegistry);
	Timer_InterfaceP = new Timer_Interface(rpcRegistry);

	_autoLoad();

	Device_InterfaceP = new Device_Interface(rpcRegistry);
	devices::instance()->autoLoad();

	Channel_InterfaceP = new Channel_Interface(rpcRegistry);
	channels::instance()->autoLoad();

	Settings_InterfaceP = new Settings_Interface(rpcRegistry);

}

void XML_RPC_Server::shutdown() {
	ACE_TRACE("XML_RPC_Server::shutdown");

	if ( _acceptor ) {
		pauseHttpServer();
		ND_NOTICE("[XML_RPC_Server] Shutting down the HTTP server.\n");

		delete _acceptor;
	}

	devices::instance()->removeAll();

	if (rpcRegistry.get()) {
		ND_NOTICE("[XML_RPC_Server] Flushing the XML-RPC method registry.\n");
		rpcRegistry.unpoint();
	}

	dll_registry::instance()->unloadAll();

	ND_NOTICE("[XML_RPC_Server] Unloading system XML-RPC interfaces.\n");
	delete Settings_InterfaceP;
	delete Channel_InterfaceP;
	delete Device_InterfaceP;
	delete Timer_InterfaceP;
	delete CE_DLL_InterfaceP;

}

void XML_RPC_Server::registerBuiltins() {
	ND_DEBUG("[XML_RPC_Server] Registering builtin XML-RPC interfaces.\n");

	if (CE_DLL_InterfaceP) CE_DLL_InterfaceP->register_methods(rpcRegistry);
	if (Timer_InterfaceP) Timer_InterfaceP->register_methods(rpcRegistry);
	if (Device_InterfaceP) Device_InterfaceP->register_methods(rpcRegistry);
	if (Channel_InterfaceP) Channel_InterfaceP->register_methods(rpcRegistry);
	if (Settings_InterfaceP) Settings_InterfaceP->register_methods(rpcRegistry);
}

void XML_RPC_Server::_autoLoad() {
	// Autoload DLLs
	if ( CEcfg::instance()->existsSrv("dllAutoload") ) {
		ND_INFO("[XML_RPC_Server] Loading any DLLs specified in the configuration file.\n");
		Setting& dll_list = CEcfg::instance()->getSrv("dllAutoload");

		for ( int idx = 0; idx < dll_list.getLength(); ++idx ) {
			std::string dllName = dll_list[idx];

			ND_DEBUG("   ... loading DLL %s.\n", dllName.c_str());
			try {
				dll_registry::instance()->loadIfNotLoaded(dllName, rpcRegistry);
			}
			catch (const RegistryError& e) {
				ND_ERROR("%s\n", e.what());
			}
		}
	}
}

void XML_RPC_Server::_setup() {
	ND_NOTICE("[XML_RPC_Server] Initializing XML-RPC method registry.\n");

	dll_registry::instance()->rebuild();

	std::string listenAddr(globalCfg.listenAddr);
	if ( CEcfg::instance()->existsSrv("listenAddress") )
		listenAddr = CEcfg::instance()->getSrv("listenAddress").c_str();

	int listenPort = globalCfg.listenPort;
	if ( CEcfg::instance()->existsSrv("listenPort") )
		listenPort = CEcfg::instance()->getSrv("listenPort");

	ND_INFO("[XML_RPC_Server] Spawning HTTP service listening at %s:%d\n", listenAddr.c_str(), listenPort);

	ACE_INET_Addr port_to_listen (listenPort, listenAddr.c_str(), AF_INET);
	_acceptor = new ClientAcceptor();
	_acceptor->reactor (ACE_Reactor::instance());
	if (_acceptor->open (port_to_listen) == -1) {
		delete _acceptor;
		throw Exception("Unable to set port for HTTP server.");
	}
}

std::string XML_RPC_Server::newTimerId(const std::string& name) {
	ACE_Utils::UUID* uuid;
	ACE_Utils::UUID_Generator uuid_generator;

	uuid = uuid_generator.generate_UUID();

	std::string id = "timer_" + name + "_" + uuid->to_string()->c_str();

	return id;
}

void XML_RPC_Server::addTimer(const std::string& timer_name, const long timer_id) {
	_timers[timer_name] = timer_id;
}

/// Get rid of the timer from the Reactor and maintain our own list. If
/// erase_only == true, just eliminate it from our list. This is helpful
/// for removing a timer that has just executed.
void XML_RPC_Server::clearTimer(const std::string& timer_name, bool erase_only) {
	ACE_Recursive_Thread_Mutex mutex("clear_timer");

	mutex.acquire();

	if ( _timers.find(timer_name) == _timers.end() ) {
		ND_NOTICE("[XML_RPC_Server] Cancellation of %s failed: not in module map.\n",
			timer_name.c_str());
		return;
	}

	if ( ! erase_only ) {
		if ( ACE_Reactor::instance()->cancel_timer(_timers[timer_name]) ) {
			ND_INFO("[XML_RPC_Server] Cancelled timer %s (Reactor ID #%d).\n",
				timer_name.c_str(), _timers[timer_name]);
		}
		else {
			ND_NOTICE("[XML_RPC_Server] Cancellation of %s (Reactor ID #%d) failed: not known to Reactor (expired?).\n",
				timer_name.c_str(), _timers[timer_name]);
		}
	}

	_timers.erase(timer_name);

	mutex.release();
}

void XML_RPC_Server::clearAllTimers() {
	ND_NOTICE("[XML_RPC_Server] Clearing all %d timer(s).\n", _timers.size());

	// Get rid of any timers that are still around
	while ( _timers.size() ) {
		std::map<std::string, long>::iterator pos = _timers.begin();
		clearTimer(pos->first);
	}
}

void XML_RPC_Server::doCall(const std::string& callName,
	xmlrpc_c::paramList const& paramList,
	xmlrpc_c::value* resultP) {

	ACE_TRACE("XML_RPC_Server::do_call");

	if (resultP == 0) resultP = new xmlrpc_c::value;

	std::string callXml, responseXml;

	try {
		xmlrpc_c::xml::generateCall(callName, paramList, &callXml);
		rpcRegistry->processCall(callXml, &responseXml);
		xmlrpc_c::xml::parseSuccessfulResponse(responseXml, resultP);
	}
	catch (const nd_error& e) {
		ND_ERROR("[XML_RPC_Server] CE error while making an internal XML-RPC call: %s\n", e.what());
	}
	catch (const girerr::error& e) {
		ND_ERROR("[XML_RPC_Server] XML-RPC error while making an internal XML-RPC call: %s\n", e.what());
	}
	catch (...) {
		ND_ERROR("[XML_RPC_Server] Unknown error occurred while making an internal XML-RPC call.\n");
	}
}

void XML_RPC_Server::pauseHttpServer() {
	_acceptor->pause();
}

void XML_RPC_Server::unpauseHttpServer() {
	_acceptor->unpause();
}

void XML_RPC_Server::switchToConfigFile(const std::string& cfgFile) {
	pauseHttpServer();

	// Destroy existing devices and channels first
	channels::instance()->removeAll();
	devices::instance()->removeAll();

	dll_registry::instance()->unloadAll();

	CEcfg::instance()->loadConfig(globalCfg.cfgFilePath, cfgFile);

	devices::instance()->autoLoad();
	channels::instance()->autoLoad();

	unpauseHttpServer();
}

} // namespace nasaCE

