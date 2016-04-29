include_directories(
	${CTSCE_SOURCE_DIR}/server
	${CTSCE_SOURCE_DIR}/lib
	${CTSCE_SOURCE_DIR}/modules
	${CTSCE_SOURCE_DIR}/modules/Devices
	${CTSCE_SOURCE_DIR}/modules/lib
	${CTSCE_SOURCE_DIR}/modules/AOS
	${CTSCE_SOURCE_DIR}/modules/ECC
	${CTSCE_SOURCE_DIR}/modules/Ethernet
	${XMLRPC_INCLUDE_DIRS}
	${ACE_INCLUDE_DIR} 
	${Boost_INCLUDE_DIRS}
	${LIBCONFIGPP_INCLUDE_DIR}
	${LIBMAGIC_INCLUDE_DIR}
	${LibNL_INCLUDE_DIR}
)

link_directories(
	${CTSCE_BINARY_DIR}/lib
	${CTSCE_BINARY_DIR}/modules/lib
	${CTSCE_SOURCE_DIR}/server/lib
	${XMLRPC_LIBRARY_DIRS}
	${Boost_LIBRARY_DIRS}
)

set(server_name "ctsced")

add_executable(netdata_test netdata_test.cpp)
target_link_libraries(netdata_test ${ACE_LIBRARY} NetworkData ${Boost_LIBRARIES})

add_executable(ethernet_frame_test ethernet_frame_test.cpp)
target_link_libraries(ethernet_frame_test ${ACE_LIBRARY} NetworkData ${Boost_LIBRARIES})

add_executable(ipv4_pkt_test ipv4_pkt_test.cpp)
target_link_libraries(ipv4_pkt_test ${ACE_LIBRARY} NetworkData ${Boost_LIBRARIES})

add_executable(arp_pkt_test arp_pkt_test.cpp)
target_link_libraries(arp_pkt_test ${ACE_LIBRARY} NetworkData ${Boost_LIBRARIES})

add_executable(hdlc_frame_test hdlc_frame_test.cpp)
target_link_libraries(hdlc_frame_test ${ACE_LIBRARY} NetworkData ${Boost_LIBRARIES})

add_executable(ppp_frame_test ppp_frame_test.cpp)
target_link_libraries(ppp_frame_test ${ACE_LIBRARY} NetworkData ${Boost_LIBRARIES})

add_executable(aos_frame_test aos_frame_test.cpp)
target_link_libraries(aos_frame_test ${ACE_LIBRARY} NetworkData ${Boost_LIBRARIES})

add_executable(tm_frame_test tm_frame_test.cpp)
target_link_libraries(tm_frame_test ${ACE_LIBRARY} NetworkData ${Boost_LIBRARIES})

add_executable(traffic_handler_test traffic_handler_test.cpp)
target_link_libraries(traffic_handler_test ${ACE_LIBRARY} NetworkData ${Boost_LIBRARIES} 
	${LIBNL_LIBRARY} ${LIBNL_ROUTE_LIBRARY})

add_executable(network_interface_test network_interface_test.cpp 
	${CTSCE_SOURCE_DIR}/modules/Devices/devEthernet.cpp
	${CTSCE_SOURCE_DIR}/server/SettingsManager.cpp)
target_link_libraries(network_interface_test 
	${ACE_LIBRARY} 
	NetworkData 
	CE_Module
	CE_Device
	${Boost_LIBRARIES}
	${LIBCONFIGPP_LIBRARIES}
	${XMLRPC_LIBRARY_NAMES}
	${LibNL_ROUTE_LIBRARY} ${LibNL_LIBRARY}
)

add_executable(ipv4_udp_test ipv4_udp_test.cpp)
target_link_libraries(ipv4_udp_test ${ACE_LIBRARY} NetworkData ${Boost_LIBRARIES})

add_executable(timeline_test timeline_test.cpp)
target_link_libraries(timeline_test ${ACE_LIBRARY} NetworkData ${Boost_LIBRARIES})

add_executable(rs_coding_test rs_coding_test.cpp)
target_link_libraries(rs_coding_test ${ACE_LIBRARY} NetworkData ${Boost_LIBRARIES})
