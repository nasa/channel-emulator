/* -*- C++ -*- */

//=============================================================================
/**
 * @file   TC_Comm_Link_Control_Word.hpp
 * @author Tad Kollar  
 *
 * $Id: TC_Comm_Link_Control_Word.hpp 1670 2012-08-28 13:25:28Z tkollar $
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _NASA_TC_COMM_LINK_CONTROL_WORD_HPP_
#define _NASA_TC_COMM_LINK_CONTROL_WORD_HPP_

#include "NetworkData.hpp"

namespace nasaCE {

//=============================================================================
/**
	@author Tad Kollar  
	@class TC_Comm_Link_Control_Word
	@brief Implementation of the Telecommand Communications Link Control Word PDU
	@see [1] CCSDS 232.0-B-1, "TC SPACE DATA LINK PROTOCOL", Blue Book

	The Communications Link Control Word (CLCW), which is the protocol data
	unit transmitted from the receiving end to the sending end, shall provide the mechanism by
	which the FARM at the receiving end reports the status of frame acceptance to the Frame
	Operation Procedure (FOP) at the sending end.

<table>
<tr>
<th rowspan='2'>Control Word Type "0"</th>
<th rowspan='2'>CLCW Version Number "00"</th>
<th rowspan='2'>Status Field</th>
<th rowspan='2'>COP In Effect</th>
<th rowspan='2'>Virtual Channel Identification</th>
<th rowspan='2'>Rsvd. Spare</th>
<th colspan='5'>Flags</th>
<th rowspan='2'>FARM-B Counter</th>
<th rowspan='2'>Rsvd. Spare</th>
<th rowspan='2'>Report Value</th>
</tr>
<tr>
<th>No RF Avail</th>
<th>No Bit Lock</th>
<th>Lock-out</th>
<th>Wait</th>
<th>Retransmit</th>
</tr>
<tr>
<td>1</td><td>2</td><td>3</td><td>2</td><td>6</td><td>2</td><td>1</td><td>1</td><td>1</td><td>1</td><td>1</td>
<td>2</td><td>1</td><td>8</td>
</tr>
<tr>
<td colspan="4">Octet 0</td><td colspan="2">Octet 1</td><td colspan="7">Octet 2</td><td colspan="1">Octet 3</td>
</tr>
</table>
*/
//=============================================================================

class TC_Comm_Link_Control_Word: public NetworkData {
public:

    /// Default constructor or construct from a buffer.
	/// @param buffer A pointer to the start of the buffer.
	/// @param bufLen The amount of data to copy.
	/// @param isOutside Whether our buffer is independent of any existing buffers.
	TC_Comm_Link_Control_Word(const size_t bufLen = 4, const ACE_UINT8* buffer = 0):
		NetworkData(bufLen, buffer) { }

	/// Copy constructor.
	TC_Comm_Link_Control_Word(TC_Comm_Link_Control_Word* other, const bool doDeepCopy = true,
		const size_t offset = 0, const size_t newLen = 0): NetworkData(other, doDeepCopy, offset, newLen) { }

	/// Destructor.
	~TC_Comm_Link_Control_Word() { }

	/// @brief Retrieve the value of the Control Word Type field.
	///
	/// This field is used to distinguish the CLCW from another type of report that may
	/// be alternatively contained in the field that carries the CLCW (e.g., the
	/// Operational Control Field of TM or AOS Transfer Frames).
	/// This one-bit field shall be set to "0".
    bool getControlWordType() const { return (bool) _getOctetBits(0, 0, 1); }

    /// Modify the value of the Control Word Type field.
    /// @param newVal The value to update the field with.
    void setControlWordType(const bool newVal = 0) { _setOctetBits(0, 0, 1, newVal); }

    /// @brief Retrieve the value of the CLCW Version Number field.
    ///
    /// The CLCW Version Number is included to provide future growth flexibility. At
    /// present a single ‘Version-1’ CLCW, whose binary encoded Version Number is
    ///‘"00", is defined in this Recommendation.
    /// This two-bit field shall be set to "00".
    ACE_UINT8 getCLCWVersionNumber() const { return _getOctetBits(0, 1, 2); }

    /// Modify the value of the CLCW Version Number field.
    /// @param newVal The value to update the field with.
    void setCLCWVersionNumber(const ACE_UINT8 newVal = 0) { _setOctetBits(0, 1, 2, newVal); }

    /// @brief Retrieve the value of the Status field.
	///
	/// Application of the Status Field is mission-specified.
	/// The Status Field may be used by Agencies for local enhancements to operations of
  	/// this protocol and is not part of the COP.
    ACE_UINT8 getStatusField() const { return _getOctetBits(0, 3, 3); }

    /// Modify the value of the Status field.
    /// @param newVal The value to update the field with.
    void setStatusField(const ACE_UINT8 newVal) { _setOctetBits(0, 3, 3, newVal); }

    /// @brief Retrieve the value of the COP In Effect field.
    ///
    /// <ul><li>Bits 6-7 of the CLCW shall contain the COP in Effect parameter and shall be used
	/// to indicate the COP that is being used.</li>
	/// <li>For COP-1, this two-bit field shall be set to "01".</li>
	/// <li>At present a single COP, COP-1, is defined in this Recommendation.</li></ul>
	ACE_UINT8 getCOPInEffect() const { return _getOctetBits(0, 6, 2); }

	/// Modify the value of the COP In Effect field.
    /// @param newVal The value to update the field with.
	void setCOPInEffect(const ACE_UINT8 newVal = 1) { _setOctetBits(0, 6, 2, newVal); }

	/// @brief Retrieve the value of the Virtual Channel Identifier.
	///
	/// Each Virtual Channel in use shall have its own CLCW reporting activated.
	ACE_UINT8 getVirtualChannelIdentifier() const { return _getOctetBits(1, 0, 6); }

	/// Modify the value of the Virtual Channel Identifier.
    /// @param newVal The value to update the field with.
	void setVirtualChannelIdentifier(const ACE_UINT8 newVal) { _setOctetBits(1, 0, 6, newVal); }

	/// @brief Retrieve the value of the No RF Available flag.
	///
	/// A setting of ‘0’ in the No RF Available Flag shall indicate that the Physical
	/// Layer is Available (i.e., any Transfer Frame will be received and processed by the Physical
	/// Layer and passed on to this protocol if correct).
	/// A setting of ‘1’ in the No RF Available Flag shall indicate that the Physical
	/// Layer is not available and that Transfer Frames cannot be transferred without corrective
	/// action within the Physical Layer.
	bool getNoRFAvailFlag() const { return _getOctetBits(2, 0, 1); }

	/// Modify the value of the No RF Available flag.
    /// @param newVal The value to update the field with.
    void setNoRFAvailFlag(const bool newVal = true) { _setOctetBits(2, 0, 1, newVal); }

	/// @brief Retrieve the value of the No Bit Lock flag.
	///
	/// The No Bit Lock Flag is an optional, mission-specific engineering measurement that
	/// provides a performance quality indicator that indicates specifically whether the
	/// Physical Layer is working normally by having enough signal energy to achieve bit
	/// synchronization with the received data stream.
	/// Failure to achieve bit lock may indicate that the Physical Layer is operating at a non-
	/// nominal performance level and that the Transfer Frame rejection rate may be
	/// correspondingly abnormally high.
	/// Use of the No Bit Lock Flag is optional; if used,
	/// a) ‘0’ shall indicate bit lock has been achieved;
	/// b) ‘1’ shall indicate bit lock has not been achieved.
	bool getNoBitLockFlag() const { return _getOctetBits(2, 1, 1); }

	/// Modify the value of the No Bit Lock flag.
    /// @param newVal The value to update the field with.
    void setNoBitLockFlag(const bool newVal = true) { _setOctetBits(2, 1, 1, newVal); }

	/// @brief Retrieve the value of the Lockout flag.
	///
	/// The Lockout Flag shall be used to indicate the Lockout status of the FARM of a
	/// particular Virtual Channel.
	/// A setting of ‘1’ in the Lockout Flag shall indicate Lockout.
	/// Lockout occurs whenever a Type-A Transfer Frame that violates certain Frame
	/// Acceptance Checks is received on a particular Virtual Channel. Once the FARM
	/// is in Lockout, all subsequent Type-A Transfer Frames will be rejected by the
	/// FARM until the condition is cleared.
	/// A setting of "0" in the Lockout Flag shall indicate that the FARM is not in
	/// Lockout.
	/// Separate Lockout Flags shall be maintained for each Virtual Channel.
	bool getLockoutFlag() const { return _getOctetBits(2, 2, 1); }

	/// Modify the value of the Lockout flag.
    /// @param newVal The value to update the field with.
    void setLockoutFlag(const bool newVal = true) { _setOctetBits(2, 2, 1, newVal); }

	/// @brief Retrieve the value of the Wait flag.
	///
	/// The Wait Flag shall be used to indicate that the receiving end is unable to accept
	/// data for processing on a particular Virtual Channel.
	/// An inability to accept data could be caused by temporary lack of storage and/or
	/// processing resources in the receiving end of this protocol or higher layers.
	/// A setting of ‘1’ (i.e., Wait) in the Wait Flag for a particular Virtual Channel
	/// shall indicate that all further Type-A Transfer Frames on that Virtual Channel will be
	/// rejected by the FARM until the condition is cleared.
	/// A setting of ‘0’ (i.e., Don’t Wait) in the Wait Flag shall indicate that the
	/// receiving end is able to accept and process incoming Type-A Transfer Frames.
	/// Separate Wait Flags shall be maintained for each Virtual Channel.
	bool getWaitFlag() const { return _getOctetBits(2, 3, 1); }

	/// Modify the value of the Wait flag.
    /// @param newVal The value to update the field with.
    void setWaitFlag(const bool newVal = true) { _setOctetBits(2, 3, 1, newVal); }

	/// @brief Retrieve the value of the Retransmit flag.
	///
	/// The Retransmit Flag is used to speed the operation of the COP by providing
	/// immediate indication to the FOP at the sending end that retransmission is
	/// necessary.
	/// <ul>
	/// <li>A setting of ‘1’ in the Retransmit Flag shall indicate that one or more Type-A
	/// Transfer Frames on a particular Virtual Channel have been rejected or found missing by the
	/// FARM and therefore retransmission is required.</li>
	/// <li>A setting of ‘0’ in the Retransmit Flag shall indicate that there are no
	/// outstanding Type-A Transfer Frame rejections in the sequence received so far, and thus
	/// retransmissions are not required.</li>
	/// <li>Separate Retransmit Flags shall be maintained for each Virtual Channel.</li></ul>
	bool getRetransmitFlag() const { return _getOctetBits(2, 4, 1); }

	/// Modify the value of the Retransmit flag.
    /// @param newVal The value to update the field with.
    void setRetransmitFlag(const bool newVal = true) { _setOctetBits(2, 4, 1, newVal); }

    /// @brief Retrieve the value of the FARM-B Counter field.
    ///
    /// Separate FARM-B Counters shall be maintained for each Virtual Channel.
	/// @note This 2-bit field contains the two least significant bits of a FARM-B Counter.
	/// This counter is maintained within the FARM and increments once each time a
	/// Type-B Transfer Frame is accepted in Bypass mode on a particular Virtual
	/// Channel. The field supports the verification that Type-B Transfer Frames
	/// (Control or User Data) were accepted by the receiving end.
	ACE_UINT8 getFARMBCounter() const { return _getOctetBits(2, 5, 2); }

	/// Modify the value of the FARM-B Counter field.
    /// @param newVal The value to update the field with.
	void setFARMBCounter(const ACE_UINT8 newVal) { _setOctetBits(2, 5, 2, newVal); }

    /// @brief Retrieve the value of the Report Value field.
    ///
	/// Separate Report Values shall be maintained for each Virtual Channel.
	/// @note This 8-bit field contains the value of the Next Expected Frame Sequence
	/// Number, N(R), which is equal to the value of FARM's
	/// receiver_Frame_Sequence_Number, V(R). The FARM V(R) counter shall
	/// increment once each time a Type-AD Transfer Frame is accepted on a particular
	/// Virtual Channel.
	ACE_UINT8 getReportValue() const { return _getOctet(3); }

	/// Modify the value of the Report Value field.
    /// @param newVal The value to update the field with.
	void setReportValue(const ACE_UINT8 newVal) { _setOctet(3, newVal); }

private:

	/// The many fields in this PDU are spread across only four octets; this function accesses them.
	/// @param octet_num Which of the four octets (0 to 3) to access.
	/// @throw ValueTooLarge If octet_num > 3.
	/// @return A pointer to the specified octet.
	ACE_UINT8* _ptrOctet(const ACE_UINT8 octet_num) const {
		if (octet_num > 3)
			throw ValueTooLarge(ValueTooLarge::msg("Octet number", octet_num, 3));
		return ptrUnit() + octet_num;
	}

	/// Retrieve the entire value of the specified octet.
	/// @param octet_num Which of the four octets (0 to 3) to access.
	ACE_UINT8 _getOctet(const ACE_UINT8 octet_num) const { return *_ptrOctet(octet_num); }

	/// Modify the entire value of the specified octet.
	/// @param octet_num Which of the four octets (0 to 3) to access.
	/// @param newVal The new value of the octet.
	void _setOctet(const ACE_UINT8 octet_num, const ACE_UINT8 newVal) {
		*_ptrOctet(octet_num) = newVal;
	}

	/// Extract the integer value from the specified bits as if it were a standalone number.
	/// @param octet_num Which of the four octets (0 to 3) to access
	/// @param msb The Most Significant Bit, or lowest-numbered-bit in the field (0 being the MSB, on the left)
	/// @param bits The number of bits to extract, 1 to 8.
	/// @return The right-shifted value of the requested bits.
	ACE_UINT8 _getOctetBits(const ACE_UINT8 octet_num, const ACE_UINT8 msb, const ACE_UINT8 bits) const {
		ACE_UINT8 lsb = msb + bits - 1;

		ACE_UINT8 andVal = (0xFF >> (8 - bits)) << (7 - lsb);
		return (_getOctet(octet_num) & andVal) >> (7 - lsb);
	}

	/// Modify the value in the specified position while preserving the state of the rest of the octet.
	/// @param octet_num Which of the four octets (0 to 3) to access.
	/// @param msb The Most Significant Bit, or lowest-numbered-bit in the field (0 being the MSB, on the left)
	/// @param bits The number of bits to extract, 1 to 8.
	/// @param val The value to integrate into the octet.
	void _setOctetBits(const ACE_UINT8 octet_num, const ACE_UINT8 msb,
		const ACE_UINT8 bits, const ACE_UINT8 val) {
		ACE_UINT8 maxVal = 0xFF >> (8 - bits);

		if ( val > maxVal )	throw ValueTooLarge("octet_num is greater than 3");

		ACE_UINT8 lsb = msb + bits - 1;

		ACE_UINT8 andVal = ! (maxVal << (7 - lsb));

		_setOctet(octet_num, (_getOctet(octet_num) & andVal) | (val << (7 - lsb)));
	}

}; // class TC_Comm_Link_Control_Word

} // namespace nasaCE

#endif // _NASA_TC_COMM_LINK_CONTROL_WORD_HPP_
