include_directories(
	${CTSCE_SOURCE_DIR}/modules/Devices
	${CTSCE_SOURCE_DIR}/lib
	${CTSCE_SOURCE_DIR}/modules/lib
	${CTSCE_SOURCE_DIR}/server
	${LibNL_INCLUDE_DIR}
	${ACE_INCLUDE_DIR}
)

link_directories(
	${CTSCE_BINARY_DIR}/lib
	${CTSCE_SOURCE_DIR}/modules/lib
)

set(STANDARD_MODS
	Ethernet
	File
	Sink
	Source
	Tcp4Client
	Tcp4Server
	Udp4
)
set(MOD_LIBS CE_Device CE_Module NetworkData)

foreach (modname ${STANDARD_MODS})
	set(devname "dev${modname}")
	add_library(${devname} MODULE ${devname}.cpp ${devname}_Interface.cpp)
	set_target_properties(${devname} PROPERTIES VERSION ${CTSCE_VERSION})
	if ( ${modname} STREQUAL "Ethernet" )
		target_link_libraries(${devname} ${MOD_LIBS}
			${LibNL_LIBRARY} ${LibNL_ROUTE_LIBRARY})
	else ( ${modname} STREQUAL "Ethernet" )
		target_link_libraries(${devname} ${MOD_LIBS})
	endif ( ${modname} STREQUAL "Ethernet" )
	install(TARGETS ${devname} LIBRARY DESTINATION lib/ctsce-modules COMPONENT ctsce-common-modules)
endforeach(modname)
