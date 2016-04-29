/* -*- C++ -*- */

//=============================================================================
/**
 * @file   AOS_Interface_Macros.hpp
 * @author Tad Kollar  
 *
 * $Id: AOS_Interface_Macros.hpp 2423 2013-12-17 14:11:40Z tkollar $
 * Copyright (c) 2009-2013.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _AOS_INTERFACE_MACROS_HPP_
#define _AOS_INTERFACE_MACROS_HPP_

#include "TrafficHandler_Interface.hpp"

#define AOS_PHYSICAL_CHANNEL_DEFINE_ACCESSORS()\
	GENERATE_INT_ACCESSORS(frame_size, setFrameSize, getFrameSize);\
	GENERATE_BOOL_ACCESSORS(use_header_ec, setUseHeaderErrorControl, getUseHeaderErrorControl);\
	GENERATE_BOOL_ACCESSORS(use_frame_ec, setUseFrameErrorControl, getUseFrameErrorControl);\
	GENERATE_INT_ACCESSORS(insert_zone_size, setInsertZoneSize, getInsertZoneSize);\
	GENERATE_BOOL_ACCESSORS(drop_bad_frames, setDropBadFrames, getDropBadFrames);\
	GENERATE_I8_ACCESSORS(valid_frame_count, setValidFrameCount, getValidFrameCount);\
	GENERATE_I8_ACCESSORS(bad_tfvn_count, setBadTFVNCount, getBadTFVNCount);\
	GENERATE_I8_ACCESSORS(bad_length_count, setBadLengthCount, getBadLengthCount);\
	\
	void set_idle_pattern(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP) {\
		ACE_TRACE("modAOS_*_Interface::set_idle_pattern");\
		const std::vector<xmlrpc_c::value> xml_rpc_pattern = paramList.getArray(2);\
		\
		std::vector<uint8_t> idlePattern;\
		\
		for (unsigned i = 0; i < xml_rpc_pattern.size(); i++ ) {\
			idlePattern[i] = xmlrpc_c::value_int(xml_rpc_pattern[i]);\
		}\
		\
		find_handler(paramList)->setIdlePattern(idlePattern);\
		\
		*retvalP = xmlrpc_c::value_nil();\
	}\
	\
	void get_idle_pattern(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP) {\
		ACE_TRACE("modAOS_*_Interface::get_idle_pattern");\
		\
		std::vector<uint8_t> idlePattern;\
		find_handler(paramList)->getIdlePattern(idlePattern);\
		\
		std::vector<xmlrpc_c::value> xml_rpc_pattern;\
		\
		for (unsigned i = 0; i < idlePattern.size(); i++ ) {\
			xml_rpc_pattern.push_back(xmlrpc_c::value_int(idlePattern[i]));\
		}\
		\
		*retvalP = xmlrpc_c::value_array(xml_rpc_pattern);\
	}

#define GET_AOS_PHYSICAL_CHANNEL_SETTINGS()\
	using namespace std;\
	vector<uint8_t> idlePattern;\
	handler->getIdlePattern(idlePattern);\
	vector<uint8_t>::iterator pos;\
	ostringstream os;\
	\
	for ( pos = idlePattern.begin(); pos != idlePattern.end(); ++pos ) \
		os << uppercase << hex << setw(2) << setfill('0') << static_cast<int>(*pos);\
	settings["idlePattern"] = xmlrpc_c::value_string(os.str());\
	settings["frameSize"] = xmlrpc_c::value_int(handler->getFrameSize());\
	settings["useHeaderErrorControl"] = xmlrpc_c::value_string(handler->getUseHeaderErrorControl()? "True" : "False");\
	settings["useFrameErrorControl"] = xmlrpc_c::value_string(handler->getUseFrameErrorControl()? "True" : "False");\
	settings["dropBadFrames"] = xmlrpc_c::value_string(handler->getDropBadFrames()? "True" : "False");\
	settings["insertZoneSize"] = xmlrpc_c::value_int(handler->getInsertZoneSize());

#define GET_AOS_PHYSICAL_CHANNEL_COUNTERS()\
	counters["validFrameCount"] = xmlrpc_c::value_i8(handler->getValidFrameCount());\
	counters["invalidTVFNCount"] = xmlrpc_c::value_i8(handler->getBadTFVNCount());\
	counters["badLengthCount"] = xmlrpc_c::value_i8(handler->getBadLengthCount());

#define AOS_MASTER_CHANNEL_DEFINE_ACCESSORS()\
	GENERATE_INT_ACCESSORS(scid, setSCID, getSCID);\
	GENERATE_I8_ACCESSORS(bad_mcid_count, setBadMCIDCount, getBadMCIDCount);

#define GET_AOS_MASTER_CHANNEL_SETTINGS()\
	settings["SCID"] = xmlrpc_c::value_int(handler->getSCID());

#define GET_AOS_MASTER_CHANNEL_COUNTERS()\
	counters["badMCIDCount"] = xmlrpc_c::value_i8(handler->getBadMCIDCount());

#define GET_AOS_VIRTUAL_CHANNEL_SETTINGS()\
	settings["useOperationalControl"] = xmlrpc_c::value_string(handler->getUseOperationalControl()? "True" : "False");\
	settings["VCID"] = xmlrpc_c::value_int(handler->getVCID());\
	settings["serviceType"] = xmlrpc_c::value_string(handler->getServiceTypeStr());\
	settings["useVCFrameCycle"] = xmlrpc_c::value_string(handler->getUseVCFrameCycle()? "True" : "False");

#define GET_AOS_VIRTUAL_CHANNEL_COUNTERS()\
	counters["badGVCIDCount"] = xmlrpc_c::value_i8(handler->getBadGVCIDCount());\
	counters["vcFrameCount"] = xmlrpc_c::value_i8(handler->getFrameCount());\
	counters["vcFrameCountCycle"] = xmlrpc_c::value_int(handler->getFrameCountCycle());\
	counters["frameCountMisses"] = xmlrpc_c::value_int(handler->getFrameCountMisses());

#define AOS_VIRTUAL_CHANNEL_DEFINE_ACCESSORS()\
	GENERATE_BOOL_ACCESSORS(use_ocf, setUseOperationalControl, getUseOperationalControl);\
	GENERATE_INT_ACCESSORS(vcid, setVCID, getVCID);\
	GENERATE_I8_ACCESSORS(vc_frame_count, setFrameCount, getFrameCount);\
	GENERATE_BOOL_ACCESSORS(use_vc_fc_cycle, setUseVCFrameCycle, getUseVCFrameCycle);\
	GENERATE_INT_ACCESSORS(frame_count_cycle, setFrameCountCycle, getFrameCountCycle);\
	GENERATE_STRING_ACCESSORS(service_type, setServiceType, getServiceTypeStr);\
	GENERATE_I8_ACCESSORS(bad_gvcid_count, setBadGVCIDCount, getBadGVCIDCount);\
	GENERATE_I8_ACCESSORS(frame_count_misses, setFrameCountMisses, getFrameCountMisses);

#define AOS_PHYSICAL_CHANNEL_GENERATE_METHODS(module_ptr)\
	GENERATE_ACCESSOR_METHODS(frame_size, i, "the AOS Transfer Frame size in octets.", module_ptr);\
	GENERATE_ACCESSOR_METHODS(use_header_ec, b, "whether the Frame Header Error Control Field will be used.", module_ptr);\
	GENERATE_ACCESSOR_METHODS(use_frame_ec, b, "whether the Frame Error Control Field will be used.", module_ptr);\
	GENERATE_ACCESSOR_METHODS(insert_zone_size, i, "the size of the Insert Zone in octets.", module_ptr);\
	GENERATE_ACCESSOR_METHODS(drop_bad_frames, b, "whether invalid frames will be dropped or allowed through.", module_ptr);\
	GENERATE_ACCESSOR_METHODS(valid_frame_count, i, "the valid frame count for the service.", module_ptr);\
	GENERATE_ACCESSOR_METHODS(bad_tfvn_count, i, "the tally of frames with incorrect Transfer Frame version numbers.", module_ptr);\
	GENERATE_ACCESSOR_METHODS(bad_length_count, i, "the tally of frames with incorrect lengths.", module_ptr);\
	METHOD_CLASS(set_idle_pattern, "n:A", "Sets a new pattern for the Idle Frame.", module_ptr);\
	METHOD_CLASS(get_idle_pattern, "A:n", "Returns the current pattern used for the Idle Frame.", module_ptr);

#define AOS_MASTER_CHANNEL_GENERATE_METHODS(module_ptr)\
	GENERATE_ACCESSOR_METHODS(scid, i, "the Spacecraft Identifier.", module_ptr);\
	GENERATE_ACCESSOR_METHODS(bad_mcid_count, i, "the tally of frames received with incorrect MCIDs.", module_ptr);

#define AOS_VIRTUAL_CHANNEL_GENERATE_METHODS(module_ptr)\
	GENERATE_ACCESSOR_METHODS(use_ocf, b, "whether the Operational Control Field is in use.", module_ptr);\
	GENERATE_ACCESSOR_METHODS(vcid, i, "the Virtual Channel Identifier.", module_ptr);\
	GENERATE_ACCESSOR_METHODS(vc_frame_count, i, "the current VC frame number.", module_ptr);\
	GENERATE_ACCESSOR_METHODS(use_vc_fc_cycle, b, "whether VC Frame Count Cycle field is in use.", module_ptr);\
	GENERATE_ACCESSOR_METHODS(frame_count_cycle, i, "the Virtual Channel Frame Count Cycle.", module_ptr);\
	GENERATE_ACCESSOR_METHODS(service_type, s, "the type of packet carried by the frame.", module_ptr);\
	GENERATE_ACCESSOR_METHODS(bad_gvcid_count, i, "the tally of frames received with incorrect GVCIDs.", module_ptr);\
	GENERATE_ACCESSOR_METHODS(frame_count_misses, i, "the tally of mismatches between counted frames and the value stored by the current frame.", module_ptr);


#define AOS_PHYSICAL_CHANNEL_REGISTER_METHODS(module_name) \
	REGISTER_ACCESSOR_METHODS(frame_size, module_name, FrameSize);\
	REGISTER_ACCESSOR_METHODS(use_header_ec, module_name, UseHeaderEC);\
	REGISTER_ACCESSOR_METHODS(use_frame_ec, module_name, UseFrameEC);\
	REGISTER_ACCESSOR_METHODS(insert_zone_size, module_name, InsertZoneSize);\
	REGISTER_ACCESSOR_METHODS(drop_bad_frames, module_name, DropBadFrames);\
	REGISTER_ACCESSOR_METHODS(valid_frame_count, module_name, ValidFrameCount);\
	REGISTER_ACCESSOR_METHODS(bad_length_count, module_name, BadLengthCount);\
	REGISTER_ACCESSOR_METHODS(bad_tfvn_count, module_name, BadTFVNCount);\
	REGISTER_ACCESSOR_METHODS(idle_pattern, module_name, IdlePattern);

#define AOS_MASTER_CHANNEL_REGISTER_METHODS(module_name) \
	REGISTER_ACCESSOR_METHODS(scid, module_name, SCID);\
	REGISTER_ACCESSOR_METHODS(bad_mcid_count, module_name, BadMCIDCount);

#define AOS_VIRTUAL_CHANNEL_REGISTER_METHODS(module_name) \
	REGISTER_ACCESSOR_METHODS(use_ocf, module_name, UseOperationalControlField);\
	REGISTER_ACCESSOR_METHODS(vcid, module_name, VCID);\
	REGISTER_ACCESSOR_METHODS(vc_frame_count, module_name, VCFrameCount);\
	REGISTER_ACCESSOR_METHODS(use_vc_fc_cycle, module_name, UseVCFrameCountCycle);\
	REGISTER_ACCESSOR_METHODS(frame_count_cycle, module_name, FrameCountCycle);\
	REGISTER_ACCESSOR_METHODS(service_type, module_name, ServiceType);\
	REGISTER_ACCESSOR_METHODS(bad_gvcid_count, module_name, BadGVCIDCount);\
	REGISTER_ACCESSOR_METHODS(frame_count_misses, module_name, FrameCountMisses);

#endif // _AOS_INTERFACE_MACROS_HPP_