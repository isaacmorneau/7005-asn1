cmake_minimum_required (VERSION 2.6)
project (7005-Asn1)

set (7005-Asn1_VERSION_MAJOR 1)
set (7005-Asn1_VERSION_MINOR 0)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_definitions(-D_POSIX_C_SOURCE=200809L)
add_definitions(-D_DEFAULT_SOURCE)

include_directories(src)

set(MAIN
    src/main.c
    src/socket.c
    src/socket.h
    )

add_executable(7005-Asn1 ${MAIN})
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(7005-Asn1 rt Threads::Threads)
