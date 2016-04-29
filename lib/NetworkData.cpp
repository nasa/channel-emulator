/* -*- C++ -*- */

//=============================================================================
/**
 * @file   NetworkData.cpp
 * @author Tad Kollar  
 *
 * $Id: NetworkData.cpp 2478 2014-02-12 14:00:13Z tkollar $
 * Copyright (c) 2008.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "nd_macros.hpp"
#include "NetworkData.hpp"
#include "utils.hpp"

#include <ace/Lock.h>
#include <ace/Lock_Adapter_T.h>
#include <ace/Log_Msg.h>
#include <ace/Thread_Semaphore.h>
#include <iostream>
#include <iomanip>

namespace nasaCE {

// ACE_Lock* NetworkData::lockingStrategy = 0;

NetworkData::NetworkData(): ACE_Message_Block(/* allocator*/ ), _isInitialized(false) {
	setTimeStamp();
	data_block()->locking_strategy(0);
}

NetworkData::NetworkData(const size_t bufferSize, const ACE_UINT8* buffer /* = 0 */ ):
	ACE_Message_Block(bufferSize),
	_isInitialized(true) {

	setTimeStamp();

	// Stopped using replicateBuffer because size checking is unnecessary.
	if (buffer) copy(reinterpret_cast<const char *>(buffer), bufferSize);

	// Stopped using clearBuffer.
	else if (bufferSize) ACE_OS::memset(base(), 0, bufferSize);

	// Make the unit the same size as the buffer.
	rd_ptr(base());
	wr_ptr(end());
}

NetworkData::NetworkData(const NetworkData* other,
	const bool doDeepCopy /* = true */,
	const size_t offset /* = 0 */,
	const size_t newLen /* = 0 */ ): ACE_Message_Block(/* allocator */) {

	if ( doDeepCopy ) {
		if (data_block()->locking_strategy()) {
			ACE_Lock* lockingStrategy = data_block()->locking_strategy();
			data_block()->locking_strategy(0);
			delete lockingStrategy;
		}
		deepCopy(other, offset, newLen);
	}
	else {
		// Initialize the locking strategy.
		if ( other->data_block()->locking_strategy() == 0 ) {
			ACE_Lock* lockingStrategy = new ACE_Lock_Adapter<ACE_Thread_Semaphore>();
			other->data_block()->locking_strategy(lockingStrategy);
		}

		_isInitialized = other->_isInitialized;
		data_block(other->data_block()->duplicate());
		rd_ptr(other->rd_ptr() + offset);
		wr_ptr(rd_ptr() + newLen);
	}

	setTimeStamp(other->getTimeStamp());
}

NetworkData::~NetworkData() {
	if (data_block() && data_block()->locking_strategy()) {
		ACE_Lock* lockingStrategy = data_block()->locking_strategy();
		data_block()->locking_strategy(0);
		delete lockingStrategy;
	}
}

ACE_UINT8* NetworkData::ptrBuffer(const size_t offset /* = 0 */) const {
	throwIfUninitialized();
	return reinterpret_cast<ACE_UINT8*>(base() + offset);
}

ACE_UINT8* NetworkData::ptrUnit(const size_t offset /* = 0 */) const {
	throwIfUninitialized();
	return reinterpret_cast<ACE_UINT8*>(rd_ptr() + offset);
}

void NetworkData::deepCopy(const NetworkData* other, const size_t bufferOffset /* = 0 */,
	const size_t dataLen /* = 0 */) {

	if ( !other ) throw nd_error("Attempt to copy from null pointer.");

	const size_t bufLen = dataLen? dataLen : other->getBufferSize();

	setBufferSize(bufLen);
	copyBuffer(reinterpret_cast<ACE_UINT8*>(base()),
		reinterpret_cast<ACE_UINT8*>(other->base() + bufferOffset), bufLen);

	_isInitialized = other->isInitialized();

	if ( ! (bufferOffset || dataLen) ){
		rd_ptr(base() + (other->rd_ptr() - other->base()));
		wr_ptr(base() + (other->wr_ptr() - other->base()));
	}
	else { rd_ptr(base()); wr_ptr(end()); }
}

void NetworkData::clearBuffer(const size_t newLen /* = 0 */, const bool resetUnit /* = true */) {
	if ( ! newLen ) throwIfUninitialized();
	else {
		_isInitialized = true;
		setBufferSize(newLen);
	}

	ACE_OS::memset(base(), 0, size());
	if ( resetUnit ) { rd_ptr(base()); wr_ptr(end()); }
}

void NetworkData::clearUnit(const size_t newLen /* = 0 */ ) {
	if ( newLen ) setUnitLength(newLen);
	else throwIfUninitialized();
	ACE_OS::memset(rd_ptr(), 0, getUnitLength());
}

unsigned NetworkData::replicateBuffer(const ACE_UINT8* ptrSrc, const size_t dataLen, const bool clear /* = true */) {
  	ACE_TRACE("NetworkData::replicateBuffer");
  	_isInitialized = true;
  	if (clear) { clearBuffer(dataLen); }
  	else { setBufferSize(dataLen); }
  	rd_ptr(base());
	wr_ptr(end());
	if (copy(reinterpret_cast<const char *>(ptrSrc), dataLen) < 0 )
		throw BufferOverflow("Data copy failed, supplied buffer length exceeds available capacity.");

	return getBufferSize();
}

void NetworkData::copyBuffer(ACE_UINT8* ptrDst, const ACE_UINT8 *ptrSrc, const size_t dataLen) {
	if ( ptrDst < reinterpret_cast<ACE_UINT8*>(base()) )
		throw BufferOverflow("Data copy failed, destination is below range.");

	if ( ptrDst + dataLen > reinterpret_cast<ACE_UINT8*>(end()))
		throw BufferOverflow("Data copy failed, supplied buffer length exceeds available capacity.");

	ACE_OS::memcpy(ptrDst, ptrSrc, dataLen);
}

void NetworkData::copyUnit(ACE_UINT8* ptrDst, const ACE_UINT8 *ptrSrc, const size_t dataLen) {
	if ( ptrDst < reinterpret_cast<ACE_UINT8*>(rd_ptr()) )
		throw BufferOverflow("Data copy failed, destination is below unit range.");

	if ( ptrDst + dataLen > reinterpret_cast<ACE_UINT8*>(wr_ptr()))
		throw BufferOverflow("Data copy failed, supplied buffer length exceeds available capacity in unit.");

	ACE_OS::memcpy(ptrDst, ptrSrc, dataLen);
}

void NetworkData::copyBuffer(ACE_UINT8* ptrDst, const NetworkData* data) {
	copyBuffer(ptrDst, data->ptrUnit(), data->size());
}

void NetworkData::copyUnit(ACE_UINT8* ptrDst, const NetworkData* data) {
	copyUnit(ptrDst, data->ptrUnit(), data->length());
}


size_t NetworkData::expectedLength() const {
	throw nd_error(typeStr() + std::string(" used NetworkData expectedLength()!"));
}

bool NetworkData::isInitialized() const { return _isInitialized; }

void NetworkData::throwIfUninitialized() const {
	if ( ! _isInitialized )
		throw PrematureAccess(PrematureAccess::msg(typeStr()));
}

void NetworkData::throwIfInitialized() const {
	if ( _isInitialized )
		throw RedundantInitialization(RedundantInitialization::msg(typeStr()));
}

void NetworkData::throwIfBadType(const std::string& expectedType) {
	if ( ! isExpectedType() ) {
		throw MalformedPayload(MalformedPayload::msg(typeStr(), expectedType));
	}
}

ACE_UINT32 NetworkData::maxSize() { return -1; }

ACE_UINT8 NetworkData::minBytesToDetermineLength() { return 4; }

size_t NetworkData::minimumLength() const { return 0; }

ACE_INT16 NetworkData::swap16(const ACE_UINT16 val) { return _swap16(val); }

ACE_INT32 NetworkData::swap32(const ACE_UINT32 val) { return _swap32(val); }

ACE_INT32 NetworkData::swap32(const ACE_UINT64 val) { return _swap64(val); }

bool NetworkData::operator ==(NetworkData& other) {
	return (getUnitLength() == other.getUnitLength() && ACE_OS::memcmp(ptrUnit(), other.ptrUnit(), getUnitLength()) == 0 );
}

bool NetworkData::operator != (NetworkData& other) { return !( *this == other ); }

ACE_UINT64 NetworkData::bits2uint(ACE_UINT8* ptr, const unsigned bit_offset,
	const unsigned bit_length, const bool byteswap) {

	if ( bit_length > 64 ) {
		throw nd_error("Length of bit field is too long; 64 max.");
	}

	ACE_UINT8* firstOctetP = ptr + (bit_offset/8);
	ACE_UINT8 firstOctetBits = 8 - (bit_offset % 8);
	ACE_UINT8 firstOctetAnd = 0xFF >> (8 - firstOctetBits);

	ACE_UINT8* middlePtr = firstOctetP + 1;
	ACE_UINT8 middleOctets = (bit_length - firstOctetBits) / 8;

	ACE_UINT8* lastOctetP = middlePtr + middleOctets;
	ACE_UINT8 lastOctetBits = (bit_length - firstOctetBits) % 8;
	ACE_UINT8 lastOctetAnd = 0xFF << (8 - lastOctetBits);

	ACE_UINT64 val = 0;
	ACE_UINT8* valP = (ACE_UINT8*) &val;
	short pos;
	if (byteswap) {
		pos = 0;
		*(valP + (pos++)) = *lastOctetP & lastOctetAnd;
		for ( int m = 0; m < middleOctets; m++ ) {
			*(valP + (pos++)) = *(middlePtr + m);
		}
	}
	else {
		pos = 7;
		*(valP + (pos--)) = *lastOctetP & lastOctetAnd;
		for ( int m = middleOctets - 1; m > 0 ; m-- ) {
			*(valP + (pos--)) = *(middlePtr + m);
		}
	}

	*(valP + pos) = *firstOctetP & firstOctetAnd;

	val >>= 8 - lastOctetBits;

	return val;
}

void NetworkData::uint2bits(const ACE_UINT64 val, ACE_UINT8* ptr, const unsigned bit_offset,
		const unsigned bit_length, const bool byteswap) {

	ACE_UINT64 tmpVal = val;
	ACE_UINT8* valP = (ACE_UINT8*) &tmpVal;

	ACE_UINT8* firstOctetP = ptr + (bit_offset/8);

	ACE_UINT8 firstOctetBits = 8 - (bit_offset % 8);
	ACE_UINT8 firstOctetAnd = 0xFF >> (8 - firstOctetBits);

	ACE_UINT8* middlePtr = firstOctetP + 1;
	ACE_UINT8 middleOctets = (bit_length - firstOctetBits) / 8;

	ACE_UINT8* lastOctetP = middlePtr + middleOctets;
	ACE_UINT8 lastOctetBits = (bit_length - firstOctetBits) % 8;
	ACE_UINT8 lastOctetAnd = 0xFF << (8 - lastOctetBits);

	tmpVal <<= (8 - lastOctetBits);

	short pos;

	if (byteswap) {
		pos = 0;
		*lastOctetP = (*lastOctetP & ~lastOctetAnd) | *(valP + (pos++));
		for ( int m = 0; m < middleOctets; m++ ) {
			*(middlePtr + m) = *(valP + (pos++));
		}
	}
	else {
		pos = 7;
		*lastOctetP = (*lastOctetP & ~lastOctetAnd) | *(valP + (pos--));
		for ( int m = 0; m < middleOctets; m++ ) {
			*(middlePtr + m) = *(valP + (pos--));
		}
	}

	*firstOctetP = (*firstOctetP & ~firstOctetAnd) | *(valP + pos);
}

	ACE_Time_Value NetworkData::getTimeStamp() const { return _timeStamp; }

	void NetworkData::setTimeStamp() { _timeStamp = ACE_High_Res_Timer::gettimeofday_hr(); }

	void NetworkData::setTimeStamp(const ACE_Time_Value& newTime) { _timeStamp = newTime; }

	ACE_UINT8* NetworkData::ptrData() const { throw nd_error(typeStr() + std::string(" used NetworkData ptrData()!")); }

	size_t NetworkData::getDataLength() const { throw nd_error(typeStr() + std::string(" used NetworkData getDataLength()!")); }

	size_t NetworkData::getExpectedDataLength() const { throw nd_error(typeStr() + std::string(" used NetworkData getExpectedDataLength()!")); }

	size_t NetworkData::getHeaderLength() const { throw nd_error(typeStr() + std::string(" used NetworkData getHeaderLength()!")); }

	bool NetworkData::isTruncated() const { return expectedLength() != size(); }

void NetworkData::setData(ACE_UINT8* data, const size_t dataLen, const bool allowResize /* = false */) {
	if ( dataLen <= getDataLength() ) copyUnit(ptrData(), data, dataLen);
	else if (allowResize) {
		// Extend the unit. "Trailer" here is anything after the data field.
		const size_t deltaLength = dataLen - getDataLength();
		const size_t trailerLength = (ptrUnit() + getUnitLength()) - (ptrData() + getDataLength());
		setUnitLength(getUnitLength() + deltaLength);

		// Relocate the trailer, if there is one.
		if ( trailerLength ) {
			const ACE_UINT8* ptrTrailer = ptrData() + dataLen - deltaLength;
			copyUnit(ptrData() + dataLen, ptrTrailer, trailerLength);
		}

		// Finally complete the copy.
		copyUnit(ptrData(), data, dataLen);
	}
	else throw BufferOverflow(BufferOverflow::msg("Data Field", dataLen, getDataLength()));
}

void NetworkData::setData(NetworkData* data, const bool allowResize /* = false */) {
	setData(data->ptrUnit(), data->getUnitLength(), allowResize);
}

void NetworkData::setDataToPattern(ACE_UINT8* pattern, const size_t& patternLen, const size_t& offset /* = 0 */) {
	const size_t fillCount = getDataLength() - offset;

	for ( size_t i = 0; i < fillCount; i += patternLen ) {
		copyUnit(ptrData() + offset + i, pattern, GetMin<size_t>(patternLen, fillCount - i));
	}
}

void NetworkData::setDataToPattern(const NetworkData* pattern, const size_t& offset /* = 0 */) {
	setDataToPattern(pattern->ptrUnit(), pattern->getUnitLength(), offset);
}

void NetworkData::prepend(const ACE_UINT8* buffer, const size_t bufLen) {
	size_t oldLength = getUnitLength();
	size_t newLength = oldLength + bufLen;

	setUnitLength(newLength);
	ACE_OS::memmove(ptrUnit() + bufLen, ptrUnit(), oldLength);
	ACE_OS::memcpy(ptrUnit(), buffer, bufLen);
}

void NetworkData::prepend(const NetworkData* data) {
	prepend(data->ptrUnit(), data->getUnitLength());
}

void NetworkData::append(const ACE_UINT8* buffer, const size_t bufLen) {
	size_t offset = getUnitLength();

	setUnitLength(offset + bufLen);

	copyUnit(ptrUnit() + offset, buffer, bufLen);
}

void NetworkData::append(const NetworkData* data) {
	append(data->ptrUnit(), data->getUnitLength());
}

// -- CONTINUATION FIELD AND REFERENCE COUNT-RELATED FUNCTIONS -------------------------

void NetworkData::flatten() {
	// ND_DEBUG("flatten(): Changing unit length from %d to %d.\n", getUnitLength(), getTotalUnitLength());
	ACE_UINT64 idx = getUnitLength();
	setUnitLength(getTotalUnitLength());

	// NetworkData* curData = this;

	/*
	ND_DEBUG("Contained unit descs:\n");
	while ((curData = curData->getNextPart())) {
		ND_DEBUG("Data type %s, size %d\n", curData->typeStr().c_str(), curData->getUnitLength());
	}
	*/

	while (getNextPart()) {
		NetworkData* curData = getNextPart();
		// ND_DEBUG("flatten(): Merging %d octets at offset %d\n", curData->getUnitLength(), idx);
		copyUnit(ptrUnit() + idx, curData);
		idx += curData->getUnitLength();
		setNextPart(curData->getNextPart());
		delete curData;
	};
}

NetworkData* NetworkData::tail() {
	NetworkData* cur;
	for ( cur = this; cur->getNextPart(); cur = cur->getNextPart() );
	return cur;
}

bool NetworkData::hasAllParts() const {
	if ( length() < minBytesToDetermineLength() )
		throw LogicError("hasAllParts(): Not enough data in head to determine expected size.");
	return ( total_length() == expectedLength() );
}

NetworkData* NetworkData::getNextPart() const { return dynamic_cast<NetworkData*>(cont()); }

void NetworkData::setNextPart(NetworkData* newPart) { cont(dynamic_cast<ACE_Message_Block*>(newPart)); }

ACE_Message_Block* NetworkData::release() {
	if ( data_block()->locking_strategy() ) {
		if ( data_block()->reference_count() == 1 ) {
			ACE_Lock* lockingStrategy = data_block()->locking_strategy();
			data_block()->locking_strategy(0);
			delete lockingStrategy;
		}
	}

	return ACE_Message_Block::release();
}

ACE_Message_Block* NetworkData::duplicate() const { return ACE_Message_Block::duplicate(); }

int NetworkData::reference_count() const { return ACE_Message_Block::reference_count(); }
// -------------------------------------------------------------------------------------

// -- TYPE-RELATED FUNCTIONS ------------------------------------------------------------
ACE_UINT16 NetworkData::enetType() const { return ( getUnitLength() <= 1500)? getUnitLength(): 0; }

bool NetworkData::isExpectedType() { return true; }

ACE_INT64 NetworkData::getIPE_Header_Val() const { return -1; }

std::string NetworkData::typeStr() const { return "Network Data"; }
// -------------------------------------------------------------------------------------

// -- UNIT/BUFFER SIZE-RELATED FUNCTIONS -----------------------------------------------
size_t NetworkData::getUnitLength() const { return length(); }
size_t NetworkData::getTotalUnitLength() const { return total_length(); }


size_t NetworkData::getBufferSize() const { return size(); }

size_t NetworkData::getTotalBufferSize() const { return total_size(); }

int NetworkData::setBufferSize(const size_t newLen) {
	const size_t originalSize = size();

	size(newLen);
	if (newLen < length()) length(newLen);

	return size() - originalSize;
}

int NetworkData::setUnitLength(const size_t newLen, const bool keepLargerBuffer /* = false */) {
	// Since we're setting the size, consider the unit initialized.
	_isInitialized = true;

	const size_t originalLength = length();

	// Determine the amount of memory managed outside the unit.
	const size_t externalData = size() - length();

	// Resize the buffer, setting rd_ptr and wr_ptr to the same offsets
	if (! keepLargerBuffer || newLen > originalLength) setBufferSize(externalData + newLen);

	// Set wr_ptr to reflect the new length.
	length(newLen);

	return length() - originalLength;
}

// -------------------------------------------------------------------------------------

// -- UTILITY FUNCTIONS ----------------------------------------------------------------

size_t NetworkData::find(const ACE_UINT8* needle, const size_t needleLen, const size_t initialOffset /* = 0 */) {
	for ( size_t needleOffset = initialOffset; needleOffset + needleLen <= getUnitLength(); needleOffset++ ) {
		if ( ACE_OS::memcmp(ptrUnit(needleOffset), needle, needleLen) == 0) {
			return needleOffset;
		}
	}

	throw nd_error("Search pattern not found in specified memory area.");

	return 0; // Won't get here
}

size_t NetworkData::find(const NetworkData* needle, const size_t initialOffset /* = 0 */) {
	return find(needle->ptrUnit(), needle->getUnitLength(), initialOffset);
}

ACE_UINT8* NetworkData::findPtr(const ACE_UINT8* needle, const size_t needleLen, const size_t initialOffset /* = 0 */) {
	return ptrUnit(find(needle, needleLen, initialOffset));
}

ACE_UINT8* NetworkData::findPtr(const NetworkData* needle, const size_t initialOffset /* = 0 */) {
	return ptrUnit(find(needle, initialOffset));
}

std::string NetworkData::num2HumanReadable(const double num,
	const int precision /* = 2 */, const std::string spacer /* = "" */, const bool useIEC /* = false */) {

	const double divisor = useIEC? 1024 : 1000;
	const char units[] = { 'k', 'M', 'G', 'T', 'P', 'E', 'Z', 'Y' };
	double newNum = num;
	int idx;

	for ( idx = -1; newNum > divisor && idx < 7; ++idx ) newNum /= divisor;

	std::stringstream os;
	int usePrecision = (trunc(newNum) == newNum)? 0 : precision;

	os << std::fixed << std::setprecision(usePrecision) << newNum;

	if ( idx >= 0 ) {
		os << spacer << units[idx];
		if ( useIEC ) os << 'i';
	}

	return os.str();
}

#ifdef ND_COLOR_LOG
	#define YELLOW "\033[33;1m"
	#define CYAN "\033[36;1m"
#else
	#define YELLOW ""
	#define CYAN ""
#endif

void NetworkData::dump(const size_t max /* = 0 */, const bool debugOnly /* = true */) {

  	ACE_TRACE("NetworkData::dump");

  	const ACE_Log_Priority pri = ( debugOnly ) ? LM_DEBUG : LM_INFO;
  	// const char *logPrefix = ( debugOnly ) ? ND_DEBUG_PREFIX : ND_INFO_PREFIX;

  	const unsigned octetsPerLine = 16;
	const unsigned octetsToPrint = (max && max < getUnitLength())? max : getUnitLength();
  	char strbuf[17] = "";

	using namespace std;
	ostringstream os;

	os << "Dumping " << dec << octetsToPrint << " octets of " << typeStr() << ":" << endl;

	size_t p;

	// Loop over every octet in the unit (not buffer).
	for ( p = 0; ptrUnit() + p != (ACE_UINT8*) end() && p < octetsToPrint; ++p ) {
		unsigned char pc = *(ptrUnit() + p);

		// Prepend the starting addess for the line.
		if ( ! (p % octetsPerLine) )
			os << YELLOW << "0x" << hex << setw(4) << setfill('0') << p << ND_COLOR_END << ": ";

		// Add the byte value in hex.
		os << hex << setw(2) << setfill('0') << static_cast<unsigned>(pc) << " ";

		// Add the ASCII character if printable, otherwise use a period.
		strbuf[p % octetsPerLine] = (pc > 31 && pc < 127)? pc : '.';

		// Add an extra space after 8 bytes.
		if ( p && ! ((p+1) % 8) ) os << " ";

		// Add the ASCII representations and newline at the end.
		if ( p && ! ((p+1) % octetsPerLine) ) {
			os << CYAN << strbuf << endl;
			ACE_OS::memset(strbuf, 0, 17);
		}
	}

	// If there's a short line, print the ASCII string.
	const unsigned leftover = p % octetsPerLine;
	if ( leftover )
		os << CYAN << setw(leftover + (octetsPerLine - leftover)*3 + 1 + ((leftover>7)? 0:1))
				<< setfill(' ') << right << strbuf << endl;

	// Print everything out.
	string dumpStr(os.str());

	// ACE limits the maximum log message to 4096 bytes by default,
	// but network data dumps may easily be longer than that. So print
	// it out in parts. Unfortunately in a multithreaded app this
	// will be disjointed.
	const unsigned printLimit = ACE_MAXLOGMSGLEN - 128;
	for ( unsigned i = 0; i < dumpStr.length(); i += printLimit ) {
		if ( ! i ) ACE_DEBUG((pri, "%s %s", dumpStr.substr(i, printLimit).c_str(), ND_COLOR_END));
		else ACE_DEBUG((pri, "%s", dumpStr.substr(i, printLimit).c_str()));
	}

	ACE_DEBUG((pri, ND_COLOR_END "\n"));
}

void NetworkData::dumpBits(const size_t maxBits /* = 0 */, const size_t colWidth /* = 80 */,
	const bool debugOnly /* = true */) {

  	ACE_TRACE("NetworkData::dumpBits");

  	const ACE_Log_Priority pri = ( debugOnly ) ? LM_DEBUG : LM_INFO;

	const unsigned bitsToPrint = (maxBits && maxBits < getUnitLength() * 8)? maxBits : getUnitLength() * 8;

	using namespace std;
	ostringstream os;

	os << "Dumping " << dec << bitsToPrint << " bits of " << typeStr() << ":" << endl;

	// Add bit column labels
	const size_t adjColWidth = (colWidth > 0xff)? 0xff : ((colWidth > getUnitLength()*8)? getUnitLength()*8 : colWidth);

	os << YELLOW;
	for ( size_t x = adjColWidth; x > 0; --x ) {
		if ( adjColWidth - x > 0 && (adjColWidth - x) % 8 == 0 ) os << " ";
		os << uppercase << hex << (x-1)/0x10;
	}
	os << endl;
	for ( size_t x = adjColWidth; x > 0 ; --x ) {
		if ( adjColWidth - x > 0 && (adjColWidth - x) % 8 == 0 ) os << " ";
		os << uppercase << hex << (x-1)%0x10;
	}
	os << ND_COLOR_END << endl;

	size_t spacing = 0;
	for ( size_t b = 0; b < bitsToPrint ; ++b ) {
		if ( b > 1 && (b % colWidth == 0) ) {
			os << endl;
			spacing = 0;
		}
		else if ( colWidth > 7 && spacing == 8) {
			os << " ";
			spacing = 0;
		}

		os << getBitRev(b);
		++spacing;
	}

	// Print everything out.
	string dumpStr(os.str());

	// ACE limits the maximum log message to 4096 bytes by default,
	// but network data dumps may easily be longer than that. So print
	// it out in parts. Unfortunately in a multithreaded app this
	// will be disjointed.
	const unsigned printLimit = ACE_MAXLOGMSGLEN - 128;
	for ( unsigned i = 0; i < dumpStr.length(); i += printLimit ) {

		ACE_DEBUG((pri, "%s", dumpStr.substr(i, printLimit).c_str()));
	}

	ACE_DEBUG((pri, ND_COLOR_END "\n"));
}

std::string NetworkData::getAsString(const size_t offset /* = 0 */, const int length /* = -1 */) {
	const size_t lengthToCopy = (length < 0)? getUnitLength() - offset : static_cast<size_t>(length);

	return std::string(reinterpret_cast<const char*>(ptrUnit(offset)), lengthToCopy);
}

ACE_UINT8* NetworkData::setFromString(const std::string& stringData, const size_t unitOffset /* = 0 */,
	const size_t stringOffset /* = 0 */, const int length /* = -1 */) {
	if (stringOffset >= stringData.length()) return ptrUnit(0); // Too far, do nothing.

	int lengthToCopy = (length <= 0 || stringOffset + length >= stringData.length())? stringData.length() - stringOffset : length;

	if ( unitOffset + lengthToCopy > getUnitLength() ) {
		setUnitLength(unitOffset + lengthToCopy);
	}

	copyUnit(ptrUnit(unitOffset), reinterpret_cast<const ACE_UINT8*>(stringData.c_str() + stringOffset), lengthToCopy);

	return ptrUnit(unitOffset);
}

// -------------------------------------------------------------------------------------

ACE_UINT16 NetworkData::to_u16_(ACE_UINT8* ptr, const bool swap /* = true */) {
	return (swap) ? _swap16(*(reinterpret_cast<ACE_UINT16*>(ptr))) :
		*reinterpret_cast<ACE_UINT16*>(ptr);
}

ACE_UINT16 NetworkData::from_u16_(const ACE_UINT16 val, ACE_UINT8* buf, const bool swap /* = true */) {
	if (! buf ) throw nd_error("Buffer passed to from_u16_() must be pre-allocated.");

	ACE_UINT16 newVal = (swap)? _swap16(val) : val;

	ACE_OS::memcpy(buf, &newVal, sizeof(ACE_UINT16));
	return newVal;
}

ACE_UINT32 NetworkData::to_u32_(const ACE_UINT8* ptr, const bool swap /* = true */) {
	return (swap) ? _swap32(*((ACE_UINT32*) ptr)) : *((ACE_UINT32*) ptr);
}

ACE_UINT32 NetworkData::from_u32_(const ACE_UINT32 val, ACE_UINT8* buf, const bool swap /* = true */) {
	if (! buf ) throw nd_error("Buffer passed to from_u32_() must be pre-allocated.");

	ACE_UINT32 newVal = (swap) ? _swap32(val) : val;

	ACE_OS::memcpy(buf, &newVal, sizeof(ACE_UINT32));
	return newVal;
}

ACE_UINT64 NetworkData::to_u64_(const ACE_UINT8* ptr, const bool swap /* = true */) {
	return (swap) ? _swap64(*((ACE_UINT64*) ptr)) : *((ACE_UINT64*) ptr);
}

ACE_UINT32 NetworkData::from_u64_(const ACE_UINT64 val, ACE_UINT8* buf, const bool swap /* = true */) {
	if (! buf ) throw nd_error("Buffer passed to from_u64_() must be pre-allocated.");

	ACE_UINT64 newVal = (swap) ? _swap64(val) : val;

	ACE_OS::memcpy(buf, &newVal, sizeof(ACE_UINT64));
	return newVal;
}

void NetworkData::setInitialized_(const bool initVal /* = true */) { _isInitialized = initVal; }

NetworkData::NetworkData (ACE_Data_Block *, Message_Flags flags /* =0 */, ACE_Allocator *message_block_allocator /* =0 */) {}

NetworkData::NetworkData (const char *data, size_t size /* =0 */, unsigned long priority /*=ACE_DEFAULT_MESSAGE_BLOCK_PRIORITY */) {}

NetworkData::NetworkData (const ACE_Message_Block &mb, size_t align) {}

bool NetworkData::getBit(ACE_UINT8* buf, const size_t bitOffset) {
	return (( *(buf + bitOffset/8) & (1 << (bitOffset % 8))) != 0 );
}

bool NetworkData::getBit(const size_t bitOffset) {
	return getBit(ptrUnit(), bitOffset);
}

bool NetworkData::getBitRev(ACE_UINT8* buf, const size_t bitOffset) {
	return (( *(buf + bitOffset/8) & (1 << (7 - (bitOffset % 8)))) != 0 );
}

bool NetworkData::getBitRev(const size_t bitOffset) {
	return getBitRev(ptrUnit(), bitOffset);
}

bool NetworkData::setBit(ACE_UINT8* buf, const size_t bitOffset, const bool val) {
	ACE_UINT8* dstIdx = buf + (bitOffset / 8);

	*dstIdx = (*dstIdx & 0xff - (1 << (bitOffset%8))) | (val << (bitOffset%8));
	return val;
}

bool NetworkData::setBitRev(ACE_UINT8* buf, const size_t bitOffset, const bool val) {
	ACE_UINT8* dstIdx = buf + (bitOffset / 8);

	*dstIdx = (*dstIdx & 0xff - (1 << (7 - (bitOffset % 8)))) | (val << (7 - (bitOffset % 8)));
	return val;
}

bool NetworkData::setBit(const size_t bitOffset, const bool val) {
	return setBit(ptrUnit(), bitOffset, val);
}

bool NetworkData::setBitRev(const size_t bitOffset, const bool val) {
	return setBitRev(ptrUnit(), bitOffset, val);
}

void NetworkData::bitCopy(const size_t srcBitOffset, const size_t dstBitOffset, const size_t bitLen, ACE_UINT8* src, ACE_UINT8* dst) {
	ACE_UINT8* srcIdx;
	ACE_UINT8* dstIdx;

	for ( size_t b = 0; b < bitLen; ++b ) {
		setBit(dst, dstBitOffset + b, getBit(src, srcBitOffset + b));
	}
}

void NetworkData::bitCopyTo(const size_t srcBitOffset, const size_t dstBitOffset, const size_t bitLen, ACE_UINT8* dst) {
	bitCopy(srcBitOffset, dstBitOffset, bitLen, ptrUnit(), dst);
}

void NetworkData::bitCopyTo(const size_t srcBitOffset, const size_t dstBitOffset, const size_t bitLen, NetworkData* dst) {
	bitCopy(srcBitOffset, dstBitOffset, bitLen, ptrUnit(), dst->ptrUnit());
}

void NetworkData::bitCopyFrom(const size_t srcBitOffset, const size_t dstBitOffset, const size_t bitLen, ACE_UINT8* src) {
	bitCopy(srcBitOffset, dstBitOffset, bitLen, src, ptrUnit());
}

void NetworkData::bitCopyFrom(const size_t srcBitOffset, const size_t dstBitOffset, const size_t bitLen, NetworkData* src) {
	bitCopy(srcBitOffset, dstBitOffset, bitLen, src->ptrUnit(), ptrUnit());
}

} // namespace nasaCE
