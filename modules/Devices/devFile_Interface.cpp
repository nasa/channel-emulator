/* -*- C++ -*- */

//=============================================================================
/**
 * @file   devFile_Interface.cpp
 * @author Tad Kollar  
 *
 * $Id: devFile_Interface.cpp 2403 2013-12-12 19:08:39Z tkollar $
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "DeviceRegistry.hpp"
#include "devFile_Interface.hpp"
#include "SettingsManager.hpp"

namespace nasaCE {

devFile_Interface::devFile_Interface(): Device_Interface_Templ<devFile>("devFile_Interface") {	}

devFile_Interface::devFile_Interface(xmlrpc_c::registryPtr& rpcRegistry):
	Device_Interface_Templ<devFile>("devFile_Interface") {
	ACE_TRACE("devFile_Interface::devFile_Interface");

	register_methods(rpcRegistry);
}

devFile_Interface::~devFile_Interface()
{
}

void devFile_Interface::manage(xmlrpc_c::paramList const& paramList,
	xmlrpc_c::value* retvalP) {

	ACE_TRACE("devFile_Interface::manage");

	std::string devname = static_cast<std::string>(paramList.getString(0));

	if (devices::instance()->exists(devname)) {
		throw Exception("Device with that name already exists.");
	}

	devFile* file_dev = new devFile(devname, this);

	std::string filename = "";
	if (paramList.size() > 1) filename = static_cast<std::string>(paramList.getString(1));
	file_dev->setFilename(filename);

	devices::instance()->add(file_dev);
	*retvalP = xmlrpc_c::value_nil();
}

void devFile_Interface::unmanage(xmlrpc_c::paramList const& paramList,
	xmlrpc_c::value* retvalP) {

	ACE_TRACE("devFile_Interface::unmanage");

	std::string devname = static_cast<std::string>(paramList.getString(0));
	devices::instance()->remove(devname);

	CEcfg::instance()->get("Devices").remove(devname);

	*retvalP = xmlrpc_c::value_nil();
}

void devFile_Interface::open_for_reading(xmlrpc_c::paramList const& paramList,
	xmlrpc_c::value* retvalP) {

	ACE_TRACE("devFile_Interface::open_for_reading");

	std::string devname = static_cast<std::string>(paramList.getString(0));
	devFile* file_dev = dynamic_cast<devFile*>(devices::instance()->device(devname));

	file_dev->openForReading();
	*retvalP = xmlrpc_c::value_nil();
}

void devFile_Interface::open_for_writing(xmlrpc_c::paramList const& paramList,
	xmlrpc_c::value* retvalP) {

	ACE_TRACE("devFile_Interface::open_for_writing");

	std::string devname = static_cast<std::string>(paramList.getString(0));
	devFile* file_dev = dynamic_cast<devFile*>(devices::instance()->device(devname));

	file_dev->openForWriting();
	*retvalP = xmlrpc_c::value_nil();
}

void devFile_Interface::close(xmlrpc_c::paramList const& paramList,
	xmlrpc_c::value* retvalP) {

	ACE_TRACE("devFile_Interface::close");

	std::string devname = static_cast<std::string>(paramList.getString(0));
	devFile* file_dev = dynamic_cast<devFile*>(devices::instance()->device(devname));

	file_dev->close();
	*retvalP = xmlrpc_c::value_nil();
}

void devFile_Interface::is_open(xmlrpc_c::paramList const& paramList,
	xmlrpc_c::value* retvalP) {

	ACE_TRACE("devFile_Interface::is_open");

	std::string devname = static_cast<std::string>(paramList.getString(0));
	devFile* file_dev = dynamic_cast<devFile*>(devices::instance()->device(devname));

	bool is_open = file_dev->isOpen();
	*retvalP = xmlrpc_c::value_boolean(is_open);
}

void devFile_Interface::set_filename(xmlrpc_c::paramList const& paramList,
	xmlrpc_c::value* retvalP) {

	ACE_TRACE("devFile_Interface::set_filename");

	std::string devname = static_cast<std::string>(paramList.getString(0));
	std::string filename = static_cast<std::string>(paramList.getString(1));

	devFile* file_dev = dynamic_cast<devFile*>(devices::instance()->device(devname));

	file_dev->setFilename(filename);
	*retvalP = xmlrpc_c::value_nil();
}

void devFile_Interface::get_filename(xmlrpc_c::paramList const& paramList,
	xmlrpc_c::value* retvalP) {

	ACE_TRACE("devFile_Interface::get_filename");

	std::string devname = static_cast<std::string>(paramList.getString(0));

	devFile* file_dev = dynamic_cast<devFile*>(devices::instance()->device(devname));

	std::string filename = file_dev->getFilename();
	*retvalP = xmlrpc_c::value_string(filename);
}

void devFile_Interface::register_methods(xmlrpc_c::registryPtr& rpcRegistry) {
	ACE_TRACE("devFile_Interface::register_methods");

	DEV_TEMPLATE_REGISTER_METHODS(devFile, devFile_Interface);
	REGISTER_METHOD(manage, "devFile.manage")
	REGISTER_METHOD(unmanage, "devFile.unmanage")
	REGISTER_METHOD(open_for_reading, "devFile.openForReading")
	REGISTER_METHOD(open_for_writing, "devFile.openForWriting")
	REGISTER_METHOD(close, "devFile.close")
	REGISTER_METHOD(is_open, "devFile.isOpen")
	REGISTER_METHOD(set_filename, "devFile.setFilename")
	REGISTER_METHOD(get_filename, "devFile.getFilename")
}

devFile_Interface* devFile_InterfaceP;

}
