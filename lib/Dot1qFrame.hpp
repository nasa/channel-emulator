/* -*- C++ -*- */

//=============================================================================
/**
 * @file   Dot1qFrame.hpp
 * @author Tad Kollar  
 *
 * $Id: Dot1qFrame.hpp 1670 2012-08-28 13:25:28Z tkollar $ 
 * Copyright (c) 2008.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _DOT1Q_FRAME_HPP_
#define _DOT1Q_FRAME_HPP_

#include <EthernetFrame.hpp>

namespace nasaCE {

/**
*/
class Dot1qFrame: public EthernetFrame {
public:
	/// Default constructor.
    Dot1qFrame(const SupportedFrameTypes frameType = PCap): EthernetFrame(frameType, type8021qLen) { }
    
    /// Initialize with an allocated buffer.
	Dot1qFrame(const size_t bufferSize, const SupportedFrameTypes frameType = PCap, ACE_UINT8* buffer = 0):
		EthernetFrame(bufferSize, frameType, type8021qLen, buffer) { }

	/// Copy constructor.
	Dot1qFrame(Dot1qFrame* other, const bool doDeepCopy = true, 
    	const size_t offset = 0, const size_t newLen = 0):
    		EthernetFrame(other, type8021qLen, doDeepCopy, offset, newLen) { }

	/// Destructor.
    ~Dot1qFrame() { }
	
	/// Check that the Ethernet type is correct and that the object
	/// knows the correct length for the field.
	bool isValid8021q() { return (this->is8021q() && this->getTypeLen() == this->type8021qLen); }	
	
	/// Throw an exception when this is not a valid 802.1q frame.
	void throwIfNot8021q() {
		if ( ! isValid8021q() )
			throw std::runtime_error("Attempted 802.1q operation on non-802.1q frame.");
	}
		
	/// A pointer to the tag field.
	/// @throw runtime_error If this is not an 802.1q frame.
	ACE_UINT8* ptrTag() { throwIfNot8021q(); return this->ptrEtherType() + 2; }
	/// The value of the 802.1q tag.
	/// @throw runtime_error If this is not an 802.1q frame.
	ACE_UINT16 getTag() { return to_u16_(ptrTag()); }
	/// Set the value of the tag field.
	/// @throw runtime_error If this is not an 802.1q frame.
	void setTag(const ACE_UINT16 newTag) { from_u16_(newTag, ptrTag()); }
	
	/// Pointer to the payload type field.
	/// @throw runtime_error If this is not an 802.1q frame.
	ACE_UINT8* ptrEther8021qType() { return ptrTag() + 2; }
	/// The type of the payload.
	/// @throw runtime_error If this is not an 802.1q frame.
	ACE_UINT16 getEther8021qType() { return to_u16_(ptrEther8021qType()); }
	/// Modify the payload type field.
	/// @throw runtime_error If this is not an 802.1q frame.	
	void setEther8021qType(const ACE_UINT16 newType) { from_u16_(newType, ptrEther8021qType()); }
	
	/// Determine the type of the payload regardless if this is an 802.1q frame or not.
	ACE_UINT16 getPayloadType() { return getEther8021qType(); }
		
	/// Calculate the MTU minus the header length.
	static ACE_UINT16 max_payload(const ACE_UINT16 mtu) {
		return mtu - estimate_length(0, 0, type8021qLen);
	}
			
	/// Construct a new 802.1q frame.
	void build(MacAddress&, MacAddress&, NetworkData*, ACE_UINT16, ACE_UINT16);
			
	/// A string describing the type of data represented by this structure.
	/// @return Will always be "Ethernet 802.1q".
	virtual std::string typeStr() const { return "Ethernet 802.1q Frame"; }			
}; // class Dot1qFrameBaseT

} // namespace nasaCE

#endif // _DOT1Q_FRAME_HPP_
