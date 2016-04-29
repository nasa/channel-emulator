/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modTM_Macros_Interface.cpp
 * @author Tad Kollar  
 *
 * $Id: modTM_Macros_Interface.cpp 2325 2013-08-08 17:03:25Z tkollar $
 * Copyright (c) 2009-2013.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "CE_DLL.hpp"
#include "InterfaceMacros.hpp"
#include "modTM_Macros.hpp"

namespace nTM_Macros {

//=============================================================================
/**
 * @class modTM_Macros_Interface
 * @author Tad Kollar  
 * @brief XML-RPC methods for automatically creating TM channels.
*/
//=============================================================================
class modTM_Macros_Interface: public CE_DLL {

public:
	modTM_Macros_Interface(xmlrpc_c::registryPtr& rpcRegistry):
		CE_DLL("modTM_Macros_Interface") {
		ACE_TRACE("modTM_Macros_Interface::modTM_Macros_Interface");

		register_methods(rpcRegistry);
	}

	void new_forward_channel(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP) {
		ACE_TRACE("modTM_Macros_Interface::new_forward_channel");
		const std::string channelName(paramList.getString(0));

		if (paramList.size() > 1) {
			const xmlrpc_c::value_struct settingsStruct(paramList[1]);
			newTMChannelProps props(settingsStruct);
			_macros.newForwardChannel(channelName, props);
		}
		else {
			newTMChannelProps props;
			_macros.newForwardChannel(channelName, props);
		}

		*retvalP = xmlrpc_c::value_nil();
	}

	void new_return_channel(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP) {
		ACE_TRACE("modTM_Macros_Interface::new_return_channel");
		const std::string channelName(paramList.getString(0));

		if (paramList.size() > 1) {
			const xmlrpc_c::value_struct settingsStruct(paramList[1]);
			newTMChannelProps props(settingsStruct);
			_macros.newReturnChannel(channelName, props);
		}
		else {
			newTMChannelProps props;
			_macros.newReturnChannel(channelName, props);
		}

		*retvalP = xmlrpc_c::value_nil();
	}

	/// Export specified methods via XML-RPC.
	void register_methods(xmlrpc_c::registryPtr&);

private:
	modTM_Macros _macros;

};

modTM_Macros_Interface* modTM_Macros_InterfaceP;

extern "C" nasaCE::CE_DLL* activate(xmlrpc_c::registryPtr& rpcRegistry) {
	ND_DEBUG("Activating modTM_Macros_Interface\n");
	modTM_Macros_InterfaceP = new modTM_Macros_Interface(rpcRegistry);
	return modTM_Macros_InterfaceP;
}

extern "C" void deactivate() {
	ND_DEBUG("Deactivating modTM_Macros_Interface\n");
	delete modTM_Macros_InterfaceP;
	modTM_Macros_InterfaceP = 0;
}

METHOD_CLASS(new_forward_channel, "n:sS",
	"Create a complete TM forward channel with the specified properties.", modTM_Macros_InterfaceP);
METHOD_CLASS(new_return_channel, "n:sS",
	"Create a complete TM return channel with the specified properties.", modTM_Macros_InterfaceP);

void modTM_Macros_Interface::register_methods(xmlrpc_c::registryPtr& rpcRegistry) {
	ACE_TRACE("modTM_Macros_Interface::register_methods");

	REGISTER_METHOD(new_forward_channel, "modTM_Macros.newForwardChannel");
	REGISTER_METHOD(new_return_channel,  "modTM_Macros.newReturnChannel");
}

} // namespace nTM_Macros
