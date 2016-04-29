/* -*- C++ -*- */

//=============================================================================
/**
 * @file   devEthernet.cpp
 * @author Tad Kollar  
 *
 * $Id: devEthernet.cpp 2320 2013-08-06 16:01:20Z tkollar $
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "devEthernet.hpp"
#include "MacAddress.hpp"
#include "SettingsManager.hpp"

#include <bitset>
#include <ace/Dirent.h>
#include <ace/OS_main.h>

using namespace Linux;

namespace nasaCE {

const std::string devEthernet::_devEthernetFlagNames[] = {
	"Up",
	"Broadcast",
	"Debug",
	"Loopback",
	"PointToPoint",
	"Notrailers",
	"Running",
	"NoARP",
	"Promisc",
	"Allmulti",
	"Master",
	"Slave",
	"Multicast",
	"Portsel",
	"Automedia",
	"Dynamic",
	"LowerUp",
	"Dormant",
	"Echo"
};

std::string devEthernet::ethernetDeviceSection = "Devices.EthernetDevice_Common";

std::string devEthernet::topNetDir = "/sys/class/net";

devEthernet::devEthernet(const std::string& newName /* = "" */,
	const CE_DLL* newDLLPtr /* = 0 */):
	CE_Device(newName, newDLLPtr),
	_ifaceName(""),
	_ifaceNameSetting(CEcfg::instance()->getOrAddString(cfgKey("ifaceName"), _ifaceName)),
	_vendor(""),
	_model(""),
	_packetSocket(ACE_INVALID_HANDLE),
	_netlinkSocket(0),
	_snapLen(CEcfg::instance()->getOrAddInt(cfgKey("snapLen"), 0)),
	_activateOnLoad(CEcfg::instance()->getOrAddBool(cfgKey("activateOnLoad"), true)),
	_noArpSetting(CEcfg::instance()->getOrAddBool(cfgKey("flagNoARP"), false)),
	_promiscSetting(CEcfg::instance()->getOrAddBool(cfgKey("flagPromisc"), false))
	{

	CEcfg::instance()->getOrAddString(cfgKey("devType")) = "Ethernet";

	if ( strcmp(_ifaceNameSetting.c_str(), "") == 0) {
		MOD_INFO("Getting interface name from Device name (%s).", newName.c_str());
		setIfaceName(newName);
	}
	else {
		MOD_INFO("Getting interface name from config file (%s).", _ifaceNameSetting.c_str());
		_ifaceName = static_cast<const char*>(_ifaceNameSetting);
	}

	if ( ! _ifaceName.empty() ) {
		if ( isProtected() )
			throw IsProtected("Device " + getIfaceName() + " is marked 'protected' and cannot be managed by the CE.");

		// IOCtlFailed is thrown if there's an error
		try {
			_ioctl(SIOCGIFFLAGS, _savedFlags, "Error getting flags for interface " + getIfaceName()
				+ " via SIOCGIFFLAGS: ");
		}
		catch (...) {
			MOD_ERROR("Could not read flags to restore later.");
		}

		try {
			if (getActivateOnLoad()) activate();
		}
		catch (...) {
			MOD_ERROR("Unable to activate at this time.");
		}

		if ( CEcfg::instance()->exists(cfgKey("MTU")) )
			setMTU(static_cast<unsigned int>(CEcfg::instance()->get(cfgKey("MTU"))));
	}

	if ( getSnapLen() == 0 ) {
		MOD_DEBUG("Automatically setting snapLen.");
		setSnapLen(static_cast<int>(getMTU() + 30));
		MOD_DEBUG("SnapLen set to %d.", getSnapLen());
	}

	MOD_DEBUG("Connecting netlink socket.");
	_netlinkSocket = Linux::nl_socket_alloc();

 	if (Linux::nl_connect(_netlinkSocket, NETLINK_ROUTE) < 0) {
 		ND_ERROR("[%s] Unable to connect netlink socket: %s\n", nl_geterror(errno));
 		Linux::nl_socket_free(_netlinkSocket);
 	}

 	MOD_DEBUG("Ethernet constructor complete.");
}

devEthernet::~devEthernet() {
	MOD_DEBUG("Running ~devEthernet().");
	try {
		_ioctl(SIOCSIFFLAGS, _savedFlags, "Error restoring saved flags via SIOCSIFFLAGS: ");
	}
	catch (...) {
	}

	closeSocket();

	if ( _netlinkSocket ) Linux::nl_socket_free(_netlinkSocket);

}

void devEthernet::_ioctl(const int op, Linux::ifreq& ifr,
	const std::string& errPrefix, ACE_HANDLE fd) {
	ACE_TRACE("devEthernet::_ioctl");

	ACE_OS::strncpy(ifr.ifr_name, getIfaceName().c_str(), IFNAMSIZ);

	if ( fd == ACE_INVALID_HANDLE ) {
		try { fd = get_ctl_fd(); }
		catch (const Exception& e) { throw e; }
	}

	int err = ACE_OS::ioctl(fd, op, &ifr);
	ACE_OS::close(fd);

	if (err < 0) throw IOCtlFailed(getIfaceName() + ": " + errPrefix + std::string(ACE_OS::strerror(errno)));
}

bool devEthernet::_checkFlag(const ACE_UINT32 flag) {
	ACE_TRACE("devEthernet::_checkFlag");

	Linux::ifreq ifr;

	_ioctl(SIOCGIFFLAGS, ifr, "Error getting flags for interface " + getIfaceName() + " via SIOCGIFFLAGS: ");

	std::bitset<numFlags> flags(ifr.ifr_flags);

	return flags[flag];
}

void devEthernet::_setFlag(const ACE_UINT32 flag, const bool newSetting) {
	ACE_TRACE("devEthernet::_setFlag");
	Linux::ifreq ifr;

	try {
		_ioctl(SIOCGIFFLAGS, ifr, "Error getting flags for interface " + getIfaceName() + " via SIOCGIFFLAGS: ");
	}
	catch (const IOCtlFailed& e) {
		MOD_ERROR("%s", e.what());
		return;
	}

	std::bitset<numFlags> flags(ifr.ifr_flags);

	if ( flags[flag] != newSetting ) {
		flags[flag] = newSetting;
		ifr.ifr_flags = flags.to_ulong();

		// IOCtlFailed is thrown if there's an error
		try {
			_ioctl(SIOCSIFFLAGS, ifr, "Error setting flags via SIOCGIFFLAGS: ");
		}
		catch (const IOCtlFailed& e) {
			MOD_ERROR("%s", e.what());
			return;
		}
	}
}

std::string devEthernet::getMacStr() {
	ACE_TRACE("devEthernet::getMacStr");
	Linux::ifreq ifr;
	ACE_HANDLE fd = ACE_OS::socket(AF_INET, SOCK_DGRAM, 0);

	if (! fd)
		throw Exception("Cannot open socket to obtain hardware address for interface " + getIfaceName() + ".");

	// IOCtlFailed is thrown if there's an error
	_ioctl(SIOCGIFHWADDR, ifr, "Error getting hw address for interface " +
		getIfaceName() + " via SIOCGIFHWADDR: ", fd);

	ACE_OS::close(fd);

	return MacAddress::addr2str(reinterpret_cast<ACE_UINT8*>(&ifr.ifr_hwaddr.sa_data));
}

bool devEthernet::isProtected(const std::string& ifaceName) {
	ACE_TRACE("devEthernet::isProtected");

	std::list<std::string>::iterator pos;

	for ( pos = globalCfg.protectedEthInterfaces.begin(); pos != globalCfg.protectedEthInterfaces.end(); ++pos) {
		if ( ! ifaceName.compare(*pos) ) return true;
	}

	return false;
}

bool devEthernet::isProtected() {
	ACE_TRACE("devEthernet::isProtected");
	return isProtected(getIfaceName());
}

void devEthernet::checkProtection(const std::string& action) {
	if ( isProtected() ) {
		throw IsProtected("Cannot " + action + " protected interface " + getIfaceName() + ".");
	}
}

void devEthernet::activate(const bool setupSocket /* = true */, const bool setFlags /* = true */) {
	ACE_TRACE("devEthernet::activate");
	MOD_DEBUG("Activating interface %s.", getIfaceName().c_str());

	checkProtection("activate");

	if ( _checkFlag(up) ) {
		MOD_WARNING("devEthernet::activate(): Device is already up.");
	}
	else {
		_setFlag(up, true); // IOCtlFailed is thrown if there's an error

		if ( ! _checkFlag(up) )
			throw OperationFailed("Failed to activate network interface " + getIfaceName() + ".");
	}

	if (setupSocket) openSocket();

	if (setFlags) _applyFlagsFromSettings();

	/*
	if ( static_cast<bool>(_promiscSetting) ) enablePromiscuous();
	else disablePromiscuous();

	if ( static_cast<bool>(_noArpSetting) ) disableARP();
	else enableARP();
	*/

}

void devEthernet::deactivate() {
	ACE_TRACE("devEthernet::deactivate");

	if ( ! isInitialized() ) return;

	checkProtection("de-activate");

	closeSocket();

	_setFlag(up, false); // IOCtlFailed is thrown if there's an error

	if ( _checkFlag(up) )
		throw OperationFailed("Failed to deactivate network interface " +
			getIfaceName() + ".");
}

bool devEthernet::setFlag(const ACE_UINT32 flag, const bool newSetting) {
	ACE_TRACE("devEthernet::setFlag");

	checkProtection("change " + _devEthernetFlagNames[flag] + " flag setting on");
/*
	if ( isNew() )
		throw IsUnitialized("Cannot change " + _devEthernetFlagNames[flag] +
			" flag setting on uninitialized interface " +
			getIfaceName() + ".");

	if ( ! isUp() )
		throw IsDown("Cannot change " + _devEthernetFlagNames[flag] +
			" flag on deactivated interface " +	getIfaceName() + ".");
*/
	_setFlag(flag, newSetting);

	// Put the change in the configuration.
	CEcfg::instance()->getOrAddBool(cfgKey("flag" + _devEthernetFlagNames[flag]), newSetting );

	return _checkFlag(flag);
}

ACE_UINT32 devEthernet::index() {
	ACE_TRACE("devEthernet::index");
	checkExistance();

	return if_nametoindex(getIfaceName().c_str());
}

bool devEthernet::exists() {
	ACE_TRACE("devEthernet::exists");
	ACE_stat fileStats;
	std::string iface_path = topNetDir + "/" + getIfaceName();

	if ( ! ACE_OS::stat(iface_path.c_str(), &fileStats) ) return true;
	return false;
}

void devEthernet::checkExistance() {
	if ( ! exists() ) throw DoesNotExist("Interface " + getIfaceName() +	" does not exist.");
}

void devEthernet::dumpBasicInfo() {
	ACE_TRACE("devEthernet::dumpBasicInfo");
	if ( isNew() ) { return; }

	ND_DEBUG("Iface name: %s, Index: %d, Up: %s, MAC: %s, Protected: %s\n",
		getIfaceName().c_str(), index(), isUp()? "true" : "false", getMacStr().c_str(),
		isProtected()? "true" : "false" );
}

void devEthernet::loadCounters(std::map<std::string, xmlrpc_c::value>& cmap) {
	ACE_TRACE("devEthernet::loadCounters");
	MOD_DEBUG("Loading counters.\n");

	ACE_Dirent statDir;
	std::string msg, stat_path = _sysDir() + "/statistics";

	if ( statDir.open(stat_path.c_str()) == -1) {
		throw OperationFailed("Failed to open interface statistics directory " +
			stat_path + " for reading: " + std::string(ACE_OS::strerror(errno)));
	}

	int entrycount = 0;
	for (ACE_DIRENT *entry; (entry = statDir.read ()) != 0; entrycount++ ) {
		std::string entryname_(entry->d_name);

		if ( entryname_ == "." || entryname_ == ".." ) continue;

		std::string contents = read_one_liner(stat_path + "/entryname_");

		cmap[entryname_] = xmlrpc_c::value_int(ACE_OS::atoi(contents.c_str()));
	}

	statDir.close();

	switch (entrycount) {
    case 0:
		throw OperationFailed("Readdir failed to read anything.");
    case 1:
		throw OperationFailed("Readdir failed, only matched directory name.");
    }
}

size_t devEthernet::getMTU() {
	ACE_TRACE("devEthernet::getMTU");
	Linux::ifreq ifr;

	MOD_DEBUG("Getting MTU from device.");

	// IOCtlFailed is thrown if there's an error
	_ioctl(SIOCGIFMTU, ifr, "Error getting MTU via SIOCGIFMTU: ");

	MOD_DEBUG("Found MTU to be %d.", ifr.ifr_mtu);

	return ifr.ifr_mtu;
}

void devEthernet::setMTU(const size_t mtu) {
	ACE_TRACE("devEthernet::setMTU");
	Linux::ifreq ifr;
	ifr.ifr_mtu = mtu;

	MOD_DEBUG("Setting MTU to %d.", mtu);

	// IOCtlFailed is thrown if there's an error
	_ioctl(SIOCSIFMTU, ifr, "Error setting MTU via SIOCSIFMTU: ");
	CEcfg::instance()->getOrAddInt(cfgKey("MTU"), static_cast<int>(mtu));

	MOD_DEBUG("Finished setting MTU.");
}

ACE_HANDLE devEthernet::openSocket() {
	MOD_DEBUG("Initializing packet socket.");
	checkProtection("initialize packet socket");

	ACE_HANDLE packetSocketFd = ACE_OS::socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));

	if ( packetSocketFd < 0 ) {
		MOD_ERROR("openSocket(): socket() error: %s.", ACE_OS::strerror(errno));
		return ACE_INVALID_HANDLE;
	}

	sockaddr_ll skAddr;
	socklen_t sockaddr_ll_len = sizeof(struct sockaddr_ll);
	skAddr.sll_family = AF_PACKET;
	skAddr.sll_protocol = htons(ETH_P_ALL);
	skAddr.sll_ifindex = index();

	if ( ACE_OS::bind(packetSocketFd, (struct sockaddr*) &skAddr, sockaddr_ll_len) < 0 ) {
		MOD_ERROR("openSocket(): bind() error: %s.", ACE_OS::strerror(errno));
		return ACE_INVALID_HANDLE;
	}

	MOD_DEBUG("Socket successfully bound.");

	return (_packetSocket = packetSocketFd);
}

int devEthernet::closeSocket() {
	int ret = 0;

	if ( _packetSocket != ACE_INVALID_HANDLE ) {
		if ( (ret = ACE_OS::close(_packetSocket)) < 0 )
			MOD_ERROR("closeSocket(): close() error: %s.", ACE_OS::strerror(errno));
		else _packetSocket = ACE_INVALID_HANDLE;
	}

	return ret;
}

bool devEthernet::hasLink() {
	if ( ! _netlinkSocket ) {
		MOD_ERROR("hasLink() called on uninitialized netlink socket.");
		return false;
	}

	nl_cache* cache;
	if ( rtnl_link_alloc_cache (_netlinkSocket, AF_UNSPEC, &cache) ) {
		MOD_ERROR("hasLink() rtnl_link_alloc_cache error: %s",
			nl_geterror(errno));
		return false;
	}

	rtnl_link* link = rtnl_link_get(cache, index());
	if ( ! link ) {
		MOD_ERROR("hasLink() rtnl_link_get error: %s", nl_geterror(errno));
			nl_cache_free(cache);
		return false;
	}

	// First, check that interface is able to send
	uint8_t operState = rtnl_link_get_operstate(link);

	#ifdef DEFINE_DEBUG
	char buf[100];
	rtnl_link_operstate2str (operState, buf, 100);
	MOD_DEBUG("operState is 0x%x (%s).", operState, buf);
	#endif

	bool ret = false;

	// Next make sure there's a carrier
	#ifndef IFF_LOWER_UP
	const int IFF_LOWER_UP = 0x10000;
	#endif

	#ifndef IFF_DORMANT
	const int IFF_DORMANT = 0x20000;
	#endif

	if ( operState == OperUp) {
		unsigned int flags = rtnl_link_get_flags (link);
		ret = ( ((flags & IFF_LOWER_UP) == IFF_LOWER_UP) && ((flags & IFF_DORMANT) != IFF_DORMANT) );
	}

	rtnl_link_put(link);
	nl_cache_free(cache);

	return ret;
}

void devEthernet::_applyFlagsFromSettings() {
	const int flagsToCheck[2] = { noarp, promisc };

	for ( int idx = 0; idx < 2; ++idx ) {
		std::string flagKey = cfgKey("flag" + _devEthernetFlagNames[flagsToCheck[idx]]);

		// Set false if unspecified.
		const bool flagVal = ( CEcfg::instance()->exists(flagKey) )? static_cast<bool>(CEcfg::instance()->get(flagKey)) : false;

		MOD_DEBUG("About to set flag %s to %s.", _devEthernetFlagNames[flagsToCheck[idx]].c_str(), flagVal? "true" : "false");
		_setFlag(flagsToCheck[idx], flagVal);
		MOD_DEBUG("Set flag complete.");
	}
}

void devEthernet::listAllInterfaces(std::list<std::string>& ifaceNames)  {
	ifaceNames.clear();

	ACE_Dirent dir;
	if ( dir.open(topNetDir.c_str()) ) {
		throw nd_error("Could not open directory " + topNetDir + " for reading.");
	}

	struct stat docStat;

	ACE_DIRENT* entry;

	while ( (entry = dir.read()) != NULL ) {
		std::string entryName(entry->d_name);
		if ( entryName[0] != '.' && ! isProtected(entryName) ) { ifaceNames.push_back(entryName); }
	}

	dir.close();
	ifaceNames.sort();
}


} // namespace nasaCE
