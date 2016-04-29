/* -*- C++ -*- */

//=============================================================================
/**
 * @file   DelayedResults.hpp
 * @author Tad Kollar  
 *
 * $Id: DelayedResults.hpp 1671 2012-08-28 15:19:00Z tkollar $ 
 * Copyright (c) 2008.
 *      NASA Glenn Research Center.  All rights reserved.
 */
//=============================================================================

#ifndef _DELAYED_RESULTS_HPP_
#define _DELAYED_RESULTS_HPP_

#include "CE_Config.hpp"

#include <xmlrpc-c/base.hpp>
#include <xmlrpc-c/registry.hpp>

#include <queue>
#include <list>

#include <ace/Cleanup.h>
#include <ace/Object_Manager.h>
#include <ace/Singleton.h>

namespace nasaCE {

struct Result {
	Result(const std::string& tname, const ACE_Time_Value& tval, xmlrpc_c::value* rP):
		timer_name(tname), time_completed(tval), retvalP(rP) {
		// ACE_Object_Manager::at_exit(this);
	}
	
	std::string timer_name;
	ACE_Time_Value time_completed;
	xmlrpc_c::value* retvalP;
	inline bool operator< (Result& b) { return (time_completed < b.time_completed); }
};

struct ResultList: public std::list<Result> {
	ResultList(): std::list<Result>() { /* ACE_Object_Manager::at_exit(this); */ }
	~ResultList() {	ACE_TRACE("~ResultList"); }
	ResultList::iterator find(const std::string& timer_name) {
		ResultList::iterator pos;
		for (pos = begin(); pos != end() && pos->timer_name != timer_name; pos++ ) {}
		
		return pos;
	}
};

/// DelayedResults contains the aftermath of calls that were made with timers.
/// The client can't wait around all day for the timer to complete, so it can
/// check back afterwards to see if the call has completed.
class DelayedResults {
private:
	ResultList _delayed_results;
	void _trim(); // Retain only # of results as specified in the config file

public:
	DelayedResults() { }
	~DelayedResults() { ACE_TRACE("~DelayedResults"); }
	
	void add_delayed_result(const std::string&, xmlrpc_c::value* retvalP);
	void yank_delayed_result(const std::string&, xmlrpc_c::value* const);
	void yank_delayed_result(xmlrpc_c::paramList const&, xmlrpc_c::value* const);
};

typedef ACE_Singleton<DelayedResults, ACE_Recursive_Thread_Mutex> delayed_results;

} // namespace nasaCE



#endif
