/* -*- C++ -*- */

//=============================================================================
/**
 * @file   TM_MasterChannel_Service.cpp
 * @author Tad Kollar  
 *
 * $Id: TM_MasterChannel_Service.cpp 2405 2013-12-12 19:56:04Z tkollar $
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "TM_MasterChannel_Service.hpp"

namespace nasaCE {
const int32_t TM_MasterChannel_Service::TFVN = 0;

TM_MasterChannel_Service::TM_MasterChannel_Service(const std::string& newName,
	const std::string& newChannelName,
    const CE_DLL* newDLLPtr):
	TM_PhysicalChannel_Service(newName, newChannelName, newDLLPtr),
	_SCID(0),
	_SCIDSetting(CEcfg::instance()->getOrAddInt(cfgKey("SCID"), -1)),
	_badMCIDCount(0),
	_useFSH(false),
	_useFSHSetting(CEcfg::instance()->getOrAddBool(cfgKey("useFSH"), _useFSH)),
	_fshSize(0),
	_fshSizeSetting(CEcfg::instance()->getOrAddInt(cfgKey("fshSize"), _fshSize)),
	_useOperationalControl(false),
	_useOperationalControlSetting(CEcfg::instance()->getOrAddBool(cfgKey("useOperationalControl"), _useOperationalControl)),
	_syncFlag(false),
	_syncFlagSetting(CEcfg::instance()->getOrAddBool(cfgKey("syncFlag"), _syncFlag)),
	_currentFrameNumber(0), _frameCountMisses(0)
{
	if ( static_cast<int>(_SCIDSetting) ) { _SCIDSetting = _SCID; _SCIDSetting.setFormat(Setting::FormatHex); }
	else _SCID = static_cast<int>(_SCIDSetting);

	_useFSH = _useFSHSetting;
	_fshSize = static_cast<int>(_fshSizeSetting);
	_useOperationalControl = _useOperationalControlSetting;
	_syncFlag = _syncFlagSetting;
}

}
