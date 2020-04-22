#Find the Rabbitmq C library

INCLUDE(LibFindMacros)

# Find the include directories
FIND_PATH(Rabbitmqc_INCLUDE_DIR
	NAMES amqp.h
    HINTS ${Rabbitmqc_DIR}/include
	)

FIND_LIBRARY(Rabbitmqc_LIBRARY
	NAMES rabbitmq.4
    HINTS ${Rabbitmqc_DIR}/lib
	)
get_filename_component(Rabbitmqc_LIB_DIR ${Rabbitmqc_LIBRARY} DIRECTORY)
message("Rabbitmqc_INCLUDE_DIR: ${Rabbitmqc_INCLUDE_DIR}")
message("Rabbitmqc_LIB_DIR: ${Rabbitmqc_LIB_DIR}")

SET(Rabbitmqc_PROCESS_INCLUDES Rabbitmqc_INCLUDE_DIR)
SET(Rabbitmqc_PROCESS_LIBS Rabbitmqc_LIBRARY)

LIBFIND_PROCESS(Rabbitmqc)

find_file(_Rabbitmqc_SSL_HEADER
  NAMES amqp_ssl_socket.h
  PATHS ${Rabbitmqc_INCLUDE_DIR}
  NO_DEFAULT_PATH
  )

string(COMPARE NOTEQUAL "${_Rabbitmqc_SSL_HEADER}"
  "_Rabbitmqc_SSL_HEADER-NOTFOUND" _rmqc_ssl_enabled)

set(Rabbitmqc_SSL_ENABLED ${_rmqc_ssl_enabled} CACHE BOOL
  "Rabbitmqc is SSL Enabled" FORCE)
mark_as_advanced(_Rabbitmqc_SSL_HEADER Rabbitmqc_SSL_ENABLED)
