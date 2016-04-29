/* -*- C++ -*- */

//=============================================================================
/**
 * @file   HttpManager.hpp
 * @author Tad Kollar  
 *
 * $Id: HttpManager.hpp 2191 2013-06-21 16:51:18Z tkollar $
 * Copyright (c) 2010.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _NASA_HTTP_MANAGER_HPP_
#define _NASA_HTTP_MANAGER_HPP_

#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "string_nocase.hpp"

#include <ace/Singleton.h>

extern "C" {
	#include <magic.h>
}

namespace nasaCE {

//=============================================================================
/**
 * @class HttpManager
 * @brief Interface between an HTTP connection and the rest of the CE.
 */
//=============================================================================
class HttpManager {
public:
	/// Default constructor.
    HttpManager();

	/// Destructor.
    ~HttpManager();

	/// @brief Figure out to do with the client request and return a response.
	/// @param req The request to handle.
	/// @param response Where the response will be generated.
	void handleRequest(const HttpRequest& req, HttpResponse& response);

	/// A string describing was this server is.
	static const std::string serverDesc;

	/// @brief Determine the MIME type of a file.
	/// @param filename Path to the file to test.
	/// @return The MIME type string.
	std::string getMimeType(const std::string& filename);

	/// @brief Given the request URI, eliminate all the ".." and "." directory references.
	/// @param path The requested URI.
	static string_nocase cleanPath(const string_nocase& path);

private:

	/// @brief Process a POST method request. Anything but an XML-RPC method call is rejected.
	/// @param req The request to handle.
	/// @param response The response to build.
	void _handlePost(const HttpRequest& req, HttpResponse& response);

	/// @brief If either the config or fileIO directories (or subdirs) are requested, list the contents.
	/// @param filePath The actual filesystem path of the directory to list.
	/// @param origReqPath The path as it was requested from the browser.
	/// @param response The HTTP response that's being built.
	/// @param origReqPath A stat structure describing the document root.
	/// @param curDocStat A stat structure describing the current directory.
	void _doDirectoryListing(const string_nocase& filePath, const string_nocase& origReqPath,
		HttpResponse& response, struct stat& docRootStat, struct stat& curDocStat);

	/// @brief Process a GET or HEAD method request. May be a file or directory listing.
	/// @param req The request to handle.
	/// @param response The response to build.
	/// @param isHead True if this is a HEAD request.
	void _handleGet(const HttpRequest& req, HttpResponse& response, bool isHead = false);

	/// @brief Process a OPTIONS method request.
	/// @param req The request to handle.
	/// @param response The response to build.
	void _handleOptions(const HttpRequest& req, HttpResponse& response);

	/// @brief Generate an error message as a response.
	/// @param response The response to build.
	/// @param errMsg The message to respond with.
	void _handleError(HttpResponse& response, string errMsg = "");

	/// @brief Process a file upload.
	/// @param req The request to handle.
	/// @param response The response to build.
	void _handleUpload(const HttpRequest& req, HttpResponse& response);

	/// A reference to the file opened by libmagic to help with MIME types.
	magic_t _magicCookie;

	/// A short table to fall back on if libmagic fails.
	static std::map<string_nocase, std::string> _mimeTypes;

	/// The path that signals the use of an XML-RPC call.
	string_nocase _xmlrpcPathStr;

	/// The browser's fake path to the file I/O directory.
	string_nocase _fileIOFakePathStr;

	/// The browser's fake path to the config file directory.
	string_nocase _configFakePathStr;

	/// The browser's fake path to perform an upload with.
	string_nocase _uploadFakeStr;

}; // class HttpManager

typedef ACE_Singleton<HttpManager, ACE_Recursive_Thread_Mutex> httpManager;

} // namespace nasaCE

#endif // _NASA_HTTP_MANAGER_HPP_
