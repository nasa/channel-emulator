/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modTM_Macros.cpp
 * @author Tad Kollar  
 *
 * $Id: modTM_Macros.cpp 2390 2013-11-26 17:05:23Z tkollar $
 * Copyright (c) 2010-2013.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================


#include "modTM_Macros.hpp"
#include "modTM_VC_Rcv_Packet.hpp"

using namespace nasaCE;

namespace nTM_Macros {

newTMChannelProps::newTMChannelProps(): frameLength(256), packetSvcCount(1),
	vcAccessSvcCount(0), vcFrameSvcCount(1), mcFrameSvcCount(1),masterChannelCount(1), maxErrorsReedSolomon(8),
	useFrameEC(false), useASM(true), usePseudoRandomize(true), minimize(false),
	segNamePrefix(""), segNameSuffix("") {

}

newTMChannelProps::newTMChannelProps(const xmlrpc_c::value_struct& settingsStruct): frameLength(256),
	packetSvcCount(0), vcAccessSvcCount(0), vcFrameSvcCount(0), mcFrameSvcCount(0), masterChannelCount(1),
	maxErrorsReedSolomon(0), 	useFrameEC(false), useASM(true), usePseudoRandomize(true), minimize(false),
	segNamePrefix(""), segNameSuffix("") {

	const xstruct settingsMap(settingsStruct);
	xstruct::const_iterator pos;

	for ( pos = settingsMap.begin(); pos != settingsMap.end(); ++pos ) {
		if (pos->first == "frameLength" ) frameLength = xmlrpc_c::value_int(pos->second);
		else if (pos->first == "multiplexingSvcCount" || pos->first == "packetSvcCount" ) packetSvcCount = xmlrpc_c::value_int(pos->second);
		else if (pos->first == "vcAccessSvcCount") vcAccessSvcCount = xmlrpc_c::value_int(pos->second);
		else if (pos->first == "vcFrameSvcCount") vcFrameSvcCount = xmlrpc_c::value_int(pos->second);
		else if (pos->first == "mcFrameSvcCount") mcFrameSvcCount = xmlrpc_c::value_int(pos->second);
		else if (pos->first == "masterChannelCount") masterChannelCount = xmlrpc_c::value_int(pos->second);
		else if (pos->first == "maxErrorsReedSolomon") maxErrorsReedSolomon = xmlrpc_c::value_int(pos->second);
		else if (pos->first == "useFrameEC") useFrameEC = xmlrpc_c::value_boolean(pos->second);
		else if (pos->first == "useASM") useASM = xmlrpc_c::value_boolean(pos->second);
		else if (pos->first == "usePseudoRandomize") usePseudoRandomize = xmlrpc_c::value_boolean(pos->second);
		else if (pos->first == "minimize") minimize = xmlrpc_c::value_boolean(pos->second);
		else if (pos->first == "segNamePrefix") segNamePrefix = xmlrpc_c::value_string(pos->second);
		else if (pos->first == "segNameSuffix") segNameSuffix = xmlrpc_c::value_string(pos->second);
	}
}

std::string newTMChannelProps::makeSegName(const std::string& baseName, const int num1 /* = -1 */, const int num2 /* = -1 */) const {
	std::ostringstream os;

	os << segNamePrefix << baseName << segNameSuffix;
	if (num1 >= 0) os << num1;
	if (num2 >= 0) os << "_" << num2;

	return os.str();
}

void modTM_Macros::_addBasicSettings(const std::string& channelPath,
	const std::string& dllName, const std::string& segName) const {
	CEcfg::instance()->getOrAddGroup(channelPath + "." + segName);
	CEcfg::instance()->getOrAddString(channelPath + "." + segName + ".dllName", dllName);
	CEcfg::instance()->getOrAddBool(channelPath + "." + segName + ".immediateStart", true);
}

void modTM_Macros::_addPrimaryOutput(const std::string& channelPath,
	const std::string& segName, const std::string& targetName) const {
	Setting& outputArr = CEcfg::instance()->getOrAddArray(channelPath + "." + segName + ".primaryOutput");
	outputArr.add(Setting::TypeString) = targetName;
	outputArr.add(Setting::TypeString) = "PrimaryInput";
}

void modTM_Macros::_addPrimaryOutputs(const std::string& channelPath,
	const std::string& segName, const std::string& targetName, const int& vcid) const {
	Setting& outputList = CEcfg::instance()->getOrAddList(channelPath + "." + segName + ".primaryOutputs");
	Setting& outputRow = outputList.add(Setting::TypeList);
	outputRow.add(Setting::TypeString) = targetName;
	outputRow.add(Setting::TypeString) = "PrimaryInput";
	outputRow.add(Setting::TypeInt) = vcid;
}

std::string modTM_Macros::_addVCGen(Channel* channel, const std::string& targetName,
	const newTMChannelProps& props, const int& mcCount, int& vcCount, const std::string& svcType) const {

	const std::string segName = _getUniqueSegName(channel, props.makeSegName((svcType == "Packet")? "vcGenPkt" : "vcGenAcc", mcCount, vcCount)),
		dllName = "modTM_VC_Gen_" + svcType,
		channelPath = channel->cfgKey();

	_addBasicSettings(channelPath, dllName, segName);
	_addPrimaryOutput(channelPath, segName, targetName);
	CEcfg::instance()->getOrAddInt(channelPath + "." + segName + ".SCID", mcCount);
	CEcfg::instance()->getOrAddInt(channelPath + "." + segName + ".VCID", vcCount);
	channel->add(dllName, segName);
	if ( vcCount < 7 ) ++vcCount;
	return segName;
}

std::string modTM_Macros::_addVCRcv(Channel* channel, const std::string& targetName,
	const newTMChannelProps& props, const int& mcCount, int& vcCount, const std::string& svcType) const {

	const std::string segName = _getUniqueSegName(channel, props.makeSegName((svcType == "Packet")? "vcRcvPkt" : "vcRcvAcc", mcCount, vcCount)),
		dllName = "modTM_VC_Rcv_" + svcType,
		channelPath = channel->cfgKey();

	_addBasicSettings(channelPath, dllName, segName);
	if ( ! targetName.empty() ) {
		if ( svcType == "Access" )
			_addPrimaryOutput(channelPath, segName, targetName);
		else if ( svcType == "Packet" )
			_addPrimaryOutputs(channelPath, segName, targetName,
				nTM_VC_Rcv_Packet::modTM_VC_Rcv_Packet::pvnEncapsulationPacket);
	}
	CEcfg::instance()->getOrAddInt(channelPath + "." + segName + ".SCID", mcCount);
	CEcfg::instance()->getOrAddInt(channelPath + "." + segName + ".VCID", vcCount);
	channel->add(dllName, segName);
	if ( vcCount < 7 ) ++vcCount;
	return segName;
}

std::string modTM_Macros::_getUniqueSegName(Channel* channel,
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

Channel* modTM_Macros::_initializeTMChannel(const std::string& channelName, const newTMChannelProps& props) const {
	// Find or create the channel
	Channel* channel = channels::instance()->exists(channelName)?
		channels::instance()->channel(channelName) :
		channels::instance()->add(channelName);

	const std::string channelPath(channel->cfgKey());

	// Set global TM settings
	CEcfg::instance()->getOrAddGroup(channelPath + ".TM_PhysicalChannel");
	CEcfg::instance()->getOrAddInt(channelPath + ".TM_PhysicalChannel.frameSize", props.frameLength);
	CEcfg::instance()->getOrAddBool(channelPath + ".TM_PhysicalChannel.useFrameErrorControl", props.useFrameEC);

	return channel;
}


void modTM_Macros::newForwardChannel(const std::string& channelName, const newTMChannelProps& props) const {
	Channel* channel = _initializeTMChannel(channelName, props);
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
		ND_INFO("[modTM_Macros] Calculated %d as the interleaving depth for RS(255, %d) with TM Transfer Frame length of %d.\n",
				interleavingDepth, 255 - 2 * maxErrorsPerCodeWord, props.frameLength);
		_addBasicSettings(channelPath, dllName, segName);
		CEcfg::instance()->getOrAddInt(channelPath + "." + segName + ".maxErrorsPerCodeword", maxErrorsPerCodeWord);
		CEcfg::instance()->getOrAddInt(channelPath + "." + segName + ".interleavingDepth", interleavingDepth);
		if ( props.useASM ) _addPrimaryOutput(channelPath, segName, lastSegName);
		channel->add(dllName, segName);
		lastSegName = segName;
	}

	// Insert modTM_AF_Gen
	segName = _getUniqueSegName(channel, props.makeSegName("afGen"));
	dllName = "modTM_AF_Gen";
	_addBasicSettings(channelPath, dllName, segName);

	if ( ! lastSegName.empty() ) _addPrimaryOutput(channelPath, segName, lastSegName);
	channel->add(dllName, segName);
	lastSegName = segName;

	// Insert modTM_MC_Mux if necessary
	if ( ! props.minimize || props.masterChannelCount + props.mcFrameSvcCount > 1 ) {
		segName = _getUniqueSegName(channel, props.makeSegName("mcMux"));
		dllName = "modTM_MC_Mux";
		_addBasicSettings(channelPath, dllName, segName);
		_addPrimaryOutput(channelPath, segName, lastSegName);
		channel->add(dllName, segName);
		lastSegName = segName;
	}

	// Insert virtual channels
	std::string mcMuxSegName(lastSegName);
	size_t mcCount;
	for ( mcCount = 0; mcCount < props.masterChannelCount; ++mcCount ) {

		// Insert modTM_MC_Gen
		segName = _getUniqueSegName(channel, props.makeSegName("mcGen", mcCount));
		dllName = "modTM_MC_Gen";
		_addBasicSettings(channelPath, dllName, segName);
		_addPrimaryOutput(channelPath, segName, mcMuxSegName);
		CEcfg::instance()->getOrAddInt(channelPath + "." + segName + ".SCID", mcCount);
		channel->add(dllName, segName);
		lastSegName = segName;

		// Insert modTM_VC_Mux if necessary
		if ( ! props.minimize || props.packetSvcCount + props.vcAccessSvcCount + props.vcFrameSvcCount > 1 ) {

			segName = _getUniqueSegName(channel, props.makeSegName("vcMux", mcCount));
			dllName = "modTM_VC_Mux";
			_addBasicSettings(channelPath, dllName, segName);
			_addPrimaryOutput(channelPath, segName, lastSegName);
			CEcfg::instance()->getOrAddInt(channelPath + "." + segName + ".SCID", mcCount);
			channel->add(dllName, segName);
			lastSegName = segName;
		}

		int vcCount = 0;

		for ( size_t m = 0; m < props.packetSvcCount; ++m ) {
			std::string vcPktName = _addVCGen(channel, lastSegName, props, mcCount, vcCount, "Packet");

			// Add the encapsulation packet encoder
			segName = _getUniqueSegName(channel, props.makeSegName("encapAdd", mcCount, vcCount - 1));
			dllName = "modEncapPkt_Add";
			_addBasicSettings(channelPath, dllName, segName);
			_addPrimaryOutput(channelPath, segName, vcPktName);
			channel->add(dllName, segName);
		}

		for ( size_t a = 0; a < props.vcAccessSvcCount; ++a ) {
			_addVCGen(channel, lastSegName, props, mcCount, vcCount, "Access");
		}

		for ( size_t f = 0; f < props.vcFrameSvcCount; ++f ) {
			segName = _getUniqueSegName(channel, props.makeSegName("vcFrame", mcCount, vcCount));
			dllName = "modTM_VC_Frame";

			_addBasicSettings(channelPath, dllName, segName);
			_addPrimaryOutput(channelPath, segName, lastSegName);
			CEcfg::instance()->getOrAddInt(channelPath + "." + segName + ".SCID", mcCount);
			CEcfg::instance()->getOrAddInt(channelPath + "." + segName + ".VCID", vcCount);
			channel->add(dllName, segName);
		}

	}

	for ( size_t mf = 0; mf < props.mcFrameSvcCount; ++mf ) {
		segName = _getUniqueSegName(channel, props.makeSegName("mcFrame", mcCount));
		dllName = "modTM_MC_Frame";

		_addBasicSettings(channelPath, dllName, segName);
		_addPrimaryOutput(channelPath, segName, mcMuxSegName);
		channel->add(dllName, segName);
	}
}

void modTM_Macros::newReturnChannel(const std::string& channelName, const newTMChannelProps& props) const {
	Channel* channel = _initializeTMChannel(channelName, props);
	const std::string channelPath(channel->cfgKey());
	std::string segName, dllName, lastSegName = "";
	std::map<std::string, int> vcMap, mcMap;

	// Build the channel starting at the end of the stream so that segments
	// have something to output to.
	size_t mcCount;
	for ( mcCount = 0; mcCount < props.masterChannelCount; ++mcCount ) {
		int vcCount = 0;
		vcMap.clear();

		for ( size_t m = 0; m < props.packetSvcCount; ++m ) {
			segName = _getUniqueSegName(channel, props.makeSegName("encapDel", mcCount, vcCount));
			dllName = "modEncapPkt_Remove";
			_addBasicSettings(channelPath, dllName, segName);
			channel->add(dllName, segName);
			lastSegName = segName;

			segName = _addVCRcv(channel, lastSegName, props, mcCount, vcCount, "Packet");
			vcMap[segName] = vcCount - 1;
		}

		for ( size_t a = 0; a < props.vcAccessSvcCount; ++a ) {
			segName = _addVCRcv(channel, "", props, mcCount, vcCount, "Access");
			vcMap[segName] = vcCount - 1;
		}
		lastSegName = "";

		// Insert modTM_VC_Demux if necessary
		if ( ! props.minimize || props.packetSvcCount + props.vcAccessSvcCount + props.vcFrameSvcCount > 1 ) {
			segName = _getUniqueSegName(channel, props.makeSegName("vcDemux", mcCount));
			dllName = "modTM_VC_Demux";
			_addBasicSettings(channelPath, dllName, segName);
			CEcfg::instance()->getOrAddInt(channelPath + "." + segName + ".SCID", mcCount);

			std::map<std::string, int>::iterator pos;
			for ( pos = vcMap.begin(); pos != vcMap.end(); ++pos ) {
				_addPrimaryOutputs(channelPath, segName, pos->first, pos->second);
			}

			channel->add(dllName, segName);
			lastSegName = segName;
		}
		else {
			if (vcMap.size() == 1) lastSegName = vcMap.begin()->first;
		}

		// Insert modTM_MC_Rcv
		segName = _getUniqueSegName(channel, props.makeSegName("mcRcv", mcCount));
		dllName = "modTM_MC_Rcv";
		_addBasicSettings(channelPath, dllName, segName);
		CEcfg::instance()->getOrAddInt(channelPath + "." + segName + ".SCID", mcCount);

		if ( ! lastSegName.empty()) _addPrimaryOutput(channelPath, segName, lastSegName);

		channel->add(dllName, segName);
		mcMap[segName] = mcCount;
	}

	lastSegName = "";

	// Insert modTM_MC_Demux if necessary
	if ( ! props.minimize || props.masterChannelCount + props.mcFrameSvcCount > 1 ) {

		segName = _getUniqueSegName(channel, props.makeSegName("mcDemux"));
		dllName = "modTM_MC_Demux";
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

	// Insert modTM_AF_Rcv
	segName = _getUniqueSegName(channel, props.makeSegName("afRcv"));
	dllName = "modTM_AF_Rcv";
	_addBasicSettings(channelPath, dllName, segName);

	if ( ! lastSegName.empty() ) _addPrimaryOutput(channelPath, segName, lastSegName);
	channel->add(dllName, segName);
	lastSegName = segName;

	const size_t maxErrorsPerCodeWord(props.maxErrorsReedSolomon);
	const size_t interleavingDepth(ceil(static_cast<double>(props.frameLength) / static_cast<double>((255 - (2 * maxErrorsPerCodeWord)))));

	if ( props.maxErrorsReedSolomon == 8 || props.maxErrorsReedSolomon == 16 ) {
		// Insert modRSDecode
		segName = _getUniqueSegName(channel, props.makeSegName("rsDecode"));
		dllName = "modRSDecode";

		ND_INFO("[modTM_Macros] Calculated %d as the interleaving depth for RS(255, %d) with TM Transfer Frame length of %d.\n",
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
		ND_INFO("[modTM_Macros] ASM remove segment will expect unit length to be %d.\n", expectedUnitLength);

		_addPrimaryOutput(channelPath, segName, lastSegName);
		channel->add(dllName, segName);
	}
}

} // namespace nTM_Macros