/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modEmulateBitErrors_Interface.cpp
 * @author Tad Kollar  
 *
 * $Id: modEmulateBitErrors_Interface.cpp 1912 2013-03-18 15:49:23Z tkollar $
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "modEmulateBitErrors.hpp"
#include "TrafficHandler_Interface.hpp"

namespace nEmulateBitErrors {

//=============================================================================
/**
 * @class modEmulateBitErrors_Interface
 * @author Tad Kollar  
 * @brief Provide an XML-RPC interface to the Bit Error Emulation module.
 */
//=============================================================================
class modEmulateBitErrors_Interface: public nasaCE::TrafficHandler_Interface<modEmulateBitErrors> {
public:
	modEmulateBitErrors_Interface(xmlrpc_c::registryPtr& rpcRegistry):
		nasaCE::TrafficHandler_Interface<modEmulateBitErrors>(rpcRegistry, "modEmulateBitErrors") {
		ACE_TRACE("modEmulateBitErrors_Interface::modEmulateBitErrors_Interface");

		register_methods(rpcRegistry);
	}

	virtual void set_error_probability(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP) {
		ACE_TRACE("modEmulateBitErrors_Interface::set_error_probability");

		double startTime = -1.0;
		double ber = 0.0;
		int maxErr = 1;

		if ( paramList[2].type() != xmlrpc_c::value::TYPE_ARRAY ) {
			if (paramList.size() == 4 ) {
				ber = paramList.getDouble(3);
				maxErr = paramList.getInt(4);
			}
			else if ( paramList.size() == 3 ) {
				if ( paramList[2].type() == xmlrpc_c::value::TYPE_INT ) {
					maxErr = paramList.getInt(2);
				}
				else {
					ber = paramList.getDouble(2);
				}
			}
		}
		else {
			const std::vector<xmlrpc_c::value> arrayData = paramList.getArray(2);
			startTime = xmlrpc_c::value_double(arrayData[0]);
			ber = xmlrpc_c::value_double(arrayData[1]);
			maxErr = xmlrpc_c::value_int(arrayData[2]);
		}

		find_handler(paramList)->setErrorProbability(ber, maxErr, startTime);

		*retvalP = xmlrpc_c::value_nil();
	}

	virtual void get_error_probability(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP) {
		ACE_TRACE("modEmulateBitErrors_Interface::get_error_probability");
		double ber = 0.0;

		if (paramList.size() > 2 ) {
			ber = find_handler(paramList)->getErrorProbabilityAtTime(paramList.getDouble(2));
		}
		else {
			ber = find_handler(paramList)->getErrorProbability();
		}

		*retvalP = xmlrpc_c::value_double(ber);
	}

	virtual void set_max_errors_per_unit(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP) {
		ACE_TRACE("modEmulateBitErrors_Interface::set_max_errors_per_unit");
		double ber = 0.0;

		if (paramList.size() > 3 ) {
			ber = find_handler(paramList)->getErrorProbabilityAtTime(paramList.getDouble(3));
		}
		else {
			ber = find_handler(paramList)->getErrorProbability();
		}

		xmlrpc_c::paramList revisedParams;
		revisedParams.addx(paramList[0])
			.addx(paramList[1])
			.addx(xmlrpc_c::value_double(ber))
			.addx(paramList[2]);

		if (paramList.size() > 3 ) revisedParams.addx(paramList[3]);

		set_error_probability(revisedParams, retvalP);
	}

	virtual void get_max_errors_per_unit(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP) {
		ACE_TRACE("modEmulateBitErrors_Interface::get_max_errors_per_unit");
		int maxErr = 0;

		if (paramList.size() > 2 ) {
			maxErr = find_handler(paramList)->getMaxErrorsPerUnitAtTime(paramList.getDouble(2));
		}
		else {
			maxErr = find_handler(paramList)->getMaxErrorsPerUnit();
		}

		*retvalP = xmlrpc_c::value_int(maxErr);
	}

	virtual void clear_timeline(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP) {
		ACE_TRACE("modEmulateBitErrors_Interface::clear_timeline");

		find_handler(paramList)->clearTimeLine();

		*retvalP = xmlrpc_c::value_nil();
	}

	GENERATE_INT_ACCESSORS(units_with_errors, setUnitsWithErrors, getUnitsWithErrors);
	GENERATE_INT_ACCESSORS(protected_header_bits, setProtectedHeaderBits, getProtectedHeaderBits);
	GENERATE_INT_ACCESSORS(protected_trailer_bits, setProtectedTrailerBits, getProtectedTrailerBits);
	GENERATE_INT_ACCESSORS(total_errors, setTotalErrors, getTotalErrors);
	GENERATE_FLOAT_ACCESSORS(timeline_cycle_secs, setTimeLineCycleSecs, getTimeLineCycleSecs);

	/// Export specified methods via XML-RPC.
	void register_methods(xmlrpc_c::registryPtr&);

protected:
	void get_counters_(xmlrpc_c::paramList const& paramList, modEmulateBitErrors* handler,
		xstruct& counters) {
		ACE_TRACE("modEmulateBitErrors_Interface::get_counters_");

		nasaCE::TrafficHandler_Interface<modEmulateBitErrors>::get_counters_(paramList, handler, counters);

		// Build a map of all associated counters
		counters["unitsWithErrors"] = xmlrpc_c::value_int(handler->getUnitsWithErrors());
		counters["totalErrors"] = xmlrpc_c::value_int(handler->getTotalErrors());
	}

	void get_settings_(xmlrpc_c::paramList const& paramList, modEmulateBitErrors* handler,
		xstruct& settings) {
		ACE_TRACE("modEmulateBitErrors_Interface::get_settings_");

		nasaCE::TrafficHandler_Interface<modEmulateBitErrors>::get_settings_(paramList, handler, settings);

		// Build a map of associated settings
		settings["errorProbability"] = xmlrpc_c::value_double(handler->getErrorProbability());
		settings["maxErrorsPerUnit"] = xmlrpc_c::value_int(handler->getMaxErrorsPerUnit());
		settings["protectedHeaderBits"] = xmlrpc_c::value_int(handler->getProtectedHeaderBits());
		settings["protectedTrailerBits"] = xmlrpc_c::value_int(handler->getProtectedTrailerBits());
		settings["timeLineCycleSecs"] = xmlrpc_c::value_double(handler->getTimeLineCycleSecs());
	}
};

modEmulateBitErrors_Interface* modEmulateBitErrors_InterfaceP;

TEMPLATE_METHOD_CLASSES(modEmulateBitErrors_Interface, modEmulateBitErrors_InterfaceP);

METHOD_CLASS(set_error_probability, "n:ssdi,n:ssA",
	"Set the BER and max errors at the optionally specified time in seconds.", modEmulateBitErrors_InterfaceP);
METHOD_CLASS(get_error_probability, "d:ssd",
	"Get the bit error rate at the optionally specified time in seconds.", modEmulateBitErrors_InterfaceP);
METHOD_CLASS(get_max_errors_per_unit, "i:ssd",
	"Get max/errors unit at the optionally specified time in seconds.", modEmulateBitErrors_InterfaceP);
METHOD_CLASS(set_max_errors_per_unit, "n:ssid",
	"Set max/errors unit at the optionally specified time in seconds.", modEmulateBitErrors_InterfaceP);
METHOD_CLASS(clear_timeline, "n:n",
	"Erase all values in the time line and stop using it (until more values are added).",
	modEmulateBitErrors_InterfaceP);

GENERATE_ACCESSOR_METHODS(units_with_errors, i, "the tally of units that have at least one error.",
	modEmulateBitErrors_InterfaceP);
GENERATE_ACCESSOR_METHODS(protected_header_bits, i, "the number of bits at the start of the unit that will not have errors introduced.",
	modEmulateBitErrors_InterfaceP);
GENERATE_ACCESSOR_METHODS(protected_trailer_bits, i, "the number of bits at the end of the unit that will not have errors introduced.",
	modEmulateBitErrors_InterfaceP);
GENERATE_ACCESSOR_METHODS(total_errors, i, "the tally of all errors introduced, including multiple per unit.",
	modEmulateBitErrors_InterfaceP);
GENERATE_ACCESSOR_METHODS(timeline_cycle_secs, d, "the number of seconds before the time line begins to repeat.",
	modEmulateBitErrors_InterfaceP);

void modEmulateBitErrors_Interface::register_methods(xmlrpc_c::registryPtr& rpcRegistry) {
	ACE_TRACE("modEmulateBitErrors_Interface::register_methods");

	TEMPLATE_REGISTER_METHODS(modEmulateBitErrors, modEmulateBitErrors_Interface);
	REGISTER_ACCESSOR_METHODS(error_probability, modEmulateBitErrors, ErrorProbability);
	REGISTER_ACCESSOR_METHODS(max_errors_per_unit, modEmulateBitErrors, MaxErrorsPerUnit);
	REGISTER_METHOD(clear_timeline, "modEmulateBitErrors.clearTimeLine");
	REGISTER_ACCESSOR_METHODS(units_with_errors, modEmulateBitErrors, UnitsWithErrors);
	REGISTER_ACCESSOR_METHODS(protected_header_bits, modEmulateBitErrors, ProtectedHeaderBits);
	REGISTER_ACCESSOR_METHODS(protected_trailer_bits, modEmulateBitErrors, ProtectedTrailerBits);
	REGISTER_ACCESSOR_METHODS(total_errors, modEmulateBitErrors, TotalErrors);
	REGISTER_ACCESSOR_METHODS(timeline_cycle_secs, modEmulateBitErrors, TimeLineCycleSecs);
}

} // namespace nEmulateBitErrors