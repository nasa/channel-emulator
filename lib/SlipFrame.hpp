/* -*- C++ -*- */

//=============================================================================
/**
 * @file   SlipFrame.hpp
 * @author Tad Kollar  
 *
 * $Id: SlipFrame.hpp 1670 2012-08-28 13:25:28Z tkollar $ 
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef NASASLIPFRAME_HPP
#define NASASLIPFRAME_HPP

#include "NetworkData.hpp"

namespace nasaCE {

class SlipFrame: public NetworkData {

public:
	enum {	
		SLIP_End = 192,
		SLIP_End_Replace = 220,
		SLIP_Escape = 219,
		SLIP_Escape_Replace = 221
	};
	
	/// A short string describing the data type (usually just the name).
	std::string typeStr() const { return "SLIP Frame"; }
};

} // namespace nasaCE

#endif // NASASLIPFRAME_HPP