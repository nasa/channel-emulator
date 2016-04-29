/* -*- C++ -*- */

//=============================================================================
/**
 * @file   Timer_Interface.hpp
 * @author Tad Kollar  
 *
 * $Id: Timer_Interface.hpp 1670 2012-08-28 13:25:28Z tkollar $ 
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef NASATIMER_INTERFACE_HPP
#define NASATIMER_INTERFACE_HPP

#include "CE_Macros.hpp"
#include "CE_Config.hpp"
#include "Time_Handler.hpp"
#include <xmlrpc-c/xml.hpp>

namespace nasaCE {

/**
	@author Tad Kollar  
*/
class Timer_Interface : public CE_DLL {
public:
    Timer_Interface(xmlrpc_c::registryPtr& rpcRegistry);

    ~Timer_Interface();
    
	void get_delayed_result(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);
	void set_scenario_zeroTime(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);	
	void get_current_wall_clock(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);
	void get_current_scenario_clock(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);
	void set_timer(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP);
	
	/// Export specified methods via XML-RPC.
	void register_methods(xmlrpc_c::registryPtr&);
}; // class Timer_Interface

extern Timer_Interface* Timer_InterfaceP;

METHOD_CLASS(get_delayed_result, "s:s",
	"Get the result of a delayed function call.", Timer_InterfaceP)
METHOD_CLASS(set_scenario_zeroTime, "n:ss",
	"Synchronize the scenario's mission time with the wall clock.", Timer_InterfaceP)
METHOD_CLASS(get_current_wall_clock, "S:n",
	"Return the current wall clock (real) time.", Timer_InterfaceP)
METHOD_CLASS(get_current_scenario_clock, "S:n",
	"Return the current scenario wall clock time.", Timer_InterfaceP)
METHOD_CLASS(set_timer, "s:ss",
	"Set a timer to execute the specified call in the future.", Timer_InterfaceP)
	
} // namespace nasaCE

#endif // NASATIMER_INTERFACE_HPP
