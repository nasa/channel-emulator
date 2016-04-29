include_directories(
	${CTSCE_SOURCE_DIR}/server
	${CTSCE_SOURCE_DIR}/server/httpd
	${CTSCE_SOURCE_DIR}/lib
	${CTSCE_SOURCE_DIR}/modules/Devices
	${CTSCE_SOURCE_DIR}/modules/lib
	${XMLRPC_INCLUDE_DIRS}
	${ACE_INCLUDE_DIR}
	${Boost_INCLUDE_DIRS}
	${LIBCONFIGPP_INCLUDE_DIR}
	${LIBMAGIC_INCLUDE_DIR}
)

link_directories(
	${CTSCE_BINARY_DIR}/lib
	${CTSCE_BINARY_DIR}/modules/lib
	${CTSCE_SOURCE_DIR}/server/lib
	${XMLRPC_LIBRARY_DIRS}
	${Boost_LIBRARY_DIRS}
)

set(server_name "ctsced")

add_executable(${server_name} Settings_Interface.cpp CE_DLL_Interface.cpp CE_DLL_Ops.cpp CE_DLL_Registry.cpp
	Channel.cpp ChannelRegistry.cpp Channel_Interface.cpp DelayedResults.cpp
	DeviceRegistry.cpp Device_Interface.cpp SettingsManager.cpp Time_Handler.cpp
	Timer_Interface.cpp XML_RPC_Server.cpp main.cpp)

target_link_libraries(${server_name}
	NetworkData
	CE_Module
	CEHttpd
	${XMLRPC_LIBRARY_NAMES}
	${ACE_LIBRARY}
	${Boost_LIBRARIES}
	${LIBCONFIGPP_LIBRARIES}
	${LIBMAGIC_LIBRARY}
	-ldl
	-lpthread
)

# set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -fPIC")
# set(CMAKE_EXE_LINKER_FLAGS "-Wl,--export-dynamic")

install(TARGETS ${server_name} RUNTIME DESTINATION sbin COMPONENT ctsce)

add_subdirectory (httpd)
