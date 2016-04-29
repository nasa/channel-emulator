/* -*- C++ -*- */

//=============================================================================
/**
 * @file   CE_DLL.hpp
 * @author Tad Kollar  
 *
 * $Id: CE_DLL.hpp 1898 2013-03-08 14:58:22Z tkollar $
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _CE_DLL_HPP_
#define _CE_DLL_HPP_

#include "CE_Config.hpp"

/// @namespace nasaCE
namespace nasaCE {

//=============================================================================
/**
 * @class CE_DLL
 * @brief Base class from which dynamically loadable libraries are derived.
 *
 * The methods of the subclasses provide any sort of useful functionality.
 */
//=============================================================================
class CE_DLL {


public:
	/// @brief Default constructor.
	/// @param name Uniquely identify the DLL.
	CE_DLL(std::string name = "None"): name_(name), _referenceCount(0) { }

	/// Default destructor.
	virtual ~CE_DLL() {
		ACE_TRACE("CE_DLL::~CE_DLL");
		if (_referenceCount)
			ND_WARNING("~CE_DLL(): DLL %s has a reference count of %d! Segfault likely.\n",
				name_.c_str(), _referenceCount);
	}

	/// @brief Gets the assigned DLL name.
	/// @return A std::string copy holding the DLL's name as assigned during
	/// the dynamic load.
	std::string getName() const { return name_; }

	/// @brief Share the appropriate methods with the global XML-RPC server.
	///
	/// Derived classes need to have another class defined for each exportable
	/// method, and register_methods must be defined to add them to the XML-RPC
	/// method registry.
	virtual void register_methods(xmlrpc_c::registryPtr&) = 0;

	/// Increase _referenceCount by one.
	void incReference() {
		++_referenceCount;
		ND_DEBUG("Added one reference to %s (total %d).\n", name_.c_str(), _referenceCount);

	}

	/// Decrease _referenceCount by one.
	void decReference() {
		--_referenceCount;
		ND_DEBUG("Removed one reference to %s (total %d).\n", name_.c_str(), _referenceCount);
	}

	/// Return the current value of _referenceCount.
	unsigned getReferenceCount() const { return _referenceCount; }

	virtual void addInstance(const std::string& segName, const std::string& channelName = "") { };

protected:
	/// @brief An identifier that the module's presence can be queried with.
	///
	/// This value serves as the access key in the DLL registry.
	std::string name_;

private:
	/// Keep track of how many modules are using this
	/// DLL to prevent premature deletion.
	unsigned _referenceCount;
};

/// @typedef activate_t
/// @brief The form of the class factory called when the DLL is loaded.
/// @param registryPtr A reference to the registry held by the XML-RPC server.
typedef CE_DLL* activate_t(xmlrpc_c::registryPtr&);

/// @typedef deactivate_t
/// @brief The form of the class factory called when the DLL is unloaded.
typedef void deactivate_t();

} // namespace nasaCE

#endif // _CE_DLL_HPP_
