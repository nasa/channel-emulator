/* -*- C++ -*- */

//=============================================================================
/**
 * @file   LcpFsm.cpp
 * @author Tad Kollar  
 *
 * $Id: LcpFsm.cpp 1670 2012-08-28 13:25:28Z tkollar $ 
 * Copyright (c) 2008.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "LcpFsm.hpp"
#include "PppFrame.hpp"

using namespace FSM;

namespace nasaCE {

void nop(LcpState& from, LcpEvent& event, LcpState& to) {
	ND_DEBUG("No-op transition: From state %d to %d, event %d.\n",
		from.getId(), to.getId(), event.getId());
}

// This-Layer-Up
void tlu(LcpState& from, LcpEvent& event, LcpState& to) {
	ND_DEBUG("This-Layer-Up transition: From state %d to state %d, event %d.\n",
		from.getId(), to.getId(), event.getId());
}

// This-Layer-Down
void tld(LcpState& from, LcpEvent& event, LcpState& to) {
	ND_DEBUG("This-Layer-Down transition: From state %d to %d, event %d.\n",
		from.getId(), to.getId(), event.getId());
}

// This-Layer-Started
void tls(LcpState& from, LcpEvent& event, LcpState& to) {
	ND_DEBUG("This-Layer-Started transition: From state %d to %d, event %d.\n",
		from.getId(), to.getId(), event.getId());
}

// This-Layer-Finished
void tlf(LcpState& from, LcpEvent& event, LcpState& to) {
	ND_DEBUG("This-Layer-Finished transition: From state %d to %d, event %d.\n",
		from.getId(), to.getId(), event.getId());
}

// Initialize-Restart-Count
void irc(LcpState& from, LcpEvent& event, LcpState& to) {
	ND_DEBUG("Initialize-Restart-Count transition: From state %d to %d, event %d.\n",
		from.getId(), to.getId(), event.getId());
}

// Zero-Restart-Count
void zrc(LcpState& from, LcpEvent& event, LcpState& to) {
	ND_DEBUG("Zero-Restart-Count transition: From state %d to %d, event %d.\n",
		from.getId(), to.getId(), event.getId());
}

// Send-Configure-Request
void scr(LcpState& from, LcpEvent& event, LcpState& to) {
	ND_DEBUG("Send-Configure-Request transition: From state %d to %d, event %d.\n",
		from.getId(), to.getId(), event.getId());
	
	ACE_UINT8 buf[10] = { LcpPacket::Configure_Request, 1, 0, 0xA, LcpPacket::AsyncMap, 6, 0, 0, 0, 1 };
	LcpPacket* req = new LcpPacket(buf, 10);
	PppOptions ppp_opts;
		
	LcpFsm::sendLcpPacket(req, ppp_opts, event.getOutputFd());
	delete(req);	
}

// Send-Configure-Ack
void sca(LcpState& from, LcpEvent& event, LcpState& to) {
	ND_DEBUG("Send-Configure-Ack transition: From state %d to %d, event %d.\n",
		from.getId(), to.getId(), event.getId());		
	event.getFrame()->setCodeVal(LcpPacket::Configure_Ack);
	
	LcpFsm::sendLcpPacket(event.getFrame(), event.getOpts(), event.getOutputFd());

}

// Send-Configure-Nak/Rej
void scn(LcpState& from, LcpEvent& event, LcpState& to) {
	ND_DEBUG("Send-Configure-Nak/Rej transition: From state %d to %d, event %d.\n",
		from.getId(), to.getId(), event.getId());
}

//  Send-Terminate-Request
void str(LcpState& from, LcpEvent& event, LcpState& to) {
	ND_DEBUG("Send-Terminate-Request transition: From state %d to %d, event %d.\n",
		from.getId(), to.getId(), event.getId());
}

// Send-Terminate-Ack
void sta(LcpState& from, LcpEvent& event, LcpState& to) {
	ND_DEBUG("Send-Terminate-Ack transition: From state %d to %d, event %d.\n",
		from.getId(), to.getId(), event.getId());
		
	event.getFrame()->setCodeVal(LcpPacket::Terminate_Ack);
	
	LcpFsm::sendLcpPacket(event.getFrame(), event.getOpts(), event.getOutputFd());
}

// Send-Code-Reject
void scj(LcpState& from, LcpEvent& event, LcpState& to) {
	ND_DEBUG("Send-Code-Reject transition: From state %d to %d, event %d.\n",
		from.getId(), to.getId(), event.getId());
}

// Send-Echo-Reply
void ser(LcpState& from, LcpEvent& event, LcpState& to) {
	ND_DEBUG("Send-Echo-Reply transition: From state %d to %d, event %d.\n",
		from.getId(), to.getId(), event.getId());
}

void irc_scr(LcpState& from, LcpEvent& event, LcpState& to) {
	irc(from, event, to);
	scr(from, event, to);
}

void irc_str(LcpState& from, LcpEvent& event, LcpState& to) {
	irc(from, event, to);
	str(from, event, to);
}

void tld_irc_str(LcpState& from, LcpEvent& event, LcpState& to) {
	tld(from, event, to);
	irc(from, event, to);
	str(from, event, to);
}

void irc_scr_sca(LcpState& from, LcpEvent& event, LcpState& to) {
	irc(from, event, to);
	scr(from, event, to);
	sca(from, event, to);
}

void sca_tlu(LcpState& from, LcpEvent& event, LcpState& to) {
	sca(from, event, to);
	tlu(from, event, to);
}

void tld_scr_sca(LcpState& from, LcpEvent& event, LcpState& to) {
	tld(from, event, to);
	scr(from, event, to);
	sca(from, event, to);
}

void irc_scr_scn(LcpState& from, LcpEvent& event, LcpState& to) {
	irc(from, event, to);
	scr(from, event, to);
	scn(from, event, to);
}

void tld_scr_scn(LcpState& from, LcpEvent& event, LcpState& to) {
	tld(from, event, to);
	scr(from, event, to);
	scn(from, event, to);
}

void irc_tlu(LcpState& from, LcpEvent& event, LcpState& to) {
	irc(from, event, to);
	tlu(from, event, to);
}

void tld_scr(LcpState& from, LcpEvent& event, LcpState& to) {
	tld(from, event, to);
	scr(from, event, to);
}

void tld_zrc_sta(LcpState& from, LcpEvent& event, LcpState& to) {
	tld(from, event, to);
	zrc(from, event, to);
	sta(from, event, to);
}

} // namespace nasaCE

using namespace nasaCE;


#define FSMStateType    LcpState
#define FSMEventType    LcpEvent

LcpFsm::LcpFsm():
	_sInitial	(LcpState::Initial),
	_sStarting	(LcpState::Starting),
	_sClosed	(LcpState::Closed),
	_sStopped	(LcpState::Stopped),
	_sClosing	(LcpState::Closing),
	_sStopping	(LcpState::Stopping),
	_sReqSent	(LcpState::ReqSent),
	_sAckRcvd	(LcpState::AckRcvd),
	_sAckSent	(LcpState::AckSent),
	_sOpened	(LcpState::Opened),
	
	_eUp			(LcpEvent::Up),
	_eDown			(LcpEvent::Down),
	_eOpen			(LcpEvent::Open),
	_eClose			(LcpEvent::Close),
	_eTimeOut1		(LcpEvent::TimeOut1),
	_eTimeOut2		(LcpEvent::TimeOut2),
	_eRcvConfReqOk	(LcpEvent::RcvConfReqOk),
	_eRcvConfReqBad	(LcpEvent::RcvConfReqBad),
	_eRcvConfAck	(LcpEvent::RcvConfAck),
	_eRcvConfNak	(LcpEvent::RcvConfNak),
	_eRcvTermReq	(LcpEvent::RcvTermReq),
	_eRcvTermAck	(LcpEvent::RcvTermAck),
	_eRcvUnknown	(LcpEvent::RcvUnknown),
	_eRcvCodeRej1	(LcpEvent::RcvCodeRej1),
	_eRcvCodeRej2	(LcpEvent::RcvCodeRej2),
	_eRcvEchoReq	(LcpEvent::RcvEchoReq),

	_machine(
	FUNCFSM_BEGIN( _sInitial )

	//                             0                  1                      2                     3                         4                  5                    6                     7                     8                     9
	FUNCFSM_STATES                 _sInitial       << _sStarting          << _sClosed           << _sStopped              << _sClosing       << _sStopping        << _sReqSent          << _sAckRcvd          << _sAckSent          << _sOpened
	
	FUNCFSM_EVENT(_eUp)            _sClosed        << _sReqSent[irc_scr]  << EXCEPTION          << EXCEPTION              << EXCEPTION       << EXCEPTION         << EXCEPTION          << EXCEPTION          << EXCEPTION          << EXCEPTION
	FUNCFSM_EVENT(_eDown)          EXCEPTION       << EXCEPTION           << _sInitial[tls]     << _sStarting             << _sInitial       << _sStarting        << _sStarting         << _sStarting         << _sStarting         << _sStarting[tld]
	FUNCFSM_EVENT(_eOpen)          _sStarting[tls] << _sStarting          << _sReqSent[irc_scr] << _sStopped              << _sStopping      << _sStopping        << _sReqSent          << _sAckRcvd          << _sAckSent          << _sOpened
	FUNCFSM_EVENT(_eClose)         _sInitial       << _sInitial[tlf]      << _sClosed           << _sClosed               << _sClosing       << _sClosing         << _sClosing[irc_str] << _sClosing[irc_str] << _sClosing[irc_str] << _sClosing[tld_irc_str]	
	FUNCFSM_EVENT(_eTimeOut1)      EXCEPTION       << EXCEPTION           << EXCEPTION          << EXCEPTION              << _sClosing[str]  << _sStopping[str]   << _sReqSent[scr]     << _sReqSent[scr]     << _sAckSent[scr]     << EXCEPTION
	FUNCFSM_EVENT(_eTimeOut2)      EXCEPTION       << EXCEPTION           << EXCEPTION          << EXCEPTION              << _sClosed[tlf]   << _sStopped[tlf]    << _sStopped[tlf]     << _sStopped[tlf]     << _sStopped[tlf]     << EXCEPTION
	

	FUNCFSM_EVENT(_eRcvConfReqOk)  EXCEPTION       << EXCEPTION           << _sClosed[sta]      << _sAckSent[irc_scr_sca] << _sClosing       << _sStopping        << _sAckSent[sca]     << _sOpened[sca_tlu]  << _sAckSent[sca]     << _sAckSent[tld_scr_sca]
	FUNCFSM_EVENT(_eRcvConfReqBad) EXCEPTION       << EXCEPTION           << _sClosed[sta]      << _sReqSent[irc_scr_scn] << _sClosing       << _sStopping        << _sReqSent[scn]     << _sAckRcvd[scn]     << _sReqSent[scn]     << _sReqSent[tld_scr_scn]
	
	FUNCFSM_EVENT(_eRcvConfAck)    EXCEPTION       << EXCEPTION           << _sClosed[sta]      << _sStopped[sta]         << _sClosing       << _sStopping        << _sAckRcvd[irc]     << _sReqSent[scr]     << _sOpened[irc_tlu]  << _sReqSent[tld_scr]
	FUNCFSM_EVENT(_eRcvConfNak)    EXCEPTION       << EXCEPTION           << _sClosed[sta]      << _sStopped[sta]         << _sClosing       << _sStopping        << _sReqSent[irc_scr] << _sReqSent[scr]     << _sAckSent[irc_scr] << _sReqSent[tld_scr]
	
	FUNCFSM_EVENT(_eRcvTermReq)    EXCEPTION       << EXCEPTION           << _sClosed[sta]      << _sStopped[sta]         << _sClosing[sta]  << _sStopping[sta]   << _sReqSent[sta]     << _sReqSent[sta]     << _sReqSent[sta]     << _sStopping[tld_zrc_sta]
	FUNCFSM_EVENT(_eRcvTermAck)    EXCEPTION       << EXCEPTION           << _sClosed           << _sStopped              << _sClosed[tlf]   << _sStopped[tlf]    << _sReqSent          << _sReqSent          << _sAckSent          << _sReqSent[tld_scr]		 
	FUNCFSM_EVENT(_eRcvUnknown)    EXCEPTION       << EXCEPTION           << _sClosed[scj]      << _sStopped[scj]         << _sClosing[scj]  << _sStopping[scj]   << _sReqSent[scj]     << _sAckRcvd[scj]     << _sAckSent[scj]     << _sOpened[scj]
	FUNCFSM_EVENT(_eRcvCodeRej1)   EXCEPTION       << EXCEPTION           << _sClosed           << _sStopped              << _sClosing       << _sStopping        << _sReqSent          << _sReqSent          << _sAckSent          << _sOpened
	FUNCFSM_EVENT(_eRcvCodeRej2)   EXCEPTION       << EXCEPTION           << _sClosed[tlf]      << _sStopped[tlf]         << _sClosed[tlf]   << _sStopped[tlf]    << _sStopped[tlf]     << _sStopped[tlf]     << _sStopped[tlf]     << _sStopping[tld_irc_str]
	FUNCFSM_EVENT(_eRcvEchoReq)    EXCEPTION       << EXCEPTION           << _sClosed           << _sStopped              << _sClosing       << _sStopping        << _sReqSent          << _sAckRcvd          << _sAckSent          << _sOpened[ser]
	
	FUNCFSM_END
	) 
	{ ACE_TRACE("LcpFsm::LcpFsm"); }
	
#undef FSMStateType
#undef FSMEventType

void LcpFsm::receive_request(LcpPacket* req, ACE_HANDLE fd, PppOptions& opts) {
	ACE_TRACE("LcpFsm::receive_request");
	switch (req->codeVal()) {
		case LcpPacket::Configure_Request: // Echo back the options we accept
			if (_handleOptions(req, opts)) {
				 ND_DEBUG("Got a good configuration Request.\n");
				LcpEvent e(LcpEvent::RcvConfReqOk, req, fd, &opts);
				try {
					_machine << e;
				}
				catch (const std::exception& e) {
					ND_ERROR("Bad state change attempted: %s\n", e.what());
				}
			}
			else {
				 ND_DEBUG("Got a bad configuration Request.\n");
				LcpEvent e(LcpEvent::RcvConfReqBad, req, fd, &opts);
				_machine << e;				
			}
			break;
		case LcpPacket::Terminate_Request:
			LcpEvent e(LcpEvent::RcvTermReq, req, fd, &opts);
			_machine << e;
			_machine << _eClose;
			_machine << _eDown;
			start();
			break;
	}	
}

bool LcpFsm::_handleOptions(LcpPacket* lcp, PppOptions& opts) {
	ACE_TRACE("LcpFsm::_handleOptions");
	int optIdx = 0;
	// ACE_UINT32 accm_union;
	bool ret = true;
	PppConfOption *opt;
	
	 ND_DEBUG("Handling LCP options, total length is %d.\n", lcp->dataLength());

	while (optIdx++) {
		try {
			opt = lcp->optionAt(optIdx);
			
			switch (opt->optType) {
			case LcpPacket::AsyncMap: {
				ACE_UINT32 newMap = LcpPacket::swap32(*((ACE_UINT32*) opt->optBuf));
				ND_DEBUG("Got an AsyncMap LCP Option (%08X).\n", newMap);		
				opts.sending_accm = newMap;
				opts.sending_accm[PppFrame::escSeq] = true;
				opts.sending_accm[PppFrame::flagSeq] = true;
//				accm_union = opts.receiving_accm.to_ulong() | newMap;
//				*((ACE_UINT32*) lcp.dataPtr() + 2) = LcpPacket::swap32(accm_union);
				}
				break;
			default:
				 ND_DEBUG("Got an unhandled LCP Option of type %d, data length %d.\n",
					opt->optType, opt->bufLen);
			}
			
			delete opt;
		}
		catch (const std::exception& e) {
			break;
		}
	}
	
	return ret;
}

void LcpFsm::sendLcpPacket(LcpPacket* lcp, PppOptions &opts, ACE_HANDLE fd) {
	ACE_TRACE("LcpFsm::sendLcpPacket");
	
	PppFrame pppFrame;
	pppFrame.build(0xC021, lcp);
	pppFrame.dump(pppFrame.getUnitLength());
	ND_DEBUG("PPP frame FCS %s valid.\n", ( pppFrame.fcsValid()? "IS" : "IS NOT" ));

	ND_DEBUG("Escaping:\n");
	pppFrame.escape(opts.sending_accm);
	ND_DEBUG("Done escaping.\n");

	pppFrame.dump(pppFrame.getUnitLength());

	int write_cnt = (int) ACE_OS::write(fd, pppFrame.ptrUnit(), pppFrame.getUnitLength());
	if ( write_cnt != (int) pppFrame.getUnitLength() )
		if ( write_cnt < 0 ) {
			ND_ERROR("Error sending LCP Packet: %s\n", ACE_OS::strerror(errno));
		}
		else {
			ND_ERROR("Sent the wrong number of bytes %d/%d.\n", write_cnt, pppFrame.getUnitLength());
		}
	else { ND_DEBUG("Sent %d bytes.\n", write_cnt); }
}

void LcpFsm::start() {
	ACE_TRACE("LcpFsm::start");
	try {
		_machine << _eOpen;
		_machine << _eUp;
	}
	catch (const std::exception& e) {
		ND_ERROR("Bad state change attempted: %s\n", e.what());
	}

}
