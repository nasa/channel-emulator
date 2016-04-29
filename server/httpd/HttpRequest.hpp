/* -*- C++ -*- */

//=============================================================================
/**
 * @file   HttpRequest.hpp
 * @author Tad Kollar  
 *
 * $Id: HttpRequest.hpp 2266 2013-06-28 20:34:08Z tkollar $
 * Copyright (c) 2010.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _NASA_HTTP_REQUEST_HPP_
#define _NASA_HTTP_REQUEST_HPP_

#include "HttpCommon.hpp"
#include "NetworkData.hpp"
#include <vector>

using namespace std;

namespace nasaCE {

//=============================================================================
/**
 * @class RequestPart
 * @brief Methods that can be used either with a full request or part of one.
*/
//=============================================================================
class RequestPart {
public:
	/// Constructor.
	RequestPart(NetworkData* newContent = 0): headersLength_(0), content_(0) {
		if ( newContent ) {
			content_ = newContent;
			headersLength_ = parseHeaders(content_, headerMap_);
		}
	}

	/// Destructor.
	~RequestPart() {
		if ( content_ ) {
			ND_DEBUG("[HTTP] ~RequestPart: Deleting %d octets of content.\n", content_->getUnitLength());
			delete content_;
			content_ = 0;
		}
	}

	/// Extract headers from an area of memory.
	/// @param data The unit to search for headers.
	/// @param headers The map to load with headers.
	/// @param dataOffset The position to begin searching for the headers.
	/// @return The discovered length of the header section in octets.
	static size_t parseHeaders(NetworkData*& data, HeaderMapType& headers, const size_t dataOffset = 0);

	/// Set a new value for the provided case-insensitive key.
	void setHeader(const string_nocase& key, const string& newVal) {
		headerMap_[key] = newVal;
	}

	/// Return the value of the provided key.
	string getHeader(const string_nocase& key) const {
		HeaderMapType::const_reference cref = *headerMap_.find(key);

		return cref.second;
	}

	/// Remove the provided key.
	void deleteHeader(const string_nocase& key) {
		headerMap_.erase(key);
	}

	/// Determine if the provided header name exists.
	bool hasHeader(const string_nocase& key) const {
		return ( headerMap_.find(key) != headerMap_.end());
	}

	/// Read-only accessor to headersLength_.
	size_t getHeadersLength() const { return headersLength_; }

	/// Read-only accessor to content_.
	NetworkData* getContent() const { return content_; }

	/// Safely determine the length of the content.
	size_t getContentLength() const {
		return (hasContent())? content_->getUnitLength() : 0;
	}

	/// True if this request has content, false otherwise.
	bool hasContent() const { return ( content_ != 0 ); }

	/// Convert the raw data to a string.
	std::string getContentAsString(size_t offset = 0, size_t length = 0) const {
		if ( ! hasContent() ) return std::string("");

		size_t lengthToCopy = length? length: getContentLength() - offset;
		if (offset + lengthToCopy > getContentLength() ) { return std::string(""); }

		return std::string(reinterpret_cast<const char*>(content_->ptrUnit(offset)), lengthToCopy);
	}

	/// Convert the raw data to a string.
	std::string getContentAfterHeadersAsString() const {
		return getContentAsString(headersLength_);
	}

	/// Return the primary media type, from Content-Type.
	string_nocase getMediaType();

	/// Return the media sub-type, from Content-Type.
	string_nocase getMediaSubType();

	/// Return the media parameter from Content-Type.
	/// @param paramName If the parameter appears in the form key=value, use "key" here.
	string_nocase getMediaParam(const string_nocase& paramName = "");

	/// Return the specified parameter from the Content-Disposition header.
	/// @param paramName If the parameter appears in the form key=value, use "key" here.
	string_nocase getContentDispParam(const string_nocase& paramName = "");

	/// Return whether the specified parameter exists in the Content-Disposition header.
	/// @param paramName If the parameter appears in the form key=value, use "key" here.
	bool hasContentDispParam(const string_nocase& paramName = "");

protected:
	/// All headers for which there is no special handling.
	HeaderMapType headerMap_;

	/// The entire length of the headers section.
	size_t headersLength_;

	/// Content that was received as part of the request.
	NetworkData* content_;
};


//=============================================================================
/**
 * @class HttpRequest
 * @brief Container for all parts of a received HTTP/1.1 request.
*/
//=============================================================================
class HttpRequest: public RequestPart {
public:
	/// @enum MethodEnum
	/// @brief Specifies one of the request methods from RFC 2616.
	/// @see http://www.w3.org/Protocols/rfc2616/rfc2616-sec9.html#sec9
	/// The Method token indicates the method to be performed on the resource
	/// identified by the Request-URI. The method is case-sensitive.
	enum MethodEnum {
		MethodOPTIONS,	/*!< A request for information about the communication
						options available on the request/response chain identified
						by the Request-URI. This method allows the client to determine
						the options and/or requirements associated with a resource,
						or the capabilities of a server. */
		MethodGET,		/*!< Retrieve whatever information (in the form of an entity)
						is identified by the Request-URI. */
		MethodHEAD,		/*!< Identical to GET except that the server MUST NOT return
						a message-body in the response. */
		MethodPOST,	    /*!< Used to request that the origin server accept the entity
						enclosed in the request as a new subordinate of the resource
						identified by the Request-URI in the Request-Line. POST is
						designed to allow a uniform method to cover the following functions:
						- Annotation of existing resources;
						- Posting a message to a bulletin board, newsgroup, mailing list,
						or similar group of articles;
						- Providing a block of data, such as the result of submitting a
						form, to a data-handling process;
						- Extending a database through an append operation. */

		MethodPUT,		/*!< Requests that the enclosed entity be stored under the
						supplied Request-URI. */
		MethodDELETE,	/*!< Requests that the origin server delete the resource
						identified by the Request-URI. */
		MethodTRACE,	/*!< Used to invoke a remote, application-layer loop- back of
						the request message. The final recipient of the request SHOULD
						reflect the message received back to the client as the entity-body
						of a 200 (OK) response. */
		MethodCONNECT,	/*!< For use with a proxy that can dynamically switch to being a tunnel */
		MethodUNKNOWN	/*!< Internal use only, not in RFC 2616. */
	};

	static const string methodStr[MethodUNKNOWN + 1];

	/// Default constructor.
	HttpRequest();

	/// Constructor. Initiate with an allocated buffer and optionally fill it.
	/// @param bufferSize The amount of memory to allocate.
	/// @param buffer Pointer to the data to copy.
	HttpRequest(const size_t bufferSize, const ACE_UINT8* buffer = 0);

	/// Constructor.
	/// @param reqAsStr The information received from the client.
	HttpRequest(const std::string& reqAsStr);

	/// Destructor.
	~HttpRequest();

	/// Write-only accessor to method_.
	void setMethod(const MethodEnum& newVal) { method_ = newVal; }

	/// Read-only accessor to method_.
	MethodEnum getMethod() const { return method_; }

	/// Write-only accessor to requestURI_.
	void setRequestURI(const string_nocase& newVal) { requestURI_ = newVal; }

	/// Read-only accessor to requestURI_.
	string_nocase getRequestURI() const { return requestURI_; }

	/// Write-only accessor to protocolVersion_.
	void setProtocolVersion(float newVal) { protocolVersion_ = newVal; }

	/// Read-only accessor to protocolVersion_.
	float getProtocolVersion() const { return protocolVersion_; }

	/// Return to initial state (e.g. to receive another request).
	void clear();

	/// Read-only accessor to gotHeaders_;
	bool hasHeaders() const { return gotHeaders_; }

	/// Add another part to request.
	void addData(const uint8_t* buf, const size_t bufLen);

	/// Declare that the request is complete and begin processing.
	void parse();

	/// Return the total size of the request.
	size_t getRequestLength() { return rawReq_->getUnitLength(); }

	/// Read-only accessor to isComplete_.
	bool isComplete() { return isComplete_; }

	/// Read-only accessor to reportedContentLength_.
	size_t getReportedContentLength() { return reportedContentLength_; }

	/// Read-only accessor to parts_.
	RequestPart& getPart(const size_t index) const { return *(parts_[index]); }

	/// Return the number of parts.
	size_t getPartCount() const { return parts_.size(); }

protected:
	/// The HTTP method extracted from the request text.
	MethodEnum method_;

	/// The URI extracted from the request text.
	string_nocase requestURI_;

	/// The HTTP Protocol version extracted from the request text (should be 1.1.)
	float protocolVersion_;

	/// The length of the message body following the header, as reported by the remote agent.
	size_t reportedContentLength_;

	/// Whether the HTTP headers have been received and parsed yet.
	bool gotHeaders_;

	/// The unmodified request from the client.
	NetworkData* rawReq_;

	/// False if data is missing from the request, true otherwise.
	bool isComplete_;

	/// Refer to individual parts of a multipart request.
	std::vector<RequestPart*> parts_;

	/// Find each part for multipart requests.
	int splitParts_();
};

} // namespace nasaCE

#endif
