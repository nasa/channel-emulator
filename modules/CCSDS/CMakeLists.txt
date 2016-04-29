include_directories(
	${CTSCE_SOURCE_DIR}/modules/CCSDS
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

set(CCSDS_MODS
	ASM_Add
	ASM_Remove
	EncapPkt_Add
	EncapPkt_Remove
	PseudoRandomize
)
set(MOD_LIBS CE_Module NetworkData)

foreach (stdmod ${CCSDS_MODS})
        set(modname "mod${stdmod}")
        add_library(${modname} MODULE ${modname}.cpp ${modname}_Interface.cpp)
        set_target_properties(${modname} PROPERTIES VERSION ${CTSCE_VERSION})
        target_link_libraries(${modname} ${MOD_LIBS})
        install(TARGETS ${modname} LIBRARY DESTINATION lib/ctsce-modules COMPONENT ctsce-common-modules)
endforeach(stdmod)
