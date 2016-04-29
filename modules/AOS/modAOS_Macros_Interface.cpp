/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modAOS_Macros_Interface.cpp
 * @author Tad Kollar  
 *
 * $Id: modAOS_Macros_Interface.cpp 2325 2013-08-08 17:03:25Z tkollar $
 * Copyright (c) 2009-2013.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "CE_DLL.hpp"
#include "InterfaceMacros.hpp"
#include "modAOS_Macros.hpp"

namespace nAOS_Macros {

//=============================================================================
/**
 * @class modAOS_Macros_Interface
 * @author Tad Kollar  
 * @brief XML-RPC methods for automatically creating AOS channels.
*/
//=============================================================================
class modAOS_Macros_Interface: public CE_DLL {

public:
	modAOS_Macros_Interface(xmlrpc_c::registryPtr& rpcRegistry):
		CE_DLL("modAOS_Macros_Interface") {
		ACE_TRACE("modAOS_Macros_Interface::modAOS_Macros_Interface");

		register_methods(rpcRegistry);
	}

	void new_forward_channel(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP) {
		ACE_TRACE("modAOS_Macros_Interface::new_forward_channel");
		const std::string channelName(paramList.getString(0));

		if (paramList.size() > 1) {
			const xmlrpc_c::value_struct settingsStruct(paramList[1]);
			newAOSChannelProps props(settingsStruct);
			_macros.newForwardChannel(channelName, props);
		}
		else {
			newAOSChannelProps props;
			_macros.newForwardChannel(channelName, props);
		}

		*retvalP = xmlrpc_c::value_nil();
	}

	void new_return_channel(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP) {
		ACE_TRACE("modAOS_Macros_Interface::new_return_channel");
		const std::string channelName(paramList.getString(0));

		if (paramList.size() > 1) {
			const xmlrpc_c::value_struct settingsStruct(paramList[1]);
			newAOSChannelProps props(settingsStruct);
			_macros.newReturnChannel(channelName, props);
		}
		else {
			newAOSChannelProps props;
			_macros.newReturnChannel(channelName, props);
		}

		*retvalP = xmlrpc_c::value_nil();
	}

	/// Export specified methods via XML-RPC.
	void register_methods(xmlrpc_c::registryPtr&);

private:
	modAOS_Macros _macros;

};

modAOS_Macros_Interface* modAOS_Macros_InterfaceP;

extern "C" nasaCE::CE_DLL* activate(xmlrpc_c::registryPtr& rpcRegistry) {
	ND_DEBUG("Activating modAOS_Macros_Interface\n");
	modAOS_Macros_InterfaceP = new modAOS_Macros_Interface(rpcRegistry);
	return modAOS_Macros_InterfaceP;
}

extern "C" void deactivate() {
	ND_DEBUG("Deactivating modAOS_Macros_Interface\n");
	delete modAOS_Macros_InterfaceP;
	modAOS_Macros_InterfaceP = 0;
}

METHOD_CLASS(new_forward_channel, "n:sS",
	"Create a complete AOS forward channel with the specified properties.", modAOS_Macros_InterfaceP);
METHOD_CLASS(new_return_channel, "n:sS",
	"Create a complete AOS return channel with the specified properties.", modAOS_Macros_InterfaceP);

void modAOS_Macros_Interface::register_methods(xmlrpc_c::registryPtr& rpcRegistry) {
	ACE_TRACE("modAOS_Macros_Interface::register_methods");

	REGISTER_METHOD(new_forward_channel, "modAOS_Macros.newForwardChannel");
	REGISTER_METHOD(new_return_channel,  "modAOS_Macros.newReturnChannel");
}

} // namespace nAOS_Macros
