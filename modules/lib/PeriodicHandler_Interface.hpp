/* -*- C++ -*- */

//=============================================================================
/**
 * @file   PeriodicHandler_Interface.hpp
 * @author Tad Kollar  
 *
 * $Id: PeriodicHandler_Interface.hpp 1978 2013-04-26 19:01:31Z tkollar $
 * Copyright (c) 2010.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _PERIODIC_HANDLER_INTERFACE_HPP_
#define _PERIODIC_HANDLER_INTERFACE_HPP_

#include "TrafficHandler_Interface.hpp"

namespace nasaCE {

//=============================================================================
/**
 * @class PeriodicHandler_Interface
 * @author Tad Kollar  
 * @brief Generates an XML-RPC interface for subclasses of PeriodicTrafficHandler.
 */
//=============================================================================
template<class PeriodicHandlerType>
class PeriodicHandler_Interface: public TrafficHandler_Interface<PeriodicHandlerType> {

public:
    PeriodicHandler_Interface(xmlrpc_c::registryPtr& rpcRegistry, const std::string& dllName):
    	TrafficHandler_Interface<PeriodicHandlerType>(rpcRegistry, dllName) {
		ACE_TRACE("PeriodicHandler_Interface::PeriodicHandler_Interface");
	}

	virtual ~PeriodicHandler_Interface() {
		ACE_TRACE("PeriodicHandler_Interface::~PeriodicHandler_Interface");
	}

	virtual void get_interval_usec(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP) {
		ACE_TRACE("PeriodicHandler_Interface::get_interval_usec");

		*retvalP = xmlrpc_c::value_int(this->find_handler(paramList)->getInterval());
	}

	virtual void set_interval_usec(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP) {
		ACE_TRACE("PeriodicHandler_Interface::set_interval_usec");
		const int newInterval = paramList.getInt(2);

		this->find_handler(paramList)->setInterval(newInterval);

		*retvalP = xmlrpc_c::value_nil();
	}

	virtual void get_idle_count(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP) {
		ACE_TRACE("PeriodicHandler_Interface::get_idle_count");

		*retvalP = xmlrpc_c::value_i8(this->find_handler(paramList)->getIdleCount());
	}

	virtual void set_idle_count(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP) {
		ACE_TRACE("PeriodicHandler_Interface::set_idle_count");
		const uint64_t newCount = paramList.getI8(2);

		this->find_handler(paramList)->setIdleCount(newCount);

		*retvalP = xmlrpc_c::value_nil();
	}

protected:
	void get_counters_(xmlrpc_c::paramList const& paramList, PeriodicHandlerType* handler,
		xstruct& counters) {
		ACE_TRACE("PeriodicHandler_Interface::get_counters_");
		counters["sentIdleUnits"] = xmlrpc_c::value_i8(handler->getIdleCount());

		this->TrafficHandler_Interface<PeriodicHandlerType>::get_counters_(paramList, handler, counters);
	}
};

} // namespace nasaCE

#define PERIODIC_METHOD_CLASSES(derived_type, pointer_name) \
\
TEMPLATE_METHOD_CLASSES(derived_type, pointer_name);\
\
METHOD_CLASS2(derived_type, get_interval_usec , "i:ss",\
	"Return the number of microseconds between transmits.",\
	pointer_name) \
\
METHOD_CLASS2(derived_type, set_interval_usec , "n:ssi",\
	"Set the number of microseconds between transmits.",\
	pointer_name)\
\
METHOD_CLASS2(derived_type, get_idle_count , "i:ss",\
	"Return the number of idle units transmitted.",\
	pointer_name) \
\
METHOD_CLASS2(derived_type, set_idle_count , "n:ssi",\
	"Set the number of idle units transmitted.",\
	pointer_name)

#define PERIODIC_REGISTER_METHODS(prefix, derived_type) \
\
	TEMPLATE_REGISTER_METHODS(prefix, derived_type);\
	REGISTER_METHOD(derived_type##_get_interval_usec , #prefix ".getIntervalUsec");\
	REGISTER_METHOD(derived_type##_set_interval_usec , #prefix ".setIntervalUsec");\
	REGISTER_METHOD(derived_type##_get_idle_count , #prefix ".getSentIdleCount");\
	REGISTER_METHOD(derived_type##_set_idle_count , #prefix ".setSentIdleCount");

#endif
