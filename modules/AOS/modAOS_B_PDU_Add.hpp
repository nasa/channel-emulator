/* -*- C++ -*- */

//=============================================================================
/**
 * @file   modAOS_B_PDU_Add.hpp
 * @author Tad Kollar  
 *
 * $Id: modAOS_B_PDU_Add.hpp 2029 2013-06-05 15:06:31Z tkollar $
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _AOS_B_PDU_ADD_HPP_
#define _AOS_B_PDU_ADD_HPP_

#include "PeriodicTrafficHandler.hpp"
#include "AOS_Bitstream_PDU.hpp"

using namespace nasaCE;

namespace nAOS_B_PDU_Add {

//=============================================================================
/**
 * @class modAOS_B_PDU_Add
 * @author Tad Kollar  
 * @brief Convert incoming data to AOS Bitstream Protocol Data Units.
*/
//=============================================================================
class modAOS_B_PDU_Add: public PeriodicTrafficHandler {
public:
	/// Primary constructor.
	/// @param newName An identifier for the module that's unique within the channel.
	/// @param newChannelName An identifier for the containing channel.
	/// @param newDLLPtr Pointer to the DLL management structure.
    modAOS_B_PDU_Add(const std::string& newName, const std::string& newChannelName,
    	 const CE_DLL* newDLLPtr);

	/// Destructor
    ~modAOS_B_PDU_Add();

	/// Read in data, encapsulate in one or more B_PDUs, and send it on.
	int svc();

	/// Wrapper for getMTU().
	size_t get_B_PDU_Length() const { return _bpduLength; }

	/// Wrapper for setMTU().
	void set_B_PDU_Length(const size_t newLength) {
		_bpduLength = newLength;
		setMTU(newLength);
		rebuildIdleUnitTemplate_();
	}

	/// Read-only accessor to _send_immediately.
	bool getSendImmediately() const { return _sendImmediately; }

	/// Write-only accessor to _send_immediately.
	void setSendImmediately(const bool newVal = true) { _sendImmediately = newVal; }

	/// Overloaded to attempt to autodetect our MTU (if unset) from the target.
	/// Create a HandlerLink to manage the existance of the connection from
	/// this handler's output to the next handler's input.
	/// @param target The Traffic Handler to output to.
	/// @param sourceOutputRank Whether to connect the source's primary or auxiliary output.
	/// @param targetInputRank Whether to connect to the target's primary or auxiliary input.
	/// @param postOpen True if the link is being created after the source is already active.
	void connectOutput(BaseTrafficHandler* target,
		const HandlerLink::OutputRank sourceOutputRank = HandlerLink::PrimaryOutput,
		const HandlerLink::InputRank targetInputRank = HandlerLink::PrimaryInput,
		const bool postOpen = false );

	/// Called by a PeriodicLink object when an idle unit is required.
	NetworkData* getIdleUnit();

private:
	/// Calculate the packet zone length from the total length.
	size_t _getDataZoneLength() const { return get_B_PDU_Length() - AOS_Bitstream_PDU::spanHeader; }

	/// Whether to send each B_PDU as it's created or wait to send them all together.
	Setting& _sendImmediately;

	/// Iterate over a list of NetworkData objects and convert them to B_PDUs.
	/// @param netdata The data block just pulled from the queue.
	/// @return A list of the B_PDUs created, if _send_immediately is false; otherwise 0.
	AOS_Bitstream_PDU* _process_data(NetworkData* netdata);

	/// Send a newly-created B_PDU to the next handler.
	/// @return True if the unit was able to be sent.
	bool _send(AOS_Bitstream_PDU* bpdu);

	/// When there's more data waiting in the queue than has been passed to
	/// _process_data(), this holds the B_PDU that is being worked on until
	/// _process_data() is called again with the next data from the queue.
	AOS_Bitstream_PDU* _unfilled_bpdu;

	/// The index to begin writing data to in the _unfilled_bpdu.
	size_t _unfilled_bpdu_idx;

	/// Reconstruct the B_PDU that is duplicated when an idle unit is requested.
	void rebuildIdleUnitTemplate_();

	/// How long each B_PDU will be.
	size_t _bpduLength;

	/// If true the MTU was specified manually instead of being obtained from the target.
	bool _mtuIsManual;

}; // class modAOS_B_PDU_Add

} // namespace nAOS_B_PDU_Add

#endif // _AOS_B_PDU_ADD_HPP_
