cmake_minimum_required (VERSION 2.6)
project (CTSCE)
message (STATUS "CMAKE_INSTALL_PREFIX is ${CMAKE_INSTALL_PREFIX}")

set (CTSCE_VERSION_MAJOR 1)
set (CTSCE_VERSION_MINOR 8)
set (CTSCE_VERSION_PATCH 6)
set (CTSCE_VERSION_TWEAK 5)
set (CTSCE_VERSION "${CTSCE_VERSION_MAJOR}.${CTSCE_VERSION_MINOR}.${CTSCE_VERSION_PATCH}.${CTSCE_VERSION_TWEAK}")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
include(GNUInstallDirs)

find_package ( Boost COMPONENTS regex REQUIRED )
find_package ( XMLRPC REQUIRED c++2 client server-util )
find_package ( ACE REQUIRED )
find_package ( LibConfig REQUIRED )
find_package ( LibMagic REQUIRED )
find_package ( LibNL REQUIRED )

# Make sure ACE is the right version and built with timed message blocks.
file (READ "${ACE_INCLUDE_DIR}/ace/Version.h" _ACE_Version_h_contents)

string (REGEX REPLACE ".*#define ACE_VERSION \"([0-9]+.[0-9]+.[0-9]+)\".*" \\1 ACE_VERSION "${_ACE_Version_h_contents}")

if ( ${ACE_VERSION} VERSION_GREATER 6.0.0 AND ${ACE_VERSION} VERSION_LESS 6.0.7 )
	message ("Good, ACE is version ${ACE_VERSION}.")
else ( ${ACE_VERSION} VERSION_GREATER 6.0.0 AND ${ACE_VERSION} VERSION_LESS 6.0.7 )
	message (FATAL_ERROR "Please install one of ACE version 6.0.1 - 6.0.6 instead of ${ACE_VERSION}.")
endif ( ${ACE_VERSION} VERSION_GREATER 6.0.0 AND ${ACE_VERSION} VERSION_LESS 6.0.7 )

include (CheckSymbolExists)
check_symbol_exists(ACE_HAS_TIMED_MESSAGE_BLOCKS "${ACE_INCLUDE_DIR}/ace/config.h" ACE_HAS_TMB)

if ( ACE_HAS_TMB )
  message( STATUS "ACE library has timed message blocks, good." )
else ( ACE_HAS_TMB )
  message( STATUS "ACE library does not have timed message blocks." )
  message( FATAL_ERROR "Please rebuild ACE with ACE_HAS_TIMED_MESSAGE_BLOCKS defined." )
endif ( ACE_HAS_TMB )

# set(CMAKE_CXX_FLAGS "-g -ggdb -O3")
set(CMAKE_CXX_FLAGS "-w -O3")

add_subdirectory (lib)
add_subdirectory (modules)
add_subdirectory (server)
# add_subdirectory (test)
