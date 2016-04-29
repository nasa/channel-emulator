/* -*- C++ -*- */

//=============================================================================
/**
 * @file   TM_Interface_Macros.hpp
 * @author Tad Kollar  
 *
 * $Id: TM_Interface_Macros.hpp 2425 2013-12-17 17:17:47Z tkollar $
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _TM_INTERFACE_MACROS_HPP_
#define _TM_INTERFACE_MACROS_HPP_

#include "PeriodicHandler_Interface.hpp"

#define TM_PHYSICAL_CHANNEL_DEFINE_ACCESSORS()\
	GENERATE_INT_ACCESSORS(frame_size, setFrameSize, getFrameSize);\
	GENERATE_BOOL_ACCESSORS(use_frame_ec, setUseFrameErrorControl, getUseFrameErrorControl);\
	GENERATE_BOOL_ACCESSORS(drop_bad_frames, setDropBadFrames, getDropBadFrames);\
	GENERATE_I8_ACCESSORS(valid_frame_count, setValidFrameCount, getValidFrameCount);\
	GENERATE_I8_ACCESSORS(bad_frame_crc_count, setBadFrameChecksumCount, getBadFrameChecksumCount);\
	GENERATE_I8_ACCESSORS(bad_header_count, setBadHeaderCount, getBadHeaderCount);\
	GENERATE_I8_ACCESSORS(bad_length_count, setBadLengthCount, getBadLengthCount);\
	\
	void set_idle_pattern(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* retvalP) {\
		ACE_TRACE("modTM_*_Interface::set_idle_pattern");\
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
		ACE_TRACE("modTM_*_Interface::get_idle_pattern");\
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

#define GET_TM_PHYSICAL_CHANNEL_SETTINGS()\
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
	settings["useFrameErrorControl"] = xmlrpc_c::value_string(handler->getUseFrameErrorControl()? "True" : "False");\
	settings["dropBadFrames"] = xmlrpc_c::value_string(handler->getDropBadFrames()? "True" : "False");\

#define GET_TM_PHYSICAL_CHANNEL_COUNTERS()\
	counters["validFrameCount"] = xmlrpc_c::value_i8(handler->getValidFrameCount());\
	counters["badFrameChecksumCount"] = xmlrpc_c::value_i8(handler->getBadFrameChecksumCount());\
	counters["badHeaderCount"] = xmlrpc_c::value_i8(handler->getBadHeaderCount());\
	counters["badLengthCount"] = xmlrpc_c::value_i8(handler->getBadLengthCount());

#define TM_MASTER_CHANNEL_DEFINE_ACCESSORS()\
	GENERATE_INT_ACCESSORS(scid, setSCID, getSCID);\
	GENERATE_I8_ACCESSORS(bad_mcid_count, setBadMCIDCount, getBadMCIDCount);\
	GENERATE_I8_ACCESSORS(frame_count_misses, setFrameCountMisses, getFrameCountMisses);\
	GENERATE_BOOL_ACCESSORS(use_fsh, setUseFSH, getUseFSH);\
	GENERATE_INT_ACCESSORS(fsh_size, setFSHSize, getFSHSize);\
	GENERATE_BOOL_ACCESSORS(use_ocf, setUseOperationalControl, getUseOperationalControl);\
	GENERATE_BOOL_ACCESSORS(sync_flag, setSyncFlag, getSyncFlag);

#define GET_TM_MASTER_CHANNEL_SETTINGS()\
	settings["SCID"] = xmlrpc_c::value_int(handler->getSCID());\
	settings["useFSH"] = xmlrpc_c::value_string(handler->getUseFSH()? "True" : "False");\
	settings["FSHSize"] = xmlrpc_c::value_int(handler->getFSHSize());\
	settings["useOCF"] = xmlrpc_c::value_string(handler->getUseOperationalControl()? "True" : "False");\
	settings["syncFlag"] = xmlrpc_c::value_string(handler->getSyncFlag()? "True" : "False");

#define GET_TM_MASTER_CHANNEL_COUNTERS()\
	counters["badMCIDCount"] = xmlrpc_c::value_i8(handler->getBadMCIDCount());\
	counters["frameCountMisses"] = xmlrpc_c::value_i8(handler->getFrameCountMisses());

#define GET_TM_VIRTUAL_CHANNEL_SETTINGS()\
	settings["VCID"] = xmlrpc_c::value_int(handler->getVCID());

#define GET_TM_VIRTUAL_CHANNEL_COUNTERS()\
	counters["badVCIDCount"] = xmlrpc_c::value_i8(handler->getBadVCIDCount());

#define TM_VIRTUAL_CHANNEL_DEFINE_ACCESSORS()\
	GENERATE_INT_ACCESSORS(vcid, setVCID, getVCID);\
	GENERATE_I8_ACCESSORS(bad_vcid_count, setBadVCIDCount, getBadVCIDCount);

#define TM_PHYSICAL_CHANNEL_GENERATE_METHODS(module_ptr)\
	GENERATE_ACCESSOR_METHODS(frame_size, i, "the TM Transfer Frame size in octets.", module_ptr);\
	GENERATE_ACCESSOR_METHODS(use_frame_ec, b, "whether the Frame Error Control Field will be used.", module_ptr);\
	GENERATE_ACCESSOR_METHODS(drop_bad_frames, b, "whether invalid frames will be dropped or allowed through.", module_ptr);\
	GENERATE_ACCESSOR_METHODS(valid_frame_count, i, "the valid frame count for the service.", module_ptr);\
	GENERATE_ACCESSOR_METHODS(bad_frame_crc_count, i, "the tally of frames with failed CRC checks.", module_ptr);\
	GENERATE_ACCESSOR_METHODS(bad_header_count, i, "the tally of frames with uncorrectable header errors.", module_ptr);\
	GENERATE_ACCESSOR_METHODS(bad_length_count, i, "the tally of frames with incorrect lengths.", module_ptr);\
	METHOD_CLASS(set_idle_pattern, "n:A", "Sets a new pattern for the Idle Frame.", module_ptr);\
	METHOD_CLASS(get_idle_pattern, "A:n", "Returns the current pattern used for the Idle Frame.", module_ptr);

#define TM_MASTER_CHANNEL_GENERATE_METHODS(module_ptr)\
	GENERATE_ACCESSOR_METHODS(scid, i, "the Spacecraft Identifier.", module_ptr);\
	GENERATE_ACCESSOR_METHODS(bad_mcid_count, i, "the tally of frames received with incorrect MCIDs.", module_ptr);\
	GENERATE_ACCESSOR_METHODS(use_fsh, b, "whether the Frame Secondary Header Field will be used.", module_ptr);\
	GENERATE_ACCESSOR_METHODS(fsh_size, i, "the length of the Frame Secondary Header.", module_ptr);\
	GENERATE_ACCESSOR_METHODS(use_ocf, b, "whether the Operational Control Field will be used.", module_ptr);\
	GENERATE_ACCESSOR_METHODS(sync_flag, b, "whether the Synchronization Flag is to be set.", module_ptr);\
	GENERATE_ACCESSOR_METHODS(frame_count_misses, i, "the tally of mismatches between counted frames and the value stored by the current frame.", module_ptr);

#define TM_VIRTUAL_CHANNEL_GENERATE_METHODS(module_ptr)\
	GENERATE_ACCESSOR_METHODS(vcid, i, "the Virtual Channel Identifier.", module_ptr);\
	GENERATE_ACCESSOR_METHODS(bad_vcid_count, i, "the tally of frames received with incorrect VCIDs.", module_ptr);

#define TM_PHYSICAL_CHANNEL_REGISTER_METHODS(module_name) \
	REGISTER_ACCESSOR_METHODS(frame_size, module_name, FrameSize);\
	REGISTER_ACCESSOR_METHODS(use_frame_ec, module_name, UseFrameEC);\
	REGISTER_ACCESSOR_METHODS(drop_bad_frames, module_name, DropBadFrames);\
	REGISTER_ACCESSOR_METHODS(valid_frame_count, module_name, ValidFrameCount);\
	REGISTER_ACCESSOR_METHODS(bad_frame_crc_count, module_name, BadFrameCRCCount);\
	REGISTER_ACCESSOR_METHODS(bad_header_count, module_name, BadHeaderCount);\
	REGISTER_ACCESSOR_METHODS(bad_length_count, module_name, BadLengthCount);\
	REGISTER_ACCESSOR_METHODS(idle_pattern, module_name, IdlePattern);

#define TM_MASTER_CHANNEL_REGISTER_METHODS(module_name) \
	REGISTER_ACCESSOR_METHODS(scid, module_name, SCID);\
	REGISTER_ACCESSOR_METHODS(bad_mcid_count, module_name, BadMCIDCount);\
	REGISTER_ACCESSOR_METHODS(use_fsh, module_name, UseFSH);\
	REGISTER_ACCESSOR_METHODS(fsh_size, module_name, FSHSize);\
	REGISTER_ACCESSOR_METHODS(use_ocf, module_name, UseOperationalControlField);\
	REGISTER_ACCESSOR_METHODS(sync_flag, module_name, SyncFlag);\
	REGISTER_ACCESSOR_METHODS(frame_count_misses, module_name, FrameCountMisses);


#define TM_VIRTUAL_CHANNEL_REGISTER_METHODS(module_name) \
	REGISTER_ACCESSOR_METHODS(vcid, module_name, VCID);\
	REGISTER_ACCESSOR_METHODS(bad_vcid_count, module_name, BadVCIDCount);

#endif // _TM_INTERFACE_MACROS_HPP_