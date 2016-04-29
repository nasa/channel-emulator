/* -*- C++ -*- */

//=============================================================================
/**
 * @file   Channel.hpp
 * @author Tad Kollar  
 *
 * $Id: Channel.hpp 2032 2013-06-05 17:44:30Z tkollar $
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _NASA_CHANNEL_HPP_
#define _NASA_CHANNEL_HPP_

#include "BaseTrafficHandler.hpp"
#include "CE_Config.hpp"
#include "CE_DLL_Registry.hpp"
#include "DeviceRegistry.hpp"
#include "XML_RPC_Server.hpp"
#include <string>
#include <map>

namespace nasaCE {

struct DuplicateHandlerName: public Exception {
	DuplicateHandlerName(const std::string e): Exception(e) { }
};

struct HandlerNotFound: public Exception {
	HandlerNotFound(const std::string e): Exception(e) { }
};

typedef std::map<std::string, BaseTrafficHandler*> HandlerMap;

//=============================================================================
/**
 * @class Channel
 * @brief A container that manages traffic handlers.
 */
//=============================================================================
class Channel {

public:
	/// Default constructor.
	Channel(const std::string& name = "");

	/// Destructor deletes then removes pointers to all internal segments,
	/// then gets rid of terminals.
	~Channel();

	/// Load all the segments specified in the config file.
	void autoLoad();

	/// Read-only accessor for the channel's name.
	std::string getName() const { return _name; }

	/// Read-write accessor for the channel's name.
	std::string setName(const std::string& newName) { return ( _name = newName ); }

	/// Iterate through the map comparing the provided name to each Handler name.
	/// @return A pointer to the found Handler or 0.
	BaseTrafficHandler* find(const std::string& handlerName);

	/// Add the provided BaseTrafficHandler to the _handlers map.
	/// @throw DuplicateHandlerName If a handler with that name is already a key in the map.
	void add(BaseTrafficHandler* newHandler);

	/// @brief Create a HandlerType traffic handler and add it to this channel.
	///
	/// @param handlerName An identifier for the new handler.
	/// @param dllPtr A pointer to the DLL the handler is associated with.
	/// @throw DuplicateHandlerName If a handler with that name is already a key in the map.
	template<class HandlerType>
	HandlerType* add(const std::string& handlerName, const CE_DLL* dllPtr = 0) {
		if ( find(handlerName) )
			throw DuplicateHandlerName(std::string("Handler named ") + handlerName +
				std::string(" already exists in Channel ") + getName() + std::string("."));

		HandlerType* handler = new HandlerType(handlerName, getName(), dllPtr);
		add(dynamic_cast<BaseTrafficHandler*>(handler));

		initializeHandlerConnections<HandlerType>(handler);

		return handler;
	}

	/// @brief Make sure the DLL is loaded first, and then create a new traffic handler.
	///
	/// @param dllName The name of the DLL to load.
	/// @param segName The name of the new traffic handler.
	void add(const std::string& dllName, const std::string& segName);

	/// @brief Using the handler's config file setting, create all of its connections.
	///
	/// @param handler A pointer to the handler to operate on.
	template<class HandlerType>
	void initializeHandlerConnections(HandlerType* handler) {
		Setting& dllName = CEcfg::instance()->getOrAddString(handler->cfgKey("dllName"), "");
		dllName = handler->ptrDLL()->getName();

		// Make default primary output connection
		if ( CEcfg::instance()->exists(handler->cfgKey("primaryOutput")) ) {
			Setting& primaryOut = CEcfg::instance()->get(handler->cfgKey("primaryOutput"));

			BaseTrafficHandler* targetHandler = find(primaryOut[0]);
			if (targetHandler) {
				HandlerLink::InputRank inputRank = HandlerLink::PrimaryInput;
				std::string primaryOutStr = primaryOut[1];

				if (tolower(primaryOutStr[0]) == 'a') {
					ND_DEBUG("[Channel %s] Connecting %s Primary Output to %s Auxiliary Input.\n",
						getName().c_str(), handler->getName().c_str(), targetHandler->getName().c_str());
					inputRank = HandlerLink::AuxInput;
				}
				else {
					ND_DEBUG("[Channel %s] Connecting %s Primary Output to %s Primary Input.\n",
						getName().c_str(), handler->getName().c_str(), targetHandler->getName().c_str());
				}

				handler->connectOutput(targetHandler, HandlerLink::PrimaryOutput, inputRank, false);
			}
			else {
				std::string primaryOutStr = primaryOut[0];
				ND_NOTICE("[Channel %s] No target named '%s' in channel to connect %s primary output to.\n",
					getName().c_str(), primaryOutStr.c_str(), handler->getName().c_str());
			}
		}

		// Make default auxiliary output connection
		if ( CEcfg::instance()->exists(handler->cfgKey("auxOutput")) ) {
			Setting& auxOut = CEcfg::instance()->get(handler->cfgKey("auxOutput"));

			BaseTrafficHandler* targetHandler = find(auxOut[0]);
			HandlerLink::InputRank inputRank = HandlerLink::PrimaryInput;
			std::string auxOutStr = auxOut[1];

			if (targetHandler) {
				if (tolower(auxOutStr[0]) == 'a') {
					ND_DEBUG("[Channel %s] Connecting %s Auxiliary Output to %s Auxiliary Input.\n",
						getName().c_str(), handler->getName().c_str(), targetHandler->getName().c_str());
					inputRank = HandlerLink::AuxInput;
				}
				else {
					ND_DEBUG("[Channel %s] Connecting %s Auxiliary Output to %s Primary Input.\n",
						getName().c_str(), handler->getName().c_str(), targetHandler->getName().c_str());
				}

				handler->connectOutput(targetHandler, HandlerLink::AuxOutput, inputRank, false);
			}
			else {
				std::string auxOutStr = auxOut[0];
				ND_NOTICE("[Channel %s] No target named '%s' in channel to connect %s auxiliary output to.\n",
					getName().c_str(), auxOutStr.c_str(), handler->getName().c_str());
			}
		}

		// Make default device connection
		if ( CEcfg::instance()->exists(handler->cfgKey("deviceName")) ) {
			Setting& deviceName = CEcfg::instance()->get(handler->cfgKey("deviceName"));
			CE_Device* device = devices::instance()->device(deviceName);

			handler->connectDevice(device);
		}

		Setting& immediateStart = CEcfg::instance()->getOrAddBool(handler->cfgKey("immediateStart"), true);
		if (static_cast<bool>(immediateStart)) handler->open();
	}

	/// Find and removed the named BaseTrafficHandler from the _handlers map.
	/// @throw HandlerNotFound If the handler name cannot be found.
	void remove(const std::string& handlerName);

	/// An accessor to the map of traffic handlers.
	HandlerMap& handlers() { return _handlers; }

	/// @brief Iterate through the map of handlers and activate each one.
	///
	/// Order is random, so each handler must detect whether its target is unavailable.
	/// Does <b>not</b> affect the hardware devices.
	void activate();

	/// @brief Iterate through the map of handlers and stop traffic in each one.
	///
	/// Order is random, so each handler must detect whether its target is unavailable.
	/// Does <b>not</b> affect the hardware devices.
	void pause();

	/// @brief Return a string naming a subsection of the Channel in the config file.
	/// @param keyName The subsection to reference.
	std::string cfgKey(const std::string& keyName = "") const {
		return cfgKey(getName(), keyName);
	}

	/// @brief Return a string naming a subsection of the Channel in the config file.
	/// @param channelName The name of the channel (since this is a static function)
	/// @param keyName The subsection to reference.
	static std::string cfgKey(const std::string& channelName, const std::string& keyName) {
		if ( keyName.empty() ) return "Channels." + channelName;

		return "Channels." + channelName + "." + keyName;
	}

	/// @brief Determine from the config file whether one segment ultimately outputs to another segment.
	/// @param possibleDependant The config file path of the segment that may depend on another.
	/// @param possibleProdider The config file path of the segment that may provide services to possibleDependant.
	/// @return True if possibleDependent does NOT depend on possibleProvider.
	static bool doesNotDependOn(std::pair<std::string, std::set<std::string> >& possibleDependant,
								std::pair<std::string, std::set<std::string> >& possibleProvider);

	/// @brief Provide a list of segments that will receive services from the named segment, according to the config file.
	/// @param seg A reference to the group of the segment to obtain information for.
	/// @param deps An initially empty set that will be filled with all dependant names.
	static void getDependencies(const Setting& seg, std::set<std::string>& deps);

	/// @brief Recursively add unique dependants to the existing set.
	/// @param depName The name of the dependant to check.
	/// @param seg A reference to the originally requested segment to find dependents for.
	/// @param deps A reference to the set that is being filled with dependant names.
	static void addDependency(const std::string& depName, const Setting& seg, std::set<std::string>& deps);

private:
	/// The name of the channel; may not be set because the enclosing map also knows the name.
	std::string _name;


	HandlerMap _handlers;
};

} // namespace nasaCE

#endif