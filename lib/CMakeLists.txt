include_directories(${CTSCE_SOURCE_DIR}/lib ${ACE_INCLUDE_DIR} ${Boost_INCLUDE_DIRS})
link_directories(${CTSCE_BINARY_DIR}/lib ${Boost_LIBRARY_DIRS})

add_library(NetworkData SHARED AOS_Bitstream_PDU.cpp AOS_Multiplexing_PDU.cpp
        AOS_Transfer_Frame.cpp ArpPacket.cpp BitPattern.cpp Dot1qFrame.cpp EncapsulationPacket.cpp
	EthernetFrame.cpp HdlcFrame.cpp IPv4Addr.cpp IPv4Packet.cpp IPv4_UDP_Datagram.cpp
	IpcpPacket.cpp LcpFsm.cpp LcpPacket.cpp MacAddress.cpp NetworkData.cpp
	PppConfPacket.cpp PppFrame.cpp RSBase.cpp RSDecoder.cpp RSEncoder.cpp SpacePacket.cpp utils.cpp
	TM_Transfer_Frame.cpp
)

target_link_libraries(NetworkData ${ACE_LIBRARY} ${Boost_LIBRARIES})

set_target_properties(NetworkData PROPERTIES VERSION ${CTSCE_VERSION} )

install(TARGETS NetworkData LIBRARY DESTINATION lib COMPONENT ctsce)
