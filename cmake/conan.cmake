include("${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
conan_basic_setup(TARGETS)

include_directories("${CONAN_INCLUDE_DIRS_BOOST}")
