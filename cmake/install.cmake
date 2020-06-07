install(TARGETS splitbill RUNTIME BUNDLE)

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
endif ()

# Windows metadata
if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(CPACK_NSIS_DISPLAY_NAME "Split Bill")
    set(CPACK_NSIS_PACKAGE_NAME "Split Bill")
    set(CMAKE_INSTALL_UCRT_LIBRARIES TRUE)
    include(InstallRequiredSystemLibraries)

    # Bundle Qt runtime
    include("${CMAKE_CURRENT_LIST_DIR}/Windeployqt.cmake")
    windeployqt(splitbill qt)
endif ()

set(CPACK_PACKAGE_VENDOR "${PROJECT_AUTHOR}")
set(CPACK_PACKAGE_CONTACT "${PROJECT_AUTHOR} <dk@dankeenan.org>")
set(CPACK_PACKAGE_ICON "${PROJECT_SOURCE_DIR}/resources/app-icon.svg")
set(CPACK_PACKAGE_CHECKSUM "MD5")
include("${CMAKE_CURRENT_LIST_DIR}/license.cmake")
set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_BINARY_DIR}/LICENSE.html")
set(CPACK_PACKAGE_EXECUTABLES splitbill;"Split Bill")
set(CPACK_MONOLITHIC_INSTALL On)
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS On)
set(CPACK_DEBIAN_PACKAGE_SECTION "misc")

include(CPack)
