/* -*- C++ -*- */

//=============================================================================
/**
 * @file   nd_error.hpp
 * @author Tad Kollar  
 *
 * $Id: nd_error.hpp 1693 2012-09-13 18:20:38Z tkollar $ 
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _ND_ERROR_HPP_
#define _ND_ERROR_HPP_

#include "nd_macros.hpp"
#include "ace/SString.h"
#include <stdexcept>
#include <string>
#include <sstream>
#include <iomanip>

namespace nasaCE {

/**
 *	@class nd_error
 *
 *  @brief A sublass of std::runtime_error that accepts different string types.
 */
class nd_error: public std::runtime_error {
protected:
	std::string type_;
	
public:
	/// @param msg The error message as a String reference.
	explicit nd_error(const std::string& msg, const std::string& etype = "nd_error"):
		std::runtime_error(msg.c_str()), type_(etype) {
		// ND_WARNING("%s thrown: %s\n", type_.c_str(), msg.c_str());
	}

	/// @param msg The error message as an ACE_TString reference.
	explicit nd_error(const ACE_TString& msg, const std::string& etype = "nd_error"):
		std::runtime_error(msg.c_str()), type_(etype) {
		// ND_WARNING("%s thrown: %s\n", type_.c_str(), msg.c_str());
	}
	
	/// @param msg The error message as char pointer.
	explicit nd_error(const char* msg, const std::string& etype = "nd_error"):
		std::runtime_error(msg), type_(etype) {
		// ND_WARNING("%s thrown: %s\n", type_.c_str(), msg);
	}
	
	static std::string article(const std::string& strToTest, bool lowerCase = true) {
		const std::string a = (lowerCase)? "a " : "A ";
		const std::string an = (lowerCase)? "an " : "An ";
		std::string art(a);
		
		if ( strToTest.find_first_of("aeiouAEIOU")) art = an;
		
		return art;
	}
	
	virtual ~nd_error() throw() {}
};

struct ValueTooSmall: public nd_error {
	ValueTooSmall(const std::string& e): nd_error(e, "ValueTooSmall") { }
	
	static std::string msg(const std::string& field, const ACE_UINT64 given, const ACE_UINT64 limit) {
		std::ostringstream os;
	
		os << field << " specified as " << given << "; minimum is " << limit << ".";
			
		return os.str();
	}
};

struct ValueTooLarge: public nd_error {
	ValueTooLarge(const std::string& e): nd_error(e, "ValueTooLarge") { }
	
	static std::string msg(const std::string& field, const ACE_UINT64 given, const ACE_UINT64 limit) {
		std::ostringstream os;
	
		os << field << " specified as " << given << "; limit is " << limit << ".";
			
		return os.str();
	}
};

struct ValueOutOfRange: public nd_error {
	ValueOutOfRange(const std::string& e): nd_error(e, "ValueOutOfRange") { }
	
	static std::string msg(const std::string& field, const ACE_UINT64 given,
		const ACE_UINT64 lower_limit, const ACE_UINT64 upper_limit) {
		std::ostringstream os;

		os      << field << " specified as " << given << "; allowed range is "
				<< lower_limit << " - " << upper_limit << ".";
				
		return os.str();
	}
};

struct BadValue: public nd_error {
	BadValue(const std::string& e): nd_error(e, "BadValue") { }
	
	static std::string msg(const std::string& field, const ACE_UINT64 given) {
		std::ostringstream os;
	
		os << field << " specified as 0x" << std::hex << given << ", which is not an allowed value.";
			
		return os.str();
	}
	
	static std::string msg(const std::string& field, const std::string& given) {
		std::ostringstream os;
	
		os << field << " specified as '" << given << "', which is not an allowed value.";
			
		return os.str();
	}
};

struct BufferOverflow: public nd_error {
	BufferOverflow(const std::string& e): nd_error(e, "BufferOverflow") { }
	
	static std::string msg(const std::string& field, const ACE_UINT64 given_length,
		const ACE_UINT64 length_limit) {
		std::ostringstream os;
	
		os << "Source buffer of length " << given_length
			<< " cannot fit into target buffer (" << field << ") of length " << length_limit << ".";
			
		return os.str();
	}
};

struct MissingField: public nd_error {
	MissingField(const std::string& e): nd_error(e, "MissingField") { }
	
	static std::string msg(const std::string& data_type, const std::string& field) {
		std::ostringstream os;
		
		os << "Access to the " << field << " field attempted in " << article(data_type) << data_type
			<< " initialized without one.";
			
		return os.str();
	}
};

struct OutOfMemory: public nd_error {
	OutOfMemory(const std::string& e): nd_error(e, "OutOfMemory") { }
	
	static std::string msg(const int octets, const std::string& sysMsg) {
		std::ostringstream os;
		
		os << "Allocation of " << octets << " failed: " << sysMsg << ".";
			
		return os.str();
	}
};

struct BufferRangeError: public nd_error {
	BufferRangeError(const std::string& e): nd_error(e, "BufferRangeError") { }
	
	static std::string msg(const std::string& msg, const int& attemptedIndex, const int& max) {
		std::ostringstream os;
		
		os << msg << ": attempt to access beyond the end of buffer: index " << attemptedIndex
			<< " attempted with max of " << max << ".";
			
		return os.str();
	}
};

struct LogicError: public nd_error {
	LogicError(const std::string& e): nd_error(e, "LogicError") { }
};

struct DoesNotExist: public nd_error {
	DoesNotExist(const std::string& e): nd_error(e, "DoesNotExist") { }
	
	static std::string msg(const std::string& rsType, const std::string& rsName) {
		std::ostringstream os;
		
		os << article(rsType) << rsType << " named " << rsName << " does not exist.";
			
		return os.str();
	}
};

struct AlreadyExists: public nd_error {
	AlreadyExists(const std::string& e): nd_error(e, "AlreadyExists") { }
	
	static std::string msg(const std::string& rsType, const std::string& rsName) {
		std::ostringstream os;
		
		os << article(rsType) << rsType << " named " << rsName << " already exists; cannot create.";
			
		return os.str();
	}
};

} // namespace nasaCE

#endif // _ND_ERROR_HPP_