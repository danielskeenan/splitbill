install(TARGETS splitbill RUNTIME BUNDLE)

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
