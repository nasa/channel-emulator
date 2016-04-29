/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modEmulateDelay_Interface.cpp
 * @author Tad Kollar  
 *
 * $Id: modEmulateDelay_Interface.cpp 1914 2013-03-18 16:51:35Z tkollar $
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "modEmulateDelay.hpp"
#include "TrafficHandler_Interface.hpp"

namespace nEmulateDelay {

//=============================================================================
/**
 * @class modEmulateDelay_Interface
 * @author Tad Kollar  
 * @brief Provide an XML-RPC interface to the Delay/Jitter Emulation module.
 */
//=============================================================================
class modEmulateDelay_Interface: public nasaCE::TrafficHandler_Interface<modEmulateDelay> {
public:
	modEmulateDelay_Interface(xmlrpc_c::registryPtr& rpcRegistry):
		nasaCE::TrafficHandler_Interface<modEmulateDelay>(rpcRegistry, "modEmulateDelay") {
		ACE_TRACE("modEmulateDelay_Interface::modEmulateDelay_Interface");

		register_methods(rpcRegistry);
	}

	virtual void get_delay_seconds(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP) {
		ACE_TRACE("modEmulateDelay_Interface::get_delay_seconds");
		double delay = 0.0;

		if (paramList.size() > 2 ) {
			delay = find_handler(paramList)->getDelaySecondsAtTime(paramList.getDouble(2));
		}
		else {
			delay = find_handler(paramList)->getDelaySeconds();
		}

		*retvalP = xmlrpc_c::value_double(delay);
	}

	virtual void get_jitter_seconds(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP) {
		ACE_TRACE("modEmulateDelay_Interface::get_jitter_seconds");
		double jitter = 0.0;

		if (paramList.size() > 2 ) {
			jitter = find_handler(paramList)->getJitterSecondsAtTime(paramList.getDouble(2));
		}
		else {
			jitter = find_handler(paramList)->getJitterSeconds();
		}

		*retvalP = xmlrpc_c::value_double(jitter);
	}

	virtual void set_delay_and_jitter(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP) {
		ACE_TRACE("modEmulateDelay_Interface::set_delay_and_jitter");

		const xmlrpc_c::value testVal = paramList[2];

		double delay = 0.0, jitter = 0.0, startTime = -1.0;

		// Delay and jitter can be specified as individual double params, or as an array of doubles.
		if ( testVal.type() != xmlrpc_c::value::TYPE_ARRAY ) {
			delay = paramList.getDouble(2);
			if ( paramList.size() > 3 ) jitter = paramList.getDouble(3);
		}
		else {
			const std::vector<xmlrpc_c::value> arrayData = paramList.getArray(2);
			startTime = xmlrpc_c::value_double(arrayData[0]);
			delay = xmlrpc_c::value_double(arrayData[1]);
			jitter = xmlrpc_c::value_double(arrayData[2]);
		}

		find_handler(paramList)->setDelayAndJitter(delay, jitter, startTime);

		*retvalP = xmlrpc_c::value_nil();
	}

	virtual void set_delay_seconds(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP) {
		ACE_TRACE("modEmulateDelay_Interface::set_delay_seconds");
		double delay = paramList.getDouble(2);
		double startTime = (paramList.size() > 3 )? paramList.getDouble(3) : -1.0;

		modEmulateDelay *seg = dynamic_cast<modEmulateDelay*>(find_handler(paramList));
		double jitter = seg->getJitterSeconds();

		seg->setDelayAndJitter(delay, jitter, startTime);

		*retvalP = xmlrpc_c::value_nil();
	}

	virtual void set_jitter_seconds(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP) {
		ACE_TRACE("modEmulateDelay_Interface::set_jitter_seconds");
		double jitter = paramList.getDouble(2);
		double startTime = (paramList.size() > 3 )? paramList.getDouble(3) : -1.0;

		modEmulateDelay *seg = dynamic_cast<modEmulateDelay*>(find_handler(paramList));
		double delay = seg->getDelaySeconds();

		seg->setDelayAndJitter(delay, jitter, startTime);

		*retvalP = xmlrpc_c::value_nil();
	}

	virtual void clear_timeline(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP) {
		ACE_TRACE("modEmulateDelay_Interface::clear_timeline");

		find_handler(paramList)->clearTimeLine();

		*retvalP = xmlrpc_c::value_nil();
	}

	GENERATE_BOOL_ACCESSORS(allow_jitter_reorder, setAllowJitterReorder, getAllowJitterReorder);
	GENERATE_INT_ACCESSORS(expected_kbits, setExpectedKbits, getExpectedKbits);
	GENERATE_BOOL_ACCESSORS(use_timestamp, setUseTimeStamp, getUseTimeStamp);
	GENERATE_FLOAT_ACCESSORS(timeline_cycle_secs, setTimeLineCycleSecs, getTimeLineCycleSecs);

	/// Export specified methods via XML-RPC.
	void register_methods(xmlrpc_c::registryPtr&);

protected:
	void get_settings_(xmlrpc_c::paramList const& paramList, modEmulateDelay* handler,
		xstruct& settings) {
		ACE_TRACE("modEmulateDelay_Interface::get_settings_");

		nasaCE::TrafficHandler_Interface<modEmulateDelay>::get_settings_(paramList, handler, settings);

		// Build a map of associated settings
		settings["delaySeconds"] = xmlrpc_c::value_double(handler->getDelaySeconds());
		settings["jitterSeconds"] = xmlrpc_c::value_double(handler->getJitterSeconds());
		settings["allowJitterReorder"] = xmlrpc_c::value_string(handler->getAllowJitterReorder()? "True" : "False");
		settings["expectedKbits"] = xmlrpc_c::value_int(handler->getExpectedKbits());
		settings["useTimeStamp"] = xmlrpc_c::value_string(handler->getUseTimeStamp()? "True" : "False");
		settings["timeLineCycleSecs"] = xmlrpc_c::value_double(handler->getTimeLineCycleSecs());
	}
};

modEmulateDelay_Interface* modEmulateDelay_InterfaceP;

TEMPLATE_METHOD_CLASSES(modEmulateDelay_Interface, modEmulateDelay_InterfaceP);

METHOD_CLASS(get_delay_seconds, "d:ssd",
	"Get the delay in seconds at the optionally specified time in seconds.", modEmulateDelay_InterfaceP);
METHOD_CLASS(set_delay_seconds, "n:ssdd",
	"Set the delay in seconds at the optionally specified time in seconds.", modEmulateDelay_InterfaceP);
METHOD_CLASS(get_jitter_seconds, "d:ssd",
	"Get the jitter in seconds at the optionally specified time in seconds.", modEmulateDelay_InterfaceP);
METHOD_CLASS(set_jitter_seconds, "n:ssdd",
	"Set the jitter in seconds at the optionally specified time in seconds.", modEmulateDelay_InterfaceP);
METHOD_CLASS(set_delay_and_jitter, "n:ssdd,n:ssA",
	"Set the delay and jitter in seconds at the optionally specified time in seconds.", modEmulateDelay_InterfaceP);
METHOD_CLASS(clear_timeline, "n:n",
	"Erase all values in the time line and stop using it (until more values are added).",
	modEmulateDelay_InterfaceP);

GENERATE_ACCESSOR_METHODS(allow_jitter_reorder, b, "whether variable delay can reorder units.",
	modEmulateDelay_InterfaceP);
GENERATE_ACCESSOR_METHODS(expected_kbits, i, "the anticipated throughput for the channel.",
	modEmulateDelay_InterfaceP);
GENERATE_ACCESSOR_METHODS(use_timestamp, b, "whether to use the timestamp on the unit or the current time to calculate delay.",
	modEmulateDelay_InterfaceP);
GENERATE_ACCESSOR_METHODS(timeline_cycle_secs, d, "the number of seconds before the time line begins to repeat.",
	modEmulateDelay_InterfaceP);

void modEmulateDelay_Interface::register_methods(xmlrpc_c::registryPtr& rpcRegistry) {
	ACE_TRACE("modEmulateDelay_Interface::register_methods");

	TEMPLATE_REGISTER_METHODS(modEmulateDelay, modEmulateDelay_Interface);
	REGISTER_METHOD(get_delay_seconds, "modEmulateDelay.getDelaySeconds");
	REGISTER_METHOD(set_delay_seconds, "modEmulateDelay.setDelaySeconds");
	REGISTER_METHOD(get_jitter_seconds, "modEmulateDelay.getJitterSeconds");
	REGISTER_METHOD(set_jitter_seconds, "modEmulateDelay.setJitterSeconds");
	REGISTER_METHOD(set_delay_and_jitter, "modEmulateDelay.setDelayAndJitter");
	REGISTER_METHOD(clear_timeline, "modEmulateDelay.clearTimeLine");
	REGISTER_ACCESSOR_METHODS(allow_jitter_reorder, modEmulateDelay, AllowJitterReorder);
	REGISTER_ACCESSOR_METHODS(expected_kbits, modEmulateDelay, ExpectedKbits);
	REGISTER_ACCESSOR_METHODS(use_timestamp, modEmulateDelay, UseTimeStamp);
	REGISTER_ACCESSOR_METHODS(timeline_cycle_secs, modEmulateDelay, TimeLineCycleSecs);

}

} // namespace nEmulateDelay