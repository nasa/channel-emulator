include_directories(
	${CTSCE_SOURCE_DIR}/lib
	${CTSCE_SOURCE_DIR}/modules/lib
	${CTSCE_SOURCE_DIR}/modules/Devices
	${CTSCE_SOURCE_DIR}/server
	${ACE_INCLUDE_DIR}
)

link_directories(
	${CTSCE_BINARY_DIR}/lib
	${CTSCE_SOURCE_DIR}/modules/lib
)

set(STANDARD_MODS
	UDP_Add UDP_Remove
	Tcp4Receiver Tcp4Transmitter
	Udp4Transmitter Udp4Receiver
)
set(MOD_LIBS CE_Module NetworkData)

foreach (modname ${STANDARD_MODS})
	add_library(mod${modname} MODULE mod${modname}.cpp mod${modname}_Interface.cpp)
	set_target_properties(mod${modname} PROPERTIES VERSION ${CTSCE_VERSION})
	target_link_libraries(mod${modname} CE_Module NetworkData)
	install(TARGETS mod${modname} LIBRARY DESTINATION lib/ctsce-modules COMPONENT ctsce-common-modules)
endforeach(modname)
