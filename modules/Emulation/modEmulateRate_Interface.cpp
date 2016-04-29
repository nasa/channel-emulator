/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modEmulateRate_Interface.cpp
 * @author Tad Kollar  
 *
 * $Id: modEmulateRate_Interface.cpp 2028 2013-06-05 13:48:37Z tkollar $
 * Copyright (c) 2010.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================


#include "modEmulateRate.hpp"
#include "TrafficHandler_Interface.hpp"

namespace nEmulateRate {

//=============================================================================
/**
 * @class modEmulateRate_Interface
 * @author Tad Kollar  
 * @brief Provide an XML-RPC interface to the Rate Emulation module.
 */
//=============================================================================
class modEmulateRate_Interface: public nasaCE::TrafficHandler_Interface<modEmulateRate> {
public:
	modEmulateRate_Interface(xmlrpc_c::registryPtr& rpcRegistry):
		nasaCE::TrafficHandler_Interface<modEmulateRate>(rpcRegistry, "modEmulateRate") {
		ACE_TRACE("modEmulateRate_Interface::modEmulateRate_Interface");

		register_methods(rpcRegistry);
	}

	virtual void get_rate_limit(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP) {
		ACE_TRACE("modEmulateRate_Interface::get_rate");
		int rate = 0;

		if (paramList.size() > 2 ) {
			rate = find_handler(paramList)->getRateLimitAtTime(paramList.getDouble(2));
		}
		else {
			rate = find_handler(paramList)->getRateLimit();
		}

		*retvalP = xmlrpc_c::value_double(rate);
	}

	virtual void set_rate_limit(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP) {
		ACE_TRACE("modEmulateRate_Interface::set_rate");
		int rate = 0, startTime = -1.0;

		if ( paramList[2].type() != xmlrpc_c::value::TYPE_ARRAY ) {
			rate = paramList.getInt(2);
		}
		else {
			const std::vector<xmlrpc_c::value> arrayData = paramList.getArray(2);
			startTime = xmlrpc_c::value_double(arrayData[0]);
			rate = xmlrpc_c::value_double(arrayData[1]);
		}

		find_handler(paramList)->setRateLimit(rate, startTime);

		*retvalP = xmlrpc_c::value_nil();
	}

	virtual void clear_timeline(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP) {
		ACE_TRACE("modEmulateRate_Interface::clear_timeline");

		find_handler(paramList)->clearTimeLine();

		*retvalP = xmlrpc_c::value_nil();
	}

	GENERATE_INT_ACCESSORS(unlimited_throughput, setUnlimitedThroughput, getUnlimitedThroughput);
	GENERATE_FLOAT_ACCESSORS(timeline_cycle_secs, setTimeLineCycleSecs, getTimeLineCycleSecs);

	/// Export specified methods via XML-RPC.
	void register_methods(xmlrpc_c::registryPtr&);

protected:
	void get_settings_(xmlrpc_c::paramList const& paramList, modEmulateRate* handler,
		xstruct& settings) {
		ACE_TRACE("modEmulateRate_Interface::get_settings_");

		nasaCE::TrafficHandler_Interface<modEmulateRate>::get_settings_(paramList, handler, settings);

		// Build a map of associated settings
		settings["rateLimit"] = xmlrpc_c::value_int(handler->getRateLimit());
		settings["unlimitedThroughput"] = xmlrpc_c::value_int(handler->getUnlimitedThroughput());
		settings["timeLineCycleSecs"] = xmlrpc_c::value_double(handler->getTimeLineCycleSecs());
	}
};

modEmulateRate_Interface* modEmulateRate_InterfaceP;

TEMPLATE_METHOD_CLASSES(modEmulateRate_Interface, modEmulateRate_InterfaceP);

METHOD_CLASS(get_rate_limit, "i:ssd",
	"Get the rate limit in kbit/s at the optionally specified time in seconds.", modEmulateRate_InterfaceP);
METHOD_CLASS(set_rate_limit, "n:ssi,n:ssA",
	"Set the rate limit in kbit/s at the optionally specified time in seconds.", modEmulateRate_InterfaceP);
METHOD_CLASS(clear_timeline, "n:n",
	"Erase all values in the time line and stop using it (until more values are added).", modEmulateRate_InterfaceP);
GENERATE_ACCESSOR_METHODS(unlimited_throughput, i, "the tested throughput of the unlimited channel, in kbit/s.",
	modEmulateRate_InterfaceP);
GENERATE_ACCESSOR_METHODS(timeline_cycle_secs, d, "the number of seconds before the time line begins to repeat.",
	modEmulateRate_InterfaceP);


void modEmulateRate_Interface::register_methods(xmlrpc_c::registryPtr& rpcRegistry) {
	ACE_TRACE("modEmulateRate_Interface::register_methods");

	TEMPLATE_REGISTER_METHODS(modEmulateRate, modEmulateRate_Interface);
	REGISTER_ACCESSOR_METHODS(rate_limit, modEmulateRate, RateLimit);
	REGISTER_ACCESSOR_METHODS(unlimited_throughput, modEmulateRate, UnlimitedThroughput);
	REGISTER_ACCESSOR_METHODS(timeline_cycle_secs, modEmulateRate, TimeLineCycleSecs);
	REGISTER_METHOD(clear_timeline, "modEmulateRate.clearTimeLine");

}

} // namespace nEmulateRate