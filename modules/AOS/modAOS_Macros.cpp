/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modAOS_Macros.cpp
 * @author Tad Kollar  
 *
 * $Id: modAOS_Macros.cpp 2350 2013-09-03 14:29:09Z tkollar $
 * Copyright (c) 2010-2013.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================


#include "modAOS_Macros.hpp"

using namespace nasaCE;

namespace nAOS_Macros {

newAOSChannelProps::newAOSChannelProps(): frameLength(256), bitstreamSvcCount(1), packetSvcCount(1),
	vcAccessSvcCount(0), vcFrameSvcCount(1), mcFrameSvcCount(1), insertZoneLength(0), masterChannelCount(1), maxErrorsReedSolomon(8),
	useIdleVC(false), useHeaderEC(false), useFrameEC(false), useASM(true), usePseudoRandomize(true), minimize(false),
	segNamePrefix(""), segNameSuffix("") {

}

newAOSChannelProps::newAOSChannelProps(const xmlrpc_c::value_struct& settingsStruct): frameLength(256),
	bitstreamSvcCount(0), packetSvcCount(0),
	vcAccessSvcCount(0), vcFrameSvcCount(0), mcFrameSvcCount(0), insertZoneLength(0), masterChannelCount(1), maxErrorsReedSolomon(0),
	useIdleVC(false), useHeaderEC(false), useFrameEC(false), useASM(true), usePseudoRandomize(true), minimize(false),
	segNamePrefix(""), segNameSuffix("") {

	const xstruct settingsMap(settingsStruct);
	xstruct::const_iterator pos;

	for ( pos = settingsMap.begin(); pos != settingsMap.end(); ++pos ) {
		if (pos->first == "frameLength" ) frameLength = xmlrpc_c::value_int(pos->second);
		else if (pos->first == "bitstreamSvcCount" ) bitstreamSvcCount = xmlrpc_c::value_int(pos->second);
		else if (pos->first == "packetSvcCount" || pos->first == "multiplexingSvcCount" ) packetSvcCount = xmlrpc_c::value_int(pos->second);
		else if (pos->first == "vcAccessSvcCount") vcAccessSvcCount = xmlrpc_c::value_int(pos->second);
		else if (pos->first == "vcFrameSvcCount") vcFrameSvcCount = xmlrpc_c::value_int(pos->second);
		else if (pos->first == "mcFrameSvcCount") mcFrameSvcCount = xmlrpc_c::value_int(pos->second);
		else if (pos->first == "insertZoneLength") insertZoneLength = xmlrpc_c::value_int(pos->second);
		else if (pos->first == "masterChannelCount") masterChannelCount = xmlrpc_c::value_int(pos->second);
		else if (pos->first == "maxErrorsReedSolomon") maxErrorsReedSolomon = xmlrpc_c::value_int(pos->second);
		else if (pos->first == "useIdleVC") useIdleVC = xmlrpc_c::value_boolean(pos->second);
		else if (pos->first == "useHeaderEC") useHeaderEC = xmlrpc_c::value_boolean(pos->second);
		else if (pos->first == "useFrameEC") useFrameEC = xmlrpc_c::value_boolean(pos->second);
		else if (pos->first == "useASM") useASM = xmlrpc_c::value_boolean(pos->second);
		else if (pos->first == "usePseudoRandomize") usePseudoRandomize = xmlrpc_c::value_boolean(pos->second);
		else if (pos->first == "minimize") minimize = xmlrpc_c::value_boolean(pos->second);
		else if (pos->first == "segNamePrefix") segNamePrefix = xmlrpc_c::value_string(pos->second);
		else if (pos->first == "segNameSuffix") segNameSuffix = xmlrpc_c::value_string(pos->second);
	}
}

std::string newAOSChannelProps::makeSegName(const std::string& baseName, const int num1 /* = -1 */, const int num2 /* = -1 */) const {
	std::ostringstream os;

	os << segNamePrefix << baseName << segNameSuffix;
	if (num1 >= 0) os << num1;
	if (num2 >= 0) os << "_" << num2;

	return os.str();
}

void modAOS_Macros::_addBasicSettings(const std::string& channelPath,
	const std::string& dllName, const std::string& segName) const {
	CEcfg::instance()->getOrAddGroup(channelPath + "." + segName);
	CEcfg::instance()->getOrAddString(channelPath + "." + segName + ".dllName", dllName);
	CEcfg::instance()->getOrAddBool(channelPath + "." + segName + ".immediateStart", true);
}

void modAOS_Macros::_addPrimaryOutput(const std::string& channelPath,
	const std::string& segName, const std::string& targetName) const {
	Setting& outputArr = CEcfg::instance()->getOrAddArray(channelPath + "." + segName + ".primaryOutput");
	outputArr.add(Setting::TypeString) = targetName;
	outputArr.add(Setting::TypeString) = "PrimaryInput";
}

void modAOS_Macros::_addPrimaryOutputs(const std::string& channelPath,
	const std::string& segName, const std::string& targetName, const int& vcid) const {
	Setting& outputList = CEcfg::instance()->getOrAddList(channelPath + "." + segName + ".primaryOutputs");
	Setting& outputRow = outputList.add(Setting::TypeList);
	outputRow.add(Setting::TypeString) = targetName;
	outputRow.add(Setting::TypeString) = "PrimaryInput";
	outputRow.add(Setting::TypeInt) = vcid;
}

std::string modAOS_Macros::_addVCGen(Channel* channel, const std::string& targetName,
	const newAOSChannelProps& props, const int& mcCount, int& vcCount, const std::string& svcType) const {

	const std::string segName = _getUniqueSegName(channel, props.makeSegName("vcGen", mcCount, vcCount)),
		dllName = "modAOS_VC_Gen",
		channelPath = channel->cfgKey();

	_addBasicSettings(channelPath, dllName, segName);
	_addPrimaryOutput(channelPath, segName, targetName);
	CEcfg::instance()->getOrAddInt(channelPath + "." + segName + ".SCID", mcCount);
	CEcfg::instance()->getOrAddInt(channelPath + "." + segName + ".VCID", vcCount);
	CEcfg::instance()->getOrAddString(channelPath + "." + segName + ".serviceType", svcType);
	channel->add(dllName, segName);
	if ( vcCount < 63 ) ++vcCount;
	return segName;
}

std::string modAOS_Macros::_addVCRcv(Channel* channel, const std::string& targetName,
	const newAOSChannelProps& props, const int& mcCount, int& vcCount, const std::string& svcType) const {

	const std::string segName = _getUniqueSegName(channel, props.makeSegName("vcRcv", mcCount, vcCount)),
		dllName = "modAOS_VC_Rcv",
		channelPath = channel->cfgKey();

	_addBasicSettings(channelPath, dllName, segName);
	_addPrimaryOutput(channelPath, segName, targetName);
	CEcfg::instance()->getOrAddInt(channelPath + "." + segName + ".SCID", mcCount);
	CEcfg::instance()->getOrAddInt(channelPath + "." + segName + ".VCID", vcCount);
	CEcfg::instance()->getOrAddString(channelPath + "." + segName + ".serviceType", svcType);
	channel->add(dllName, segName);
	if ( vcCount < 63 ) ++vcCount;
	return segName;
}

std::string modAOS_Macros::_getUniqueSegName(Channel* channel,
	const std::string& baseName) const {

	if ( channel->find(baseName) == 0 )
		return baseName;

	std::ostringstream os;
	os << baseName << "_0";
	for ( int x = 1; channel->find(os.str()) != 0; ++x ) {
		os.str("");
		os << baseName << "_" << x;
	}

	return os.str();
}

Channel* modAOS_Macros::_initializeAOSChannel(const std::string& channelName, const newAOSChannelProps& props) const {
	// Find or create the channel
	Channel* channel = channels::instance()->exists(channelName)?
		channels::instance()->channel(channelName) :
		channels::instance()->add(channelName);

	const std::string channelPath(channel->cfgKey());

	// Set global AOS settings
	CEcfg::instance()->getOrAddGroup(channelPath + ".AOS_PhysicalChannel");
	CEcfg::instance()->getOrAddInt(channelPath + ".AOS_PhysicalChannel.frameSize", props.frameLength);
	CEcfg::instance()->getOrAddBool(channelPath + ".AOS_PhysicalChannel.useHeaderErrorControl", props.useHeaderEC);
	CEcfg::instance()->getOrAddBool(channelPath + ".AOS_PhysicalChannel.useFrameErrorControl", props.useFrameEC);
	if ( props.insertZoneLength > 0 ) {
		CEcfg::instance()->getOrAddBool(channelPath + ".AOS_PhysicalChannel.useInsertZone", true);
		CEcfg::instance()->getOrAddInt(channelPath + ".AOS_PhysicalChannel.insertZoneSize", props.insertZoneLength);
	}
	else {
		CEcfg::instance()->getOrAddBool(channelPath + ".AOS_PhysicalChannel.useInsertZone", false);
		CEcfg::instance()->getOrAddInt(channelPath + ".AOS_PhysicalChannel.insertZoneSize", 0);
	}

	return channel;
}


void modAOS_Macros::newForwardChannel(const std::string& channelName, const newAOSChannelProps& props) const {
	Channel* channel = _initializeAOSChannel(channelName, props);
	const std::string channelPath(channel->cfgKey());
	std::string segName, dllName, lastSegName = "";

	// Build the channel starting at the end of the stream so that segments
	// have something to output to.

	if ( props.useASM ) {
		// Insert modASM_Add
		segName = _getUniqueSegName(channel, props.makeSegName("asmAdd"));
		dllName = "modASM_Add";
		_addBasicSettings(channelPath, dllName, segName);
		channel->add(dllName, segName);
		lastSegName = segName;
	}

	if ( props.usePseudoRandomize ) {
		// Insert modPseudoRandomize
		segName = _getUniqueSegName(channel, props.makeSegName("randomize"));

		dllName = "modPseudoRandomize";
		_addBasicSettings(channelPath, dllName, segName);
		_addPrimaryOutput(channelPath, segName, lastSegName);
		channel->add(dllName, segName);
		lastSegName = segName;
	}

	if ( props.maxErrorsReedSolomon == 8 || props.maxErrorsReedSolomon == 16) {
		// Insert modRSEncode
		segName = _getUniqueSegName(channel, props.makeSegName("rsEncode"));
		dllName = "modRSEncode";
		const size_t maxErrorsPerCodeWord(props.maxErrorsReedSolomon);
		const size_t interleavingDepth(ceil(static_cast<double>(props.frameLength) / static_cast<double>((255 - (2 * maxErrorsPerCodeWord)))));
		ND_INFO("[modAOS_Macros] Calculated %d as the interleaving depth for RS(255, %d) with AOS Transfer Frame length of %d.\n",
				interleavingDepth, 255 - 2 * maxErrorsPerCodeWord, props.frameLength);
		_addBasicSettings(channelPath, dllName, segName);
		CEcfg::instance()->getOrAddInt(channelPath + "." + segName + ".maxErrorsPerCodeword", maxErrorsPerCodeWord);
		CEcfg::instance()->getOrAddInt(channelPath + "." + segName + ".interleavingDepth", interleavingDepth);
		if ( props.useASM ) _addPrimaryOutput(channelPath, segName, lastSegName);
		channel->add(dllName, segName);
		lastSegName = segName;
	}

	// Insert modAOS_AF_Gen
	segName = _getUniqueSegName(channel, props.makeSegName("afGen"));
	dllName = "modAOS_AF_Gen";
	_addBasicSettings(channelPath, dllName, segName);
	if ( ! lastSegName.empty() ) _addPrimaryOutput(channelPath, segName, lastSegName);
	channel->add(dllName, segName);
	lastSegName = segName;

	// Insert modAOS_MC_Mux if necessary
	if ( ! props.minimize || props.masterChannelCount + props.mcFrameSvcCount > 1 ) {
		segName = _getUniqueSegName(channel, props.makeSegName("mcMux"));
		dllName = "modAOS_MC_Mux";
		_addBasicSettings(channelPath, dllName, segName);
		_addPrimaryOutput(channelPath, segName, lastSegName);
		channel->add(dllName, segName);
		lastSegName = segName;
	}

	// Insert virtual channels
	std::string mcMuxSegName(lastSegName);
	size_t mcCount;
	for ( mcCount = 0; mcCount < props.masterChannelCount; ++mcCount ) {

		// Insert modAOS_VC_Mux if necessary
		if ( ! props.minimize || props.packetSvcCount + props.bitstreamSvcCount + props.vcAccessSvcCount + props.vcFrameSvcCount > 1 ) {

			segName = _getUniqueSegName(channel, props.makeSegName("vcMux", mcCount));
			dllName = "modAOS_VC_Mux";
			_addBasicSettings(channelPath, dllName, segName);
			_addPrimaryOutput(channelPath, segName, mcMuxSegName);
			if ( props.useIdleVC ) {
				CEcfg::instance()->getOrAddInt(channelPath + "." + segName + ".sendIntervalUsec", 10000);
			}
			CEcfg::instance()->getOrAddInt(channelPath + "." + segName + ".SCID", mcCount);
			channel->add(dllName, segName);
			lastSegName = segName;
		}

		std::string vcMuxSegName(lastSegName);

		int vcCount = 0;

		for ( size_t b = 0; b < props.bitstreamSvcCount; ++b ) {
			// Add the VC generator
			const std::string vcName = _addVCGen(channel, vcMuxSegName, props, mcCount, vcCount, "Bitstream");

			// Add the bitstream protocol data unit encoder
			segName = _getUniqueSegName(channel, props.makeSegName("bpduAdd", mcCount, vcCount - 1));
			dllName = "modAOS_B_PDU_Add";
			_addBasicSettings(channelPath, dllName, segName);
			_addPrimaryOutput(channelPath, segName, vcName);
			CEcfg::instance()->getOrAddInt(channelPath + "." + segName + ".maxUsecsForNextPacket", 100);
			channel->add(dllName, segName);
		}

		for ( size_t m = 0; m < props.packetSvcCount; ++m ) {
			// Add the VC generator
			const std::string vcName = _addVCGen(channel, vcMuxSegName, props, mcCount, vcCount, "Packet");

			// Add the multiplexing protocol data unit encoder
			segName = _getUniqueSegName(channel, props.makeSegName("mpduAdd", mcCount, vcCount - 1));
			dllName = "modAOS_M_PDU_Add";
			_addBasicSettings(channelPath, dllName, segName);
			_addPrimaryOutput(channelPath, segName, vcName);
			CEcfg::instance()->getOrAddInt(channelPath + "." + segName + ".maxUsecsForNextPacket", 100);
			channel->add(dllName, segName);
			lastSegName = segName;

			// Add the encapsulation packet encoder
			segName = _getUniqueSegName(channel, props.makeSegName("encapAdd", mcCount, vcCount - 1));
			dllName = "modEncapPkt_Add";
			_addBasicSettings(channelPath, dllName, segName);
			_addPrimaryOutput(channelPath, segName, lastSegName);
			channel->add(dllName, segName);
		}

		for ( size_t a = 0; a < props.vcAccessSvcCount; ++a ) {
			_addVCGen(channel, vcMuxSegName, props, mcCount, vcCount, "Access");
		}

		for ( size_t f = 0; f < props.vcFrameSvcCount; ++f ) {
			segName = _getUniqueSegName(channel, props.makeSegName("vcFrame", mcCount, vcCount));
			dllName = "modAOS_VC_Frame";

			_addBasicSettings(channelPath, dllName, segName);
			_addPrimaryOutput(channelPath, segName, vcMuxSegName);
			CEcfg::instance()->getOrAddInt(channelPath + "." + segName + ".SCID", mcCount);
			CEcfg::instance()->getOrAddInt(channelPath + "." + segName + ".VCID", vcCount);
			channel->add(dllName, segName);
		}

	}

	for ( size_t mf = 0; mf < props.mcFrameSvcCount; ++mf ) {
		segName = _getUniqueSegName(channel, props.makeSegName("mcFrame", mcCount));
		dllName = "modAOS_MC_Frame";

		_addBasicSettings(channelPath, dllName, segName);
		_addPrimaryOutput(channelPath, segName, mcMuxSegName);
		channel->add(dllName, segName);
	}
}

void modAOS_Macros::newReturnChannel(const std::string& channelName, const newAOSChannelProps& props) const {
	Channel* channel = _initializeAOSChannel(channelName, props);
	const std::string channelPath(channel->cfgKey());
	std::string segName, dllName, lastSegName = "", vcName = "";
	std::map<std::string, int> vcMap, mcMap;

	// Build the channel starting at the end of the stream so that segments
	// have something to output to.
	size_t mcCount;
	for ( mcCount = 0; mcCount < props.masterChannelCount; ++mcCount ) {
		int vcCount = 0;
		vcMap.clear();

		for ( size_t b = 0; b < props.bitstreamSvcCount; ++b ) {
			segName = _getUniqueSegName(channel, props.makeSegName("bpduDel", mcCount, vcCount));
			dllName = "modAOS_B_PDU_Remove";
			_addBasicSettings(channelPath, dllName, segName);
			channel->add(dllName, segName);

			vcName = _addVCRcv(channel, segName, props, mcCount, vcCount, "Bitstream");
			vcMap[vcName] = vcCount - 1;
		}

		for ( size_t m = 0; m < props.packetSvcCount; ++m ) {
			segName = _getUniqueSegName(channel, props.makeSegName("encapDel", mcCount, vcCount));
			dllName = "modEncapPkt_Remove";
			_addBasicSettings(channelPath, dllName, segName);
			channel->add(dllName, segName);
			lastSegName = segName;

			segName = _getUniqueSegName(channel, props.makeSegName("mpduDel", mcCount, vcCount));
			dllName = "modAOS_M_PDU_Remove";
			_addBasicSettings(channelPath, dllName, segName);
			_addPrimaryOutput(channelPath, segName, lastSegName);
			channel->add(dllName, segName);

			vcName = _addVCRcv(channel, segName, props, mcCount, vcCount, "Packet");
			vcMap[vcName] = vcCount - 1;
		}

		for ( size_t a = 0; a < props.vcAccessSvcCount; ++a ) {
			vcName = _addVCRcv(channel, lastSegName, props, mcCount, vcCount, "Access");
			vcMap[vcName] = vcCount - 1;
		}

		// Insert modAOS_VC_Demux if necessary
		if ( ! props.minimize || props.packetSvcCount + props.bitstreamSvcCount + props.vcAccessSvcCount + props.vcFrameSvcCount > 1 ) {

			segName = _getUniqueSegName(channel, props.makeSegName("vcDemux", mcCount));
			dllName = "modAOS_VC_Demux";
			_addBasicSettings(channelPath, dllName, segName);
			CEcfg::instance()->getOrAddInt(channelPath + "." + segName + ".SCID", mcCount);

			std::map<std::string, int>::iterator pos;
			for ( pos = vcMap.begin(); pos != vcMap.end(); ++pos ) {
				_addPrimaryOutputs(channelPath, segName, pos->first, pos->second);
			}

			channel->add(dllName, segName);
		}
		mcMap[segName] = mcCount;
	}

	lastSegName = "";

	// Insert modAOS_MC_Demux if necessary
	if ( ! props.minimize || props.masterChannelCount + props.mcFrameSvcCount > 1 ) {

		segName = _getUniqueSegName(channel, props.makeSegName("mcDemux"));
		dllName = "modAOS_MC_Demux";
		_addBasicSettings(channelPath, dllName, segName);

		std::map<std::string, int>::iterator mpos;
		for ( mpos = mcMap.begin(); mpos != mcMap.end(); ++mpos ) {
			_addPrimaryOutputs(channelPath, segName, mpos->first, mpos->second);
		}

		channel->add(dllName, segName);
		lastSegName = segName;
	}
	else {
		if (mcMap.size() == 1) lastSegName = mcMap.begin()->first;
	}

	// Insert modAOS_AF_Rcv
	segName = _getUniqueSegName(channel, props.makeSegName("afRcv"));
	dllName = "modAOS_AF_Rcv";
	_addBasicSettings(channelPath, dllName, segName);
	CEcfg::instance()->getOrAddBool(channelPath + "." + segName + ".performErrorControlHere",
		props.useHeaderEC | props.useFrameEC
	);
	if ( ! lastSegName.empty() ) _addPrimaryOutput(channelPath, segName, lastSegName);
	channel->add(dllName, segName);
	lastSegName = segName;

	const size_t maxErrorsPerCodeWord(props.maxErrorsReedSolomon);
	const size_t interleavingDepth(ceil(static_cast<double>(props.frameLength) / static_cast<double>((255 - (2 * maxErrorsPerCodeWord)))));

	if ( props.maxErrorsReedSolomon == 8 || props.maxErrorsReedSolomon == 16 ) {
		// Insert modRSDecode
		segName = _getUniqueSegName(channel, props.makeSegName("rsDecode"));
		dllName = "modRSDecode";

		ND_INFO("[modAOS_Macros] Calculated %d as the interleaving depth for RS(255, %d) with AOS Transfer Frame length of %d.\n",
				interleavingDepth, 255 - 2 * maxErrorsPerCodeWord, props.frameLength);
		_addBasicSettings(channelPath, dllName, segName);
		CEcfg::instance()->getOrAddInt(channelPath + "." + segName + ".maxErrorsPerCodeword", maxErrorsPerCodeWord);
		CEcfg::instance()->getOrAddInt(channelPath + "." + segName + ".interleavingDepth", interleavingDepth);
		_addPrimaryOutput(channelPath, segName, lastSegName);
		channel->add(dllName, segName);
		lastSegName = segName;
	}

	if ( props.usePseudoRandomize ) {
		// Insert modPseudoRandomize
		segName = _getUniqueSegName(channel, props.makeSegName("unrandomize"));

		dllName = "modPseudoRandomize";
		_addBasicSettings(channelPath, dllName, segName);
		_addPrimaryOutput(channelPath, segName, lastSegName);
		channel->add(dllName, segName);
		lastSegName = segName;
	}

	if ( props.useASM ) {
		// Insert modASM_Remove
		segName = _getUniqueSegName(channel, props.makeSegName("asmDel"));

		dllName = "modASM_Remove";
		_addBasicSettings(channelPath, dllName, segName);
		const size_t expectedUnitLength = props.frameLength + interleavingDepth * maxErrorsPerCodeWord * 2;
		CEcfg::instance()->getOrAddInt(channelPath + "." + segName + ".expectedUnitLength", expectedUnitLength);
		ND_INFO("[modAOS_Macros] ASM remove segment will expect unit length to be %d.\n", expectedUnitLength);

		_addPrimaryOutput(channelPath, segName, lastSegName);
		channel->add(dllName, segName);
	}
}

} // namespace nAOS_Macros