/* -*- C++ -*- */

//=============================================================================
/**
 * @file   NetworkData.hpp
 * @author Tad Kollar  
 *
 * $Id: NetworkData.hpp 2359 2013-09-12 19:13:54Z tkollar $
 * Copyright (c) 2008.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _NETWORK_DATA_HPP_
#define _NETWORK_DATA_HPP_

#include "nd_error.hpp"
#include <ace/Malloc_T.h>
#include <ace/Message_Block.h>
#include <ace/OS_main.h>
#include <ace/Time_Value.h>
#include <ace/High_Res_Timer.h>

#include <string>
#include <memory>

extern "C" {
	#include <byteswap.h>
}

namespace nasaCE {

#if __BYTE_ORDER == __LITTLE_ENDIAN && defined (bswap_16)
#define _swap16(x) bswap_16(x)
#define _swap32(x) bswap_32(x)
#define _swap64(x) bswap_64(x)
#else
#define _swap16(x) x
#define _swap32(x) x
#define _swap64(x) x
#endif

/**
 *	@class NetworkData
 *
 *  @brief An untyped network data chunk that's been received or is ready to transmit.
 */
class NetworkData: public ACE_Message_Block {

public:
	// --- EXCEPTIONS ---------------------------------------------------------------
	// Exception classes specific to NetworkData and its subclasses.

	struct NetDataError: public nd_error {
		NetDataError(const std::string& e): nd_error(e) { }
	};

	/// @class UnderMinLength
	/// @brief Thrown when the PDU is shorter than minimumLength() returns.
	struct UnderMinLength: public NetDataError {
		UnderMinLength(const std::string& e): NetDataError(e) { }

		static std::string msg(const std::string& typeStr,
			const size_t minLength, const size_t actualLength) {
			std::ostringstream os;

			os << typeStr << " is shorter than the minimum length of " << std::dec << minLength
				<< "; actual length is " << actualLength;

			return os.str();
		}
	};

	/// @class WrongSize
	/// @brief Thrown when the PDU size is other than expected.
	struct WrongSize: public NetDataError {
		WrongSize(const std::string& e): NetDataError(e) { }

		static std::string msg(const std::string& typeStr,
			const size_t expectedLength, const size_t actualLength) {
			std::ostringstream os;

			os << typeStr << " is not the expected size of " << std::dec << expectedLength
				<< "; actual length is " << actualLength;

			return os.str();
		}
	};

	/// @class InvalidChecksum
	/// @brief Thrown when the checksum doesn't match the expected value.
	struct InvalidChecksum: public NetDataError {
		InvalidChecksum(const std::string& e): NetDataError(e) { }

		static std::string msg(const std::string& typeStr, const std::string& crcName) {
			std::ostringstream os;

			os << typeStr << " has an invalid " << crcName << ".";
			return os.str();
		}
	};

	/// @class PrematureAccess
	/// @brief Thrown when the access to an uninitialized buffer is attempted.
	struct PrematureAccess: public NetDataError {
		PrematureAccess(const std::string& e): NetDataError(e) { }

		static std::string msg(const std::string& typeStr) {
			std::ostringstream os;

			os << "Access to the unitialized buffer of " << article(typeStr)
				<< typeStr << " was attempted.";

			return os.str();
		}
	};

	/// @class RedundantInitialization
	/// @brief Thrown when an initialization is attempted on an already-initialized object.
	struct RedundantInitialization: public NetDataError {
		RedundantInitialization(const std::string& e): NetDataError(e) { }

		static std::string msg(const std::string& typeStr) {
			std::ostringstream os;

			os << "Attempt to re-initialize " << article(typeStr) << typeStr
				<< " was not allowed.";

			return os.str();
		}
	};

	/// @class MalformedPayload
	/// @brief Thrown when the payload is a different type or format than expected.
	struct MalformedPayload: public NetDataError {
		MalformedPayload(const std::string& e): NetDataError(e) { }

		static std::string msg(const std::string& typeStr, const std::string& expectedPayload) {
			std::ostringstream os;

			os << typeStr << " has a malformed payload. Expected, but did not find "
				<< article(expectedPayload) << "'" << expectedPayload << "'.";

			return os.str();
		}
	};
	// ------------------------------------------------------------------------------

	// static ACE_Lock* lockingStrategy;

	/// Default constructor.
    NetworkData();

    /// Initialize with an allocated buffer.
	NetworkData(const size_t bufferSize, const ACE_UINT8* buffer = 0);

	/// Copy constructor.
	/// @param other A pointer to the NetworkData to copy.
	/// @param doDeepCopy If true, makes a true duplicate of the data. Otherwise, shares other's buffer.
	/// @note ACE_MESSAGE_BLOCK doesn't work well with references, so pointers must be used.
    NetworkData(const NetworkData* other,
    	const bool doDeepCopy = true,
    	const size_t offset = 0,
    	const size_t newLen = 0);

	/// Destructor.
    virtual ~NetworkData();

	/// @brief Pointer to the start of the buffer.
	/// @param offset Added to the start of the buffer.
	/// @throw PrematureAccess If not initialized.
	ACE_UINT8* ptrBuffer(const size_t offset = 0) const;

	/// @brief Pointer to the beginning of the data unit within the buffer.
	/// @param offset Added to the start of the unit.
	/// @throw PrematureAccess If not initialized.
	ACE_UINT8* ptrUnit(const size_t offset = 0) const;

	/// Allocates and makes a true copy of the buffer and state from the other NetworkData.
	/// @param other Points to the NetworkData object to copy from.
	/// @param bufferOffset Where to begin duplicating the buffer from.
	/// @param dataLen If zero, copy the entire buffer; otherwise copy the specified amount.
	void deepCopy(const NetworkData* other, const size_t bufferOffset = 0, const size_t dataLen = 0);

    /// Set all bytes in the buffer to zero.
    /// @param newLen If greater than zero, resize the buffer and declare the object initialized.
    /// @param resetUnit If true, set the unit length to be the same as the buffer length.
    /// @throw PrematureAccess When newLen is 0 and _isInitialized is false.
    void clearBuffer(const size_t newLen = 0, const bool resetUnit = true);

	/// Set all bytes between rd_ptr and wr_ptr to zero.
	void clearUnit(const size_t newLen = 0);

	/// Make a true duplicate of the provided buffer.
	/// @param ptrSrc The start of the source buffer.
	/// @param dataLen The limit on bytes to copy (not necessary the length of the source buffer).
	/// @param clear Erase the target buffer before copying.
	virtual unsigned replicateBuffer(const ACE_UINT8* ptrSrc, const size_t dataLen, const bool clear = true);

	/// Copy new data from a buffer to the specified internal location anywhere in the internal buffer.
	/// @param ptrDst The internal location where the new data will begin.
	/// @param ptrSrc A pointer to the buffer to be duplicated.
	/// @param dataLen The amount of data to copy from the buffer.
	/// @throw BufferOverflow If the new buffer size is longer than the available capacity.
	void copyBuffer(ACE_UINT8* ptrDst, const ACE_UINT8 *ptrSrc, const size_t dataLen);

	/// Copy new data from a buffer to the specified internal location anywhere in the internal unit.
	/// @param ptrDst The internal location where the new data will begin.
	/// @param ptrSrc A pointer to the buffer to be duplicated.
	/// @param dataLen The amount of data to copy from the buffer.
	/// @throw BufferOverflow If the new buffer size is longer than the available capacity in the unit.
	void copyUnit(ACE_UINT8* ptrDst, const ACE_UINT8 *ptrSrc, const size_t dataLen);

	/// Copy the entire buffer of the supplied NetworkData object to the specified internal location.
	/// @param ptrDst The internal location where the new data will begin.
	/// @param data The NetworkData object to copy from.
	void copyBuffer(ACE_UINT8* ptrDst, const NetworkData* data);

	/// Copy the entire unit of the supplied NetworkData object to the specified internal location.
	/// @param ptrDst The internal location where the new data will begin.
	/// @param data The NetworkData object to copy from.
	void copyUnit(ACE_UINT8* ptrDst, const NetworkData* data) ;

	/// In derived classes, this may return the length of the buffer as extracted from
	/// the data itself.
	/// @throw nd_error When invoked by the base class.
	virtual size_t expectedLength() const;

	/// Accessor for _isInitialized.
	bool isInitialized() const;

	/// Tested when an operation is about to be performed on the data.
	/// @throw PrematureAccess If _isInitialized is false.
	void throwIfUninitialized() const;

	/// Tested when the buffer is expected to be empty.
	/// @throw RedundantInitialization If _isInitialized is true.
	void throwIfInitialized() const;

	/// Derived classes use this, but it will never throw in the base class because
	/// isExpectedType() always returns true.
	/// @throw MalformedPayload If the payload type is not what it's supposed to be.
    void throwIfBadType(const std::string& expectedType);

	/// Largest expected unit. Derived classes will frequently override.
	static ACE_UINT32 maxSize();

	/// The number of bytes required to extract the expected length
	/// from the encapsulated payload.
	static ACE_UINT8 minBytesToDetermineLength();

	/// To be overridden by descendants; always returns 0 for NetworkData.
	virtual size_t minimumLength() const;

	/// Swap the byte order of a short due to endian problems.
	static ACE_INT16 swap16(const ACE_UINT16 val);

	/// Swap the byte order of a long due to endian problems.
	static ACE_INT32 swap32(const ACE_UINT32 val);

	/// Swap the byte order of a long long due to endian problems.
	static ACE_INT32 swap32(const ACE_UINT64 val);

	/// Compare buffer contents.
	/// @param other Another NetworkData object to check against.
	/// @return True if the units are the same size and have identical contents.
	bool operator ==(NetworkData& other);

	/// Negate comparison of buffer contents.
	/// @param other Another NetworkData object to check against.
	/// @return True if the units are different sizes or have mismatched contents.
	bool operator != (NetworkData& other);

	/// @brief Extract a set of bits from memory and convert it to an unsigned integer value.
	/// @param ptr Pointer to the start of memory.
	/// @param bit_offset The number of bits from ptr that the pertitent field begins.
	/// @param bit_length The total number of bits to gather, up to 64.
	/// @param byteswap Whether the order of the bytes should be inverted; choose depending on endianness.
	/// @return A 64-bit unsigned integer containing the value of the bit field.
	static ACE_UINT64 bits2uint(ACE_UINT8* ptr, const unsigned bit_offset, const unsigned bit_length,
		const bool byteswap = true);

	/// @brief Store a specified number of bits from an integer value into a buffer without disturbing other bits.
	/// @param val The unsigned integer to store, up to 64 bits in size.
	/// @param ptr A pointer to preallocated memory to store the value in.
	/// @param bit_offset The number of bits from the buffer start that the bit field will begin.
	/// @param bit_length The total number of bits to store, up to 64.
	/// @param byteswap Whether the order of the bytes should be inverted.
	static void uint2bits(const ACE_UINT64 val, ACE_UINT8* ptr, const unsigned bit_offset,
		const unsigned bit_length, const bool byteswap = true);

	/// Read-only accessor for _timeStamp.
	ACE_Time_Value getTimeStamp() const;

	/// Updates _timeStamp to the current time.
	void setTimeStamp();

	/// Sets the time stamp to the provided value.
	void setTimeStamp(const ACE_Time_Value& newTime);

	/// Pointer to the payload, to be used by descendants.
	/// @throw nd_error If not overridden.
	virtual ACE_UINT8* ptrData() const;

	/// Length of the payload in octets, to be used by descendants.
	/// @throw nd_error If not overridden.
	virtual size_t getDataLength() const;

	/// Expected length of the payload in octets, to be used by descendants.
	/// @throw nd_error If not overridden.
	virtual size_t getExpectedDataLength() const;

	/// Length of the header in octets, to be used by descendants.
	/// @throw nd_error If not overridden.
	virtual size_t getHeaderLength() const;

	/// Determine whether the expected length is equivalent to the size.
	/// Won't work on a base NetworkData.
	virtual	bool isTruncated() const;

	/// @brief Copy the supplied buffer into the start of the data field.
	/// Assumes the header has enough information to describe the size of the data field.
	/// @param data Pointer to the buffer to copy.
	/// @param dataLen Size of the buffer to copy.
	/// @param allowResize Permit the unit to be extended to make room for the data.
	/// @throw BufferOverflow If length is greater than the data field size and allowResize is false.
	virtual void setData(ACE_UINT8* data, const size_t dataLen, const bool allowResize = false);

	/// Copies the buffer from the provided NetworkData into our data area,
	/// then repoints that object to the inside of our buffer.
	/// @param data Pointer to the NetworkData object to incorporate.
	/// @param allowResize Permit the unit to be extended to make room for the data.
	virtual void setData(NetworkData* data, const bool allowResize = false);

	/// Fill the data field with the specified pattern, repeating if there is room.
	/// @param pattern Pointer to a buffer containing the pattern.
	/// @param patternLen The length of the pattern up to the size of the buffer.
	/// @param offset Where to begin filling.
	virtual void setDataToPattern(ACE_UINT8* pattern, const size_t& patternLen, const size_t& offset = 0);

	/// Fill the data field with the specified pattern, repeating if there is room.
	/// @param pattern Unit containing the pattern in its buffer.
	/// @param offset Where to begin filling.
	virtual void setDataToPattern(const NetworkData* pattern, const size_t& offset = 0);

	/// Increase the internal buffer by bufLen, and prepend the provided buffer.
	/// @param buffer The buffer to add.
	/// @param bufLen The amount of data to prepend.
	virtual void prepend(const ACE_UINT8* buffer, const size_t bufLen);

	/// Increase the internal buffer and prepend the provided complete NetworkData.
	/// @param data Pointer to the NetworkData to prepend.
	virtual void prepend(const NetworkData* data);

	/// Increase the internal buffer by bufLen, and append the provided buffer.
	/// @param buffer The buffer to add.
	/// @param bufLen The amount of data to add.
	virtual void append(const ACE_UINT8* buffer, const size_t bufLen);

	/// Increase the internal buffer and add the provided complete NetworkData.
	/// @param data Pointer to the NetworkData to add.
	virtual void append(const NetworkData* data);

	/// @brief Generate an object to operate on the protocol data unit contained in the payload.
	/// A new buffer is NOT created to duplicate the provided one; the two wrappers operate
	/// on a shared buffer.
	/// @param pduSize If not specified, will be determined by the container's getDataLength().
	/// @param pduStart If not specified, will be determined by the container's ptrData();
	/// @return A new object of the specified NetworkDataType type.
	template <class NetworkDataType>
	NetworkDataType* wrapInnerPDU(size_t pduSize = 0, ACE_UINT8* pduStart = 0) {
		if ( ! pduSize ) pduSize = getDataLength();
		if ( ! pduStart ) pduStart = ptrData();

		return new NetworkDataType((NetworkDataType*) this, false, pduStart - ptrUnit(), pduSize);

	}

	// -- TYPE-RELATED FUNCTIONS ------------------------------------------------------------
	// Functions that identify the type contained in the buffer to specific subclasses.

	/// Derived classes override this to return a 16-bit value to store
	/// as the Ethernet type in an Ethernet frame. The base class returns the length
	/// if less than or equal to 1500, or zero otherwise.
	virtual ACE_UINT16 enetType() const;

	/// Confirm that the data contained in the buffer matches the type
	/// of the class. For NetworkData this always returns truel; derived classes
	/// override this.
	virtual bool isExpectedType();

	/// Get the Internet Protocol Extension header value for use in an Encapsulation Packet.
	/// @return -1 if there is no IPE defined for this data type.
	virtual ACE_INT64 getIPE_Header_Val() const;

	/// A short string describing the data type (usually just the name),
	/// overridden by derived classes.
	virtual std::string typeStr() const;
	// -------------------------------------------------------------------------------------

	// -- UNIT/BUFFER SIZE-RELATED FUNCTIONS -----------------------------------------------
	/// Get the length of the portion of the buffer that this type of unit occupies.
	size_t getUnitLength() const;

	/// @brief Resize the length of the unit, which may be smaller than the total buffer size.
	/// Important the rd_ptr is preserved and wr_ptr is positioned to the new end.
	/// @param newLen The size to change to.
	/// @param keepLargerBuffer If true, don't resize the buffer when newLen is smaller than current length.
	/// @return The length delta.
	int setUnitLength(const size_t newLen, const bool keepLargerBuffer = false);

	/// Return the entire length of all unit (not buffer) parts added together.
	size_t getTotalUnitLength() const;

	/// Get the total length of the buffer, which may contain data unused by this object.
	size_t getBufferSize() const;

	/// @brief Resize the buffer. Will affect objects sharing the same buffer.
	/// @param newLen The new size of the buffer.
	/// @return The size delta.
	int setBufferSize(const size_t newLen);

	/// Return the entire length of all buffer (not unit) parts added together.
	size_t getTotalBufferSize() const;
	// -------------------------------------------------------------------------------------

	// -- CONTINUATION FIELD AND REFERENCE COUNT-RELATED FUNCTIONS -------------------------
	/// Take our internal linked list of messages and combine them into one data area.
	/// Assumes this is the top level message.
	virtual void flatten();

	/// Get a pointer to the final populated NetworkData in the list.
	virtual NetworkData* tail();

	/// Determine if the total size of all parts exactly matches the expected length.
	/// @throw LogicError If there's not enough date to determine the expected length.
	virtual bool hasAllParts() const;

	/// Get the next part in the continuation field.
	NetworkData* getNextPart() const;

	/// Set the next part in the continuation field.
	void setNextPart(NetworkData* newPart);

	/// Decrease the reference count by one. If it reaches 0, release the entire continuation chain.
	ACE_Message_Block* release();

	/// Create a shallow copy of the buffer and increase the reference count by one.
	ACE_Message_Block* duplicate() const;

	/// Read-only accessor to the current reference count.
	int reference_count() const;
	// -------------------------------------------------------------------------------------

	// -- UTILITY FUNCTIONS ----------------------------------------------------------------
	/// @brief Find a needle in the haystack of the NetworkData unit space.
	/// @param needle The pattern buffer to search for.
	/// @param needleLen The size of the pattern buffer.
	/// @param initialOffset Where to start.
	/// @return The byte offset in haystack of the start of needle.
	/// @throw nd_error If needle is not found.
	size_t find(const ACE_UINT8* needle, const size_t needleLen, const size_t initialOffset = 0);

	/// @brief Find a needle in the haystack of the NetworkData unit space.
	/// @param needle The pattern buffer to search for as a NetworkData object.
	/// @param initialOffset Where to start.
	/// @return The byte offset in haystack of the start of needle.
	/// @throw nd_error If needle is not found.
	size_t find(const NetworkData* needle, const size_t initialOffset = 0);

	/// @brief Find a needle in the haystack of the NetworkData unit space.
	/// @param needle The pattern buffer to search for.
	/// @param needleLen The size of the pattern buffer.
	/// @param initialOffset Where to start.
	/// @return Pointer to the offset in haystack of the start of needle.
	/// @throw nd_error If needle is not found.
	ACE_UINT8* findPtr(const ACE_UINT8* needle, const size_t needleLen, const size_t initialOffset = 0);

	/// @brief Find a needle in the haystack of the NetworkData unit space.
	/// @param needle The pattern buffer to search for as a NetworkData object.
	/// @param initialOffset Where to start.
	/// @return Pointer to the offset in haystack of the start of needle.
	/// @throw nd_error If needle is not found.
	ACE_UINT8* findPtr(const NetworkData* needle, const size_t initialOffset = 0);

	/// @brief Convert a number to a human-readable format.
	/// @param num The number to process.
	/// @param precision The number of decimal places to include.
	/// @param spacer A string (such as spacing) to insert between the number and the unit.
	/// @param useIEC If true, do a International Electrotechnical Commission conversion (gibi-, mebi-, etc) instead of IS (giga-, mega-)
	/// @return A string representing the lowest number greater than 1 with the specified unit appended (T, G, M, k, etc).
	static std::string num2HumanReadable(const double num, const int precision = 2, const std::string spacer = "", const bool useIEC = false);

	/// @brief Print the bytes in the buffer to stdout, twenty space-delimited hex bytes per row.
	/// @param max The maximum number of octets to print.
	/// @param debugOnly Only perform when compiled with debugging enabled.
	virtual void dump(const size_t max = 0, const bool debugOnly = true);

	/// @brief Print the bits in the buffer to stdout as a continuous stream of 0s and 1s.
	/// @param maxBits The maximum number of bits to print.
	/// @param colWidth The number of bits to print per line.
	/// @param debugOnly Only perform when compiled with debugging enabled.
	virtual void dumpBits(const size_t maxBits = 0, const size_t colWidth = 72, const bool debugOnly = true);

	/// @brief Create a std::string and copy at least part of the unit into it.
	/// Won't work well if there are non-printable characters.
	/// @param offset Offset from beginning of the unit.
	/// @param length Length to convert (default to end of unit).
	std::string getAsString(const size_t offset = 0, const int length = -1);

	/// @brief Copy the contents of a std::string buffer to a point in the unit.
	/// Some resizing will happen if the string length doesn't exactly match that of the area it's being copied to.
	/// @param stringData String to copy from.
	/// @param unitOffset Offset from the start of the unit to begin writing.
	/// @param stringOffset Offset from the start of the string to begin reading.
	/// @param length Length of the string data to copy if not the full buffer. If too long, just copy to the end.
	ACE_UINT8* setFromString(const std::string& stringData, const size_t unitOffset = 0, const size_t stringOffset = 0, const int length = -1);

	/// @brief Return the value of a single bit from a buffer at the specified bit offset.
	/// @param buf Pointer to the buffer to examine.
	/// @param bitOffset Location of the bit value to return.
	static bool getBit(ACE_UINT8* buf, const size_t bitOffset);

	/// @brief Return the value of a single bit from a buffer at the specified bit offset.
	/// The MSB and LSB are reversed with respect to getBit().
	/// @param buf Pointer to the buffer to examine.
	/// @param bitOffset Location of the bit value to return.
	static bool getBitRev (ACE_UINT8* buf, const size_t bitOffset);

	/// @brief Return the value of a single bit from the unit at the specified bit offset.
	/// @param bitOffset Location of the bit value in the unit to return.
	bool getBit(const size_t bitOffset);

	/// @brief Return the value of a single bit from the unit at the specified bit offset.
	/// The MSB and LSB are reversed with respect to getBit().
	/// @param bitOffset Location of the bit value in the unit to return.
	bool getBitRev(const size_t bitOffset);

	/// @brief Set the value of a single bit in a buffer at the specified bit offset.
	/// @param buf Pointer to the buffer to modify.
	/// @param bitOffset Location of the bit value to change.
	/// @param val The value to set.
	/// @return The value of the bit that was just set.
	static bool setBit(ACE_UINT8* buf, const size_t bitOffset, const bool val);

	/// @brief Set the value of a single bit in the unit at the specified bit offset.
	/// @param bitOffset Location of the bit value to change.
	/// @param val The value to set.
	/// @return The value of the bit that was just set.
	bool setBit(const size_t bitOffset, const bool val);

	/// @brief Set the value of a single bit in a buffer at the specified bit offset.
	/// The MSB and LSB are reversed with respect to setBit().
	/// @param buf Pointer to the buffer to modify.
	/// @param bitOffset Location of the bit value to change.
	/// @param val The value to set.
	/// @return The value of the bit that was just set.
	static bool setBitRev(ACE_UINT8* buf, const size_t bitOffset, const bool val);

	/// @brief Set the value of a single bit in the unit at the specified bit offset.
	/// The MSB and LSB are reversed with respect to setBit().
	/// @param bitOffset Location of the bit value to change.
	/// @param val The value to set.
	/// @return The value of the bit that was just set.
	bool setBitRev(const size_t bitOffset, const bool val);

	/// @brief Copy data by specifying bits instead of octets.
	/// This is SLOW so should not be used for speed-sensitive operations.
	/// @param srcBitOffset The first bit from the start of src to begin copying from.
	/// @param dstBitOffset The first bit from the start of dst to begin copying into.
	/// @param bitLen The number of bits to copy.
	/// @param src The buffer to copy from. Must be at least as large as srcBitOffset + bitLen.
	/// @param dst The buffer to copy into. Must be at least as large as dstBitOffset + bitLen.
	static void bitCopy(const size_t srcBitOffset, const size_t dstBitOffset, const size_t bitLen, ACE_UINT8* src, ACE_UINT8* dst);

	/// @brief Copy data by specifying bits instead of octets, using this NetworkData as the source.
	/// This is SLOW so should not be used for speed-sensitive operations.
	/// @param srcBitOffset The first bit from the start of src to begin copying from.
	/// @param dstBitOffset The first bit from the start of dst to begin copying into.
	/// @param bitLen The number of bits to copy.
	/// @param dst The buffer to copy into. Must be at least as large as dstBitOffset + bitLen.
	void bitCopyTo(const size_t srcBitOffset, const size_t dstBitOffset, const size_t bitLen, ACE_UINT8* dst);

	/// @brief Copy data by specifying bits instead of octets, using this NetworkData as the source.
	/// This is SLOW so should not be used for speed-sensitive operations.
	/// @param srcBitOffset The first bit from the start of src to begin copying from.
	/// @param dstBitOffset The first bit from the start of dst to begin copying into.
	/// @param bitLen The number of bits to copy.
	/// @param dst The NetworkData whose Unit will be copied into. Must be at least as large as dstBitOffset + bitLen.
	void bitCopyTo(const size_t srcBitOffset, const size_t dstBitOffset, const size_t bitLen, NetworkData* dst);

	/// @brief Copy data by specifying bits instead of octets, using this NetworkData as the destination.
	/// This is SLOW so should not be used for speed-sensitive operations.
	/// @param srcBitOffset The first bit from the start of src to begin copying from.
	/// @param dstBitOffset The first bit from the start of dst to begin copying into.
	/// @param bitLen The number of bits to copy.
	/// @param src The buffer to copy from. Must be at least as large as srcBitOffset + bitLen.
	void bitCopyFrom(const size_t srcBitOffset, const size_t dstBitOffset, const size_t bitLen, ACE_UINT8* src);

	/// @brief Copy data by specifying bits instead of octets, using this NetworkData as the destination.
	/// This is SLOW so should not be used for speed-sensitive operations.
	/// @param srcBitOffset The first bit from the start of src to begin copying from.
	/// @param dstBitOffset The first bit from the start of dst to begin copying into.
	/// @param bitLen The number of bits to copy.
	/// @param src The NetworkData whose Unit will be copied from. Must be at least as large as srcBitOffset + bitLen.
	void bitCopyFrom(const size_t srcBitOffset, const size_t dstBitOffset, const size_t bitLen, NetworkData* src);

	// -------------------------------------------------------------------------------------

protected:
	/// Convert two bytes in a buffer to a 16-bit unsigned integer.
	/// @param ptr Points to the start of the buffer.
	/// @param swap Swap the byte order if true (default).
	static ACE_UINT16 to_u16_(ACE_UINT8* ptr, const bool swap = true);

	/// Convert a 16-bit unsigned integer to two bytes in a pre-allocated buffer.
	/// @param val The integer to convert.
	/// @param buf Points to the start of the pre-allocated buffer.
	/// @param swap Swap the byte order if true (default).
	static ACE_UINT16 from_u16_(const ACE_UINT16 val, ACE_UINT8* buf, const bool swap = true);

	/// Convert four bytes in a buffer to a 32-bit unsigned integer.
	/// @param ptr Points to the start of the buffer.
	/// @param swap Swap the byte order if true (default).
	static ACE_UINT32 to_u32_(const ACE_UINT8* ptr, const bool swap = true);

	/// Convert a 32-bit unsigned integer to four bytes in a pre-allocated buffer.
	/// @param val The integer to convert.
	/// @param buf Points to the start of the pre-allocated buffer.
	/// @param swap Swap the byte order if true (default).
	static ACE_UINT32 from_u32_(const ACE_UINT32 val, ACE_UINT8* buf, const bool swap = true);

	/// Convert eight bytes in a buffer to a 64-bit unsigned integer.
	/// @param ptr Points to the start of the buffer.
	/// @param swap Swap the byte order if true (default).
	static ACE_UINT64 to_u64_(const ACE_UINT8* ptr, const bool swap = true);

	/// Convert a 64-bit unsigned integer to eight bytes in a pre-allocated buffer.
	/// @param val The integer to convert.
	/// @param buf Points to the start of the pre-allocated buffer.
	/// @param swap Swap the byte order if true (default).
	static ACE_UINT32 from_u64_(const ACE_UINT64 val, ACE_UINT8* buf, const bool swap = true);

	/// An accessor to _isInitialized.
	void setInitialized_(const bool initVal = true );

private:
	/// Set to true when the buffer has been allocated. Most methods will
	/// throw an exception if operations are attempted on the object and
	/// this is false.
    bool _isInitialized;

    /// Set when the object was initialized, but can be updated later.
    ACE_Time_Value _timeStamp;

	/// Restrict constructor from use (does nothing).
	NetworkData (ACE_Data_Block *, Message_Flags flags=0, ACE_Allocator *message_block_allocator=0);

	/// Restrict constructor from use (does nothing).
	NetworkData (const char *data, size_t size=0, unsigned long priority=ACE_DEFAULT_MESSAGE_BLOCK_PRIORITY);

	/// Restrict constructor from use (does nothing).
	NetworkData (const ACE_Message_Block &mb, size_t align);

}; // class NetworkData

/// @brief Delete the object, and only the data block if it has zero references left.
/// This should be called for any NetworkData or ACE_Message_Block object on the heap
/// or else memory leaks will occur due to shallow copies.
/// @param ndPtr Pointer to the object to delete.
/// @return Always returns 0.
template <class NetworkDataType>
NetworkDataType* ndSafeRelease(NetworkDataType*& ndPtr) {
	if ( ndPtr != 0 ) {
		ndPtr->release();
		ndPtr = 0;
	}

	return ndPtr;
}

} // namespace nasaCE

#endif // NASANETWORKDATA_HPP
