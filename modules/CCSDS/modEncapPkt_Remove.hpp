/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modEncapPkt_Remove.hpp
 * @author Tad Kollar  
 *
 * $Id: modEncapPkt_Remove.hpp 1523 2012-06-01 15:07:54Z tkollar $ 
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _MOD_ENCAP_PKT_REMOVE_HPP_
#define _MOD_ENCAP_PKT_REMOVE_HPP_

#include <BaseTrafficHandler.hpp>

namespace nEncapPkt_Remove {

//=============================================================================
/**
	@class modEncapPkt_Remove
	@author Tad Kollar  
	@brief Remove the Encapsulation Packet header and send the data on.
*/
//=============================================================================
class modEncapPkt_Remove: public BaseTrafficHandler {
public:

	/// Primary constructor.
	/// @param newName An identifier for the module that's unique within the channel.
	/// @param newChannelName An identifier for the containing channel.
	/// @param newDLLPtr Pointer to the DLL management structure.
    modEncapPkt_Remove(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr);

	/// Destructor.
    ~modEncapPkt_Remove();

	/// Reads in an Encapsulation Packet, removes the header, and sends it to the output handler.
	int svc();

	/// Write-only accessor to _ipeSupport.
	void setIpeSupport(bool newVal) { _ipeSupport = newVal; _ipeSupportSetting = newVal; }
	
	/// Read-only accessor to _ipeSupport.
	bool getIpeSupport() const { return _ipeSupport; }
	
private:
	/// Whether the Internet Protocol Extension is supported.
	bool _ipeSupport;
	
	/// Configuration file reference to _ipeSupport.
	Setting& _ipeSupportSetting;
}; // class modEncapPkt_Remove

} // namespace nEncapPkt_Remove

#endif // _MOD_ENCAP_PKT_REMOVE_HPP_
