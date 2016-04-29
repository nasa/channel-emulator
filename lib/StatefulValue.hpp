/* -*- C++ -*- */

//=============================================================================
/**
 * @file   StatefulValue.hpp
 * @author Tad Kollar  
 *
 * $Id: StatefulValue.hpp 1670 2012-08-28 13:25:28Z tkollar $ 
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _STATEFUL_VALUE_HPP_
#define _STATEFUL_VALUE_HPP_

#include <ace/OS.h>
#include <ace/Time_Value.h>

namespace nasaCE {

//=============================================================================
/**
 * @class StatefulValue
 * @brief Maintain some additional information about a single value.
 *
 * This template class knows whether the value it contains has ever been set
 * or changed from its initial setting. It's intended for use in configuration
 * settings, to make it easier to manage whether a value has been set by the
 * system, the user, or at all.
 */
//=============================================================================
template<class T> 
class StatefulValue {
public:
	/// Default constructor. Makes _isSet and _hasChanged both false.
	StatefulValue(): _isSet(false), _hasChanged(false) {
		_updateChangeTime();
	}
	
	/// Construct by copying newVal to _value, setting _isSet true and _hasChanged false.
	StatefulValue(const T& newVal): _value(newVal), _isSet(true), _hasChanged(false) {
		_updateChangeTime();
	}
	
	/// Const copy constructor.
	StatefulValue(const StatefulValue<T>& other): _value(other._value), _isSet(true), _hasChanged(false) {
		_updateChangeTime();
	}	
	
	/// Destructor.
	virtual ~StatefulValue() { }
	
	/// Set _value and make _isSet and _hasChanged true.
	/// @param newVal The data to modify _value with.
	/// @return Reference to the newly set value.
	T& set(const T& newVal) {
		_updateChangeTime();
		_isSet = true;
		_hasChanged = true;
		return (_value = newVal);
	}
	
	/// Read-only accessor to _value.
	T& get() const { return _value; }
	
	/// Shorthand way of calling set().
	/// @param newVal The data to modify _value with.
	/// @return Reference to the newly set value.
	T& operator=(const T& newVal) { return set(newVal); }
	
	/// Shorthand way of calling set().
	/// @param other Another StatefulValue object containing the data to copy.
	/// @return Reference to the newly set value.
	T& operator=(const StatefulValue<T>& other) { return set(other.get()); }
	
	/// Compare otherVal to _value and return equality.
	/// @param otherVal A reference to the value to compare to.
	/// @return Whether the values are equal.
	bool operator==(const T& otherVal) { return ( _value == otherVal ); }
	
	/// Compare the value contained in other to _value and return equality.
	/// @param other Another StatefulValue object containing the data to compare.
	/// @return Whether the values are equal.
	bool operator==(const StatefulValue<T>& other) { return ( _value == other.get()); }
	
	/// Read-only accessor to _isSet.
	bool isSet() const { return _isSet; }
	
	/// Read-only accessor to _hasChanged.
	bool hasChanged() const { return _hasChanged; }
	
	/// Return true if other is changed more recently.
	bool isNewer(const StatefulValue<T>& other) {
		return (other._changeTime > _changeTime);
	}
	
	/// Set only if the other value has been set.
	T& copyIfSet(const StatefulValue<T>& other) {
		if (other.isSet()) set(other._value);
		return _value;
	}
	
	/// Set only if the other value has been set and is more recent.	
	T& copyIfSetAndNewer(const StatefulValue<T>& other) {
		if (other.isSet() && isNewer(other) ) return set(other._value);
		return _value;
	}
	
	/// Set only if the other value has been modified after construction.	
	T& copyIfChanged(const StatefulValue<T>& other) {
		if (other.hasChanged()) set(other._value);
		return _value;
	}
	
	/// Set only if the other value has been modified after construction and is more recent.	
	T& copyIfChangedAndNewer(const StatefulValue<T>& other) {
		if (other.hasChanged() && isNewer(other) ) set(other._value);
		return _value;
	}
	
	operator T () { return _value; }

private:
	/// The actual data we're storing.
	T _value;
	
	/// Whether _value has been set at any point, either during construction or later.
	bool _isSet;
	
	/// True if _value has been modified after construction.
	bool _hasChanged;
	
	/// When the object was constructed or when _value was last modified.
	ACE_Time_Value _changeTime;
	
	/// Set _changeTime to the result of gettimeofday().
	void _updateChangeTime() {
		_changeTime = ACE_OS::gettimeofday();
	}
};

} // namespace nasaCE

#endif // _STATEFUL_VALUE_HPP_