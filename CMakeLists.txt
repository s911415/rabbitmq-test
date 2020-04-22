cmake_minimum_required(VERSION 3.16)
project(rabbitmq_test)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/Modules)
set(CMAKE_CXX_STANDARD 14)

if (WIN32)
    set(Rabbitmqc_DIR "C:/Program Files (x86)/rabbitmq-c")
endif()

find_package(Rabbitmqc REQUIRED)

include_directories(${Rabbitmqc_INCLUDE_DIR})
link_directories(${Rabbitmqc_LIB_DIR})
set(CMAKE_EXE_LINKER_FLAGS "-static")

set(COMMON_FILES rabbit_mq.cpp rabbit_mq.h def.h)
add_executable(rabbitmq_sender sender.cpp ${COMMON_FILES})
add_executable(rabbitmq_receiver receiver.cpp ${COMMON_FILES})

target_link_libraries(rabbitmq_sender ${Rabbitmqc_LIBRARY} "-static")
target_link_libraries(rabbitmq_receiver ${Rabbitmqc_LIBRARY} "-static")
