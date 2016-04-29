/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modTM_Macros.hpp
 * @author Tad Kollar  
 *
 * $Id: modTM_Macros.hpp 2350 2013-09-03 14:29:09Z tkollar $
 * Copyright (c) 2009-2013.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _MOD_TM_MACROS_HPP_
#define _MOD_TM_MACROS_HPP_

#include "CE_Config.hpp"
#include "CE_Macros.hpp"
#include "Channel.hpp"
#include "ChannelRegistry.hpp"
#include "InterfaceMacros.hpp"

namespace nTM_Macros {

//=============================================================================
/**
 * @class newTMChannelProps
 * @author Tad Kollar  
 * @brief General settings for an entire TM channel.
*/
//=============================================================================
class newTMChannelProps {
public:
	size_t frameLength;
	size_t packetSvcCount;
	size_t vcAccessSvcCount;
	size_t vcFrameSvcCount;
	size_t mcFrameSvcCount;
	size_t masterChannelCount;
	size_t maxErrorsReedSolomon;
	bool useFrameEC;
	bool useASM;
	bool usePseudoRandomize;
	bool minimize; // If true, don't insert unnecessary modules
	std::string segNamePrefix;
	std::string segNameSuffix;

	/// Default constructor.
	newTMChannelProps();

	/// Construct with default values plus settings from an XML-RPC call.
	/// @param settingsStruct An XML-RPC map of values extracted from a paramList.
	newTMChannelProps(const xmlrpc_c::value_struct& settingsStruct);

	/// Generate a name for the segment based on the provided values plus
	/// segNamePrefix and segNameSuffix, if set.
	/// @param baseName The identifier to attach the other strings to.
	/// @param num An optional value to append.
	std::string makeSegName(const std::string& baseName, const int num1 = -1, const int num2 = -1) const;
};

//=============================================================================
/**
 * @class modTM_Macros
 * @author Tad Kollar  
 * @brief Methods for automatically creating TM channels.
*/
//=============================================================================
class modTM_Macros {
public:
	/// @brief Perform all steps to create a new TM forward channel.
	/// @param channelName The identifier of the new physical channel.
	/// @param props General properties for the entire channel.
	void newForwardChannel(const std::string& channelName, const newTMChannelProps& props) const;

	/// @brief Perform all steps to create a new TM return channel.
	/// @param channelName The identifier of the new physical channel.
	/// @param props General properties for the entire channel.
	void newReturnChannel(const std::string& channelName, const newTMChannelProps& props) const;

private:
	/// @brief Find or create the specified channel by name, then add some global settings.
	/// @param channelName The identifier of the channel to find/create.
	/// @param props The general properties for the entire channel.
	Channel* _initializeTMChannel(const std::string& channelName, const newTMChannelProps& props) const;

	/// @brief Create the config file group, add the dllName and immediateStart(false) settings.
	/// @param channelPath Path to the channel in the config file.
	/// @param dllName Name of the module's shared library.
	/// @param segName What to call the new segment.
	void _addBasicSettings(const std::string& channelPath, const std::string& dllName, const std::string& segName) const;

	/// @brief Create a PrimaryOutput setting for one segment.
	/// @param channelPath Path to the channel in the config file.
	/// @param segName The name of the segment to create the setting for.
	/// @param targetName The name of the segment to output to.
	void _addPrimaryOutput(const std::string& channelPath, const std::string& segName, const std::string& targetName) const;

	/// @brief Create/add to a PrimaryOutputs setting for a demux segment.
	/// @param channelPath Path to the channel in the config file.
	/// @param segName The name of the segment to create the setting for.
	/// @param targetName The name of the segment to output to.
	/// @param vcid The Virtual Channel Identifier to associate this output with.
	void _addPrimaryOutputs(const std::string& channelPath, const std::string& segName, const std::string& targetName, const int& vcid) const;

	/// @brief Create a settings group for an TM Virtual Channel Generation segment.
	/// @param channel A reference to an existing channel.
	/// @param targetName The segment that the new one will output to.
	/// @param props The general properties for the entire channel.
	/// @param mcCount The current master channel count; to be used as the Spacecraft Identifier.
	/// @param vcCount The number of this virtual channel; it will be the VCID and in the segment name, then incremented.
	/// @param svcType One of "Bitstream", "Multiplexing", "Access", or "Idle"
	std::string _addVCGen(Channel* channel, const std::string& targetName,
		const newTMChannelProps& props, const int& mcCount, int& vcCount, const std::string& svcType) const;

	/// @brief Create a settings group for an TM Virtual Channel Reception segment.
	/// @param channel A reference to an existing channel.
	/// @param targetName The segment that the new one will output to.
	/// @param props The general properties for the entire channel.
	/// @param mcCount The current master channel count; to be used as the Spacecraft Identifier.
	/// @param vcCount The number of this virtual channel; it will be the VCID and in the segment name, then incremented.
	/// @param svcType One of "Bitstream", "Multiplexing", "Access", or "Idle"
	std::string _addVCRcv(Channel* channel, const std::string& targetName,
		const newTMChannelProps& props, const int& mcCount, int& vcCount, const std::string& svcType) const;

	/// @brief Append digits to a segment name until it's unique to the channel.
	/// @param channel Pointer to the channel to test with.
	/// @param baseName The segment name to start testing with.
	std::string _getUniqueSegName(Channel* channel, const std::string& baseName) const;
};

} // namespace nTM_Macros

#endif // _MOD_TM_MACROS_HPP_
