/* -*- C++ -*- */

//=============================================================================
/**
 * @file   devFile.hpp
 * @author Tad Kollar  
 *
 * $Id: devFile.hpp 2035 2013-06-06 18:52:43Z tkollar $
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _NASA_FILE_DEVICE_HPP_
#define _NASA_FILE_DEVICE_HPP_

#include "CE_Device.hpp"
#include "Device_Interface_Templ.hpp"
#include "SettingsManager.hpp"
#include <string>

namespace nasaCE {

//=============================================================================
/**
 * @class devFile
 * @author Tad Kollar  
 * @brief Treat a file as a communications endpoint.
*/
//=============================================================================
class devFile: public CE_Device {
public:
	// --- EXCEPTIONS ---------------------------------------------------------------
	/// @class AlreadyOpen
	/// @brief Thrown when an open operation is attempted on an already open file.
	struct AlreadyOpen: public Exception {
		AlreadyOpen(const std::string e): Exception(e) { }
	};

	/// @class FilenameNotSet
	/// @brief Thrown when an operation is attempted before the filename is set.
	struct FilenameNotSet: public Exception {
		FilenameNotSet(const std::string e): Exception(e) { }
	};

	/// The config file path to common file device settings.
	static std::string fileDeviceSection;

	/// Default constructor.
    devFile(const std::string& newName = "", const CE_DLL* newDLLPtr = 0);

	/// Destructor.
    ~devFile();

    /// Set a new filename if the file is unopened.
    /// @param newFilename The name of the file within the globally-configured directory.
    /// @throw AlreadyOpen If the file is already in use.
    void setFilename(const std::string& newFilename);

    /// Read-only accessor to _filename.
    std::string getFilename() const;

    /// If the file is not open yet, open it in write-only mode.
    /// @throw AlreadyOpen If the file has already been opened and not closed yet.
    /// @throw OperationFailed If the open failed for another reason.
    ACE_HANDLE openForWriting(const bool append = false);

    /// If the file is not open yet, open it in read-only mode.
    /// @throw AlreadyOpen If the file has already been opened and not closed yet.
    /// @throw OperationFailed If the open failed for another reason.
    ACE_HANDLE openForReading();

    /// Read-only accessor to _fd.
    ACE_HANDLE getHandle();

    /// Close the file if it has been opened.
    int close();

    /// Determine if the file has been opened yet.
    bool isOpen();

	std::string getType() const { return "File"; }


private:
	/// The name of the file to operate on, without the path.
	Setting& _filename;

	/// The full path to the file. Common to all other file devices.
	std::string _directoryName;

	/// Whether to open for reading or writing.
	Setting& _isInput;

	/// Descriptor for the open file.
	ACE_HANDLE _fd;

	/// Construct the total path name from the stored filename and
	/// the globally configured directory name.
	/// @throw FilenameNotSet If _filename is empty.
	std::string _pathname();

}; // class devFile

} // namespace nasaCE

#endif // _NASA_FILE_DEVICE_HPP_
