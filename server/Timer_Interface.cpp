/* -*- C++ -*- */

//=============================================================================
/**
 * @file   Timer_Interface.cpp
 * @author Tad Kollar  
 *
 * $Id: Timer_Interface.cpp 2082 2013-06-13 13:22:28Z tkollar $
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "Timer_Interface.hpp"
#include "nd_macros.hpp"
#include <ace/Reactor.h>

namespace nasaCE {
Timer_Interface* Timer_InterfaceP;

Timer_Interface::Timer_Interface(xmlrpc_c::registryPtr& rpcRegistry):
	CE_DLL("Timer_Interface") {
	ACE_TRACE("Timer_Interface::Timer_Interface");
	register_methods(rpcRegistry);
}

Timer_Interface::~Timer_Interface()
{
}

void Timer_Interface::register_methods(xmlrpc_c::registryPtr& rpcRegistry) {
	ACE_TRACE("Timer_Interface::register_methods");
	REGISTER_METHOD(get_delayed_result, "time.getResult")
	REGISTER_METHOD(set_scenario_zeroTime, "time.setZeroTime")
	REGISTER_METHOD(get_current_wall_clock, "time.getWallClock")
	REGISTER_METHOD(get_current_scenario_clock, "time.getScenarioClock")
	REGISTER_METHOD(set_timer, "time.setTimer")
}

void Timer_Interface::get_delayed_result(xmlrpc_c::paramList const& paramList,
	xmlrpc_c::value* retvalP) {
	ACE_TRACE("Timer_Interface::get_delayed_result");
   	delayed_results::instance()->yank_delayed_result(paramList, retvalP);
}

void Timer_Interface::set_scenario_zeroTime(xmlrpc_c::paramList const& paramList,
	xmlrpc_c::value* retvalP) {
	ACE_TRACE("Timer_Interface::set_scenario_zeroTime");
	std::string wallclock_time, scenario_time;
	wallclock_time = paramList.getString(0);
	scenario_time =  (paramList.size() > 1) ? paramList.getString(1) : wallclock_time;

	scenarioTimeKeeper::instance()->zeroTime(wallclock_time, scenario_time);

	*retvalP = xmlrpc_c::value_nil();
}

void Timer_Interface::get_current_wall_clock(xmlrpc_c::paramList const& paramList,
	xmlrpc_c::value* retvalP) {
	ACE_TRACE("Timer_Interface::get_current_wall_clock");
	ACE_Time_Value now = scenarioTimeKeeper::instance()->now();
	std::map<std::string, xmlrpc_c::value> xml_tv;
	xml_tv["sec"] = xmlrpc_c::value_int(now.sec());
	xml_tv["usec"] = xmlrpc_c::value_int(now.usec());

	*retvalP = xmlrpc_c::value_struct(xml_tv);
}

void Timer_Interface::get_current_scenario_clock(xmlrpc_c::paramList const& paramList,
	xmlrpc_c::value* retvalP) {
	ACE_TRACE("Timer_Interface::get_current_scenario_clock");

	ACE_Time_Value scen_now = scenarioTimeKeeper::instance()->scenarioNow();
	std::map<std::string, xmlrpc_c::value> xml_tv;
	xml_tv["sec"] = xmlrpc_c::value_int(scen_now.sec());
	xml_tv["usec"] = xmlrpc_c::value_int(scen_now.usec());

	*retvalP = xmlrpc_c::value_struct(xml_tv);
}


void Timer_Interface::set_timer(xmlrpc_c::paramList const& paramList,
	xmlrpc_c::value* retvalP) {
	ACE_TRACE("Timer_Interface::set_timer");

	std::string user_time = paramList.getString(0),
		method_name = paramList.getString(1),
		callXml;

	ND_DEBUG("Received request to delay execution of %s until %s.\n",
		method_name.c_str(), user_time.c_str());
	ACE_Time_Value exec_time(nasaCE::scenarioTimeKeeper::instance()->
		calculateAlarmTime(user_time));

	xmlrpc_c::paramList newParamList;

	for (unsigned i = 2; i < paramList.size(); i++) newParamList.add(paramList[i]);

	xmlrpc_c::xml::generateCall(method_name, newParamList, &callXml);
	nasaCE::Time_Handler* th = new nasaCE::Time_Handler(method_name, callXml);
	std::string* timer_name = new std::string(xmlrpc_server::instance()->newTimerId(method_name));
	long timer_id = ACE_Reactor::instance()->schedule_timer(th, (const void *) timer_name, exec_time);
	xmlrpc_server::instance()->addTimer(*timer_name, timer_id);
	*retvalP = xmlrpc_c::value_string(*timer_name);
}

}
