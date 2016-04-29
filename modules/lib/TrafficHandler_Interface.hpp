/* -*- C++ -*- */

//=============================================================================
/**
 * @file   TrafficHandler_Interface.hpp
 * @author Tad Kollar  
 *
 * $Id: TrafficHandler_Interface.hpp 2369 2013-09-25 17:49:33Z tkollar $
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _TRAFFIC_HANDLER_INTERFACE_HPP_
#define _TRAFFIC_HANDLER_INTERFACE_HPP_

#include "BaseTrafficHandler.hpp"
#include "CE_Config.hpp"
#include "CE_Macros.hpp"
#include "Channel.hpp"
#include "ChannelRegistry.hpp"
#include "DeviceRegistry.hpp"
#include "InterfaceMacros.hpp"
#include "httpd/string_nocase.hpp"

namespace nasaCE {

//=============================================================================
/**
 * @class TrafficHandler_Interface
 * @author Tad Kollar  
 * @brief Generates an XML-RPC interface for subclasses of BaseTrafficHandler.
*/
//=============================================================================
template<class HandlerType>
class TrafficHandler_Interface: public CE_DLL {

public:
    TrafficHandler_Interface(xmlrpc_c::registryPtr& rpcRegistry, const std::string& dllName):
    	nasaCE::CE_DLL(dllName) {
		ACE_TRACE("TrafficHandler_Interface::TrafficHandler_Interface");
	}

	virtual ~TrafficHandler_Interface() {
		ACE_TRACE("TrafficHandler_Interface::~TrafficHandler_Interface");
	}

	HandlerType* find_handler(const std::string channelName, const std::string handlerName) {
		return find_handler_<HandlerType>(channelName, handlerName);
	}

	HandlerType* find_handler(xmlrpc_c::paramList const& paramList) {
		std::string channelName = (std::string) paramList.getString(0);
		std::string handlerName = (std::string) paramList.getString(1);

		return find_handler_<HandlerType>(channelName, handlerName);
	}

	HandlerLink::InputRank getInputRank(xmlrpc_c::paramList const& paramList, const int& paramIdx) {
		std::string inputRankStr = (std::string) paramList.getString(paramIdx);

		if (inputRankStr[0] == 'p' || inputRankStr[0] == 'P' ) {
			return HandlerLink::PrimaryInput;
		}
		else if (inputRankStr[0] == 'a' || inputRankStr[0] == 'A' ) {
			return HandlerLink::AuxInput;
		}

		throw nd_error("Input rank can only be 'Primary' or 'Auxiliary'.");
	}

	void addInstance(const std::string& segName, const std::string& channelName) {
		ACE_TRACE("TrafficHandler_Interface::addInstance");

		Channel* channel = channels::instance()->channel(channelName);

		try {
			channel->add<HandlerType>(segName, this);
		}
		catch (const Exception &e) {
			throw nd_error("Unable to add new handler " + segName + ": " + e.what());
		}

		ND_DEBUG("Added handler %s to channel %s.\n", segName.c_str(), channelName.c_str());
	}

	virtual void add(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP) {
		ACE_TRACE("TrafficHandler_Interface::add");

		std::string channelName = paramList.getString(0);
		std::string newName;
		if (paramList.size() > 1) { newName = (std::string) paramList.getString(1); }

		addInstance(newName, channelName);

		*retvalP = xmlrpc_c::value_string(newName);
	}

	virtual void startup(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP) {
		ACE_TRACE("TrafficHandler_Interface::start");
		find_handler(paramList)->open();

		*retvalP = xmlrpc_c::value_nil();
	}

	virtual void pause(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP) {
		ACE_TRACE("TrafficHandler_Interface::pause");
		find_handler(paramList)->stopTraffic(false);

		*retvalP = xmlrpc_c::value_nil();
	}

	virtual void get_output(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP) {
		ACE_TRACE("TrafficHandler_Interface::get_output");

		std::string channelName(paramList.getString(0));
		std::string handlerName(paramList.getString(1));
		BaseTrafficHandler::LinkType outputRank = BaseTrafficHandler::PrimaryOutputLink;

		if (paramList.size() > 2) {
			std::string outputRankStr(paramList.getString(2));
			if (outputRankStr[0] == 'p' || outputRankStr[0] == 'P' ) {
				outputRank = BaseTrafficHandler::PrimaryOutputLink;
			}
			else if (outputRankStr[0] == 'a' || outputRankStr[0] == 'A' ) {
				outputRank = BaseTrafficHandler::AuxOutputLink;
			}
			else throw nd_error("Output rank can only be 'Primary' or 'Auxiliary'.");
		}
		BaseTrafficHandler* handler = find_handler(channelName, handlerName);

		HandlerLink* link = handler->getLink(outputRank);
		std::string outputName = "";
		if (link) outputName = link->getTarget()->getName();

		*retvalP = xmlrpc_c::value_string(outputName);
	}

	virtual void connect_output(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP) {
		ACE_TRACE("TrafficHandler_Interface::connect_output");

		std::string channelName(paramList.getString(0));
		std::string handlerName(paramList.getString(1));
		std::string targetName(paramList.getString(2));
		HandlerLink::OutputRank outputRank = HandlerLink::PrimaryOutput;
		HandlerLink::InputRank inputRank = HandlerLink::PrimaryInput;

		if (paramList.size() > 3) {
			string_nocase outputRankStr(paramList.getString(3).c_str());
			if (outputRankStr.substr(0,3) == "pri" ) outputRank = HandlerLink::PrimaryOutput;
			else if (outputRankStr.substr(0,3) == "aux") outputRank = HandlerLink::AuxOutput;
			else throw BadValue("Output rank can only be 'Primary' or 'Auxiliary'.");
		}

		if (paramList.size() > 4) {
			string_nocase inputRankStr(paramList.getString(4).c_str());
			if (inputRankStr.substr(0,3) == "pri" ) inputRank = HandlerLink::PrimaryInput;
			else if (inputRankStr.substr(0,3) == "aux" ) inputRank = HandlerLink::AuxInput;
			else throw BadValue("Input rank can only be 'Primary' or 'Auxiliary'.");
		}

		HandlerType* handler = find_handler(channelName, handlerName);

		// No target, must want to disconnect
		if ( targetName.empty() ) {
			handler->disconnectOutput(outputRank);
		}
		else {
			BaseTrafficHandler* targetHandler = find_handler_<BaseTrafficHandler>(channelName, targetName);
			handler->connectOutput(targetHandler, outputRank, inputRank, true);
		}

		*retvalP = xmlrpc_c::value_nil();
	}

	// Another way of using connect_output more conistent with the config file layout.
	virtual void connect_primary_output(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP) {
		ACE_TRACE("TrafficHandler_Interface::connect_primary_output");

		std::vector<xmlrpc_c::value> targetArray(paramList.getArray(2));
		xmlrpc_c::paramList revisedParams(5);

		revisedParams.addx(paramList[0])
			.addx(paramList[1])
			.addx(targetArray[0])
			.addx(xmlrpc_c::value_string("Primary"))
			.addx(targetArray[1]);

		connect_output(revisedParams, retvalP);
	}

	// Another way of using connect_output more conistent with the config file layout.
	virtual void connect_aux_output(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP) {
		ACE_TRACE("TrafficHandler_Interface::connect_aux_output");

		std::vector<xmlrpc_c::value> targetArray(paramList.getArray(2));
		xmlrpc_c::paramList revisedParams(5);

		revisedParams.addx(paramList[0])
			.addx(paramList[1])
			.addx(targetArray[0])
			.addx(xmlrpc_c::value_string("Aux"))
			.addx(targetArray[1]);

		connect_output(revisedParams, retvalP);
	}

	virtual void set_device(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP) {
		ACE_TRACE("TrafficHandler_Interface::set_device");
		std::string deviceName = (std::string) paramList.getString(2);

		CE_Device* device = 0;

		if (! deviceName.empty() ) device = devices::instance()->device(deviceName);

		HandlerType* handler = find_handler(paramList);

		handler->disconnectDevice(); // Get rid of any existing device.
		if ( device) {
			handler->connectDevice(device);
			CEcfg::instance()->getOrAddString(handler->cfgKey("deviceName")) = deviceName.c_str();
		}
		else {
			CEcfg::instance()->remove(handler->cfgKey("deviceName"));
		}


		*retvalP = xmlrpc_c::value_nil();
	}

	virtual void get_device(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP) {
		ACE_TRACE("TrafficHandler_Interface::get_device");

		HandlerType* handler = find_handler(paramList);
		CE_Device* device = handler->getDevice();

		std::string deviceName = device? device->getName() : "";

		*retvalP = xmlrpc_c::value_string(deviceName);
	}

	GENERATE_INT_ACCESSORS(mru, setMRU, getMRU);
	GENERATE_INT_ACCESSORS(mtu, setMTU, getMTU);

	virtual void get_low_water_mark(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP) {
		ACE_TRACE("TrafficHandler_Interface::get_low_water_mark");

		*retvalP = xmlrpc_c::value_int(find_handler(paramList)->getLowWaterMark());
	}

	virtual void set_low_water_mark(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP) {
		ACE_TRACE("TrafficHandler_Interface::set_low_water_mark");
		const int newMark = paramList.getInt(2);

		find_handler(paramList)->setLowWaterMark(newMark);

		*retvalP = xmlrpc_c::value_nil();
	}

	virtual void get_high_water_mark(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP) {
		ACE_TRACE("TrafficHandler_Interface::get_high_water_mark");

		*retvalP = xmlrpc_c::value_int(find_handler(paramList)->getHighWaterMark());
	}

	virtual void set_high_water_mark(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP) {
		ACE_TRACE("TrafficHandler_Interface::set_high_water_mark");
		const int newMark = paramList.getInt(2);

		find_handler(paramList)->setHighWaterMark(newMark);

		*retvalP = xmlrpc_c::value_nil();
	}

	GENERATE_I8_ACCESSORS(received_unit_count, setReceivedUnitCount, getReceivedUnitCount);
	GENERATE_I8_ACCESSORS(received_octet_count, setReceivedOctetCount, getReceivedOctetCount);

	virtual void get_queued_unit_count(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP) {
		ACE_TRACE("TrafficHandler_Interface::get_queued_unit_count");
		HandlerLink::InputRank inputRank = HandlerLink::PrimaryInput;

		if (paramList.size() > 2) inputRank = getInputRank(paramList, 2);

		const bool useAux = ( inputRank == HandlerLink::AuxInput )? true: false;

		*retvalP = xmlrpc_c::value_int(find_handler(paramList)->getQueuedUnitCount(useAux));
	}

	virtual void get_queued_octet_count(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP) {
		ACE_TRACE("TrafficHandler_Interface::get_queued_octet_count");
		HandlerLink::InputRank inputRank = HandlerLink::PrimaryInput;

		if (paramList.size() > 2) inputRank = getInputRank(paramList, 2);

		const bool useAux = ( inputRank == HandlerLink::AuxInput )? true: false;

		*retvalP = xmlrpc_c::value_int(find_handler(paramList)->getQueuedOctetCount(useAux));
	}

	virtual void get_input_links(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP) {
		ACE_TRACE("TrafficHandler_Interface::get_input_links");

		BaseTrafficHandler* handler = find_handler(paramList);

		xstruct linkDescs;

		HandlerLink* primaryInputLink = handler->getLink(BaseTrafficHandler::PrimaryInputLink);
		if (primaryInputLink)
			linkDescs[primaryInputLink->getSource()->getName()] = xmlrpc_c::value_string("Primary");

		HandlerLink* auxInputLink = handler->getLink(BaseTrafficHandler::AuxInputLink);
		if (auxInputLink)
			linkDescs[auxInputLink->getSource()->getName()] = xmlrpc_c::value_string("Aux");

		*retvalP = xmlrpc_c::value_struct(linkDescs);
	}

	virtual void get_output_links(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP) {
		ACE_TRACE("TrafficHandler_Interface::get_output_links");

		BaseTrafficHandler* handler = find_handler(paramList);

		xstruct linkDescs;

		HandlerLink* primaryOutputLink = handler->getLink(BaseTrafficHandler::PrimaryOutputLink);
		if (primaryOutputLink)
			linkDescs[primaryOutputLink->getTarget()->getName()] = xmlrpc_c::value_string("Primary");

		HandlerLink* auxOutputLink = handler->getLink(BaseTrafficHandler::AuxOutputLink);
		if (auxOutputLink)
			linkDescs[auxOutputLink->getTarget()->getName()] = xmlrpc_c::value_string("Aux");

		*retvalP = xmlrpc_c::value_struct(linkDescs);
	}

	virtual void get_info(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP) {
		ACE_TRACE("TrafficHandler_Interface::get_info");

		HandlerType* handler = find_handler(paramList);

		ACE_Time_Value startTime = ACE_High_Res_Timer::gettimeofday_hr();

		xstruct info, names, counters, settings, timestamps;

		build_info_(paramList, handler, names, counters, settings);

		// Get the input/output links
		xmlrpc_c::value inputLinks, outputLinks;
		get_input_links(paramList, &inputLinks);
		get_output_links(paramList, &outputLinks);

		// Subclasses will make additions here.

		ACE_Time_Value endTime = ACE_High_Res_Timer::gettimeofday_hr();
		timestamps["callStartSec"] = xmlrpc_c::value_int(startTime.sec());
		timestamps["callStartUsec"] = xmlrpc_c::value_int(startTime.usec());
		timestamps["callEndSec"] = xmlrpc_c::value_int(endTime.sec());
		timestamps["callEndUsec"] = xmlrpc_c::value_int(endTime.usec());

		combine_info_(info, names, inputLinks, outputLinks, counters, settings, timestamps);

		*retvalP = xmlrpc_c::value_struct(info);
	}

	#ifdef DEFINE_DEBUG
	GENERATE_BOOL_ACCESSORS(debug, setDebugging, getDebugging);
	#endif

protected:
	virtual void get_counters_(xmlrpc_c::paramList const& paramList, HandlerType* handler,
		xstruct& counters) {
		ACE_TRACE("TrafficHandler_Interface::get_counters_");

		// Build a map of all associated counters
		counters["receivedUnits"] = xmlrpc_c::value_i8(handler->getReceivedUnitCount());
		counters["receivedOctets"] = xmlrpc_c::value_i8(handler->getReceivedOctetCount());
		counters["queuedUnits"] = xmlrpc_c::value_int(handler->getQueuedUnitCount());
		counters["queuedOctets"] = xmlrpc_c::value_int(handler->getQueuedOctetCount());

		if (handler->getAuxQueue()) {
			counters["auxReceivedUnits"] =
				xmlrpc_c::value_i8(handler->getReceivedUnitCount(HandlerLink::AuxInput));
			counters["auxReceivedOctets"] =
				xmlrpc_c::value_i8(handler->getReceivedOctetCount(HandlerLink::AuxInput));
			counters["auxQueuedUnits"] = xmlrpc_c::value_int(handler->getQueuedUnitCount(true));
			counters["auxQueuedOctets"] = xmlrpc_c::value_int(handler->getQueuedOctetCount(true));
		}
	}

	virtual void get_settings_(xmlrpc_c::paramList const& paramList, HandlerType* handler,
		xstruct& settings) {
		ACE_TRACE("TrafficHandler_Interface::get_settings_");

		// Build a map of associated settings
		settings["highWaterMark"] = xmlrpc_c::value_int(handler->msg_queue()->high_water_mark());
		settings["lowWaterMark"] = xmlrpc_c::value_int(handler->msg_queue()->low_water_mark());
		settings["MRU"] = xmlrpc_c::value_int(handler->getMRU());
		settings["MTU"] = xmlrpc_c::value_int(handler->getMTU());
		settings["processing"] = xmlrpc_c::value_boolean(handler->continueService());
	}

	virtual void build_info_(xmlrpc_c::paramList const& paramList, HandlerType* handler,
		xstruct& names, xstruct& counters, xstruct& settings) {
		ACE_TRACE("TrafficHandler_Interface::build_info_");

		// Build a map of all associated identifiers
		names["channel"] = paramList[0];
		names["segment"] = xmlrpc_c::value_string(handler->getName());
		names["dll"] = xmlrpc_c::value_string(handler->ptrDLL()->getName());
		if ( handler->getDevice() )
			names["device"] = xmlrpc_c::value_string(handler->getDevice()->getName());

		get_counters_(paramList, handler, counters);
		get_settings_(paramList, handler, settings);
	}

	virtual void combine_info_(xstruct& info, xstruct& names, xmlrpc_c::value& inputLinks,
		xmlrpc_c::value& outputLinks, xstruct& counters, xstruct& settings, xstruct& timestamps) {

		info["names"] = xmlrpc_c::value_struct(names);
		info["inputs"] = inputLinks;
		info["outputs"] = outputLinks;
		info["counters"] = xmlrpc_c::value_struct(counters);
		info["settings"] = xmlrpc_c::value_struct(settings);
		info["timestamps"] = xmlrpc_c::value_struct(timestamps);
	}

	template <class HType>
	HType* find_handler_(const std::string channelName, const std::string handlerName) {

		Channel* channel = channels::instance()->channel(channelName);

		if ( ! channel )
			throw nd_error("Channel named '" + channelName + "' does not exist.");

		BaseTrafficHandler* handler = channel->find(handlerName);

		if ( ! handler)
			throw nd_error("Segment named '" + handlerName + "' does not exist in channel '" + channelName + "'");

		// Make sure segment X didn't search for a name used by a segment Y.
		// if (typeid(*handler) != typeid(HType))
		if (! dynamic_cast<HType*>(handler))
			throw nd_error("Segment named '" + handlerName + "' found in channel '" +
				channelName + "', but has the wrong type.");

		return dynamic_cast<HType*>(handler);
	}

}; // class TrafficHandler_Interface

} // namespace nasaCE

//=============================================================================
/// @def TEMPLATE_METHOD_CLASSES
/// @brief Generate classes to access the TrafficHandler_Interface template methods.
//=============================================================================
#define TEMPLATE_METHOD_CLASSES_BASE(derived_type, pointer_name) \
\
METHOD_CLASS2(derived_type, add , "s:ss",\
	"Create a new handler and add it to the specified channel.",\
	pointer_name) \
\
METHOD_CLASS2(derived_type, startup , "n:ss",\
	"Locate the specified handler in the named channel and call its open() method.",\
	pointer_name) \
\
METHOD_CLASS2(derived_type, pause , "n:ss",\
	"Locate the specified handler in the named channel and call its open() method.",\
	pointer_name) \
\
METHOD_CLASS2(derived_type, get_output , "s:sss",\
	"Get the target name of the primary or auxiliary output link.",\
	pointer_name) \
\
METHOD_CLASS2(derived_type, connect_output , "n:sssss,n:ssisss",\
	"Direct either primary or auxiliary output of one existing handler to another existing handler.",\
	pointer_name) \
\
METHOD_CLASS2(derived_type, connect_primary_output , "n:ssA",\
	"Direct the primary output of one existing handler to another existing handler.",\
	pointer_name) \
\
METHOD_CLASS2(derived_type, connect_aux_output , "n:ssA",\
	"Direct the auxiliary output of one existing handler to another existing handler.",\
	pointer_name) \
\
METHOD_CLASS2(derived_type, set_device , "n:sss",\
	"Establish (or possibly break) a connection between a configured Device and existing handler.",\
	pointer_name) \
\
METHOD_CLASS2(derived_type, get_device , "s:ss",\
	"Get the name of the Device associated with the segment.",\
	pointer_name) \
\
METHOD_CLASS2(derived_type, get_mru , "i:ss",\
	"Get the maximum receive unit (MRU) value.",\
	pointer_name) \
\
METHOD_CLASS2(derived_type, set_mru , "n:ssi",\
	"Set the the maximum receive unit (MRU) value.",\
	pointer_name) \
\
METHOD_CLASS2(derived_type, get_mtu , "i:ss",\
	"Get the maximum transmit unit (MTU) value.",\
	pointer_name) \
\
METHOD_CLASS2(derived_type, set_mtu , "n:ssi",\
	"Set the the maximum receive unit (MTU) value.",\
	pointer_name) \
\
METHOD_CLASS2(derived_type, get_low_water_mark , "i:ss",\
	"Get the point at which the handler accepts new data again after hitting the high water mark.",\
	pointer_name) \
\
METHOD_CLASS2(derived_type, set_low_water_mark , "n:ssi",\
	"Set the point at which the handler accepts new data again after hitting the high water mark.",\
	pointer_name) \
\
METHOD_CLASS2(derived_type, get_high_water_mark , "i:ss",\
	"Get the point at which the handler's message queue stops accepting new data.",\
	pointer_name) \
\
METHOD_CLASS2(derived_type, set_high_water_mark , "n:ssi",\
	"Set the point at which the handler's message queue stops accepting new data.",\
	pointer_name) \
\
METHOD_CLASS2(derived_type, get_received_unit_count , "i:ss,i:sss",\
	"Get the number of data units received by the segment.",\
	pointer_name) \
\
METHOD_CLASS2(derived_type, set_received_unit_count , "n:ssi,n:ssis",\
	"set the number of data units received by the segment.",\
	pointer_name) \
\
METHOD_CLASS2(derived_type, get_received_octet_count , "i:ss,i:sss",\
	"Get the number of octets received by the segment.",\
	pointer_name) \
\
METHOD_CLASS2(derived_type, set_received_octet_count , "n:ssi,n:ssis",\
	"Set the number of octets received by the segment.",\
	pointer_name) \
\
METHOD_CLASS2(derived_type, get_queued_unit_count , "i:ss,i:sss",\
	"Get the number of data units held in the message queue.",\
	pointer_name) \
\
METHOD_CLASS2(derived_type, get_queued_octet_count , "i:ss,i:sss",\
	"Get the number of octets held in the message queue.",\
	pointer_name) \
\
METHOD_CLASS2(derived_type, get_input_links , "S:ss",\
	"Get a map of the input links for the segment.",\
	pointer_name) \
\
METHOD_CLASS2(derived_type, get_output_links , "S:ss",\
	"Get a map of the output links for the segment.",\
	pointer_name) \
\
METHOD_CLASS2(derived_type, get_info , "S:ss",\
	"Get a maps of all identifiers, links, counters, and settings for the segment.",\
	pointer_name)

#define DLL_ACTIVATE_AND_DEACTIVATE(derived_type, pointer_name) \
extern "C" nasaCE::CE_DLL* activate(xmlrpc_c::registryPtr& rpcRegistry) {\
	ND_DEBUG("Activating %s\n", #derived_type);\
	pointer_name = new derived_type(rpcRegistry);\
	return pointer_name;\
}\
\
extern "C" void deactivate() {\
	ND_DEBUG("Deactivating %s\n", #derived_type);\
	delete pointer_name;\
	pointer_name = 0;\
}

#ifdef DEFINE_DEBUG
	#define TEMPLATE_METHOD_CLASSES(derived_type, pointer_name)\
		TEMPLATE_METHOD_CLASSES_BASE(derived_type, pointer_name)\
		METHOD_CLASS2(derived_type, get_debug , "b:ss",\
			"Get whether the segment is printing debug info.",\
			pointer_name) \
		\
		METHOD_CLASS2(derived_type, set_debug , "n:ssb",\
			"Set whether the segment is printing debug info.",\
			pointer_name) \
		DLL_ACTIVATE_AND_DEACTIVATE(derived_type, pointer_name)
#else
	#define TEMPLATE_METHOD_CLASSES(derived_type, pointer_name)\
		TEMPLATE_METHOD_CLASSES_BASE(derived_type, pointer_name)\
		DLL_ACTIVATE_AND_DEACTIVATE(derived_type, pointer_name)
#endif

//=============================================================================
/// @def TEMPLATE_REGISTER_METHODS
/// @brief Generate code to put into register_methods() of a TrafficHandler_Interface subclass.
//=============================================================================
#define TEMPLATE_REGISTER_METHODS_BASE(prefix, derived_type) \
	REGISTER_METHOD(derived_type##_add ,				#prefix ".add");\
	REGISTER_METHOD(derived_type##_startup ,			#prefix ".startup");\
	REGISTER_METHOD(derived_type##_pause ,				#prefix ".pause");\
	REGISTER_METHOD(derived_type##_connect_output ,		#prefix ".connectOutput");\
	REGISTER_METHOD(derived_type##_connect_primary_output ,		#prefix ".connectPrimaryOutput");\
	REGISTER_METHOD(derived_type##_connect_aux_output ,		#prefix ".connectAuxiliaryOutput");\
	REGISTER_METHOD(derived_type##_get_output ,		#prefix ".getOutput");\
	REGISTER_METHOD(derived_type##_get_input_links , #prefix ".getInputLinks");\
	REGISTER_METHOD(derived_type##_get_output_links , #prefix ".getOutputLinks");\
	REGISTER_METHOD(derived_type##_set_device ,		#prefix ".setDevice");\
	REGISTER_METHOD(derived_type##_get_device ,		#prefix ".getDevice");\
	REGISTER_METHOD(derived_type##_get_mru , #prefix ".getMRU");\
	REGISTER_METHOD(derived_type##_set_mru , #prefix ".setMRU");\
	REGISTER_METHOD(derived_type##_get_mtu , #prefix ".getMTU");\
	REGISTER_METHOD(derived_type##_set_mtu , #prefix ".setMTU");\
	REGISTER_METHOD(derived_type##_get_low_water_mark , #prefix ".getLowWaterMark");\
	REGISTER_METHOD(derived_type##_set_low_water_mark , #prefix ".setLowWaterMark");\
	REGISTER_METHOD(derived_type##_get_high_water_mark , #prefix ".getHighWaterMark");\
	REGISTER_METHOD(derived_type##_set_high_water_mark , #prefix ".setHighWaterMark");\
	REGISTER_METHOD(derived_type##_get_received_unit_count , #prefix ".getReceivedUnitCount");\
	REGISTER_METHOD(derived_type##_set_received_unit_count , #prefix ".setReceivedUnitCount");\
	REGISTER_METHOD(derived_type##_get_received_octet_count , #prefix ".getReceivedOctetCount");\
	REGISTER_METHOD(derived_type##_set_received_octet_count , #prefix ".setReceivedOctetCount");\
	REGISTER_METHOD(derived_type##_get_queued_unit_count , #prefix ".getQueuedUnitCount");\
	REGISTER_METHOD(derived_type##_get_queued_octet_count , #prefix ".getQueuedOctetCount");\
	REGISTER_METHOD(derived_type##_get_info , #prefix ".getInfo");
#endif

#ifdef DEFINE_DEBUG
	#define TEMPLATE_REGISTER_METHODS(prefix, derived_type) \
		TEMPLATE_REGISTER_METHODS_BASE(prefix, derived_type)\
		REGISTER_METHOD(derived_type##_get_debug , #prefix ".getDebugging");\
		REGISTER_METHOD(derived_type##_set_debug , #prefix ".setDebugging");
#else
	#define TEMPLATE_REGISTER_METHODS(prefix, derived_type) \
		TEMPLATE_REGISTER_METHODS_BASE(prefix, derived_type)
#endif

// _TRAFFIC_HANDLER_INTERFACE_HPP_
