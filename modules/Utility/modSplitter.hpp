/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modSplitter.hpp
 * @author Tad Kollar  
 *
 * $Id: modSplitter.hpp 1469 2012-05-03 16:21:39Z tkollar $ 
 * Copyright (c) 2010.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _MOD_SPLITTER_HPP_
#define _MOD_SPLITTER_HPP_

#include <BaseTrafficHandler.hpp>

namespace nSplitter {

//=============================================================================
/**
	@class modSplitter
	@author Tad Kollar  
	@brief Send a copy of incoming data to an auxiliary output as well as primary.
*/
//=============================================================================
class modSplitter: public BaseTrafficHandler {
public:
	/// Primary constructor.
	/// @param newName An identifier for the module that's unique within the channel.
	/// @param newChannelName An identifier for the containing channel.
	/// @param newDLLPtr Pointer to the DLL management structure.
    modSplitter(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr);

	/// Destructor.
    ~modSplitter();

	/// Read in a data unit and send a copy out both the primary and auxiliary outputs.
	int svc();
	
	/// Return the MRU of the primary output if it exists.
	/// Otherwise, return the local MRU setting, which is otherwise meaningless.
	size_t getMRU() const { 
		if ( links_[PrimaryOutputLink] ) return	links_[PrimaryOutputLink]->getTarget()->getMRU();
		else return BaseTrafficHandler::getMRU();
	}
	
	/// Return the MTU of the primary input if it exists.
	/// Otherwise, return the local MTU setting, which is otherwise meaningless.
	size_t getMTU() const { 
		if ( links_[PrimaryInputLink] ) return links_[PrimaryInputLink]->getSource()->getMTU();
		else return BaseTrafficHandler::getMTU();
	}	

private:
	/// Disable default constructor.
	modSplitter();
	
}; // class modSplitter

} // namespace nSplitter

#endif // _MOD_SPLITTER_HPP_
