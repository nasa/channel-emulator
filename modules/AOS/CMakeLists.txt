include_directories(
	${CTSCE_SOURCE_DIR}/modules/AOS
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

# ---- CE_AOS_Module library -----

add_library(CE_AOS_Module SHARED
	AOS_Demux_Base.cpp
	AOS_MasterChannel_Service.cpp
	AOS_Mux_Base.cpp
	AOS_PhysicalChannel_Service.cpp
	AOS_VirtualChannel_Service.cpp
)

target_link_libraries(CE_AOS_Module CE_Module NetworkData
	${ACE_LIBRARY} ${Boost_LIBRARIES} ${LIBCONFIGPP_LIBRARIES})

set_target_properties(CE_AOS_Module PROPERTIES VERSION ${CTSCE_VERSION} )

install(TARGETS CE_AOS_Module LIBRARY DESTINATION lib COMPONENT ctsce-aos-modules)

set(MOD_LIBS CE_Module NetworkData)

# ---- AOS modules, linked with CE_AOS_Module ----

set(AOS_MODS
	AOS_AF_Gen
	AOS_AF_Rcv
	AOS_B_PDU_Add
	AOS_B_PDU_Remove
	AOS_MC_Demux
	AOS_MC_Frame
	AOS_MC_Mux
	AOS_M_PDU_Add
	AOS_M_PDU_Remove
	AOS_VC_Demux
	AOS_VC_Frame
	AOS_VC_Gen
	AOS_VC_Mux
	AOS_VC_Rcv
	AOS_Macros
	AOS_OCF_Insert
	AOS_OCF_Extract
	AOS_IN_SDU_Insert
	AOS_IN_SDU_Extract
)

foreach (aosmod ${AOS_MODS})
	set(modname "mod${aosmod}")
	add_library(${modname} MODULE ${modname}.cpp ${modname}_Interface.cpp)
	set_target_properties(${modname} PROPERTIES VERSION ${CTSCE_VERSION})
	target_link_libraries(${modname} CE_AOS_Module ${MOD_LIBS})
	install(TARGETS ${modname} LIBRARY DESTINATION lib/ctsce-modules COMPONENT ctsce-aos-modules)
endforeach(aosmod)
