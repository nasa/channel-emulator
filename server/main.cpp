/* -*- C++ -*- */

//=============================================================================
/**
 * @file   main.cpp
 * @author Tad Kollar  
 *
 * $Id: main.cpp 2339 2013-08-30 19:28:15Z tkollar $
 * Copyright (c) 2008.
 *      NASA Glenn Research Center.  All rights reserved.
 */
//=============================================================================

#include "CE_Config.hpp"
#include "Time_Handler.hpp"
#include "XML_RPC_Server.hpp"
#include "ChannelRegistry.hpp"
#include "SettingsManager.hpp"

#include <ace/Arg_Shifter.h>
#include <ace/Local_Memory_Pool.h>
#include <ace/streams.h>
#include <ace/Reactor.h>
#include <ace/TP_Reactor.h>
#include <ace/Thread_Manager.h>
#include <ace/Thread_Semaphore.h>

#include <cstdlib>

namespace nasaCE {

GlobalCfgClass::GlobalCfgClass():
	defaultSrvCfgFile("/etc/ctsced.cfg"),
	defaultCfgFilePath("/etc/ctsce"),
	defaultCfgFileWebPath("/cfg"),
	defaultCfgFileName("devices-channels.cfg"),
	defaultDocTopDir("/var/lib/ctsce"),
	defaultFileIOPath(defaultDocTopDir + "/fileIO"),
	defaultFileIOWebPath("/fileIO"),
	defaultWebDocPath(defaultDocTopDir + "/www"),
	defaultLogFilePath("/var/log/ctsced.log"),
	defaultListenAddr("0.0.0.0"),
	defaultListenPort(8080),
	defaultDebugLevel(7),
	#ifdef DEFINE_DEBUG
	defaultDebug(true),
	#endif
	srvCfgFile(defaultSrvCfgFile),
	cfgFilePath(defaultCfgFilePath),
	cfgFileWebPath(defaultCfgFileWebPath),
	cfgFileName(defaultCfgFileName),
	docTopDir(defaultDocTopDir),
	fileIOPath(defaultFileIOPath),
	fileIOWebPath(defaultFileIOWebPath),
	webDocPath(defaultWebDocPath),
	logFilePath(defaultLogFilePath),
	listenAddr(defaultListenAddr),
	listenPort(defaultListenPort),
	debugLevel(defaultDebugLevel)
	#ifdef DEFINE_DEBUG
	,debug(defaultDebug)
	#endif
	{

}

GlobalCfgClass globalCfg;

void setupLogging(const bool foreground, const int debug_level) {
	if ( ! foreground ) {

		ACE_OSTREAM_TYPE *logfile = new std::ofstream(globalCfg.logFilePath.c_str());
		ACE_LOG_MSG->msg_ostream(logfile, 1);
		ACE_LOG_MSG->set_flags(ACE_Log_Msg::OSTREAM);
		ACE_LOG_MSG->clr_flags(ACE_Log_Msg::STDERR);
	}

	unsigned long mask = LM_DEBUG|LM_INFO|LM_NOTICE|LM_WARNING|
		LM_ERROR|LM_CRITICAL|LM_ALERT|LM_EMERGENCY;

	if ( debug_level < 8 ) mask &= ~LM_DEBUG;
	if ( debug_level < 7 ) mask &= ~LM_INFO;
	if ( debug_level < 6 ) mask &= ~LM_NOTICE;
	if ( debug_level < 5 ) mask &= ~LM_WARNING;
	if ( debug_level < 4 ) mask &= ~LM_ERROR;
	if ( debug_level < 3 ) mask &= ~LM_CRITICAL;
	if ( debug_level < 2 ) mask &= ~LM_ALERT;
	if ( debug_level < 1 ) mask &= ~LM_EMERGENCY;

	ACE_LOG_MSG->priority_mask(mask, ACE_Log_Msg::PROCESS);
}

void usage() {
	cerr	<< "Usage: ce_admind [options]" << endl
			<< "  --config/-c <filename>            Specify initial devices/channels config file name (Default: " << globalCfg.defaultCfgFileName << ")" << endl
			<< "  --config-path/-C <directory>      Specify path to devices/channels config file directory (Default: " << globalCfg.defaultCfgFilePath << ")" << endl
			<< "  --debug/-d <level>                Specify debug level (0-8) (Default: " << globalCfg.defaultDebugLevel << ")" << endl
			#ifdef DEFINE_DEBUG
			<< "  --default-debug/-D                With -d 8, whether all modules should output by default (Default:" << globalCfg.defaultDebug << ")" << endl
			#endif
			<< "  --foreground/-f                   Remain in the foreground at startup" << endl
			<< "  --fileio-path/-F <directory>      Specify path to file I/O directory (Default: " << globalCfg.defaultFileIOPath << ")" << endl
			<< "  --log/-l <filename>               Specify path to the log file (Default: " << globalCfg.defaultLogFilePath << ")" << endl
			<< "  --protect/-p ethX,ethY,...        Comma-separated list of Ethernet interfaces to avoid" << endl
			<< "  --srv-config/-s <filename>        Specify path to the server config file (Default: " << globalCfg.defaultSrvCfgFile << ")" << endl
			<< "  --no-save/-n                      Do not save a new config file on exit" << endl
			<< "  --web-doc-path/-w <direcrory>     Specify path to web documents (Default: " << globalCfg.defaultWebDocPath << ")" << endl;
}

int parseCommandLine(int argc, ACE_TCHAR* argv[]) {

	ACE_Arg_Shifter arg(argc, argv);
	arg.consume_arg();
	int cl_debug_level = -1;
	int cl_foreground = -1;
	bool
		cl_cfg_path = false,
		cl_cfg_name = false,
		cl_file_io = false,
		cl_web_doc = false,
		cl_log_file = false,
		cl_no_save = false,
		cl_protect = false;
	struct stat fileStats;

	#ifdef DEFINE_DEBUG
	int cl_default_debug = -1;
	#endif

	// Read in the command line options
	while ( arg.is_anything_left() ) {
		std::string current_arg = arg.get_current();

		if ( current_arg == "-c" || current_arg == "--config" ) {
			if (! arg.is_option_next()) { usage(); return -1; }
			arg.consume_arg();
			globalCfg.cfgFileName = arg.get_current();
			cl_cfg_name = true;
			ND_INFO("[Main] Initial device/channel configuration file specified as \"%s\".\n", globalCfg.cfgFileName.c_str());
		}
		else if ( current_arg == "-C" || current_arg == "--config-path" ) {
			if (! arg.is_option_next()) { usage(); return -1; }
			arg.consume_arg();
			globalCfg.cfgFilePath = arg.get_current();
			cl_cfg_path = true;
			ND_INFO("[Main] Path to device/channel configuration files specified as \"%s\".\n", globalCfg.cfgFilePath.c_str());
		}
		else if ( current_arg == "-s" || current_arg == "--srv-config" ) {
			if (! arg.is_option_next()) { usage(); return -1; }
			arg.consume_arg();
			globalCfg.srvCfgFile = arg.get_current();
			ND_INFO("[Main] Server configuration file specified as \"%s\".\n", globalCfg.srvCfgFile.c_str());
		}
		else if ( current_arg == "-F" || current_arg == "--fileio-path" ) {
			if (! arg.is_option_next()) { usage(); return -1; }
			arg.consume_arg();
			globalCfg.fileIOPath = arg.get_current();
			cl_file_io = true;
			ND_INFO("[Main] Path to file I/O directory specified as \"%s\".\n", globalCfg.fileIOPath.c_str());
		}
		else if ( current_arg == "-w" || current_arg == "--web-doc-path" ) {
			if (! arg.is_option_next()) { usage(); return -1; }
			arg.consume_arg();
			globalCfg.webDocPath = arg.get_current();
			cl_web_doc = true;
			ND_INFO("[Main] Path to web document directory specified as \"%s\".\n", globalCfg.webDocPath.c_str());
		}
		else if ( current_arg == "-d" || current_arg == "--debug" ) {
			if (! arg.is_option_next()) { usage(); return -1; }
			arg.consume_arg();
			cl_debug_level = ACE_OS::strtol(arg.get_current(), 0, 10);
		}
		else if ( current_arg == "-f" || current_arg == "--foreground" ) {
			cl_foreground = true;
		}
		else if ( current_arg == "-n" || current_arg == "--no-save" ) {
			cl_no_save = true;
			CEcfg::instance()->setSaveFlag(false);
		}
		else if ( current_arg == "-l" || current_arg == "--log" ) {
			if (! arg.is_option_next()) { usage(); return -1; }
			arg.consume_arg();
			globalCfg.logFilePath = arg.get_current();
			cl_log_file = true;
			ND_INFO("[Main] Path to log file specified as \"%s\".\n", globalCfg.logFilePath.c_str());
		}
		else if ( current_arg == "-p" || current_arg == "--protect" ) {
			if (! arg.is_option_next()) { usage(); return -1; }
			arg.consume_arg();
			cl_protect = true;
			std::istringstream ifaceList(arg.get_current());
			std::string iface;
			while(getline(ifaceList, iface, ',')) {
				ND_INFO("[Main] Adding %s to protected Ethernet interface list.\n", iface.c_str());
				globalCfg.protectedEthInterfaces.push_back(iface);
			}
		}
		#ifdef DEFINE_DEBUG
		else if ( current_arg == "-D" || current_arg == "--default-debug" ) {
			if (! arg.is_option_next()) { usage(); return -1; }
			cl_default_debug = true;
		}
		#endif
		else { usage(); return -1; }
		arg.consume_arg();
	}

	// Initialize the settings manager by reading the config files.
	try {
		CEcfg::instance()->initialize(cl_cfg_path, cl_cfg_name);
	}
	catch (const libconfig::FileIOException& err) {
		ACE_ERROR_RETURN((LM_ERROR, "[Main] Unable to read specified configuration file: %s\n", err.what()), -1);
	}

	// Command line settings take precedence over config file settings.
	if ( cl_debug_level > -1 ) {
		globalCfg.debugLevel = cl_debug_level;
	}
	else if ( CEcfg::instance()->existsSrv("debugLevel") ) {
		globalCfg.debugLevel = static_cast<int>(CEcfg::instance()->getSrv("debugLevel"));
	}

	#ifdef DEFINE_DEBUG
	if ( cl_default_debug ) {
		globalCfg.debug = cl_default_debug;
	}
	else if ( CEcfg::instance()->existsSrv("defaultDebug") ) {
		globalCfg.debug = static_cast<bool>(CEcfg::instance()->getSrv("defaultDebug"));
	}
	#endif

	bool foreground = false;
	if ( cl_foreground == true ) {
		foreground = true;
	}
	else if ( CEcfg::instance()->existsSrv("foreground") ) {
		foreground = static_cast<bool>(CEcfg::instance()->getSrv("foreground"));
	}

	if ( ! cl_log_file && CEcfg::instance()->existsSrv("logFile") ) {
		globalCfg.logFilePath = std::string(CEcfg::instance()->getSrv("logFile").c_str());
	}
	setupLogging(foreground, globalCfg.debugLevel);

	if ( ! cl_web_doc && CEcfg::instance()->existsSrv("webDocDir") ) {
		globalCfg.webDocPath = std::string(CEcfg::instance()->getSrv("webDocDir").c_str());
	}

	if ( ACE_OS::stat(globalCfg.webDocPath.c_str(), &fileStats) == -1 ) {
		ACE_ERROR_RETURN((LM_ERROR, "[Main] Unable to access web document directory: %s\n", strerror(errno)), -1);
	}

	if ( ! cl_file_io && CEcfg::instance()->existsSrv("fileIODir") ) {
		globalCfg.fileIOPath = std::string(CEcfg::instance()->getSrv("fileIODir").c_str());
	}

	if ( ACE_OS::stat(globalCfg.fileIOPath.c_str(), &fileStats) == -1 ) {
		const int errVal = errno;
		if (errVal == ENOENT) {
			ACE_ERROR((LM_NOTICE, "[Main] Creating specified file I/O directory %s.", globalCfg.fileIOPath.c_str()));
			if ( ACE_OS::mkdir(globalCfg.fileIOPath.c_str(), 0700) == -1 ) {
				ACE_ERROR_RETURN((LM_ERROR, "[Main] Unable to create file I/O directory: %s\n", strerror(errno)), -1);
			}
		}
		else {
			ACE_ERROR_RETURN((LM_ERROR, "[Main] Unable to access file I/O directory: %s\n", strerror(errVal)), -1);
		}
	}

	if ( ! cl_no_save && CEcfg::instance()->existsSrv("saveCfgAtExit") ) {
		CEcfg::instance()->setSaveFlag(static_cast<bool>(CEcfg::instance()->getSrv("saveCfgAtExit")));
	}

	// If protected interfaces were not specified on the command line, check the server config file.
	if ( ! cl_protect && CEcfg::instance()->existsSrv("protectedEthInterfaces")) {
		Setting& protIfaces = CEcfg::instance()->getSrv("protectedEthInterfaces");

		for ( int idx = 0; idx < protIfaces.getLength(); ++idx ) {
			std::string iface(static_cast<const char*>(protIfaces[idx]));
			ND_INFO("[Main] Adding %s to protected Ethernet interface list.\n", iface.c_str());
			globalCfg.protectedEthInterfaces.push_back(iface);
		}
	}

	if ( ! foreground ) { return ACE_OS::fork(); }

	return 0;
}

bool exit_requested = false;

#ifdef DEFINE_DEBUG
bool globalDefaultDebug = true;
#endif

// Handle the signal when the initiator is expecting an int* return type
class ExitHandler: public ACE_Event_Handler {
public:
	int handle_signal(int signum, siginfo_t*, ucontext_t*) {
		ND_NOTICE("[Exit] Received request to exit the program.\n");

		if (exit_requested) {
			ND_WARNING("[Exit] Already shutting down, ignoring additional exit request.\n");
			return 0;
		}

		exit_requested = true;
		CEcfg::instance()->shutdown();

		// Shut down the reactor before we delete DLLs it may be using.
		ND_DEBUG("[Exit] Shutting down the reactor.\n");
		ACE_Reactor::instance()->end_reactor_event_loop();
		ACE_Reactor::instance()->close();

		// Remove channels before the DLLs they are composed of are deleted.
		channels::instance()->removeAll();

		// Shutdown manually instead of being destroyed so we can unload
		// things in an orderly fashion before the globals that we rely
		// on are also destroyed.
		ND_DEBUG("[Exit] Shutting down the XML-RPC server.\n");
		xmlrpc_server::instance()->shutdown();

		ND_DEBUG("[Exit] Finished handling the shutdown signal.\n");

		return 0;
	}
};

} // namespace nasaCE

void savePid(std::string& pidFileName) {
	struct stat fileStats;

	pid_t pid = ACE_OS::getpid();

	ND_INFO("[Main] Starting CE administrative daemon with PID %d.\n", pid);

	if ( ! ACE_OS::stat("/var/run", &fileStats) && ! (fileStats.st_mode & 0040000) ) {
		pidFileName = "/tmp/ctsced.pid";
	}

	// If an existing PID file is found, see if the process is really still there
	if ( ! ACE_OS::stat(pidFileName.c_str(), &fileStats) ) {
		ACE_HANDLE oldPidFd = ACE_OS::open(pidFileName.c_str(), O_RDONLY);
		if ( oldPidFd > -1 ) {
			char pidStr[20];
			ACE_OS::memset(pidStr, 0, 20);
			ACE_OS::read(oldPidFd, pidStr, 19);
			ACE_OS::close(oldPidFd);
			std::string procFilename = std::string("/proc/") + std::string(pidStr);

			if ( ! ACE_OS::stat(procFilename.c_str(), &fileStats) && (fileStats.st_mode & 0040000) ) {
				ND_ERROR("An existing Channel Emulator process is running with PID %s.\n", pidStr);
				exit(1);
			}
		}
	}

	ACE_HANDLE pidFd = ACE_OS::open(pidFileName.c_str(), O_CREAT|O_WRONLY|O_TRUNC);

	if ( pidFd < 0 ) {
		ND_WARNING("[Main] Could not create PID savefile %s: %s\n", pidFileName.c_str(),
			ACE_OS::strerror(errno));
		return;
	}

	std::ostringstream os;

	os << pid;
	std::string pidStr(os.str());
	ACE_OS::write(pidFd, pidStr.c_str(), pidStr.length());

	ACE_OS::close(pidFd);
}

using namespace nasaCE;

static ACE_THR_FUNC_RETURN eventLoop(void* arg) {
	ACE_Reactor *reactor = static_cast<ACE_Reactor *>(arg);

	reactor->owner(ACE_OS::thr_self());
	reactor->run_reactor_event_loop();
	return 0;
};

ExitHandler* doExit = 0;

int ACE_TMAIN(int argc, ACE_TCHAR* argv[]) {
	std::string pidFileName = "/var/run/ctsced.pid";

	if ( ACE_OS::getuid() != 0 && ACE_OS::geteuid() != 0 ) {
		ND_ALERT("%s must be run as superuser.\n", argv[0]);
		return 1;
	}

	// NetworkData::allocator = new ACE_Dynamic_Cached_Allocator<ACE_Process_Mutex>(500000, 1024);
	// NetworkData::allocator = new ACE_Allocator_Adapter< ACE_Malloc<ACE_Local_Memory_Pool,ACE_Process_Mutex> >("NetworkData");

	// Seed here so various modules won't need to seed as they're loaded.
	srand48(ACE_OS::time());

	/*
	ACE_Lock_Adapter<ACE_Thread_Semaphore> dataLock;
	NetworkData::lockingStrategy = &dataLock;
	*/

	int cli_res = parseCommandLine(argc, argv);
	if (cli_res < 0) return cli_res; // Error setting up, exit.
	else if (cli_res > 0) return 0; // Exit after forking a child to the background.

	savePid(pidFileName);

	ACE_TP_Reactor tp_reactor;
	ACE_Reactor ce_reactor(&tp_reactor);
	ACE_Reactor::instance(&ce_reactor);

	scenarioTimeKeeper::instance()->initialize();
	xmlrpc_server::instance()->initialize();

	doExit = new ExitHandler;
	ACE_Reactor::instance()->register_handler(SIGINT, doExit);
	ACE_Reactor::instance()->register_handler(SIGTERM, doExit);

	int reactorThreads = 4;
	if ( CEcfg::instance()->existsSrv("reactorThreads") ) {
		reactorThreads = CEcfg::instance()->getSrv("reactorThreads");
	}

	ND_INFO("[Main] Spawning %d threads to handle reactor events.\n", reactorThreads);
	ACE_Thread_Manager::instance()->spawn_n(reactorThreads, eventLoop, ACE_Reactor::instance());

	int exitVal = ACE_Thread_Manager::instance()->wait();

	ND_DEBUG("[Main] End of channel emulator main function, waiting for remaining threads...\n");

	if ( ACE_OS::unlink(pidFileName.c_str()) < 0 )
		ND_WARNING("[Main] Problem removing PID file %s: %s\n", pidFileName.c_str(),
			ACE_OS::strerror(errno));

	return exitVal;
}
