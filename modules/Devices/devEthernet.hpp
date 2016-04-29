/* -*- C++ -*- */

//=============================================================================
/**
 * @file   devEthernet.hpp
 * @author Tad Kollar  
 *
 * $Id: devEthernet.hpp 2320 2013-08-06 16:01:20Z tkollar $
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _NASA_DEV_ETHERNET_HPP_
#define _NASA_DEV_ETHERNET_HPP_

#include "CE_Device.hpp"
#include "CE_Macros.hpp"
#include "EthernetFrame.hpp"
#include "SettingsManager.hpp"
#include "utils.hpp"

namespace Linux {
	extern "C" {
		#include <linux/types.h>
		#include <linux/rtnetlink.h>
		#include <netlink/netlink.h>
		#include <net/if.h>
		#include <linux/if_packet.h>
		#include <linux/if_ether.h>
		#include <netinet/in.h>
		#include <netlink/netlink-kernel.h>
		#include <netlink/socket.h>
		#include <netlink/cache.h>
		#include <netlink/object-api.h>
		#include <netlink/route/link.h>
	}
}

using namespace Linux;

namespace nasaCE {

//=============================================================================
/**
 * @class devEthernet
 * @brief Everything about an Ethernet interface that's useful for emulation.
 */
//=============================================================================
class devEthernet: public CE_Device {
public:
	// --- EXCEPTIONS ---------------------------------------------------------------
	/// @class IOCtlFailed
	/// @brief Thrown when an ioctl() returns an error.
	struct IOCtlFailed: public Exception {
		IOCtlFailed(const std::string e): Exception(e) { }
	};

	/// @class IsProtected
	/// @brief Thrown when a modification or write is attempted on a protected/read-only device.
	struct IsProtected: public Exception {
		IsProtected(const std::string e): Exception(e) { }
	};

	/// @class IsDown
	/// @brief Thrown when a method that requires an active device is attempted on an inactive one.
	struct IsDown: public Exception {
		IsDown(const std::string e): Exception(e) { }
	};

	/// @class DoesNotExist
	/// @brief Thrown when the device is unknown to the host operating system.
	struct DoesNotExist: public Exception {
		DoesNotExist(const std::string e): Exception(e) { }
	};
	// ------------------------------------------------------------------------------

	/// Path to the sysfs net directory.
	static std::string topNetDir;

	/// The config file path to common Ethernet device settings.
	static std::string ethernetDeviceSection;

	/// @brief Default constructor.
	/// @param newName The name of the interface as known to Linux.
	/// @throw IsProtected If the device is marked "protected" in the configuration.
	devEthernet(const std::string& newName = "", const CE_DLL* newDLLPtr = 0);

	/// Destructor.
	~devEthernet();

	/// @brief Return the state of the interface.
	/// @return True if the object has <b>not</b> been associated with an
	/// existing interface yet; false if it <b>has</b>.
	bool isNew() const { return ! isInitialized(); }

	/// Is the interface up (true) or down (false).
	bool isUp() { return _checkFlag(up); }

	/// Gets the 6-octet MAC address.
	std::string getMacStr();

	/// Determine if the user has configured the specified interface to be read-only.
	/// @param ifaceName The name of the network interface to check.
	/// @return True if protected, false if modifiable.
	static bool isProtected(const std::string& ifaceName);

	/// Determine if the user has configured this interface to be read-only.
	/// @return True if protected, false if modifiable.
	bool isProtected();

	/// Determine if the user has configured the interface setup to be read-only.
	/// @param action A description of the desired action on the interface.
	/// @throw IsProtected Thrown if the interface is protected.
	void checkProtection(const std::string& action);

	/// Set the status of the interface to "up".
	/// @param setupSocket If true, run openSocket().
	/// @param setFlags If true, set flags as found in config file.
	/// @throw IsProtected If the interface is protected.
	/// @throw DoesNotExtist If the interface is unknown to the OS.
	/// @throw OperationFailed If the command otherwise fails.
	void activate(const bool setupSocket = true, const bool setFlags = true);

	/// Set the status of the interface to "down".
	/// @throw IsProtected If the interface is protected.
	/// @throw DoesNotExtist If the interface is unknown to the OS.
	/// @throw OperationFailed If the command otherwise fails.
	void deactivate();

	/// True means the interface is in promiscuous mode; false, not.
	inline bool isPromiscuous() { return _checkFlag(promisc); }

	/// Put the interface into promiscuous mode.
	/// @throw IsProtected If the interface is protected.
	/// @throw IsUninitialized If the interface has not been configured.
	/// @throw IsDown If the interface is not activated.
	/// @throw IOCtlFailed If ioctl returns an error.
	inline void enablePromiscuous() { setFlag(promisc, true); _promiscSetting = true; }

	/// Take the interface out of promiscuous mode.
	/// @throw IsProtected If the interface is protected.
	/// @throw IsUninitialized If the interface has not been configured.
	/// @throw IsDown If the interface is not activated.
	/// @throw IOCtlFailed If ioctl returns an error.
	inline void disablePromiscuous() { setFlag(promisc, false); _promiscSetting = false; }

	/// Allow the interface to process ARP packets.
	/// @throw IsProtected If the interface is protected.
	/// @throw IsUninitialized If the interface has not been configured.
	/// @throw IsDown If the interface is not activated.
	/// @throw IOCtlFailed If ioctl returns an error.
	inline void enableARP() { setFlag(noarp, false); _noArpSetting = false; }

	/// Prevent the interface from processing ARP packets.
	/// @throw IsProtected If the interface is protected.
	/// @throw IsUninitialized If the interface has not been configured.
	/// @throw IsDown If the interface is not activated.
	/// @throw IOCtlFailed If ioctl returns an error.
	inline void disableARP() { setFlag(noarp, true); _noArpSetting = true; }

	/// Return whether the interface is processing ARP packets or not.
	bool isNoARP() { return _checkFlag(noarp); }

	/// Determine if the device has a link and has finished negotiating.
	bool hasLink();

	/// Retrieve the state and address of the interface from Linux.
    void dumpBasicInfo();

	/// Get the index that the kernel knows of the device as.
	/// @throw DoesNotExist If exists() returns false.
	ACE_UINT32 index();

	/// Check for an entry in sysfs.
	/// @return True if device name is under /sys/class/net, false otherwise.
	bool exists();

	/// Call exists() and throws an exception if the result is false.
	/// @throw DoesNotExist If exists() returns false.
	void checkExistance();

	/// Clears then loads a list with discovered Ethernet interface names.
	static void listAllInterfaces(std::list<std::string>& ifaceNames);

	/// Loads counters from sysfs into the provided map.
	void loadCounters(std::map<std::string, xmlrpc_c::value>&);

	/// Turns the flag on or off with safety checks.
	/// @param flag The flag to modify.
	/// @param newSetting The value to change the flag to.
	/// @return The setting of the flag after the operation.
	bool setFlag(const ACE_UINT32 flag, const bool newSetting);

	/// Get the maximum transfer unit (MTU) of the interface.
	/// @return The retrieved MTU value.
	/// @throw IOCtlFailed If the ioctl call fails.
	size_t getMTU();

	/// Set the maximum transfer unit (MTU) of the interface.
	/// @param mtu The new MTU value.
	/// @throw IOCtlFailed If the ioctl call fails.
	void setMTU(const size_t);

	/// Create a packet socket for sending and receiving frames.
	/// @return The new descriptor, or -1 on error.
	ACE_HANDLE openSocket();

	/// Read-only accessor to _packetSocket.
	ACE_HANDLE getSocket() { return _packetSocket; }

	/// Shutdown the packet socket, if open.
	/// @return Zero if close() succeeded, -1 on error.
	int closeSocket();

	/// Override CE_Device::getType to return "Ethernet".
	std::string getType() const { return "Ethernet"; }

	/// Write-only accessor to _snapLen.
	void setSnapLen(const int newVal) { _snapLen = newVal; }

	/// Read-only accessor to _snapLen.
	int getSnapLen() const { return _snapLen; }

	/// Write-only accessor to _activateOnLoad.
	void setActivateOnLoad(const bool newVal) { _activateOnLoad = newVal; }

	/// Read-only accessor to _activateOnLoad.
	bool getActivateOnLoad() const { return _activateOnLoad; }

	/// Write-only accessor to _ifaceName/_ifaceNameSetting.
	/// If newVal is not empty, the devEthernet is now considered initialized.
	void setIfaceName(const std::string& newVal) {
		if ( ! newVal.empty() ) setInitialized(true);
		else setInitialized(false);

		_ifaceName = newVal;
		_ifaceNameSetting = newVal;
	}

	/// Read-only accessor to _ifaceName.
	std::string getIfaceName() const { return _ifaceName; }

    nl_sock* nl_socket_alloc();
    int& nl_connect(nl_sock* _netlinkSocket, int arg2);
    void nl_socket_free(nl_sock* _netlinkSocket);



private:
	/// The OS identifier for the interface may differ from the CE name
	std::string _ifaceName;
	Setting& _ifaceNameSetting;

	/// The name of the network card's manufacturer.
	std::string _vendor;

	/// The model of the network card.
	std::string _model;

	/// Network device flags derived directly from linux/if.h.
	enum devEthernetFlags {
		up,				/*!< Interface is up */
		broadcast,		/*!< Broadcast address valid */
		debug,			/*!< Turn on debugging */
		loopback,		/*!< Is a loopback net */
		point_to_point, /*!< Interface has p-p link */
		notrailers,		/*!< Avoid use of trailers */
		running,		/*!< Interface RFC2863 OPER_UP */
		noarp,			/*!< No ARP protocol */
		promisc,		/*!< Receive all packets */
		allmulti,		/*!< Receive all multicast packets */
		master,			/*!< Master of a load balancer */
		slave,			/*!< Slave of a load balancer */
		multicast,		/*!< Supports multicast */
		portsel,		/*!< Can set media type */
		automedia,		/*!< Auto media select active */
		dynamic,		/*!< Dialup device with changing addresses */
		lowerUp,		/*!< Driver signals L1 up */
		dormant,		/*!< Driver signals dormant */
		echo,			/*!< Echo sent packets */
		numFlags		/*!< Helpful for looping operations */
	};

	enum OperStates {
		OperUnknown,
		OperNotPreset,
		OperDown,
		OperLowerLayerDown,
		OperTesting,
		OperDormant,
		OperUp
	};

	/// Printable names of all network device flags.
	static const std::string _devEthernetFlagNames[];

	/// Check the setting of the specified flag against the interface state.
	bool _checkFlag(const ACE_UINT32 flag);

	/// Change the specified flags on the network device.
	void _setFlag(const ACE_UINT32 flag, const bool newSetting);

	/// Returns the sysfs path of the interface.
	inline std::string _sysDir() const { return topNetDir + "/" + getIfaceName(); }

	/// Perform an ioctl with the specified arguments.
	/// @param op The type of request.
	/// @param ifreq A reference to a pre-allocated ifreq structure.
	/// @param errPrefix The message to prepend to a thrown Exception.
	/// @param fd A pre-opened descriptor (if it must be obtained in a different method).
	/// @throw IOCtlFailed Thrown when the ioctl call fails.
	void _ioctl(const int op, Linux::ifreq& ifreq, const std::string& errPrefix = "ioctl error: ",
		ACE_HANDLE fd = ACE_INVALID_HANDLE);

	/// Store the original flags here so they can be restored when management ends.
	Linux::ifreq _savedFlags;

	/// The socket that full ethernet frames can be read from or written to.
	ACE_HANDLE _packetSocket;

	/// The interface with Linux Netlink.
	nl_sock* _netlinkSocket;

	/// Suggested length in octets of the read to be used for the packet socket.
	Setting& _snapLen;

	/// Whether to activate (bring up) the interface when it is constructed.
	Setting& _activateOnLoad;

	/// Config file reference for the noarp flag
	Setting& _noArpSetting;

	/// Config file reference for the promisc flag
	Setting& _promiscSetting;

	/// Get flags from the config file and apply them to the interface.
	void _applyFlagsFromSettings();
};

} // namespace nasaCE

#endif // _NASA_DEV_ETHERNET_HPP_
