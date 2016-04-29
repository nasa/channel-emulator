/* -*- C++ -*- */

//=============================================================================
/**
 * @file   devSink_Interface.cpp
 * @author Tad Kollar  
 *
 * $Id: devSink_Interface.cpp 1883 2013-02-20 19:35:17Z tkollar $
 * Copyright (c) 2011.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "DeviceRegistry.hpp"
#include "devSink_Interface.hpp"
#include "SettingsManager.hpp"

namespace nasaCE {

devSink_Interface::devSink_Interface(): Device_Interface_Templ<devSink>("devSink_Interface") {	}

devSink_Interface::devSink_Interface(xmlrpc_c::registryPtr& rpcRegistry):
	Device_Interface_Templ<devSink>("devSink_Interface") {
	ACE_TRACE("devSink_Interface::devSink_Interface");

	register_methods(rpcRegistry);
}

devSink_Interface::~devSink_Interface()
{
}

void devSink_Interface::register_methods(xmlrpc_c::registryPtr& rpcRegistry) {
	ACE_TRACE("devSink_Interface::register_methods");

	DEV_TEMPLATE_REGISTER_METHODS(devSink, devSink_Interface);
}

devSink_Interface* devSink_InterfaceP;

}