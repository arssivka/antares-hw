cmake_minimum_required(VERSION 3.2)
project(antares-hw)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -D_GLIBCXX_USE_NANOSLEEP -Wall")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS} -D_GLIBCXX_USE_NANOSLEEP -Wall")

option(ANTARES_HW_BUILD_TEST "Builds the tests" OFF)
option(ANTARES_HW_BUILD_CPP_WRAPPER "Build cpp interface wrapper" ON)
option(ANTARES_HW_BUFFER_LENGTH_CHECK "Check buffer length in all functions" ON)

file(GLOB_RECURSE SRCS "src/*.c")
file(GLOB_RECURSE HDRS "src/*.h")

add_library(${PROJECT_NAME} STATIC ${SRCS} ${HDRS})
target_include_directories(${PROJECT_NAME} PUBLIC src)

if(ANTARES_HW_BUILD_TEST)
    set(Boost_USE_STATIC_LIBS ON) # Link main function for Boost Unit Test Framework
    find_package(Boost COMPONENTS unit_test_framework REQUIRED)
    file(GLOB_RECURSE TSTS "tst/*.cpp" "tst/.c" "tst/*.h")
    add_executable(${PROJECT_NAME}-test ${TSTS})
    target_link_libraries(${PROJECT_NAME}-test ${PROJECT_NAME} ${Boost_LIBRARIES})
    target_include_directories(${PROJECT_NAME}-test PUBLIC ${Boost_INCLUDE_DIRS})
endif()

install(FILES ${HDRS} DESTINATION include/${PROJECT_NAME})
install(TARGETS antares-hw DESTINATION lib/${PROJECT_NAME})
