/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modExtractor.hpp
 * @author Tad Kollar  
 *
 * $Id: modExtractor.hpp 1469 2012-05-03 16:21:39Z tkollar $ 
 * Copyright (c) 2010.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _MOD_EXTRACTOR_HPP_
#define _MOD_EXTRACTOR_HPP_

#include <BaseTrafficHandler.hpp>

namespace nExtractor {

//=============================================================================
/**
 * @class modExtractor
 * @brief Remove any header/trailer of incoming traffic and send the remainder.
 */
//=============================================================================
class modExtractor : public BaseTrafficHandler {
public:
	/// Primary constructor.
	/// @param newName An identifier for the module that's unique within the channel.
	/// @param newChannelName An identifier for the containing channel.
	/// @param newDLLPtr Pointer to the DLL management structure.
    modExtractor(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr);

	/// Destructor.
    ~modExtractor();

	/// Reads in data, extracts a portion, and sends it to the output handler.
	int svc();

	/// Write-only accessor to _headerLength.
	void setHeaderLength(const int& newVal) {	_headerLength = newVal;	}
	
	/// Read-only accessor to _headerLength.
	int getHeaderLength() const { return _headerLength; }

	/// Write-only accessor to _trailerLength.
	void setTrailerLength(const int& newVal) { _trailerLength = newVal; }
	
	/// Read-only accessor to _trailerLength.
	int getTrailerLength() const { return _trailerLength; }

	/// Write-only accessor to _headerOctetCount.
	void setHeaderOctetCount(const int& newVal) { _headerOctetCount = newVal; }
	
	/// Read-only accessor to _headerOctetCount.
	int getHeaderOctetCount() const { return _headerOctetCount; }

	/// Increase _headerOctetCount by the specified amount.
	int incHeaderOctetCount(const int& step = 1) const { 
		return ( _headerOctetCount = getHeaderOctetCount() + step ) ; }

	/// Write-only accessor to _trailerOctetCount.
	void setTrailerOctetCount(const int& newVal) { _trailerOctetCount = newVal; }
	
	/// Read-only accessor to _trailerOctetCount.
	int getTrailerOctetCount() const { return _trailerOctetCount; }

	/// Increase _trailerOctetCount by the specified amount.
	int incTrailerOctetCount(const int& step = 1) const { 
		return ( _trailerOctetCount = getTrailerOctetCount() + step ) ; }
	
	/// Write-only accessor to _stubCount.
	void setStubCount(const int& newVal) { _stubCount = newVal;	}
	
	/// Read-only accessor to _stubCount.
	int getStubCount() const { return _stubCount; }

	/// Increase _stubCount by the specified amount.
	int incStubCount(const int& step = 1) const {
		return ( _stubCount = getStubCount() + step ) ; }
		
private:
	Setting& _headerLength; /// Number of octets to remove from the front.
	
	Setting& _trailerLength; /// Number of octets to remove from the end.
	
	Setting& _headerOctetCount; /// Octet tally of headers extracted from all traffic received.
	
	Setting& _trailerOctetCount; /// Octet tally of trailers extracted from all traffic received.
	
	Setting& _stubCount; /// The number of units that were too short to extract.
	
}; // class modExtractor

} // namespace nExtractor

#endif // _MOD_EXTRACTOR_HPP_
