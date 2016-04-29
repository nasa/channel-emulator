/* -*- C++ -*- */

//=============================================================================
/**
 * @file   PeriodicLink.hpp
 * @author Tad Kollar  
 *
 * $Id: PeriodicLink.hpp 1670 2012-08-28 13:25:28Z tkollar $ 
 * Copyright (c) 2010.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _NASA_PERIODIC_LINK_HPP_
#define _NASA_PERIODIC_LINK_HPP_

#include "HandlerLink.hpp"
#include "PeriodicTransmitter.hpp"

namespace nasaCE {

//=============================================================================
/**
	@author Tad Kollar  
*/
//=============================================================================
class PeriodicLink : public HandlerLink
{
public:
	/// Preferred constructor, calls _connectSourceToTarget() automatically.
	PeriodicLink( BaseTrafficHandler* source,
		BaseTrafficHandler* target,
		const ACE_Time_Value& interval = ACE_Time_Value::zero,
		const OutputRank sourceOutputRank = PrimaryOutput,
		const InputRank targetInputRank = PrimaryInput,
		const bool openTransmitter = true );
		
	/// Destructor.
	~PeriodicLink();

	/// Queue the data to be sent at a constant rate, which is then passed
	/// on to HandlerLink::send.
	/// @param data The information to pass from the source to the target.
	/// @throw DisconnectedLink If the connection has not been initialized.
	virtual int send(NetworkData* data);

	/// Read-only accessor to _periodicTX.	
	PeriodicTransmitter* getPeriodicTX() const { return _periodicTX; }
	
private:
	
	/// Used to be send data at a constant rate.
	PeriodicTransmitter* _periodicTX;
};

} // namespace nasaCE

#endif // _NASA_PERIODIC_LINK_HPP_
