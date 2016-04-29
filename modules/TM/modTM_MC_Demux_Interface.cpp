/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modTM_MC_Demux_Interface.cpp
 * @author Tad Kollar  
 *
 * $Id: modTM_MC_Demux_Interface.cpp 2375 2013-09-27 18:10:39Z tkollar $
 * Copyright (c) 2012.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "modTM_MC_Demux.hpp"
#include "TM_Interface_Macros.hpp"
#include "httpd/string_nocase.hpp"

namespace nTM_MC_Demux {

using namespace nasaCE;

//=============================================================================
/**
 * @class modTM_MC_Demux_Interface
 * @author Tad Kollar  
 * @brief Provide an XML-RPC interface to the TM Virtual Channel Demultiplexing module.
 */
//=============================================================================
class modTM_MC_Demux_Interface: public nasaCE::TrafficHandler_Interface<modTM_MC_Demux> {
public:
	modTM_MC_Demux_Interface(xmlrpc_c::registryPtr& rpcRegistry):
		nasaCE::TrafficHandler_Interface<modTM_MC_Demux>(rpcRegistry, "modTM_MC_Demux") {
		ACE_TRACE("modTM_MC_Demux_Interface::modTM_MC_Demux_Interface");

		register_methods(rpcRegistry);
	}

	void connect_output(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP) {
		ACE_TRACE("modTM_MC_Demux_Interface::connect_output");
		std::string channelName(paramList.getString(0));
		std::string handlerName(paramList.getString(1));
		ACE_UINT8 MCID = paramList.getInt(2);
		std::string targetName(paramList.getString(3));

		HandlerLink::OutputRank outputRank = HandlerLink::PrimaryOutput;
		if (paramList.size() > 4) {
			string_nocase outputRankStr(paramList.getString(4).c_str());
			if (outputRankStr.substr(0,3) == "pri" ) outputRank = HandlerLink::PrimaryOutput;
			else if (outputRankStr.substr(0,3) == "aux") outputRank = HandlerLink::AuxOutput;
			else throw BadValue("Output rank can only be 'Primary' or 'Auxiliary'.");
		}

		HandlerLink::InputRank inputRank = HandlerLink::PrimaryInput;
		if (paramList.size() > 5) {
			string_nocase inputRankStr(paramList.getString(5).c_str());
			if (inputRankStr.substr(0,3) == "pri" ) inputRank = HandlerLink::PrimaryInput;
			else if (inputRankStr.substr(0,3) == "aux" ) inputRank = HandlerLink::AuxInput;
			else throw BadValue("Input rank can only be 'Primary' or 'Auxiliary'.");
		}

		Channel* channel = channels::instance()->channel(channelName);

		modTM_MC_Demux* handler = dynamic_cast<modTM_MC_Demux*>(channel->find(handlerName));
		BaseTrafficHandler* targetHandler = channel->find(targetName);

		if ( outputRank == HandlerLink::PrimaryOutput )
			handler->connectPrimaryOutput(MCID, targetHandler, inputRank);
		else
			handler->connectOutput(targetHandler, outputRank, inputRank);

		*retvalP = xmlrpc_c::value_nil();
	}

	// Another way of using connect_output more conistent with the config file layout.
	void connect_primary_output(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP) {
		ACE_TRACE("modTM_MC_Demux_Interface::connect_primary_output");

		std::vector<xmlrpc_c::value> targetArray(paramList.getArray(2));
		xmlrpc_c::paramList revisedParams(5);

		revisedParams.addx(paramList[0])
			.addx(paramList[1])
			.addx(targetArray[2])
			.addx(targetArray[0])
			.addx(xmlrpc_c::value_string("Primary"))
			.addx(targetArray[1]);

		connect_output(revisedParams, retvalP);
	}

	void get_output_links(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP) {
		ACE_TRACE("modTM_MC_Demux_Interface::get_output_links");

		modTM_MC_Demux* handler = dynamic_cast<modTM_MC_Demux*>(find_handler(paramList));

		std::list<ACE_UINT8> idList;
		handler->getIdentifiers(idList);

		std::map<std::string, xmlrpc_c::value> linkDescs;

		std::list<ACE_UINT8>::iterator pos;

		for ( pos = idList.begin(); pos != idList.end(); ++pos ) {

			HandlerLink* primaryOutputLink = handler->getLink(BaseTrafficHandler::PrimaryOutputLink, *pos);
			if (primaryOutputLink)
				linkDescs[primaryOutputLink->getTarget()->getName()] = xmlrpc_c::value_string("Primary");
		}

		HandlerLink* auxOutputLink = handler->getLink(BaseTrafficHandler::AuxOutputLink);
		if (auxOutputLink)
			linkDescs[auxOutputLink->getTarget()->getName()] = xmlrpc_c::value_string("Aux");

		*retvalP = xmlrpc_c::value_struct(linkDescs);
	}

	TM_PHYSICAL_CHANNEL_DEFINE_ACCESSORS();

	/// Export specified methods via XML-RPC.
	void register_methods(xmlrpc_c::registryPtr&);

protected:
	void get_counters_(xmlrpc_c::paramList const& paramList, modTM_MC_Demux* handler,
		xstruct& counters) {
		ACE_TRACE("modTM_MC_Demux_Interface::get_counters_");

		nasaCE::TrafficHandler_Interface<modTM_MC_Demux>::get_counters_(paramList, handler, counters);

		GET_TM_PHYSICAL_CHANNEL_COUNTERS();
	}

	void get_settings_(xmlrpc_c::paramList const& paramList, modTM_MC_Demux* handler,
		xstruct& settings) {
		ACE_TRACE("modTM_MC_Demux_Interface::get_settings_");

		nasaCE::TrafficHandler_Interface<modTM_MC_Demux>::get_settings_(paramList, handler, settings);

		GET_TM_PHYSICAL_CHANNEL_SETTINGS();
	}
};

modTM_MC_Demux_Interface* modTM_MC_Demux_InterfaceP;

TEMPLATE_METHOD_CLASSES(modTM_MC_Demux_Interface, modTM_MC_Demux_InterfaceP);
TM_PHYSICAL_CHANNEL_GENERATE_METHODS(modTM_MC_Demux_InterfaceP);

void modTM_MC_Demux_Interface::register_methods(xmlrpc_c::registryPtr& rpcRegistry) {
	ACE_TRACE("modTM_MC_Demux_Interface::register_methods");

	TEMPLATE_REGISTER_METHODS(modTM_MC_Demux, modTM_MC_Demux_Interface);
	TM_PHYSICAL_CHANNEL_REGISTER_METHODS(modTM_MC_Demux);
}

} // namespace nTM_MC_Demux
