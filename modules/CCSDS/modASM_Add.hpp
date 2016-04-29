/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modASM_Add.hpp
 * @author Tad Kollar  
 *
 * $Id: modASM_Add.hpp 1931 2013-04-02 18:19:22Z tkollar $
 * Copyright (c) 2010.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _MOD_ASM_ADD_HPP_
#define _MOD_ASM_ADD_HPP_

#include <BaseTrafficHandler.hpp>

namespace nASM_Add {

//=============================================================================
/**
	@class modASM_Add
	@author Tad Kollar  
	@brief Add ASM markers to frames.
*/
//=============================================================================
class modASM_Add: public BaseTrafficHandler {
public:
	/// Primary constructor.
	/// @param newName An identifier for the module that's unique within the channel.
	/// @param newChannelName An identifier for the containing channel.
	/// @param newDLLPtr Pointer to the DLL management structure.
    modASM_Add(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr);

	/// Destructor.
    ~modASM_Add();

	/// Reads in some data and looks for sync markers.
	int svc();

	/// Set the contents of _marker.
	/// @param asmBuf The buffer holding the new marker.
	/// @param bufLen The length of the pattern in the buffer.
	void setMarker(const uint8_t* asmBuf, const int bufLen);

	/// Set the contents of _marker.
	/// @param newMarker A vector containing the new pattern.
	void setMarker(const std::vector<uint8_t>& newMarker);

	/// Return the contents of _marker.
	/// @param marker A vector to copy _marker into.
	void getMarker(std::vector<uint8_t>& marker) const;

	/// Write-only accessor to _asmCount.
	void setAsmCount(const uint64_t& newVal) {
		_asmCount = newVal;
	}

	/// Read-only accessor to _asmCount.
	uint64_t getAsmCount() const {
		return _asmCount;
	}

private:
	/// The type of ASM marker.
	Setting& _marker;

	/// Whether _marker has been updated or the ASM buffer otherwise needs to be (re)created.
	bool _rebuildMarker;

	/// Create a buffer with the contents of the new marker.
	/// @param buffer Deleted if not null. Then a new buffer is allocated and filled.
	/// @param bufferLen Receives the new marker length.
	void _updateMarker(ACE_UINT8*& buffer, size_t& bufferLen);

	/// The grand total of ASMs that were added.
	uint64_t _asmCount;

}; // class modASM_Add

} // namespace nASM_Add

#endif // _MOD_ASM_ADD_HPP_
