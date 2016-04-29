/* -*- C++ -*- */

//=============================================================================
/**
 * @file   HttpResponse.hpp
 * @author Tad Kollar  
 *
 * $Id: HttpResponse.hpp 1670 2012-08-28 13:25:28Z tkollar $ 
 * Copyright (c) 2010.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _NASA_HTTP_RESPONSE_HPP_
#define _NASA_HTTP_RESPONSE_HPP_

#include "HttpCommon.hpp"

using namespace std;

namespace nasaCE {

//=============================================================================
/**
 * @class HttpResponse
 * @brief Compose an HTTP response to send back to the client.
*/
//=============================================================================
class HttpResponse {

public:
	static std::map<int, string> statusCodeStr;

	/// Constructor.
	/// @param protocolVersion HTTP version, should be 1.1.
	/// @param statusCode A 3-digit code from RFC 2616.
	/// @param content The body of the message,
    HttpResponse(const float protocolVersion = 1.1, const int statusCode = 200,
    	const string& contentType = "text/html", const string& content = "");

	/// Destructor.
    ~HttpResponse();

	/// Write-only accessor to protocolVersion_.
	void setProtocolVersion(float newVal) {	protocolVersion_ = newVal; }
	
	/// Read-only accessor to protocolVersion_.
	float getProtocolVersion() const { return protocolVersion_; }

	/// Write-only accessor to statusCode_.
	void setStatusCode(int newVal) { statusCode_ = newVal; }
	
	/// Read-only accessor to statusCode_.
	int getStatusCode() const { return statusCode_; }
	
	/// Return the string describing the specified code.
	static string getStatusStr(const int code) { return statusCodeStr[code]; }
	
	/// Return the string describing statusCode_.
	string getStatusStr() { return getStatusStr(statusCode_); }

	/// Set a new value for the provided case-insensitive key.
	void setHeader(const string_nocase& key, const string& newVal) {
		headerMap_[key] = newVal;
	}

	/// Return the value of the provided key.
	string getHeader(const string_nocase& key) {
		return headerMap_[key];
	}
	
	/// Remove the provided key.
	void deleteHeader(const string_nocase& key) {
		headerMap_.erase(key);
	}

	/// Write-only accessor to contentType_.
	void setContentType(const string& newVal) { contentType_ = newVal; }
	
	/// Read-only accessor to contentType_.
	string getContentType() const { return contentType_; }
	
	/// Write-only accessor to content_.
	void setContent(const string& newVal) { content_ = newVal; }
	
	/// Read-only accessor to content_.
	string getContent() const { return content_; }
	
	/// Append a string to existing content_.
	void addContent(const string& newVal) { content_ += newVal; }
		
	/// Create the string to send to the client.
	/// @param addContent Whether to append content or not (false for response to HEAD).
	string makeClientStr(bool addContent = true);


protected:
	/// HTTP version, should be 1.1.
	float protocolVersion_;
	
	/// A 3-digit code from RFC 2616.
	int statusCode_;
	
	/// All headers for which there is no special handling by HttpResponse.
	HeaderMapType headerMap_;
	
	/// The type of the content to report.
	string contentType_;
	
	/// The body of the message,
	string content_;
};



} // namespace nasaCE

#endif
