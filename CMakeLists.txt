cmake_minimum_required(VERSION 3.2)
project(antares-hw)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -D_GLIBCXX_USE_NANOSLEEP")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS} -D_GLIBCXX_USE_NANOSLEEP")

option(BUILD_TEST "Builds the tests" ON)
option(BUILD_CPP_WRAPPER "Build cpp interface wrapper" ON)
option(DEBUG_OUTPUT "Build library with console debug output" OFF)

add_definitions(-DPRINT_PACKET)
include_directories(include)

set(SRCS
		src/dxl_controller.c
		src/packet_handler.c
		src/port_controller.c
		src/port_handler.c
		src/port_handler_al.c)

set(HDRS
		include/dxl_controller.h
		include/packet_handler.h
		include/port_controller.h
		include/port_handler.h
		include/port_handler_al.h)


add_library(antares-hw SHARED ${SRCS} ${HDRS})
target_link_libraries(antares-hw rt)

if(BUILD_HW_TEST)
	set(TSTS tests/src/linux_hw_test.cpp)
	add_executable(antares_hw_test ${TSTS})
	target_link_libraries(antares_hw_test antares-hw)
endif()


install(FILES ${HDRS} DESTINATION include/${PROJECT_NAME})
install(TARGETS antares-hw DESTINATION lib/${PROJECT_NAME})
