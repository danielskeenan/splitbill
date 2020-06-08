cmake_minimum_required(VERSION 3.16)
project(splitbill
    VERSION 0.0.1
    DESCRIPTION "Split bills among several people fairly")
set(PROJECT_AUTHOR "Dan Keenan")
set(PROJECT_AUTHOR_DOMAIN "org.dankeenan")
set(CMAKE_CXX_STANDARD 17)
include("${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
conan_basic_setup(TARGETS)
add_definitions(-DBOOST_AUTO_LINK_SYSTEM)

add_subdirectory(src)
add_subdirectory(resources)

if(${CMAKE_PROJECT_NAME} STREQUAL ${PROJECT_NAME})
    include(CTest)
endif()
if(${CMAKE_PROJECT_NAME} STREQUAL ${PROJECT_NAME} AND ${BUILD_TESTING})
    add_subdirectory(tests)
endif()

set(BUILD_PACKAGE Off CACHE BOOL "Create packages, installers, etc.")
if (BUILD_PACKAGE)
    include(cmake/install.cmake)
endif ()
