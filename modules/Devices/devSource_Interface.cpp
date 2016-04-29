/* -*- C++ -*- */

//=============================================================================
/**
 * @file   devSource_Interface.cpp
 * @author Tad Kollar  
 *
 * $Id: devSource_Interface.cpp 2319 2013-08-05 19:26:58Z tkollar $
 * Copyright (c) 2011.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "DeviceRegistry.hpp"
#include "devSource_Interface.hpp"
#include "SettingsManager.hpp"

namespace nasaCE {

devSource_Interface::devSource_Interface(): Device_Interface_Templ<devSource>("devSource_Interface") {	}

devSource_Interface::devSource_Interface(xmlrpc_c::registryPtr& rpcRegistry):
	Device_Interface_Templ<devSource>("devSource_Interface") {
	ACE_TRACE("devSource_Interface::devSource_Interface");

	register_methods(rpcRegistry);
}

devSource_Interface::~devSource_Interface()
{
}

void devSource_Interface::set_pattern(xmlrpc_c::paramList const& paramList,
	xmlrpc_c::value* retvalP) {
	ACE_TRACE("devSource_Interface::set_pattern");
	const std::vector<xmlrpc_c::value> xml_rpc_pattern = paramList.getArray(1);

	std::vector<uint8_t> newPattern;

	for (unsigned i = 0; i < xml_rpc_pattern.size(); i++ ) {
		newPattern.push_back(xmlrpc_c::value_int(xml_rpc_pattern[i]));
	}

	find_device_(paramList)->setPattern(newPattern);

	*retvalP = xmlrpc_c::value_nil();
}

void devSource_Interface::get_pattern(xmlrpc_c::paramList const& paramList,
	xmlrpc_c::value* retvalP) {
	ACE_TRACE("devSource_Interface::get_pattern");

	std::vector<uint8_t> pattern;
	find_device_(paramList)->getPattern(pattern);

	std::vector<xmlrpc_c::value> xml_rpc_pattern;

	for (unsigned i = 0; i < pattern.size(); i++ ) {
		xml_rpc_pattern.push_back(xmlrpc_c::value_int(pattern[i]));
	}

	*retvalP = xmlrpc_c::value_array(xml_rpc_pattern);
}

void devSource_Interface::set_pattern_buffer_size(xmlrpc_c::paramList const& paramList,
	xmlrpc_c::value* retvalP) {
	ACE_TRACE("devSource_Interface::set_pattern_buffer_size");
	const int newSize = paramList.getInt(1);

	find_device_(paramList)->setPatternBufferSize(newSize);

	*retvalP = xmlrpc_c::value_nil();
}

void devSource_Interface::get_pattern_buffer_size(xmlrpc_c::paramList const& paramList,
	xmlrpc_c::value* retvalP) {
	ACE_TRACE("devSource_Interface::get_pattern_buffer_size");

	*retvalP = xmlrpc_c::value_int(find_device_(paramList)->getPatternBufferSize());
}

void devSource_Interface::register_methods(xmlrpc_c::registryPtr& rpcRegistry) {
	ACE_TRACE("devSource_Interface::register_methods");

	DEV_TEMPLATE_REGISTER_METHODS(devSource, devSource_Interface);
	REGISTER_METHOD(set_pattern, "devSource.setPattern");
	REGISTER_METHOD(get_pattern, "devSource.getPattern");
	REGISTER_METHOD(set_pattern_buffer_size, "devSource.setPatternBufferSize");
	REGISTER_METHOD(get_pattern_buffer_size, "devSource.getPatternBufferSize");
}

devSource_Interface* devSource_InterfaceP;

}