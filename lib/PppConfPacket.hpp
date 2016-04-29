/* -*- C++ -*- */

//=============================================================================
/**
 * @file   PppConfPacket.hpp
 * @author Tad Kollar  
 *
 * $Id: PppConfPacket.hpp 1670 2012-08-28 13:25:28Z tkollar $ 
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef NASAPPPCONFPACKET_HPP
#define NASAPPPCONFPACKET_HPP

#include "NetworkData.hpp"
#include "PppOptions.hpp"
#include <ace/OS_main.h>
#include <vector>

/*
Debugging log of PPP initialization from gx24 to ce0:

**** TEST 1 ****
$pppd call ce0
using channel 142
Using interface ppp0
Connect: ppp0 <--> /dev/ttyPS0
sent [LCP ConfReq id=0x1 <asyncmap 0x1>]
rcvd [LCP ConfReq id=0x1 <asyncmap 0x1>]
sent [LCP ConfAck id=0x1 <asyncmap 0x1>]
rcvd [LCP ConfReq id=0x2 <asyncmap 0x1>]
sent [LCP ConfAck id=0x2 <asyncmap 0x1>]
rcvd [LCP ConfAck id=0x1 <asyncmap 0x1>]
sent [IPCP ConfReq id=0x1 <addr 10.20.50.50>]
rcvd [IPCP ConfReq id=0x1 <addr 10.20.50.51>]
sent [IPCP ConfAck id=0x1 <addr 10.20.50.51>]
rcvd [IPCP ConfAck id=0x1 <addr 10.20.50.50>]
local  IP address 10.20.50.50
remote IP address 10.20.50.51
Script /etc/ppp/ip-up started (pid 23322)
Script /etc/ppp/ip-up finished (pid 23322), status = 0x0

**** TEST 2 ****
$pppd call ce0
using channel 143
Using interface ppp0
Connect: ppp0 <--> /dev/ttyPS0
sent [LCP ConfReq id=0x1 <asyncmap 0x1>]
rcvd [LCP ConfReq id=0x1 <asyncmap 0x1>]
sent [LCP ConfAck id=0x1 <asyncmap 0x1>]
rcvd [LCP ConfAck id=0x1 <asyncmap 0x1>]
sent [IPCP ConfReq id=0x1 <addr 10.20.50.50>]
rcvd [IPCP ConfReq id=0x1 <addr 10.20.50.51>]
sent [IPCP ConfAck id=0x1 <addr 10.20.50.51>]
rcvd [IPCP ConfAck id=0x1 <addr 10.20.50.50>]
local  IP address 10.20.50.50
remote IP address 10.20.50.51
Script /etc/ppp/ip-up started (pid 23351)
Script /etc/ppp/ip-up finished (pid 23351), status = 0x0

Other side:

**** TEST 1 ****
>pppd call server
using channel 7
Using interface ppp0
Connect: ppp0 <--> /dev/ttyPS7
sent [LCP ConfReq id=0x1 <asyncmap 0x1>]
LCP: timeout sending Config-Requests
rcvd [LCP ConfReq id=0x1 <asyncmap 0x1>]
sent [LCP ConfReq id=0x2 <asyncmap 0x1>]
sent [LCP ConfAck id=0x1 <asyncmap 0x1>]
rcvd [LCP ConfAck id=0x1 <asyncmap 0x1>]
rcvd [LCP ConfAck id=0x2 <asyncmap 0x1>]
sent [IPCP ConfReq id=0x1 <addr 10.20.50.51>]
rcvd [IPCP ConfReq id=0x1 <addr 10.20.50.50>]
sent [IPCP ConfAck id=0x1 <addr 10.20.50.50>]
rcvd [IPCP ConfAck id=0x1 <addr 10.20.50.51>]
local  IP address 10.20.50.51
remote IP address 10.20.50.50
Script /etc/ppp/ip-up started (pid 16935)
Script /etc/ppp/ip-up finished (pid 16935), status = 0x0

**** TEST 2 ****
>pppd call server
using channel 8
Using interface ppp0
Connect: ppp0 <--> /dev/ttyPS7
sent [LCP ConfReq id=0x1 <asyncmap 0x1>]
rcvd [LCP ConfReq id=0x1 <asyncmap 0x1>]
sent [LCP ConfAck id=0x1 <asyncmap 0x1>]
rcvd [LCP ConfAck id=0x1 <asyncmap 0x1>]
sent [IPCP ConfReq id=0x1 <addr 10.20.50.51>]
rcvd [IPCP ConfReq id=0x1 <addr 10.20.50.50>]
sent [IPCP ConfAck id=0x1 <addr 10.20.50.50>]
rcvd [IPCP ConfAck id=0x1 <addr 10.20.50.51>]
local  IP address 10.20.50.51
remote IP address 10.20.50.50
Script /etc/ppp/ip-up started (pid 16981)
Script /etc/ppp/ip-up finished (pid 16981), status = 0x0

*/

namespace nasaCE {

/**
 * @class PppConfOption
 *
 * @brief A single option used by PPP network configuration protocols.
 */
struct PppConfOption {
	/// The option type identifier.
	ACE_UINT8 optType;
	
	/// The length of the optBuf (NOT including type or length fields).
	ACE_UINT8 bufLen;
	
	/// Only the option itself, does NOT include the type or length.
	ACE_UINT8* optBuf;
	
	/// Default constructor.
	PppConfOption(): optType(0), bufLen(0), optBuf(0) { }
	
	/// Require values for all data members.
	PppConfOption(ACE_UINT8 newType, ACE_UINT8 newLen, ACE_UINT8* newBuf):
		optType(newType), bufLen(newLen) {
		optBuf = (ACE_UINT8*) ACE_OS::malloc (newLen);
		ACE_OS::memcpy(optBuf, newBuf, newLen);
	}
	
	/// Destructor.
	~PppConfOption() { free(optBuf); }
};

/* CP Packet structure:
    0                   1                   2                   3
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |     Code      |  Identifier   |            Length             |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   | Data ...
   +-+-+-+-+
*/

/**
 * @class PppConfPacket
 *
 * @brief Base class for PPP Control Protocols (e.g. LCP, NCP).
 */
class PppConfPacket: public NetworkData {

public:
	enum ControlCodes {
		Vendor_Specific = 0,
		Configure_Request = 1, /*!< Valid for LCP, IPCP */
  		Configure_Ack = 2,     /*!< Valid for LCP, IPCP */
		Configure_Nak = 3,     /*!< Valid for LCP, IPCP */
		Configure_Reject = 4,  /*!< Valid for LCP, IPCP */
		Terminate_Request = 5, /*!< Valid for LCP, IPCP */
		Terminate_Ack = 6,     /*!< Valid for LCP, IPCP */
		Code_Reject = 7,       /*!< Valid for LCP, IPCP */
		Protocol_Reject = 8,   /*!< Valid for LCP only */
		Echo_Request = 9,      /*!< Valid for LCP only */
		Echo_Reply = 10,       /*!< Valid for LCP only */
		Discard_Request = 11,  /*!< Valid for LCP only */
		Identification = 12,   /*!< Valid for LCP only */
		Time_Remaining = 13,   /*!< Valid for LCP only */
		ControlCodeCount = 14
	};
	
	static std::string codeTypeStr[ControlCodeCount];
	
	enum { headerByteLen = 4 }; /*!< The length of the non-variable fields in the header only. */
	
	/// @return The header length + trailer length (data size omitted).
	unsigned nonDataLength() { return PppConfPacket::headerByteLen; }
	
	/// Default constructor.
	/// @param allocate If true, set up an empty buffer.
	PppConfPacket(bool allocate = false): NetworkData() {
		if (allocate) { setBufferSize(nonDataLength()); }
	}
	
	/// Initialize with a buffer containing an existing packet.
	PppConfPacket(ACE_UINT8* buf, const unsigned bufLen) { replicateBuffer(buf, bufLen); }
	
	/// Copy constructor.
	PppConfPacket(PppConfPacket* other): NetworkData(other) { }
	
	/// Assignment operator, copies the buffer of another packet if initialized.
	/// @param other The source packet.
	void operator= (PppConfPacket* other) { deepCopy(other); }
	
	/// Pointer to the code field.
	ACE_UINT8* codePtr() { return ptrUnit(); }
	/// Value identifying the kind of packet.
	ACE_UINT8 codeVal() { return *codePtr(); }
	/// Set a new code value.
	void setCodeVal(ACE_UINT8 newCode) { *codePtr() = newCode; }
	
	/// Pointer to the identifier field.
	ACE_UINT8* identPtr() { return codePtr() + 1; }
	/// Helps in matching requests and replies.
	ACE_UINT8 identVal() { return *identPtr(); }
	/// Set the identifier field value.
	void setIdentVal(ACE_UINT8 newIdent) { *identPtr() = newIdent; }
	
	/// Pointer to the length field.
	ACE_UINT8* lengthPtr() { return identPtr() + 1; }
	/// Total length of the code, identifier, length, and data fields.
	ACE_UINT16 lengthVal() { return to_u16_(lengthPtr()); }
	/// Insert a new value for the length into the packet.
	void setLengthVal(ACE_UINT16 newLen) { from_u16_(newLen, lengthPtr()); }
	
	/// Pointer to the data.
	ACE_UINT8* dataPtr() { return lengthPtr() + 2; }
	/// The length of just the data.
	ACE_UINT8 dataLength() { return lengthVal() - 4; }
		
	/// @return A pointer the Nth PppConfOption contained in the PppConfPacket
	/// @param optIdx The index of the option to return.
	PppConfOption* optionAt(const int);
	
	/// @return A pointer the Nth PppConfOption contained in the PppConfPacket
	/// @param optIdx The index of the option to return.
	PppConfOption* operator[] (const int optIdx) { return optionAt(optIdx); }
	
	/// Add up all the options contained by the packet.
	unsigned countOpts();
	
	/// Add a new option.
	///
	/// This resizes the packet buffer with realloc() and can result in
	/// a new packet buffer, so be careful about retaining pointers to it.
	/// @param opt A pointer to the option to be added.
	void appendOption(PppConfOption*);
	
	/// Genererate a fresh packet.
	/// @p code Value identifying the kind of packet.
	/// @p identifier Helps matching requests and replies.
	/// @p options A vector containing various PppConfOptions to add.
	void build(const ACE_UINT8, const ACE_UINT8, std::vector<PppConfOption*>&);
};

} // namespace nasaCE

#endif // NASAPPPCONFPACKET_HPP
