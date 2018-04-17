cmake_minimum_required(VERSION 3.2)
project(antares-hw)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -D_GLIBCXX_USE_NANOSLEEP -Wall")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS} -D_GLIBCXX_USE_NANOSLEEP -Wall")

option(ANTARES_HW_BUILD_TEST "Builds the tests" ON)
option(ANTARES_HW_BUILD_CPP_WRAPPER "Build cpp interface wrapper" ON)
option(ANTARES_HW_BUFFER_LENGTH_CHECK "Check buffer length in all functions" ON)

file(GLOB_RECURSE SRCS "src/*.c")
file(GLOB_RECURSE HDRS "src/*.h")

add_library(antares-hw SHARED ${SRCS} ${HDRS})
target_include_directories(antares-hw PUBLIC src)

if(BUILD_HW_TEST)
endif()

install(FILES ${HDRS} DESTINATION src/${PROJECT_NAME})
install(TARGETS antares-hw DESTINATION lib/${PROJECT_NAME})
