include_directories(
	${CTSCE_SOURCE_DIR}/lib
	${CTSCE_SOURCE_DIR}/server
	${CTSCE_SOURCE_DIR}/modules/lib
	${CTSCE_SOURCE_DIR}/modules/Devices
	${ACE_INCLUDE_DIR}
	${Boost_INCLUDE_DIRS}
)

link_directories(
	${CTSCE_BINARY_DIR}/lib
	${CTSCE_BINARY_DIR}/modules/lib
	${Boost_LIBRARY_DIRS}
)

add_library(CE_Module SHARED BaseTask.cpp BaseTrafficHandler.cpp HandlerLink.cpp
        PeriodicLink.cpp PeriodicTrafficHandler.cpp PeriodicTransmitter.cpp FdInputHandler.cpp)
target_link_libraries(CE_Module NetworkData ${ACE_LIBRARY} ${Boost_LIBRARIES} 
	${LIBCONFIGPP_LIBRARIES})
set_target_properties(CE_Module PROPERTIES VERSION ${CTSCE_VERSION} )


add_library(CE_Device SHARED Ipv4DeviceBase.cpp Tcp4DeviceBase.cpp)
target_link_libraries(CE_Device ${ACE_LIBRARY} ${Boost_LIBRARIES}
        ${LIBCONFIGPP_LIBRARIES})
set_target_properties(CE_Device PROPERTIES VERSION ${CTSCE_VERSION} )

install(TARGETS CE_Module CE_Device LIBRARY DESTINATION lib COMPONENT ctsce)
