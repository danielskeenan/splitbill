cmake_minimum_required(VERSION 3.16)
project(splitbill
    VERSION 0.1.0
    DESCRIPTION "Split bills among several people fairly")
set(PROJECT_DISPLAY_NAME "Split Bill")
set(PROJECT_AUTHOR "Dan Keenan")
set(PROJECT_AUTHOR_DOMAIN "org.dankeenan")
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_MODULE_PATH "${PROJECT_BINARY_DIR}")

set(BUILD_APP On CACHE BOOL "Build program")
if (BUILD_APP)
    add_subdirectory(src)
    add_subdirectory(resources)

    if (${CMAKE_PROJECT_NAME} STREQUAL ${PROJECT_NAME})
        include(CTest)
    endif ()
    if (${CMAKE_PROJECT_NAME} STREQUAL ${PROJECT_NAME} AND ${BUILD_TESTING})
        add_subdirectory(tests)
    endif ()

    set(BUILD_PACKAGE Off CACHE BOOL "Create packages, installers, etc.")
    if (BUILD_PACKAGE)
        include(cmake/install.cmake)
    endif ()
endif ()

set(BUILD_DOC Off CACHE BOOL "Create documentation")
if (BUILD_DOC)
    add_subdirectory(doc)
endif ()
