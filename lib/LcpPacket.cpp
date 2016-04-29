/* -*- C++ -*- */

//=============================================================================
/**
 * @file   LcpPacket.cpp
 * @author Tad Kollar  
 *
 * $Id: LcpPacket.cpp 1670 2012-08-28 13:25:28Z tkollar $ 
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "LcpPacket.hpp"
#include <string>

using namespace nasaCE;

std::string LcpPacket::optTypeStr[OptionTypesCount] = {
	"Vendor",
	"MRU",
	"AsyncMap",
	"AuthType",
	"Quality",
	"MagicNumber",
	"Undef6",
	"PCompression",
	"ACCompression",
	"FCSAltern",
	"SDP",
	"Numbered",
	"Undef12",
	"Callback",
	"Undef14",
	"Undef15",
	"Undef16",
	"MRRU",
	"SSNHF",
	"EPDisc",
	"Undef20",
	"Undef21",
	"MPPlus",
	"LDisc",
	"LCPAuth",
	"COBS",
	"PrefElis",
	"MPHdrFmt",
	"I18n",
	"SDL"
};