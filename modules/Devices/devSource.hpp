/* -*- C++ -*- */

//=============================================================================
/**
 * @file   devSource.hpp
 * @author Tad Kollar  
 *
 * $Id: devSource.hpp 2354 2013-09-06 21:34:24Z tkollar $
 * Copyright (c) 2011.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================


#ifndef _NASA_DEV_SOURCE_HPP_
#define _NASA_DEV_SOURCE_HPP_

#include "CE_Device.hpp"
#include "Device_Interface_Templ.hpp"
#include "SettingsManager.hpp"
#include "NetworkData.hpp"
#include "httpd/string_nocase.hpp"
#include <ace/Pipe.h>
#include <string>
#include <set>

namespace nasaCE {

//=============================================================================
/**
 * @class devSource
 * @author Tad Kollar  
 * @brief Provide a continuous stream of traffic.
*/
//=============================================================================
class devSource: public CE_Device {
public:
	/// Default constructor.
    devSource(const std::string& newName = "", const CE_DLL* newDLLPtr = 0);

	/// Destructor.
    ~devSource();

    /// Read-only accessor to read-side pipe file descriptor. Different from
	/// get_handle(), which returns the write-side, the one the reactor is
	/// watching.
    ACE_HANDLE getHandle();

	/// What kind of device this is.
	std::string getType() const { return "Source"; }

	/// Read-only accessor to _patternBufferSize.
	int getPatternBufferSize() const { return _patternBufferSize; }

	/// Set _patternBufferSize and change the pattern buffer length.
	/// @param newSize The size to change the _pattern buffer to.
	void setPatternBufferSize(const int newSize) { _patternBufferSize = newSize; _rebuildPatternBuffer(); }

	/// Set a new pattern, which may be used in _patternBuffer partially
	/// or repeatedly depending on _patternBufferSize.
	/// @param patternBuf A buffer with the source pattern.
	/// @param bufLen The length of patternBuf,
	/// @param restart If true, stop traffic, rebuild the buffer, then start traffic.
	void setPattern(const uint8_t* patternBuf, const int bufLen, const bool restart = true);

	/// Set a new pattern, which may be used in _patternBuffer partially
	/// or repeatedly depending on _patternBufferSize.
	/// @param newPattern Vector that will be extracted to _patternBuffer.
	/// @param restart If true, stop traffic, rebuild the buffer, then start traffic.
	void setPattern(const std::vector<uint8_t>& newPattern, const bool restart = true);

	/// Fill the supplied vector with the contents of _patternSetting.
	/// @param pattern The vector to fill.
	void getPattern(std::vector<uint8_t>& pattern) const;

    /// The functional loop that makes this an active object.
    int svc();

private:

	/// Descriptors for the source pipe.
	ACE_Pipe _pipe;

	/// The actual pattern buffer to send or receive into.
	NetworkData* _patternBuffer;

	/// If the user specifies a pattern, store it in here.
	Setting& _patternSetting;

	/// The size of the pattern buffer.
	Setting& _patternBufferSize;

	/// Restrict access to _patternBuffer so it can be modified on the fly.
	ACE_Process_Mutex _patternBufferAccess;

	/// Copy _patternSetting into _patternBuffer until _patternBufferSize has been reached.
	/// @param restart If true, stop traffic, rebuild the buffer, then start traffic.
	void _rebuildPatternBuffer(const bool restart = true);

	/// Create the pipe.
	void _openPipe();

	/// Close both ends of the pipe.
	void _closePipe();

	/// The names of recognized patterns.
	std::set<string_nocase> _knownPatterns;


}; // class devSource

} // namespace nasaCE

#endif // _NASA_DEV_SOURCE_HPP_

