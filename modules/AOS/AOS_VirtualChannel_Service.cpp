/* -*- C++ -*- */

//=============================================================================
/**
 * @file   AOS_VirtualChannel_Service.cpp
 * @author Tad Kollar  
 *
 * $Id: AOS_VirtualChannel_Service.cpp 2405 2013-12-12 19:56:04Z tkollar $
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "AOS_VirtualChannel_Service.hpp"

namespace nasaCE {


AOS_VirtualChannel_Service::AOS_VirtualChannel_Service(const std::string& newName,
	const std::string& newChannelName,
    const CE_DLL* newDLLPtr):
	AOS_MasterChannel_Service(newName, newChannelName, newDLLPtr),
	_useOperationalControl(CEcfg::instance()->getOrAddBool(cfgKey("useOperationalControl"), false)),
	_VCID(CEcfg::instance()->getOrAddInt(cfgKey("VCID"), 0)),
	_serviceTypeStr(CEcfg::instance()->getOrAddString(cfgKey("serviceType"), serviceType2Str(ServicePacket))),
	_useVCFrameCycle(CEcfg::instance()->getOrAddBool(cfgKey("useVCFrameCycle"), false)),
	_vcFrameCount(0L),
	_vcFrameCountCycle(0),
	_badGVCIDCount(0L),
	_frameCountMisses(0L) {

	_VCID.setFormat(Setting::FormatHex);
}

std::string AOS_VirtualChannel_Service::serviceType2Str(const ServiceTypes svc) {
	switch(svc) {
	case ServicePacket: return "Packet"; break;
	case ServiceBitstream: return "Bitstream"; break;
	case ServiceAccess: return "Access"; break;
	case ServiceIdle: return "Idle"; break;
	case ServiceInvalid: return "Invalid"; break;
	}

	return "Invalid";
}

AOS_VirtualChannel_Service::ServiceTypes AOS_VirtualChannel_Service::str2ServiceType(const std::string svcStr) {
	if ( svcStr[0] == 'M' || svcStr[0] == 'm' || svcStr[0] == 'P' || svcStr[0] == 'p' ) return ServicePacket;
	if ( svcStr[0] == 'B' || svcStr[0] == 'b' ) return ServiceBitstream;
	if ( svcStr[0] == 'A' || svcStr[0] == 'a' ) return ServiceAccess;
	if ( svcStr[0] == 'I' || svcStr[0] == 'i' ) return ServiceIdle;

	ND_ERROR("str2ServiceType didn't recognize %s as Packet/Multiplexing, Bitstream, Access, or Idle.\n",
		svcStr.c_str());

	return ServiceInvalid;
}

void AOS_VirtualChannel_Service::setUseOperationalControl(const bool newVal) {
	_useOperationalControl = newVal; updateTemplate_();
}

bool AOS_VirtualChannel_Service::getUseOperationalControl() const { return _useOperationalControl; }
void AOS_VirtualChannel_Service::setVCID(const int32_t newVal) { _VCID = newVal; }
int8_t AOS_VirtualChannel_Service::getVCID() const { return static_cast<int32_t>(_VCID) & 0x3F; }
void AOS_VirtualChannel_Service::setFrameCount(const int64_t newVal) { _vcFrameCount = newVal; }
int64_t AOS_VirtualChannel_Service::getFrameCount() const { return _vcFrameCount; }
int64_t AOS_VirtualChannel_Service::incFrameCount() { return ++_vcFrameCount; }
void AOS_VirtualChannel_Service::setUseVCFrameCycle(const bool newVal) { _useVCFrameCycle = newVal; updateTemplate_(); }
bool AOS_VirtualChannel_Service::getUseVCFrameCycle() const { return _useVCFrameCycle; }
void AOS_VirtualChannel_Service::setFrameCountCycle(const int32_t newVal) { _vcFrameCountCycle = newVal; }
int8_t AOS_VirtualChannel_Service::getFrameCountCycle() const { return _vcFrameCountCycle; }
int8_t AOS_VirtualChannel_Service::incFrameCountCycle() { return ++_vcFrameCountCycle; }
uint16_t AOS_VirtualChannel_Service::getGVCID() const { return ((getMCID() << 6) | getVCID()) & 0xFFFF; }
void AOS_VirtualChannel_Service::setBadGVCIDCount(const uint64_t& newVal) { _badGVCIDCount = newVal; }
uint64_t AOS_VirtualChannel_Service::getBadGVCIDCount() const { return _badGVCIDCount; }
uint64_t AOS_VirtualChannel_Service::incBadGVCIDCount() { return (_badGVCIDCount = getBadGVCIDCount() + 1); }
void AOS_VirtualChannel_Service::setServiceType(const ServiceTypes newVal) { _serviceTypeStr = serviceType2Str(newVal); }
AOS_VirtualChannel_Service::ServiceTypes AOS_VirtualChannel_Service::getServiceType() const { return str2ServiceType(_serviceTypeStr); }
std::string AOS_VirtualChannel_Service::serviceType2Str(const ServiceTypes svc);
AOS_VirtualChannel_Service::ServiceTypes AOS_VirtualChannel_Service::str2ServiceType(const std::string svcStr);
void AOS_VirtualChannel_Service::setServiceType(const std::string& newVal) { _serviceTypeStr = newVal; }
std::string AOS_VirtualChannel_Service::getServiceTypeStr() const { return _serviceTypeStr; }
void AOS_VirtualChannel_Service::setFrameCountMisses(const u_int64_t& newVal) { _frameCountMisses = newVal; }
u_int64_t AOS_VirtualChannel_Service::getFrameCountMisses() const { return _frameCountMisses; }
u_int64_t AOS_VirtualChannel_Service::incFrameCountMisses() { return ++_frameCountMisses; }
}
