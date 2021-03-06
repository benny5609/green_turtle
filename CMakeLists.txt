CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(green_turtle)

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/green_turtle)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/deps/gtest-1.6.0)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/deps/gtest-1.6.0/include)

SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)

#LINK_DIRECTORIES(${CMAKE_SOURCE_DIR}/lib)

IF(NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE "Debug")
ENDIF()

SET(CXX_FLAGS
    -g
    -Wall
    -Wextra
    -Werror
    -std=c++0x
    #-stdlib=libc++
    -fomit-frame-pointer
    -fstack-protector-all
    -fno-rtti
    -fno-exceptions
    -DGTEST_HAS_RTTI=0
    -DGTEST_HAS_EXCEPTIONS=0
    -DGTEST_HAS_PTHREAD=1
    -DGTEST_USE_OWN_TR1_TUPLE=0
    )
STRING(REPLACE ";" " " CMAKE_CXX_FLAGS "${CXX_FLAGS}")
MESSAGE(STATUS "CXX flags : " "${CMAKE_CXX_FLAGS}")

SET(CMAKE_CXX_COMPILER "g++")
SET(CMAKE_CXX_FLAGS_DEBUG   "-DDEBUG -D_DEBUG -D_LIBCPP_DEBUG2=1 -O0 -pthread")
SET(CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG -O2 -pthread")

FIND_LIBRARY(TCMALLOC_LIBRARY NAMES tcmalloc)
FIND_LIBRARY(JEMALLOC_LIBRARY NAMES jemalloc)

ADD_SUBDIRECTORY(deps/gtest-1.6.0)
ADD_SUBDIRECTORY(green_turtle)
ADD_SUBDIRECTORY(test)
ADD_SUBDIRECTORY(unittest)
