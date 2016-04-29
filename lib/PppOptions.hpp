/* -*- C++ -*- */

//=============================================================================
/**
 * @file   PppOptions.hpp
 * @author Tad Kollar  
 *
 * $Id: PppOptions.hpp 1670 2012-08-28 13:25:28Z tkollar $ 
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _PPP_OPTIONS_HPP_
#define _PPP_OPTIONS_HPP_

#include "PppFrame.hpp"
#include <bitset>

namespace nasaCE {

struct PppOptions {
	/// Async-Control-Character-Maps
	std::bitset<256> sending_accm; 
	std::bitset<32> receiving_accm;
	
	PppOptions(): sending_accm(0xFFFFFFFF), receiving_accm(0xFFFFFFFF) {
		sending_accm[PppFrame::escSeq] = true;
		sending_accm[PppFrame::flagSeq] = true;
		receiving_accm[PppFrame::escSeq] = true;
		receiving_accm[PppFrame::flagSeq] = true;	
	}	
};

} // namespace nasaCE

#endif // _PPP_OPTIONS_HPP_
