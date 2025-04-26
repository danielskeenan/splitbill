set(CPACK_PACKAGE_VENDOR "${PROJECT_AUTHOR}")
set(CPACK_PACKAGE_CONTACT "${PROJECT_AUTHOR} <dk@dankeenan.org>")
#set(CPACK_PACKAGE_ICON "${PROJECT_SOURCE_DIR}/resources/app-icon.svg")
set(CPACK_PACKAGE_CHECKSUM "SHA256")
include("${CMAKE_CURRENT_LIST_DIR}/license.cmake")
set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_BINARY_DIR}/LICENSE.html")
set(CPACK_PACKAGE_EXECUTABLES "splitbill;${PROJECT_DISPLAY_NAME}")
set(CPACK_MONOLITHIC_INSTALL On)
set(CPACK_STRIP_FILES On)

# Linux metadata
if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    include(GNUInstallDirs)
    # .desktop file is filled in with configure-time info
    configure_file("${PROJECT_SOURCE_DIR}/meta/linux/splitbill.desktop" "${PROJECT_BINARY_DIR}/meta/splitbill.desktop")
    install(FILES "${PROJECT_BINARY_DIR}/meta/splitbill.desktop"
        DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/applications")

    # App icon
    install(FILES "${PROJECT_SOURCE_DIR}/resources/app-icon.svg"
        RENAME "splitbill.svg"
        DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/scalable/apps")

    set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS On)
    set(CPACK_DEBIAN_PACKAGE_SECTION "misc")
    set(CPACK_RPM_PACKAGE_LICENSE "GPL-3.0")
    find_program(PANDOC_PATH NAMES pandoc)
    if (NOT PANDOC_PATH)
        message(FATAL_ERROR "Could not find pandoc, required for building installers")
    endif ()
    execute_process(COMMAND ${PANDOC_PATH}
            "--standalone" "--embed-resources" "--from" "commonmark" "--to" "plain" "${PROJECT_SOURCE_DIR}/README.md"
            OUTPUT_FILE "${PROJECT_BINARY_DIR}/package_description.txt"
            ENCODING "UTF8"
    )
    set(CPACK_PACKAGE_DESCRIPTION_FILE "${PROJECT_BINARY_DIR}/package_description.txt")
endif ()

# Windows metadata
if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(CPACK_PACKAGE_INSTALL_DIRECTORY "${CMAKE_PROJECT_NAME}")
    set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_BINARY_DIR}/LICENSE.rtf")
    set(CPACK_NSIS_DISPLAY_NAME "${PROJECT_DISPLAY_NAME}")
    set(CPACK_NSIS_PACKAGE_NAME "${PROJECT_DISPLAY_NAME}")
    set(CMAKE_INSTALL_UCRT_LIBRARIES TRUE)
    include(InstallRequiredSystemLibraries)
endif ()

# Mac OS metadata
if (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_BINARY_DIR}/LICENSE.rtf")
endif ()

include(CPack)
