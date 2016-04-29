/* -*- C++ -*- */

//=============================================================================
/**
 * @file   TM_VirtualChannel_Service.cpp
 * @author Tad Kollar  
 *
 * $Id: TM_VirtualChannel_Service.cpp 1750 2012-10-25 15:56:05Z tkollar $
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "TM_VirtualChannel_Service.hpp"

namespace nasaCE {

TM_VirtualChannel_Service::TM_VirtualChannel_Service(const std::string& newName,
	const std::string& newChannelName,
    const CE_DLL* newDLLPtr):
	TM_MasterChannel_Service(newName, newChannelName, newDLLPtr),
	_VCID(0),
	_VCIDSetting(CEcfg::instance()->getOrAddInt(cfgKey("VCID"), -1)),
	_badVCIDCount(0L)
{
	if ( static_cast<int>(_VCIDSetting) == -1) {
		_VCIDSetting = _VCID;
		_VCIDSetting.setFormat(Setting::FormatHex);
	}
	else _VCID = static_cast<int>(_VCIDSetting);

}

}

