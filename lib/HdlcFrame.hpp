/* -*- C++ -*- */

//=============================================================================
/**
 * @file   HdlcFrame.hpp
 * @author Tad Kollar  
 *
 * $Id: HdlcFrame.hpp 1670 2012-08-28 13:25:28Z tkollar $ 
 * Copyright (c) 2008.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef NASAHDLCFRAME_HPP
#define NASAHDLCFRAME_HPP

#include <NetworkData.hpp>
/*

      +------+---------+---------+--------------------+------+------+
Bits  | 8    |   8/16  |   8/16  | Variable           | 8/16 | 8    |
      +------+---------+---------+--------------------+------+------+
Field | Flag | Address | Control | Data               | FCS  | Flag |
      +------+---------+---------+--------------------+------+------+

Flag: A single byte that indicates the beginning or end of a frame.
      The flag field consists of the binary sequence 01111110 (7E).
      May also contain an abort sequence 01111111 (7F) which means
      the frame should be discarded. Also may contain 11111111 (FF)
      which is interframe-filler.

Address:
	8-bit address:
	  1 bit ) 0: Unicast, 1: Broadcast or Multicast
	  6 bits) Node Address
	  1 bit ) Extended Address; 1: 8-bit address
	
	16-bit address:
	  1 bit ) 0: Unicast, 1: Broadcast or Multicast
	  6 bits) Node Address
      1 bit ) Extended Address; 0: 16-bit address
	  7 bits) Node Address
	  1 bit ) Extended Address; 1: End of Address

Control: The Control field is 8 or 16 bits and defines the frame type;
      Control or Data. The Control field is protocol dependent.


Data: The Data field may vary in length depending upon the protocol
      using the frame. Layer 3 frames are carried in the data field.
      
Frame check sequence (FCS):
	CRC-16-CCITT (X.25, V.41, CDMA, Bluetooth, XMODEM, HDLC,PPP, IrDA, BACnet; known as CRC-CCITT, MMC,SD):
	FCS [16 bits] = X16 + X12 + X5 + 1
	
	CRC-32-IEEE 802.3 (V.42, MPEG-2, PNG):
	FCS [32 bits] = X32 + X26 + X23 + X22 + X16 + X12 + X11 + X10 +
		X8 + X7 + X5 + X4 + X2 + X + 1 
*/

namespace nasaCE {

/**
	@author Tad Kollar  
*/
class HdlcFrame : public NetworkData {
private:
	ACE_UINT8 _addrLen;
	ACE_UINT8 _controlLen;
	ACE_UINT8 _fcsLen;
	
	static ACE_UINT16 _fcstab[256];
	static ACE_UINT32 _fcstab_32[256];
	
protected:
	/// Sets the length of the address field.
	/// @param newLen The new length of the field (should be 1 or 2)
	void setAddressLength_(const ACE_UINT8 newLen) { _addrLen = newLen; }	

	/// Sets the length of the control field.
	/// @param newLen The new length of the field (should be 1 or 2)
	void setControlLength_(const ACE_UINT8 newLen) { _controlLen = newLen; }	
	
	/// Sets the length of the Frame Check Sequence (FCS) field.
	/// @param newLen The new length of the field (should be 2 or 4)
	void setFCSLength_(ACE_UINT8 newLen) { _fcsLen = newLen; }
	
public:
	enum { headerByteLen = 1 }; /*!< The length of the non-variable fields;
		does not include address, control, or FCS field lengths. */
	enum {	InitFCS16 = 0xffff,	/*!< Initial 16-bit FCS value */
			GoodFCS16 = 0xf0b8	/*!< Good final 16-bit FCS value */
	};
	enum {	InitFCS32 = 0xffffffff,	/*!< Initial 32-bit FCS value */
   			GoodFCS32 = 0xdebb20e3	/*!< Good final 32-bit FCS value */
	};
	
	/// Default constructor.
	HdlcFrame(const ACE_UINT8 addressLen = 1, const ACE_UINT8 controlLen = 1,
		const ACE_UINT8 fcsLen = 2): NetworkData(),
		_addrLen(addressLen), _controlLen(controlLen), _fcsLen(fcsLen) { }
	
	/// Copy constructor.
	HdlcFrame(HdlcFrame* other, const bool doDeepCopy = true, 
    	const size_t offset = 0, const size_t newLen = 0): NetworkData(other, doDeepCopy, offset, newLen) { }	
	
	/// @return The header length + trailer length (data length omitted)
	virtual unsigned nonDataLength() { 
		return HdlcFrame::headerByteLen + _addrLen + _controlLen + _fcsLen + 1;
	}
	
	/// Pointer to the initial flag field.
	ACE_UINT8* startFlagPtr() { return ptrUnit(); }
	/// The first byte in the frame.
	ACE_UINT8 startFlagVal() { return *startFlagPtr(); }
	
	/// @return The length of the address field.
	ACE_UINT8 getAddressLength() { return _addrLen; }
	/// @return Pointer to the address field.
	ACE_UINT8* addressPtr() { return startFlagPtr() + 1; }
	/// @return The value of the address field.
	ACE_UINT16 addressVal() { return (( _addrLen == 1 )? *addressPtr() : to_u16_(addressPtr())); }
	/// @p address The new value of the address field.
	void setAddress(const ACE_UINT16 address) {
		( _addrLen == 1 )? *addressPtr() = address & 0xFF : from_u16_(address, addressPtr());
	}
	
	/// @return The length of the control field.
	ACE_UINT8 getControlLength() { return _controlLen; }
	/// Pointer to the control field.
	ACE_UINT8* controlPtr() { return addressPtr() + _addrLen; }
	/// The third byte in the frame should always equal 0x03.
	ACE_UINT16 controlVal() { return (( _controlLen == 1 )? *controlPtr() : to_u16_(controlPtr())); }
	/// Set the control field value.
	void setControl(const ACE_UINT16 control) {
		( _controlLen == 1 )? *controlPtr() = control & 0xFF : from_u16_(control, controlPtr());
	}	
	/// Pointer to the encapsulated data.
	virtual ACE_UINT8* dataPtr() { return controlPtr() + _controlLen; }
	/// Computes the length of the data field (should be < 1500)
	int dataLength() { return getUnitLength() - nonDataLength(); }

	/// Copy the payload from the provided NetworkData.
	/// @param newData A pointer to the NetworkData to copy.
	/// @param resize Re-allocate the frame's buffer.
	void setData(NetworkData*, const bool resize = true);
	
	/// @return The length of the Frame Check Sequence (FCS) field.
	ACE_UINT8 getFCSLength() { return _fcsLen; }
	/// Pointer to the Frame Check Sequence (FCS) field.
	ACE_UINT8* fcsPtr() { return dataPtr() + dataLength(); }
	/// Value of the FCS field. May be either 2 or 4 bytes.
	ACE_UINT32 fcsVal() {
		return (( _fcsLen == 2 )? to_u16_(fcsPtr()) : to_u32_(fcsPtr()));
	}
	/// Sets a 16-bit FCS, first checking that the expected length is 2 bytes.
	/// @param newVal The value to complement, then store in the FCS field.
	/// @param doComp Perform the ones complement, defaults to true.
	void setFcsVal16(ACE_UINT16 newVal, const bool doComp = true) {
		if (_fcsLen != 2) throw nd_error("Called setFcsVal16 when the FCS length != 2 bytes.");
		if (doComp) newVal ^= 0xFFFF;
		from_u16_(newVal, fcsPtr(), false);
	}
	
	/// Sets a 32-bit FCS, first checking that the expected length is 4 bytes.
	/// @param newVal The value to complement, then store in the FCS field.
	/// @param doComp Perform the ones complement, defaults to true.
	void setFcsVal32(ACE_UINT32 newVal, const bool doComp = true) {
		if (_fcsLen != 4) throw nd_error("Called setFcsVal32 when the FCS length != 4 bytes.");
		if (doComp) newVal ^= 0xFFFFFFFF;
		from_u32_(newVal, fcsPtr(), false);
	}
	
	/// Pointer to the final flag field.
	ACE_UINT8* endFlagPtr() { return fcsPtr() + _fcsLen; }
	/// The first byte in the frame should always equal 0x7E.
	ACE_UINT8 endFlagVal() { return *endFlagPtr(); }
		
	/// Construct a new frame to send out.
	/// @param addressLen The byte length of the address field (1 or 2).
	/// @param address The type of the payload.
	/// @param controlLen The byte length of the control field (1 or 2).
	/// @param control Defines the frame type (control or data).
	/// @param data The payload to integrate into the packet.
	/// @param fcsLen The length of the FCS field (2 or 4).
	/// @param flagVal The value to terminate the frame with.
	virtual void build(const ACE_UINT8, const ACE_UINT16, const ACE_UINT8, const ACE_UINT16,
		NetworkData*, const ACE_UINT8, const ACE_UINT8 flagVal = 0x7E);
	
	ACE_UINT16 fcs16();
	static ACE_UINT16 fcs16(ACE_UINT16, ACE_UINT8*, int);
	
	ACE_UINT32 fcs32();
	static ACE_UINT32 fcs32(ACE_UINT32, ACE_UINT8*, int);
	
	bool fcsValid() {
		return (( _fcsLen == 2)? (fcs16() == GoodFCS16) : (fcs32() == GoodFCS32));
	}
	
	/// String representation of the contained data type.
	std::string typeStr() const { return "HDLC Frame"; }	
}; // class HdlcFrame

} // namespace nasaCE

#endif // NASAHDLCFRAME_HPP
