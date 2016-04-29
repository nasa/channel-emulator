/* -*- C++ -*- */

//=============================================================================
/**
 * @file   LcpFsm.hpp
 * @author Tad Kollar  
 *
 * $Id: LcpFsm.hpp 1670 2012-08-28 13:25:28Z tkollar $ 
 * Copyright (c) 2008.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef NASALCPFSM_HPP
#define NASALCPFSM_HPP

#include "LcpPacket.hpp"
#include "PppOptions.hpp"
#include "fsm.hpp"
#include "nd_error.hpp"

/*
State Transition Table from RFC 1661

   Events                                   Actions

   Up   = lower layer is Up                 tlu = This-Layer-Up
   Down = lower layer is Down               tld = This-Layer-Down
   Open = administrative Open               tls = This-Layer-Started
   Close= administrative Close              tlf = This-Layer-Finished

   TO+  = Timeout with counter > 0          irc = Initialize-Restart-Count
   TO-  = Timeout with counter expired      zrc = Zero-Restart-Count

   RCR+ = Receive-Configure-Request (Good)  scr = Send-Configure-Request
   RCR- = Receive-Configure-Request (Bad)
   RCA  = Receive-Configure-Ack             sca = Send-Configure-Ack
   RCN  = Receive-Configure-Nak/Rej         scn = Send-Configure-Nak/Rej

   RTR  = Receive-Terminate-Request         str = Send-Terminate-Request
   RTA  = Receive-Terminate-Ack             sta = Send-Terminate-Ack

   RUC  = Receive-Unknown-Code              scj = Send-Code-Reject
   RXJ+ = Receive-Code-Reject (permitted)
       or Receive-Protocol-Reject
   RXJ- = Receive-Code-Reject (catastrophic)
       or Receive-Protocol-Reject
   RXR  = Receive-Echo-Request              ser = Send-Echo-Reply
       or Receive-Echo-Reply
       or Receive-Discard-Request

          | State
          |    0         1         2         3         4         5         6         7         8           9
    Events| Initial   Starting  Closed    Stopped   Closing   Stopping Req-Sent Ack-Rcvd   Ack-Sent     Opened
    ------+----------------------------------------------------------------------------------------------------
     Up   |    2     irc,scr/6     -         -         -         -         -         -         -           -
     Down |    -         -         0       tls/1       0         1         1         1         1         tld/1
     Open |  tls/1       1     irc,scr/6     3r        5r        5r        6         7         8           9r
     Close|    0       tlf/0       2         2         4         4     irc,str/4 irc,str/4 irc,str/4 tld,irc,str/4
          |
      TO+ |    -         -         -         -       str/4     str/5     scr/6     scr/6     scr/8         -
      TO- |    -         -         -         -       tlf/2     tlf/3     tlf/3p    tlf/3p    tlf/3p        -
          |
     RCR+ |    -         -       sta/2 irc,scr,sca/8   4         5       sca/8   sca,tlu/9   sca/8   tld,scr,sca/8
     RCR- |    -         -       sta/2 irc,scr,scn/6   4         5       scn/6     scn/7     scn/6   tld,scr,scn/6
     RCA  |    -         -       sta/2     sta/3       4         5       irc/7     scr/6x  irc,tlu/9   tld,scr/6x
     RCN  |    -         -       sta/2     sta/3       4         5     irc,scr/6   scr/6x  irc,scr/8   tld,scr/6x
          |
     RTR  |    -         -       sta/2     sta/3     sta/4     sta/5     sta/6     sta/6     sta/6   tld,zrc,sta/5
     RTA  |    -         -         2         3       tlf/2     tlf/3       6         6         8       tld,scr/6
          |
     RUC  |    -         -       scj/2     scj/3     scj/4     scj/5     scj/6     scj/7     scj/8       scj/9
     RXJ+ |    -         -         2         3         4         5         6         6         8           9
     RXJ- |    -         -       tlf/2     tlf/3     tlf/2     tlf/3     tlf/3     tlf/3     tlf/3   tld,irc,str/5
          |
     RXR  |    -         -         2         3         4         5         6         7         8         ser/9

[p] Passive option; see Stopped state discussion.
[r] Restart option; see Open event discussion.
[x] Crossed connection; see RCA event discussion. 
*/

using namespace FSM;

namespace nasaCE {

class LcpEvent {
private:
	int _event_id;
	LcpPacket* _rcvd_lcp;
	ACE_HANDLE _output_fd;
	PppOptions* _opts;
	
public:
	enum Events {
		Up,
		Down,
		Open,
		Close,
		TimeOut1,
		TimeOut2,
		RcvConfReqOk,
		RcvConfReqBad,
		RcvConfAck,
		RcvConfNak,
		RcvTermReq,
		RcvTermAck,
		RcvUnknown,
		RcvCodeRej1,
		RcvCodeRej2,
		RcvEchoReq
	};
	
	/// Default constuctor.
	LcpEvent():_event_id(-1), _rcvd_lcp(0), _output_fd(ACE_INVALID_HANDLE),
		_opts(0) { 
		ACE_TRACE("LcpEvent::LcpEvent()");
	}
	
	LcpEvent(const int eid, ACE_HANDLE fd = ACE_INVALID_HANDLE):
		_event_id(eid), _rcvd_lcp(0), _output_fd(fd), _opts(0) { 
		ACE_TRACE("LcpEvent::LcpEvent(eid, fd)");
		// _rcvd_lcp = new LcpPacket();
	}
	
	LcpEvent(const int eid, LcpPacket* lcp, ACE_HANDLE fd = ACE_INVALID_HANDLE, PppOptions* opts = 0):
		_event_id(eid), _rcvd_lcp(0), _output_fd(fd), _opts(opts) {
		ACE_TRACE("LcpEvent::LcpEvent(eid, lcp, fd, opts)");
		if (lcp) { _rcvd_lcp = new LcpPacket(lcp); }
	}
	
	LcpEvent(const LcpEvent& other): _event_id(other._event_id), _rcvd_lcp(0),
		_output_fd(other._output_fd), _opts(other._opts) {
		ACE_TRACE("LcpEvent::LcpEvent(other)");
		if (other._rcvd_lcp) {  _rcvd_lcp = new LcpPacket(other._rcvd_lcp); }
	}
		
	/// Destructor.
	~LcpEvent() { 
		ACE_TRACE("LcpEvent::~LcpEvent");
		if (_rcvd_lcp) { delete _rcvd_lcp; }
	}
	
	bool operator== (const LcpEvent& other_event) const {
		return (_event_id == other_event._event_id);
	}
	
	int operator= (LcpEvent& other_event) {
		_event_id = other_event._event_id;
		if ( _rcvd_lcp ) {
			_rcvd_lcp->deepCopy(other_event._rcvd_lcp);
		}
		else {
			_rcvd_lcp = new LcpPacket(other_event._rcvd_lcp);
		}
		return _event_id;
	}
	
	int operator= (const int new_event_id) { return (_event_id = new_event_id ); }	
	
    int getId() { return _event_id; }
	LcpPacket* getFrame() { 
		if ( ! _rcvd_lcp ) {
			throw nd_error("LcpEvent: Request for uninitialized _rcvd_lcp (LcpPacket pointer).");
		}
		return _rcvd_lcp;
	}
	ACE_HANDLE getOutputFd() { return _output_fd; }
	PppOptions& getOpts() { return *_opts; }
};

class LcpState: public StateBase<LcpState, LcpEvent>
{
private:	
	int _state_id;

public:
	enum LcpStates {
		Initial,	/*!< Down, hasn't been opened. */
		Starting,	/*!< Down, been opened. */
    	Closed,		/*!< Up, hasn't been opened. */
    	Stopped,	/*!< Open, waiting for down event. */
		Closing,	/*!< Terminating the connection, not open. */
    	Stopping,	/*!< Terminating, but open. */
    	ReqSent,	/*!< We've sent a Config Request. */
    	AckRcvd,	/*!< We've received a Config Ack. */
    	AckSent,	/*!< We've sent a Config Ack. */
    	Opened		/*!< Connection available. */
    };
  
	LcpState(): _state_id(Initial) { ACE_TRACE("LcpState::LcpState()"); }
	LcpState(const int id): _state_id(id) { ACE_TRACE("LcpState::LcpState(id)"); }
	LcpState(const LcpState& other_state): StateBase<LcpState, LcpEvent>(other_state),
		_state_id(other_state._state_id) { ACE_TRACE("LcpState::LcpState(other_state)"); }
	~LcpState() { ACE_TRACE("LcpState::~LcpState"); }
	
	int operator= (const LcpState& RHS) { return (_state_id = RHS._state_id); }
	int operator= (const int new_state_id) { return (_state_id = new_state_id); }
            
	/// @brief Compare two states.
	/// @param RHS Right hand-side state.
    /// @return True if states are equal.
	bool operator== (const LcpState& RHS) const {
		return _state_id == RHS._state_id;
	}
    
    int getId() { return _state_id; }
};
/*
void nop(LcpState&, LcpEvent&, LcpState&);

// This-Layer-Up
void tlu(LcpState&, LcpEvent&, LcpState&);

// This-Layer-Down
void tld(LcpState&, LcpEvent&, LcpState&);

// This-Layer-Started
void tls(LcpState&, LcpEvent&, LcpState&);

// This-Layer-Finished
void tlf(LcpState&, LcpEvent&, LcpState&);

// Initialize-Restart-Count
void irc(LcpState&, LcpEvent&, LcpState&);

// Zero-Restart-Count
void zrc(LcpState&, LcpEvent&, LcpState&);

// Send-Configure-Request
void scr(LcpState&, LcpEvent&, LcpState&);

// Send-Configure-Ack
void sca(LcpState&, LcpEvent&, LcpState&);

// Send-Configure-Nak/Rej
void scn(LcpState&, LcpEvent&, LcpState&);

//  Send-Terminate-Request
void str(LcpState&, LcpEvent&, LcpState&);

// Send-Terminate-Ack
void sta(LcpState&, LcpEvent&, LcpState&);

// Send-Code-Reject
void scj(LcpState&, LcpEvent&, LcpState&);

// Send-Echo-Reply
void ser(LcpState&, LcpEvent&, LcpState&);

void irc_scr(LcpState&, LcpEvent&, LcpState&);
void irc_str(LcpState&, LcpEvent&, LcpState&);
void tld_irc_str(LcpState&, LcpEvent&, LcpState&);
void irc_scr_sca(LcpState&, LcpEvent&, LcpState&);
void sca_tlu(LcpState&, LcpEvent&, LcpState&);
void tld_scr_sca(LcpState&, LcpEvent&, LcpState&);
void irc_scr_scn(LcpState&, LcpEvent&, LcpState&);
void tld_scr_scn(LcpState&, LcpEvent&, LcpState&);
void irc_tlu(LcpState&, LcpEvent&, LcpState&);
void tld_scr(LcpState&, LcpEvent&, LcpState&);
void tld_zrc_sta(LcpState&, LcpEvent&, LcpState&);
*/
class LcpFsm {
private:
	LcpState _sInitial, _sStarting, _sClosed, _sStopped, _sClosing,
		_sStopping, _sReqSent, _sAckRcvd, _sAckSent, _sOpened;
		
	LcpEvent _eUp, _eDown, _eOpen, _eClose, _eTimeOut1, _eTimeOut2,
		_eRcvConfReqOk, _eRcvConfReqBad, _eRcvConfAck, _eRcvConfNak,
		_eRcvTermReq, _eRcvTermAck, _eRcvUnknown, _eRcvCodeRej1,
		_eRcvCodeRej2, _eRcvEchoReq;
		
	SFuncStateMachine<LcpState, LcpEvent> _machine;
	bool _handleOptions(LcpPacket*, PppOptions&);

public:
	LcpFsm();
	
	void receive_request(LcpPacket*, ACE_HANDLE, PppOptions&);
	static void sendLcpPacket(LcpPacket*, PppOptions&, ACE_HANDLE fd = ACE_INVALID_HANDLE);
	
	int stateId() { return _machine.GetCurrentState().getId(); }
	
	void start();
};

} // namespace nasaCE

#endif // NASALCPFSM_HPP
