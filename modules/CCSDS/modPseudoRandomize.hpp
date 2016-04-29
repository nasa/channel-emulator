/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modPseudoRandomize.hpp
 * @author Tad Kollar  
 *
 * $Id: modPseudoRandomize.hpp 2322 2013-08-07 16:14:42Z tkollar $
 * Copyright (c) 2008-2013.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _MOD_PSEUDO_RANDOMIZE_HPP_
#define _MOD_PSEUDO_RANDOMIZE_HPP_

#include <BaseTrafficHandler.hpp>

namespace nPseudoRandomize {

//=============================================================================
/**
	@class modPseudoRandomize
	@author Tad Kollar  
	@brief XOR incoming units w/sequence in CCSDS TM Sync/Channel Coding Blue Book.
*/
//=============================================================================
class modPseudoRandomize: public BaseTrafficHandler {
public:
	static const int seqBufLen;

	/// Primary constructor.
	/// @param newName An identifier for the module that's unique within the channel.
	/// @param newChannelName An identifier for the containing channel.
	/// @param newDLLPtr Pointer to the DLL management structure.
	modPseudoRandomize(const std::string& newName, const std::string& newChannelName,
		const CE_DLL* newDLLPtr);

	/// Destructor.
	~modPseudoRandomize();

	/// Read in a data unit, perform the XOR, then send it via the primary output.
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
	modPseudoRandomize();

	/// The length of the buffer to generate the repeating bit sequence into.
	uint8_t* _seqBuf;

}; // class modPseudoRandomize

} // namespace nPseudoRandomize

#endif // _MOD_PSEUDO_RANDOMIZE_HPP_
