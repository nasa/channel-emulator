/* -*- C++ -*- */

//=============================================================================
/**
 * @file   HttpManager.cpp
 * @author Tad Kollar  
 *
 * $Id: HttpManager.cpp 2272 2013-07-08 17:57:06Z tkollar $
 * Copyright (c) 2010.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "HttpManager.hpp"
#include "nd_macros.hpp"
#include "SettingsManager.hpp"
#include "XML_RPC_Server.hpp"
#include "NetworkData.hpp"
#include <boost/assign.hpp>
#include <boost/regex.hpp>
#include <sstream>
#include <ace/Dirent.h>

namespace Linux {
	extern "C" {
		#include <fcntl.h>
	}
}

using namespace std;
using namespace boost::assign;
using namespace boost;

namespace nasaCE {

map<string_nocase, std::string> HttpManager::_mimeTypes = map_list_of
	("gif", "image/gif")
	("ico", "image/x-ico")
	("jpg", "image/jpeg")
	("jpe", "image/jpeg")
	("jpeg", "image/jpeg")
	("png", "image/png")
	("svg", "image/svg+xml")
	("tif", "image/tiff")
	("tiff", "image/tiff")
	("htm", "text/html")
	("html", "text/html")
	("shtml", "text/shtml")
	("txt", "text/plain")
	("css", "text/css")
	("csv", "text/csv")
	("js", "application/javascript")
	("jgz", "application/javascript");

const std::string HttpManager::serverDesc("NASA Channel Emulator HTTP Server");

HttpManager::HttpManager(): _magicCookie(0),
	_xmlrpcPathStr("/RPC2"),
	_fileIOFakePathStr(globalCfg.fileIOWebPath.c_str()),
	_configFakePathStr(globalCfg.cfgFileWebPath.c_str()),
	_uploadFakeStr("/upload") {
	ND_DEBUG("[HTTP] Initializing the %s.\n", serverDesc.c_str());
	ND_DEBUG("[HTTP] Will use %s as the document root.\n", globalCfg.webDocPath.c_str());
	ND_DEBUG("[HTTP] Will use %s as the config file web root.\n", _configFakePathStr.c_str());
	ND_DEBUG("[HTTP] Will use %s as the file I/O web root.\n", _fileIOFakePathStr.c_str());

	_magicCookie = magic_open(MAGIC_SYMLINK|MAGIC_MIME_TYPE);

	if ( CEcfg::instance()->existsSrv("xmlrpcPath") ) {
		_xmlrpcPathStr = string_nocase(CEcfg::instance()->getSrv("xmlrpcPath") );
	}

	if ( ! _magicCookie ) {
		ND_ERROR("[HTTP] Failed to open magic file; may not respond to GET requests correctly.\n");
    }
    else {
    	if ( magic_load(_magicCookie, 0) ) {
			ND_ERROR("[HTTP] Error: '%s' May not respond to GET requests correctly.\n",
				magic_error(_magicCookie));
			magic_close(_magicCookie);
			_magicCookie = 0;
    	}
    }
}

HttpManager::~HttpManager() {
	if ( _magicCookie ) magic_close(_magicCookie);
}

void HttpManager::handleRequest(const HttpRequest& req, HttpResponse& response) {

	switch (req.getMethod()) {
		case HttpRequest::MethodGET:
			_handleGet(req, response);
			break;
		case HttpRequest::MethodHEAD:
			_handleGet(req, response, true);
			break;
		case HttpRequest::MethodPOST:
			_handlePost(req, response);
			break;
		case HttpRequest::MethodOPTIONS:
			_handleOptions(req, response);
			break;
		case HttpRequest::MethodPUT:
		case HttpRequest::MethodDELETE:
		case HttpRequest::MethodTRACE:
		case HttpRequest::MethodCONNECT:
		case HttpRequest::MethodUNKNOWN:
			return;
	}

	response.setHeader("Server", serverDesc);
}

string HttpManager::getMimeType(const string& filename) {
	char* magicCStr = 0;

	int dotIdx = filename.rfind('.');
	string_nocase extension(filename.substr(dotIdx + 1).c_str());
	if ( _mimeTypes.find(extension) != _mimeTypes.end() )
		return _mimeTypes[extension];

	// Didn't have a special case for it, use magic.
	if ( _magicCookie ) {
		magicCStr = const_cast<char*>(magic_file(_magicCookie, filename.c_str()));
		if ( ! magicCStr ) {
			ND_ERROR("[HTTP] Error determining MIME type: %s\n", magic_error(_magicCookie));
		}

		return string(magicCStr);
	}

	if ( ! magicCStr ) {
		ND_WARNING("[HTTP] Could not determine proper MIME type for %s.\n",
			filename.c_str());
	}

	// If we get here, we've failed utterly at finding the true MIME type.
	return string("text/plain");
}

void HttpManager::_handlePost(const HttpRequest& req, HttpResponse& response) {
	string xmlResponse;

	ND_DEBUG("[HTTP] Received the entire POST request content (%d octets).\n", req.getContentLength());
	ND_DEBUG("[HTTP] Requested URI: %s\n", req.getRequestURI().c_str());

	if ( req.getRequestURI() == _xmlrpcPathStr) {
		xmlrpc_server::instance()->rpcRegistry->processCall(req.getContentAsString(), &xmlResponse);

		response.setContentType("text/xml");
		response.setContent(xmlResponse);
	}
	else if ( req.getRequestURI() == _uploadFakeStr ) {
		_handleUpload(req, response);
	}
	else {
		response.setStatusCode(404);
		_handleError(response, "Requested file does not exist.");
	}
}

void HttpManager::_handleUpload(const HttpRequest& req, HttpResponse& response) {
	ND_INFO("[HTTP] Receiving file upload.\n");
	bool isConfig = false, isData = false, allowOverwrite = false, useCfgNow = false;
	string_nocase filename = "", name = "";

	size_t parts = req.getPartCount(), filePart;
	ND_DEBUG("[HTTP] Request has %d parts.\n", parts);

	if ( parts > 0 ) {
		for ( size_t p = 0; p < parts; ++p ) {
			if ( req.getPart(p).hasContentDispParam("filename") ) {
				filename = req.getPart(p).getContentDispParam("filename");
				ND_DEBUG("[HTTP] Filename is %s, and is in part %d of %d.\n", filename.c_str(), p + 1, parts);
				filePart = p;
			}

			name = req.getPart(p).getContentDispParam("name");

			if ( name == "data_upload_name" ) {
				ND_DEBUG("[HTTP] File is a data file.\n");
				isData = true;
			}
			else if (name == "cfg_upload_name" ) {
				ND_DEBUG("[HTTP] File is a config file.\n");
				isConfig = true;
			}
			else if ( name == "overwrite" ) {
				std::string overwriteVal(req.getPart(p).getContentAfterHeadersAsString());
				ND_DEBUG("[HTTP] Found overwrite: ==>%s<==.\n", overwriteVal.c_str());
				if (overwriteVal.substr(0,2) == "on") allowOverwrite = true;
			}
			else if ( name == "use_now" ) {
				std::string useNowVal(req.getPart(p).getContentAfterHeadersAsString());
				ND_DEBUG("[HTTP] Found use_now: ==>%s<==.\n", useNowVal.c_str());
				if (useNowVal.substr(0,2) == "on") useCfgNow = true;
			}
		}
	}


	// select the path
	string_nocase outputName("");
	if (isData) outputName = globalCfg.fileIOPath.c_str();
	else if (isConfig) outputName = globalCfg.cfgFilePath.c_str();
	else {
		response.setStatusCode(403);
		_handleError(response, "Attempted file upload to unknown/unspecified location.");
		return;
	}

	// append filename
	outputName += string_nocase("/") + filename;

	struct stat fileStat;
	if ( ACE_OS::stat(outputName.c_str(), &fileStat) > -1 && ! allowOverwrite ) {
		ND_NOTICE("[HTTP] Attempt to overwrite %s with overwrites disabled.\n", outputName.c_str());
		response.setStatusCode(500);
		_handleError(response, "Attempted to overwrite file with same name when overwrites are disabled.");
		return;
	}

	// open for writing
	ACE_HANDLE outputFd = ACE_OS::open(outputName.c_str(), O_WRONLY|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP);

	if ( outputFd == ACE_INVALID_HANDLE ) {
		ND_ERROR("[HTTP] %p.\n", "Could not open upload file for writing");
		response.setStatusCode(500);
		_handleError(response, "Error opening save file.");
		return;
	}

	RequestPart& reqPart = req.getPart(filePart);
	NetworkData* partContent = reqPart.getContent();
	NetworkData* fileContents = partContent->wrapInnerPDU<NetworkData>(
		reqPart.getContentLength() - reqPart.getHeadersLength() - 2, // -2 because there's a CR/LF.
		partContent->ptrUnit( reqPart.getHeadersLength() )
	);

	ND_DEBUG("[HTTP] %s file %s is %d octets in length.\n", isData? "Data" : "Config", filename.c_str(), fileContents->getUnitLength());

	// write data
	ssize_t writeCount = ACE_OS::write(outputFd, fileContents->ptrUnit(), fileContents->getUnitLength());
	delete fileContents;

	if ( writeCount < static_cast<ssize_t>(fileContents->getUnitLength()) ) {
		ND_ERROR("[HTTP] %p.", "Error writing to upload file.");
		response.setStatusCode(500);
		_handleError(response, "Error writing save file.");
		return;
	}

	if ( ACE_OS::close(outputFd) < 0 ) {
		ND_ERROR("[HTTP] %p.", "Error closing upload file.");
		response.setStatusCode(500);
		_handleError(response, "Error closing save file.");
		return;
	}

	if (useCfgNow) xmlrpc_server::instance()->switchToConfigFile(filename.c_str());

	// print status to response
	response.setHeader("Location","/");
	response.setStatusCode(303);
}

string_nocase HttpManager::cleanPath(const string_nocase& path) {
	regex matchE(".*/\\.{1,2}/.*|.*/\\.{1,2}$|^\\.{1,2}/.*"),
		deleteE("/\\.{1,2}/|/\\.{1,2}$|^\\.{1,2}/");

	std::string ret(path.c_str());

	while ( regex_match(ret, matchE) ) {
		ret = regex_replace(ret, deleteE, "/");
	}

	regex spaceE("%20");
	ret = regex_replace(ret, spaceE, " ");

	ND_DEBUG("[HTTP] Original Path: '%s', Clean Path: '%s'\n", path.c_str(), ret.c_str());

	return string_nocase(ret.c_str());
}

void HttpManager::_doDirectoryListing(const string_nocase& filePath, const string_nocase& origReqPath,
	HttpResponse& response, struct stat& docRootStat, struct stat& curDocStat) {
	ND_DEBUG("[HTTP] Sending directory listing of %s.\n", filePath.c_str());

	ACE_Dirent dir;
	if ( dir.open(filePath.c_str()) ) {
		response.setStatusCode(404);
		_handleError(response, "Requested file or directory does not exist.");
		return;
	}

	struct stat docStat;
	ostringstream html;

	response.setContentType("text/html");
	html
		<< "<html>" << endl
		<< "<head>" << endl
		<< "<title>Directory Listing</title>" << endl
		<< "<style type='text/css'>" << endl
		<< ".fixed { font-family: Monaco, Courier New, Fixed; padding-right: 20px;}" << endl
		<< ".num { text-align: right; }" << endl
		<< "</style>" << endl
		<< "</head>" << endl
		<< "<body>" << endl
		// << "<p><a href='/'>Return to Channel Emulator Control Panel</a></p>" << endl
		<< "<table class='fileListing'>" << endl
		<< "<tr>" << endl
		<< "<th>Name</th>" << endl
		<< "<th>Last Modified</th>" << endl
		<< "<th>Size</th>" << endl
		<< "</tr>" << endl;

	ACE_DIRENT* entry;
	std::list<string_nocase> entries;

	while ( (entry = dir.read()) != NULL ) {
		string_nocase entryName(entry->d_name);
		if ( entryName != "." && entryName != ".." ) { entries.push_back(entryName); }
	}

	dir.close();
	entries.sort();

	std::list<string_nocase>::iterator pos;

	// First, parent directory for subdirectories only
	if ( docRootStat.st_ino != curDocStat.st_ino ) {
		char *initialDir = get_current_dir_name();
		string_nocase parentDir(filePath + "/..");
		int err = 0;

		err -= chdir(parentDir.c_str());

		char *parentDirPath = get_current_dir_name();
		err -= chdir(initialDir);
		free(initialDir);

		string_nocase rootPathStr, fakeRootPathStr, parentDirStr(parentDirPath);
		free(parentDirPath);

		if (string_nocase(parentDirPath) == string_nocase(globalCfg.fileIOPath.c_str())) {
			rootPathStr = globalCfg.fileIOPath.c_str();
			fakeRootPathStr = _fileIOFakePathStr;
		}
		else {
			rootPathStr = globalCfg.cfgFilePath.c_str();
			fakeRootPathStr = _configFakePathStr;
		}

		parentDirStr = fakeRootPathStr + parentDirStr.substr(rootPathStr.size(), string_nocase::npos);

		if ( err >= 0 ) {
			html
				<< "<tr>" << endl
				<< "<td class='fixed'><img src='/icons/folder.png' alt=' '/> "
				<< "<a href='" << parentDirStr << "'>Parent Folder</a></td>" << endl
				<< "<td></td>" << endl << "<td></td>" << endl << "</tr>" << endl;
		}
	}

	for ( pos = entries.begin(); pos != entries.end(); ++pos ) {
		string_nocase fullPath(filePath + "/" + *pos);
		ACE_OS::stat(fullPath.c_str(), &docStat);

		if ( S_ISREG(docStat.st_mode) ) {
			html
				<< "<tr>" << endl
				<< "<td class='fixed'><img src='/icons/x-office-document.png' alt=' '/> "
				<< "<a href='" << origReqPath << "/" << *pos << "'>" << *pos << "</a></td>" << endl
				<< "<td>" << asctime(localtime(&(docStat.st_mtime))) << "</td>" << endl
				<< "<td class='num'>" << NetworkData::num2HumanReadable(docStat.st_size, 2, "", true) << "B</td>" << endl
				<< "</tr>" << endl;
		}
		else if ( S_ISDIR(docStat.st_mode) ) {
			html
				<< "<tr>" << endl
				<< "<td class='fixed'><img src='/icons/folder.png' alt=' '/> "
				<< "<a href='" << origReqPath << "/" << *pos << "'>" << *pos << "</a></td>" << endl
				<< "<td>" << asctime(localtime(&(docStat.st_mtime))) << "</td>" << endl
				<< "<td></td>" << endl
				<< "</tr>" << endl;
		}
	}

	html << "</table>" << endl << "</body>" << endl << "</html>" << endl;
	response.addContent(html.str());
}

void HttpManager::_handleGet(const HttpRequest& req, HttpResponse& response, bool isHead /* = false */) {
	const std::string getOrHead = isHead? "HEAD" : "GET";

	ND_DEBUG("[HTTP] Received %s request.\n", getOrHead.c_str());
	ND_DEBUG("[HTTP] Requested URI: %s\n", req.getRequestURI().c_str());

	string_nocase docRoot(globalCfg.webDocPath.c_str()), filePath(docRoot),
		reqPath(cleanPath(req.getRequestURI().c_str())), fakePath;

	const string_nocase origReqPath(reqPath);
	struct stat docStat, docRootStat, curDocStat;
	bool doingFileTransfer = false;

	if ( reqPath.substr(0, _fileIOFakePathStr.size()) == _fileIOFakePathStr ) {
		reqPath = reqPath.substr(_fileIOFakePathStr.size(), string::npos);
		ND_DEBUG("[HTTP] Will get file/dir as file I/O.\n");
		docRoot = globalCfg.fileIOPath.c_str();
		doingFileTransfer = true;
	}
	else if ( reqPath.substr(0, _configFakePathStr.size()) == _configFakePathStr ) {
		reqPath = reqPath.substr(_configFakePathStr.size(), string::npos);
		ND_DEBUG("[HTTP] Will get file/dir as config file listing.\n");
		docRoot = globalCfg.cfgFilePath.c_str();
		doingFileTransfer = true;
	}

	filePath = docRoot + reqPath;

	if ( ACE_OS::stat(docRoot.c_str(), &docRootStat) || ! S_ISDIR(docRootStat.st_mode ) ){
		response.setStatusCode(500);
		_handleError(response, "The document root directory does not exist on the server." );
		return;
	}

	// Check for the existance of the file, as well as the possibility
	// that an index file might be there.
	if (ACE_OS::stat(filePath.c_str(), &curDocStat) || S_ISDIR(curDocStat.st_mode) ){
		if ( ! doingFileTransfer ) {
			string_nocase indexPath = filePath + "/index.html";
			if (! ACE_OS::stat(indexPath.c_str(), &curDocStat)) filePath = indexPath;
			else {
				indexPath = filePath + "/index.htm";
				if (! ACE_OS::stat(indexPath.c_str(), &curDocStat)) filePath = indexPath;
				else {
					response.setStatusCode(404);
					_handleError(response, "Requested file does not exist.");
					return;
				}
			}
		}
		else { // for the File I/O or Config dirs, provide a directory listing.
			_doDirectoryListing(filePath, origReqPath, response, docRootStat, curDocStat);
			return;
		}
	}

	response.setContentType(getMimeType(filePath.c_str()).c_str());

	// Perform a normal, full GET on a file.
	if ( !req.hasHeader("range") ) {
		ACE_HANDLE fd = ACE_OS::open(filePath.c_str(), O_RDONLY);

		if ( fd < 0 ) {
			ND_ERROR("[HTTP] Failed to open %s: %s\n", filePath.c_str(), ACE_OS::strerror(errno));
			response.setStatusCode(500);
			_handleError(response, "Document exists but could not be opened.");
			return;
		}

		int recv_cnt = 0;
		char buffer[8192];

		while ( (recv_cnt = ACE_OS::read(fd, buffer, 8192)) > 0 ) {
			string newContent(buffer, recv_cnt);
			response.addContent(newContent);
		}
		if ( recv_cnt < 0 ) {
			ND_ERROR("[HTTP] Error occurred while reading file %s: %s\n",
				filePath.c_str(), ACE_OS::strerror(errno));
			response.setStatusCode(500);

			_handleError(response, "Error occurred while reading file.");
			return;
		}

		ACE_OS::close(fd);
	}

	// Return only part or parts of file.
	else {
		ND_DEBUG("[HTTP] Responding to a partial %s request.\n", getOrHead.c_str());

		int fileLength = docStat.st_size;

		regex re_allRanges("^bytes\\s*=\\s*([\\d\\-,]+\\s*$)");
		regex re_comma(",");

		match_results<string::const_iterator> what;
		string origRange(req.getHeader("range").c_str());

		if ( regex_match(origRange, what, re_allRanges) ) {
			ND_DEBUG("[HTTP] Found Range header in correct initial format.\n");
			string ranges = what[1];

			sregex_token_iterator it(ranges.begin(), ranges.end(), re_comma, -1);
			sregex_token_iterator end;
			regex re_oneRange("^\\s*(\\d*)\\s*\\-\\s*(\\d*)\\s*$");
			int64_t fileBegin = 0, fileEnd = fileLength - 1;

			while (it != end) {
				string curRange(*it);
				match_results<string::const_iterator> nums;
				if ( regex_match(curRange, nums, re_oneRange) ) {

					if ( nums[1].str().empty() && ! nums[2].str().empty()) {
						fileBegin = fileLength - 1 - ACE_OS::atoi(nums[2].str().c_str());
					}
					else if ( ! nums[1].str().empty() && nums[2].str().empty()) {
						fileBegin = ACE_OS::atoi(nums[1].str().c_str());
					}
					else if ( ! nums[1].str().empty() && ! nums[2].str().empty()) {
						fileBegin = ACE_OS::atoi(nums[1].str().c_str());
						fileEnd = ACE_OS::atoi(nums[2].str().c_str());
					}
					else {
						ND_WARNING("[HTTP] Unusable Range value: %s\n", curRange.c_str());
						response.setStatusCode(416);
						_handleError(response);
						return;
					}

					if ( fileBegin < 0 || fileEnd < 0 || fileEnd < fileBegin ||
						fileEnd + 1 > fileLength ) {
						ND_WARNING("[HTTP] Invalid range (%d - %d) give for file of length %d.\n",
							fileBegin, fileEnd, fileLength);
						response.setStatusCode(416);
						_handleError(response);
						return;
					}

					ACE_HANDLE fd = ACE_OS::open(filePath.c_str(), O_RDONLY);
					if ( fd < 0 ) {
						ND_ERROR("[HTTP] Failed to open %s: %s\n",
							filePath.c_str(), ACE_OS::strerror(errno));
						response.setStatusCode(500);

						_handleError(response, "Document exists but could not be opened.");
						return;
					}

					ND_DEBUG("[HTTP] Returning octets %d - %d of requested file.\n",
						fileBegin, fileEnd);
					int recv_cnt = 0;
					const int bufferLen = 8192;
					char buffer[bufferLen];
					int64_t remaining = (fileEnd - fileBegin) + 1;

					if ( ACE_OS::lseek(fd, fileBegin, SEEK_SET) < 0 ) {
						ND_ERROR("[HTTP] Failed to lseek to %d in %s: %s\n",
							fileBegin, filePath.c_str(), ACE_OS::strerror(errno));
						response.setStatusCode(500);

						_handleError(response, "File seek failed.");
						ACE_OS::close(fd);
						return;
					}

					while ( remaining > 0 &&
						(recv_cnt = ACE_OS::read(fd, buffer, (remaining < bufferLen)? remaining : bufferLen)) > 0) {
						string newContent(buffer, recv_cnt);
						response.addContent(newContent);
						remaining -= recv_cnt;
					}
					if ( recv_cnt < 0 ) {
						ND_ERROR("[HTTP] Error occurred while reading file %s: %s\n",
							filePath.c_str(), ACE_OS::strerror(errno));
						response.setStatusCode(500);

						_handleError(response, "Error occurred while reading file.");
					}

					ACE_OS::close(fd);
				}
				else {
					ND_WARNING("[HTTP] Range '%s' did not match correctly.\n", curRange.c_str());
				}
				++it;
			}
		}
		else {
			ND_WARNING("[HTTP] Unparsable Range header value: %s\n", origRange.c_str());
			response.setStatusCode(416);
			_handleError(response);
			return;
		}
	}
}

void HttpManager::_handleOptions(const HttpRequest& req, HttpResponse& response) {
	ND_DEBUG("[HTTP] Received OPTIONS request.\n");

	response.setStatusCode(200);
	response.setContentType("text/plain");
	response.setHeader("Access-Control-Allow-Origin", "*");
	response.setHeader("Access-Control-Allow-Methods", "POST, GET, HEAD, OPTIONS");
	response.setHeader("Access-Control-Allow-Headers", "X-PINGOTHER");
	response.setHeader("Access-Control-Max-Age", "1728000");
}

void HttpManager::_handleError(HttpResponse& response, string errMsg /* = "" */) {
	ostringstream os;

	response.setContentType("text/html");

	os	<< "<html>\n<head>\n<title>" << response.getStatusCode() << " "
		<< response.getStatusStr() << "</title>\n</head>\n"
		<< "<body>\n<h1>" << response.getStatusStr() << "</h1>\n"
		<< "<p>" << errMsg << "</p>\n"
		<< "<hr>\n<p><i>" << serverDesc << "</i></p>\n</body>\n</html>" << endl;

	response.setContent(os.str());
}

}
