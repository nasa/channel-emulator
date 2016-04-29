/* -*- C++ -*- */

//=============================================================================
/**
 * @file   HttpResponse.cpp
 * @author Tad Kollar  
 *
 * $Id: HttpResponse.cpp 1670 2012-08-28 13:25:28Z tkollar $ 
 * Copyright (c) 2010.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "nd_macros.hpp"
#include "HttpResponse.hpp"
#include "Rfc1123Date.hpp"
#include <ace/OS_NS_time.h>
#include <boost/assign.hpp>
#include <sstream>

using namespace std;
using namespace boost::assign;

namespace nasaCE {

ostream &crlf(ostream &out) { out << "\r\n"; return out.flush(); }

map<int, std::string> HttpResponse::statusCodeStr = map_list_of
	(100, "Continue")
	(101, "Switching Protocols")
	(200, "OK")
	(201, "Created")
	(202, "Accepted")
	(203, "Non-Authoritative Information")
	(204, "No Content")
	(205, "Reset Content")
	(206, "Partial Content")
	(300, "Multiple Choices")
	(301, "Moved Permanently")
	(302, "Found")
	(303, "See Other")
	(304, "Not Modified")
	(305, "Use Proxy")
	(307, "Temporary Redirect")
	(400, "Bad Request")
	(401, "Unauthorized")
	(402, "Payment Required")
	(403, "Forbidden")
	(404, "Not Found")
	(405, "Method Not Allowed")
	(406, "Not Acceptable")
	(407, "Proxy Authentication Required")
	(408, "Request Time-out")
	(409, "Conflict")
	(410, "Gone")
	(411, "Length Required")
	(412, "Precondition Failed")
	(413, "Request Entity Too Large")
	(414, "Request-URI Too Large")
	(415, "Unsupported Media Type")
	(416, "Requested range not satisfiable")
	(417, "Expectation Failed")
	(500, "Internal Server Error")
	(501, "Not Implemented")
	(502, "Bad Gateway")
	(503, "Service Unavailable")
	(504, "Gateway Time-out")
	(505, "HTTP Version not supported");

HttpResponse::HttpResponse(const float protocolVersion /* = 1.1 */, const int statusCode /* = 200 */,
	const std::string& contentType /* = "text/html" */, const std::string& content /* = "" */):
	protocolVersion_(protocolVersion),
	statusCode_(statusCode),
	contentType_(contentType),
	content_(content) { }


HttpResponse::~HttpResponse() { }

std::string HttpResponse::makeClientStr(bool addContent /* = true */) {
	std::string clientStr;
	
	std::ostringstream os;
	
	os	<< "HTTP/" << protocolVersion_ << " " << statusCode_ << " " << statusCodeStr[statusCode_] << crlf
		<< "Date: " << Rfc1123Date::convertSecondsToString(ACE_OS::time(0)) << crlf
		<< "Content-Length: " << content_.length() << crlf
		<< "Content-Type: " << contentType_ << crlf
		<< "Accept-Ranges: bytes" << crlf
		<< "Content-Language: en-us" << crlf;
		
	HeaderMapType::iterator pos;
	
	for (pos = headerMap_.begin(); pos != headerMap_.end(); ++pos )
		os << pos->first << ": " << pos->second << crlf;
		
	os << crlf;
	 
	// ND_DEBUG("[HTTP] Sending header:\n%s\n", os.str().c_str());
	 
	if (addContent) os << content_;
		
	return os.str();
}



}
