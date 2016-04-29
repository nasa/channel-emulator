include_directories(
	${CTSCE_SOURCE_DIR}/server
	${CTSCE_SOURCE_DIR}/server/httpd
	${CTSCE_SOURCE_DIR}/modules/Devices
	${CTSCE_SOURCE_DIR}/modules/lib
	${XMLRPC_INCLUDE_DIRS}
	${ACE_INCLUDE_DIR} 
	${Boost_INCLUDE_DIRS}
	${LIBCONFIGPP_INCLUDE_DIR}
	${LIBMAGIC_INCLUDE_DIR}
)

add_library(CEHttpd STATIC ClientAcceptor.cpp ClientService.cpp HttpManager.cpp
	HttpRequest.cpp HttpResponse.cpp Rfc1123Date.cpp)
