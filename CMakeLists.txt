cmake_minimum_required(VERSION 3.26)
project(splitbill
    VERSION 0.1.1
    DESCRIPTION "Split bills among several people fairly")
set(PROJECT_DISPLAY_NAME "Split Bill")
set(PROJECT_AUTHOR "Dan Keenan")
set(PROJECT_AUTHOR_DOMAIN "org.dankeenan")
set(CMAKE_CXX_STANDARD 17)

set(BUILD_APP On CACHE BOOL "Build program")
if (BUILD_APP)
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTORCC ON)
    set(CMAKE_AUTOUIC ON)

    # Platform config
    # This is more portable across compilers compared to other methods
    if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
        add_compile_definitions(PLATFORM_LINUX)
    elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
        add_compile_definitions(PLATFORM_WINDOWS)
    elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
        add_compile_definitions(PLATFORM_MACOS)
    endif ()

    include_directories(${PROJECT_SOURCE_DIR}/include)

    find_package(Qt6 COMPONENTS
        Core
        Svg
        Widgets
        REQUIRED
    )
    qt_standard_project_setup()

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
