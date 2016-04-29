/* -*- C++ -*- */

//=============================================================================
/**
 * @file    CE_Config.hpp
 * @author Tad Kollar  
 *
 * $Id: CE_Config.hpp 2320 2013-08-06 16:01:20Z tkollar $
 * Copyright (c) 2008.
 *      NASA Glenn Research Center.  All rights reserved.
 */
//=============================================================================

#ifndef _CE_CONFIG_HPP_
#define _CE_CONFIG_HPP_

#include <nd_macros.hpp>
#include <xmlrpc-c/base.hpp>
#include <xmlrpc-c/registry.hpp>

#include <string>
#include <map>
#include <stdexcept>

#include <ace/Log_Msg.h>
#include <ace/Process_Mutex.h>
#include <ace/SString.h>

#include <libconfig.h++>

namespace nasaCE {

extern bool exit_requested;

//=============================================================================
/**
 * @class GlobalCfgClass
 * @author Tad Kollar  
 * @brief A simple class to contain globally-accessible settings.
 */
//=============================================================================
class GlobalCfgClass {
public:
	const std::string defaultSrvCfgFile;     // The default server config file path + name.
	const std::string defaultCfgFilePath;    // The default directory in which the channel/device config files are found.
	const std::string defaultCfgFileWebPath; // The default string to browse the config file directory with.
	const std::string defaultCfgFileName;    // The default name of the initial channel/device config file.
	const std::string defaultDocTopDir;      // The default parent directory of the fileIO and www folders.
	const std::string defaultFileIOPath;     // The default area where data files may be read from/written to.
	const std::string defaultFileIOWebPath;  // The default string to browse as the file IO directory with.
	const std::string defaultWebDocPath;     // The default area where web documents are loaded from.
	const std::string defaultLogFilePath;    // The default path + name of the log file, if used.
	const std::string defaultListenAddr;     // The default IPv4 address to listen on.
	const int defaultListenPort;             // The default TCP port to listen on.
	const int defaultDebugLevel;             // The default verbosity to log/debug at.
	#ifdef DEFINE_DEBUG
	const bool defaultDebug;                 // The default setting of whether debugging is on or off.
	#endif

	std::string srvCfgFile;     // The server config file path + name.
	std::string cfgFilePath;    // The directory in which the channel/device config files are found.
	std::string cfgFileWebPath; // The string to browse the config file directory with.
	std::string cfgFileName;    // The name of the initial channel/device config file.
	std::string docTopDir;      // The parent directory of the fileIO and www folders.
	std::string fileIOPath;     // The area where data files may be read from/written to.
	std::string fileIOWebPath;  // The string to browse the file IO directory with.
	std::string webDocPath;     // The area where web documents are loaded from.
	std::string logFilePath;    // The path + name of the log file, if used.
	std::string listenAddr;     // The IPv4 address to listen on.
	std::list<std::string> protectedEthInterfaces; // List of interfaces not to alter.
	int listenPort;             // The TCP port to listen on.
	int debugLevel;             // The verbosity to log/debug at.
	#ifdef DEFINE_DEBUG
	bool debug;                 // Whether debugging is on or off.
	#endif

	GlobalCfgClass();
};

extern GlobalCfgClass globalCfg;

typedef std::vector<xmlrpc_c::value> xarray;
typedef std::map<std::string, xmlrpc_c::value> xstruct;

//=============================================================================
/** @class Exception
 *  @brief A sublass of std::runtime_error that accepts different string types.
 */
//=============================================================================
struct Exception: public std::runtime_error {
protected:
	std::string type_;

public:
	/// @param msg The error message as a String reference.
	explicit Exception(const std::string& msg, const std::string& etype = "Exception"):
		std::runtime_error(msg.c_str()), type_(etype) {
		ND_WARNING("%s thrown: %s\n", type_.c_str(), msg.c_str());
	}

	/// @param msg The error message as an ACE_TString reference.
	explicit Exception(const ACE_TString& msg, const std::string& etype = "Exception"):
		std::runtime_error(msg.c_str()), type_(etype) {
		ND_WARNING("%s thrown: %s\n", type_.c_str(), msg.c_str());
	}

	/// @param msg The error message as char pointer.
	explicit Exception(const char* msg, const std::string& etype = "Exception"):
		std::runtime_error(msg), type_(etype) {
		ND_WARNING("%s thrown: %s\n",	type_.c_str(), msg);
	}

	virtual ~Exception() throw() {}
};

} // namespace nasaCE

#endif
