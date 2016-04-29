/* -*- C++ -*- */

//=============================================================================
/**
 * @file   AOS_MasterChannel_Service.cpp
 * @author Tad Kollar  
 *
 * $Id: AOS_MasterChannel_Service.cpp 2002 2013-05-31 18:03:06Z tkollar $
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "AOS_MasterChannel_Service.hpp"

namespace nasaCE {

int32_t AOS_MasterChannel_Service::TFVN = 01;

AOS_MasterChannel_Service::AOS_MasterChannel_Service(const std::string& newName,
	const std::string& newChannelName,
    const CE_DLL* newDLLPtr):
	AOS_PhysicalChannel_Service(newName, newChannelName, newDLLPtr),
	_SCID(CEcfg::instance()->getOrAddInt(cfgKey("SCID"), 0)),
	_badMCIDCount(0L)
{

}


}
