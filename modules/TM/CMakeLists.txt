include_directories(
	${CTSCE_SOURCE_DIR}/modules/TM
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

# ---- CE_TM_Module library -----

add_library(CE_TM_Module SHARED
	TM_PhysicalChannel_Service.cpp
	TM_MasterChannel_Service.cpp
	TM_VirtualChannel_Service.cpp
	TM_Mux_Base.cpp
	TM_Demux_Base.cpp
)

target_link_libraries(CE_TM_Module CE_Module NetworkData
	${ACE_LIBRARY} ${Boost_LIBRARIES} ${LIBCONFIGPP_LIBRARIES})

set_target_properties(CE_TM_Module PROPERTIES VERSION ${CTSCE_VERSION} )

install(TARGETS CE_TM_Module LIBRARY DESTINATION lib COMPONENT ctsce-tm-modules)

set(MOD_LIBS CE_Module NetworkData)

# ---- TM modules, linked with CE_TM_Module ----

set(TM_MODS
	TM_AF_Gen
	TM_AF_Rcv
	TM_MC_Demux
	TM_MC_Gen
	TM_MC_Frame
	TM_MC_Mux
	TM_MC_Rcv
	TM_VC_Demux
	TM_VC_Frame
	TM_VC_Gen_Access
	TM_VC_Gen_Packet
	TM_VC_Mux
	TM_VC_Rcv_Access
	TM_VC_Rcv_Packet
	TM_OCF_Insert
	TM_OCF_Extract
	TM_FSH_Insert
	TM_FSH_Extract
	TM_Macros
)

foreach (tmmod ${TM_MODS})
	set(modname "mod${tmmod}")
	add_library(${modname} MODULE ${modname}.cpp ${modname}_Interface.cpp)
	set_target_properties(${modname} PROPERTIES VERSION ${CTSCE_VERSION})
	target_link_libraries(${modname} CE_TM_Module ${MOD_LIBS})
	install(TARGETS ${modname} LIBRARY DESTINATION lib/ctsce-modules COMPONENT ctsce-tm-modules)
endforeach(tmmod)
