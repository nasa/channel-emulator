/* -*- C++ -*- */

//=============================================================================
/**
 * @file   HttpRequest.cpp
 * @author Tad Kollar  
 *
 * $Id: HttpRequest.cpp 2264 2013-06-28 20:23:08Z tkollar $
 * Copyright (c) 2010 - 2013.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "nd_macros.hpp"
#include "HttpRequest.hpp"
#include <boost/regex.hpp>

namespace nasaCE {

size_t RequestPart::parseHeaders(NetworkData*& data, HeaderMapType& headers, const size_t dataOffset /* =0 */) {
	ND_DEBUG("[HTTP] Searching for headers (total length %d).\n", data->getUnitLength());
	using namespace boost;
	using namespace std;

	ACE_UINT8 crLfCrLf[4] = { 13, 10, 13, 10 };
	std::string topHeaders;
	size_t offset = 0;

	try {
		offset = data->find(crLfCrLf, 4, dataOffset);
		ND_DEBUG("[HTTP] Found end of headers at offset %d.\n", offset + 4);
		topHeaders = data->getAsString(dataOffset, (offset + 4) - dataOffset);
	}
	catch (const nd_error& e) {
		throw nd_error("Did not find end of headers in client HTTP request!");
	}

	ND_DEBUG("[HTTP] Parsing header:\n%s", topHeaders.c_str());

	regex re_crlf("(\\r\\n)");
	regex re_Header("^\\s*([\\w\\-_]+):\\s*(.+)$");

	sregex_token_iterator it(topHeaders.begin(), topHeaders.end(), re_crlf, -1);
	sregex_token_iterator end;

	match_results<string::const_iterator> req, header;

	bool gotAllHeaders = false;
	while (it != end && ! gotAllHeaders) {
		string val(*it);

		if ( ! gotAllHeaders ) {
			if ( regex_match(val, header, re_Header) ) {
				const string_nocase headerName(header[1]);
				ND_DEBUG("[HTTP] Found header %s.\n", headerName.c_str());
				headers[headerName] = header[2];
			}
			else if ( val.empty() ) gotAllHeaders = true;
			else ND_WARNING("[HTTP] Request parse error: Did not recognize '%s'.\n'", string(*it).c_str());
		}

		it++;
	}

	if ( ! gotAllHeaders ) throw nd_error("Did not find the proper end of the header section.");

	return offset + 4;
}


string_nocase RequestPart::getMediaType()  {
	using namespace boost;
	string_nocase ret(""), headerName("Content-Type");
	regex re_mediaType("^(.+)/.*$");

	if ( headerMap_.find(headerName) != headerMap_.end() ) {
		std::string val(headerMap_[headerName]);
		match_results<string::const_iterator> ctype;
		if (regex_match(val, ctype, re_mediaType)) { ret = ctype[1]; }
	}

	return ret;
}

string_nocase RequestPart::getMediaSubType() {
	using namespace boost;
	string_nocase ret(""), headerName("Content-Type");
	regex re_mediaSubType_WithParam("^.+/(.+);.*$"),
		re_mediaSubType_WithoutParam("^.+/(.+)$");

	if ( headerMap_.find(headerName) != headerMap_.end() ) {
		std::string val(headerMap_[headerName]);
		match_results<string::const_iterator> ctype;
		if (regex_match(val, ctype, re_mediaSubType_WithParam)) { ret = ctype[1]; }
		else if (regex_match(val, ctype, re_mediaSubType_WithoutParam)) { ret = ctype[1]; }
	}

	return ret;
}

string_nocase RequestPart::getMediaParam(const string_nocase& paramName /* = "" */) {
	using namespace boost;
	string_nocase ret(""), headerName("Content-Type");

	regex re_mediaParam;

	if ( paramName.empty() ) { re_mediaParam = "^.+/.+\\s*;\\s*(\\S+)\\s*"; }
	else { re_mediaParam = "^.+/.+\\s*;\\s*" + paramName + "=(\\S+)\\s*"; };

	if ( headerMap_.find(headerName) != headerMap_.end() ) {
		std::string val(headerMap_[headerName]);
		match_results<string::const_iterator> ctype;
		if (regex_match(val, ctype, re_mediaParam)) { ret = ctype[1]; }
	}

	return ret;
}

string_nocase RequestPart::getContentDispParam(const string_nocase& paramName /* = "" */) {
	using namespace boost;
	string_nocase ret(""), headerName("Content-Disposition");

	regex re_Param("^\\s*form-data\\s*;.*?\\s" + paramName + "=\"(.+?)\".*$");

	if ( headerMap_.find(headerName) != headerMap_.end() ) {
		std::string val(headerMap_[headerName]);
		match_results<string::const_iterator> ctype;
		if (regex_match(val, ctype, re_Param)) { ret = ctype[1]; }
	}

	return ret;
}

bool RequestPart::hasContentDispParam(const string_nocase& paramName /* = "" */) {
	using namespace boost;
	string_nocase headerName("Content-Disposition");

	regex re_Param("^\\s*form-data\\s*;.*?\\s" + paramName + "=\"(.+?)\".*$");

	if ( headerMap_.find(headerName) != headerMap_.end() ) {
		std::string val(headerMap_[headerName]);
		match_results<string::const_iterator> ctype;
		if (regex_match(val, ctype, re_Param)) return true;
	}

	return false;
}

const string HttpRequest::methodStr[HttpRequest::MethodUNKNOWN + 1] =
	{ "OPTIONS", "GET", "HEAD", "POST", "PUT", "DELETE", "TRACE", "CONNECT", "UNKNOWN" };

HttpRequest::HttpRequest(): RequestPart(0),
	method_(MethodUNKNOWN),
	requestURI_(""),
	protocolVersion_(0.0),
	reportedContentLength_(0),
	gotHeaders_(false),
	rawReq_(0),
	isComplete_(false) {
}

HttpRequest::HttpRequest(const std::string& reqAsStr): RequestPart(0),
	method_(MethodUNKNOWN),
	requestURI_(""),
	protocolVersion_(0.0),
	reportedContentLength_(0),
	gotHeaders_(false),
	rawReq_(0),
	isComplete_(false) {

	if ( ! reqAsStr.empty()) {
		rawReq_ = new NetworkData(static_cast<size_t>(reqAsStr.size()), reinterpret_cast<const ACE_UINT8*>(reqAsStr.c_str()));
		parse();
	}
};

HttpRequest::HttpRequest(const size_t bufferSize, const ACE_UINT8* buffer /* = 0 */): RequestPart(0),
	method_(MethodUNKNOWN),
	requestURI_(""),
	protocolVersion_(0.0),
	reportedContentLength_(0),
	gotHeaders_(false),
	rawReq_(0),
	isComplete_(false) {

	rawReq_ = new NetworkData(bufferSize, buffer);
}

HttpRequest::~HttpRequest() {
	clear();
}

void HttpRequest::parse() {
	ND_DEBUG("[HTTP] Parsing request (total length %d).\n", rawReq_->getTotalUnitLength());
	using namespace boost;
	using namespace std;

	ACE_UINT8 crLf[2] = { 13, 10 };
	size_t firstLineLen;

	rawReq_->flatten();
	try {
		firstLineLen = rawReq_->find(crLf, 2) + 2;
	}
	catch (const nd_error& e) {
		throw nd_error("Did not find the first line!");
	}

	std::string firstLine = rawReq_->getAsString(0, firstLineLen);
	regex re_RequestLine("^\\s*([A-Z]+)\\s+(.+)\\s+HTTP/(.{3})\\s*$");
	match_results<string::const_iterator> req;
		bool gotRequestLine = false;

	if ( regex_match(firstLine, req, re_RequestLine) ) {
		ND_DEBUG("[HTTP] Parsing request line (%s).\n", firstLine.c_str());
		for (int m = MethodOPTIONS; m < MethodUNKNOWN; ++m) {
			if (req[1] == methodStr[m]) { method_ = static_cast<MethodEnum>(m); break; }
		}

		requestURI_ = req[2];
		protocolVersion_ = atof(static_cast<string>(req[3]).c_str());
		gotRequestLine = true;
	}

	if ( ! gotRequestLine ) throw nd_error("Did not find the request line!");

	size_t headerLen = 0;

	try {
		headerLen = parseHeaders(rawReq_, headerMap_, firstLineLen);
		gotHeaders_ = true;
	}
	catch (const nd_error& e) {
		gotHeaders_ = false;
	}

	if ( hasHeader("content-length") )
		reportedContentLength_ = ACE_OS::atoi(static_cast<string>(headerMap_["content-length"]).c_str());

	isComplete_ = false;
	if ( gotHeaders_ ) {
		if ( ! reportedContentLength_ ) {
			ND_DEBUG("[HTTP] No content in request (OK).\n");
			isComplete_ = true;
		}
		else if ( rawReq_->getUnitLength() - headerLen >= reportedContentLength_ ) {
			ND_DEBUG("[HTTP] Content is complete.\n");
			isComplete_ = true;
			content_ = rawReq_->wrapInnerPDU<NetworkData>(rawReq_->getUnitLength() - headerLen, rawReq_->ptrUnit(headerLen));
			if ( getMediaType() == "multipart" ) splitParts_();
		}
	}

	ND_DEBUG("[HTTP] Got headers: %s, stored content length: %d, expected length: %d, complete: %s\n",
		gotHeaders_? "yes" : "no", getContentLength(), reportedContentLength_, isComplete_? "yes" : "no");

}

void HttpRequest::clear() {
	ND_DEBUG("[HTTP] Entering HttpRequest::clear.\n");
	method_ = MethodUNKNOWN;
	requestURI_ = "";
	protocolVersion_ = 0.0,
	reportedContentLength_ = 0;
	headerMap_.clear();
	gotHeaders_ = false;
	isComplete_ = false;
	while (parts_.begin() != parts_.end()) {
		delete *parts_.begin();
		parts_.erase(parts_.begin());
	}

	if ( hasContent() ) {
		ND_DEBUG("[HTTP] HttpRequest::clear: Deleting %d octets of content.\n", getContentLength());
		delete content_;
		content_ = 0;
	}

	if ( rawReq_ ) {
		ND_DEBUG("[HTTP] HttpRequest::clear: Deleting entire %d-octet request.\n", rawReq_->getUnitLength());
		delete rawReq_;
		rawReq_ = 0;
	}
}

void HttpRequest::addData(const uint8_t* buf, const size_t bufLen) {
	ND_DEBUG("[HTTP] Adding data to request (length %d).\n", bufLen);
	NetworkData* newPart = new NetworkData(bufLen, buf);

	if ( ! rawReq_ ) {
		rawReq_ = newPart;
	}
	else {
		rawReq_->setNextPart(newPart);
	}
}

int HttpRequest::splitParts_() {
	ND_DEBUG("[HTTP] Entering HttpRequest::splitParts_()\n");
	const std::string boundaryParam(getMediaParam("boundary").c_str());

	if ( boundaryParam.empty() ) {
		ND_ERROR("[HTTP] Media boundary parameter not found, cannot handle multipart request.\n");
		return 0;
	}

	std::string boundary("--");
	boundary += boundaryParam;

	ND_DEBUG("[HTTP] HttpRequest::splitParts_(): Using boundary ==>%s<==\n", boundary.c_str());

	const size_t boundaryLength = boundary.length();
	NetworkData* boundaryData = new NetworkData(boundaryLength);
	boundaryData->setFromString(boundary);

	size_t firstBoundaryOffset = 0, secondBoundaryOffset = 0, endOfFirstBoundary = 0, endOfSecondBoundary = 0;
	bool finished = false;

	while ( ! finished ) {
		ND_DEBUG("[HTTP] HttpRequest::splitParts_(): Looking for next pair of boundaries.\n");
		try {
			firstBoundaryOffset = (secondBoundaryOffset == 0)? content_->find(boundaryData, 0) : secondBoundaryOffset;
			endOfFirstBoundary = firstBoundaryOffset + boundaryLength + 2; // +2 to account for the CR/LF.
			ND_DEBUG("[HTTP] HttpRequest::splitParts_(): First boundary at %d - %d.\n", firstBoundaryOffset, endOfFirstBoundary);


			secondBoundaryOffset = content_->find(boundaryData, endOfFirstBoundary);
			endOfSecondBoundary = secondBoundaryOffset + boundaryLength;
			ND_DEBUG("[HTTP] HttpRequest::splitParts_(): Second boundary at %d - %d.\n", secondBoundaryOffset, endOfSecondBoundary + 2);

			RequestPart* newPart = new RequestPart(content_->wrapInnerPDU<NetworkData>(
				secondBoundaryOffset - endOfFirstBoundary,
				content_->ptrUnit(endOfFirstBoundary)));

			parts_.push_back(newPart);

			if ( endOfSecondBoundary >= getContentLength() ) {
				ND_NOTICE("[HTTP] HttpRequest::splitParts_():  Overran end of content.\n");
				finished = true;
			}
			else if (getContentAsString(endOfSecondBoundary, 2) == "--" ) {
				endOfSecondBoundary += 2;
				ND_DEBUG("[HTTP] HttpRequest::splitParts_(): Found end of last boundary at content offset %d. Parts: %d.\n",
					endOfSecondBoundary + 2, parts_.size());
				finished = true;
			}

			endOfSecondBoundary += 2; // +2 to account for the CR/LF.
		}
		catch(const nd_error& e) {
			ND_ERROR("[HTTP] Media boundary not found, cannot handle multipart request.\n");
			delete boundaryData;
			return 0;
		}
	}

	delete boundaryData;
	return parts_.size();
}


} // namespace nasaCE
