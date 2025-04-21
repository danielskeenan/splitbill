find_package(Python3 COMPONENTS Interpreter REQUIRED)

set(DOC_VENV_PATH "${CMAKE_CURRENT_BINARY_DIR}/.venv")
if (NOT IS_DIRECTORY "${DOC_VENV_PATH}")
    message(STATUS "Creating Python venv for documentation in ${DOC_VENV_PATH}...")
    execute_process(
            COMMAND "${Python3_EXECUTABLE}" -m venv "${DOC_VENV_PATH}"
            COMMAND_ERROR_IS_FATAL ANY
    )
endif ()

# See https://discourse.cmake.org/t/possible-to-create-a-python-virtual-env-from-cmake-and-then-find-it-with-findpython3/1132
set(ENV{VIRTUAL_ENV} "${DOC_VENV_PATH}")
set(Python3_FIND_VIRTUALENV ONLY)
unset(Python3_EXECUTABLE)
# Need to unset these env vars as otherwise GitHub Actions will force the system python, not the venv.
unset(ENV{Python_ROOT_DIR})
unset(ENV{Python3_ROOT_DIR})
find_package(Python3 COMPONENTS Interpreter REQUIRED)
execute_process(
        COMMAND "${Python3_EXECUTABLE}" -m pip install -U -r "${CMAKE_CURRENT_SOURCE_DIR}/requirements.txt"
        COMMAND_ERROR_IS_FATAL ANY
)

# Generate doc
find_program(SPHINX_BUILD sphinx-build
        PATHS "${DOC_VENV_PATH}"
        PATH_SUFFIXES bin Scripts
        NO_DEFAULT_PATH
        REQUIRED
)

set(DOC_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}")
set(DOC_OUTPUT_DIR "${CMAKE_CURRENT_LIST_DIR}/_build" CACHE PATH "Documentation output directory")
set(DOC_PACKAGES_DIR "${CMAKE_BINARY_DIR}/packages" CACHE PATH "Path to find packages for display")

configure_file(conf.py.in "${CMAKE_CURRENT_BINARY_DIR}/conf.py")

# Generate paths and checksums
function(calc_file_hash HASH_VAR FILE_SUFFIX)
    set(FILE_PATH "${DOC_PACKAGES_DIR}/${CMAKE_PROJECT_NAME}-${CMAKE_PROJECT_VERSION}-${FILE_SUFFIX}")
    if (EXISTS ${FILE_PATH})
        file(MD5 "${FILE_PATH}" FILE_HASH)
    else ()
        set(FILE_HASH "No hash available")
    endif ()
    set(${HASH_VAR} ${FILE_HASH} PARENT_SCOPE)
endfunction()
calc_file_hash(HASH_DARWIN_DMG Darwin.dmg)
calc_file_hash(HASH_DARWIN_ZIP Darwin.zip)
calc_file_hash(HASH_LINUX_DEB Linux.deb)
calc_file_hash(HASH_LINUX_TBZ Linux.tar.bz2)
calc_file_hash(HASH_LINUX_TGZ Linux.tar.gz)
calc_file_hash(HASH_WIN64_INSTALLER win64.exe)
calc_file_hash(HASH_WIN64_ZIP win64.zip)
configure_file(download.rst.in "${CMAKE_CURRENT_SOURCE_DIR}/download.rst")

add_custom_target(doc
        COMMAND "${SPHINX_BUILD}" -b html -j auto -c "${CMAKE_CURRENT_BINARY_DIR}" "${DOC_SOURCE_DIR}" "${DOC_OUTPUT_DIR}"
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        BYPRODUCTS "${DOC_OUTPUT_DIR}/index.html"
        COMMENT "Generating documentation (html)"
        VERBATIM
)
set_property(TARGET doc APPEND PROPERTY ADDITIONAL_CLEAN_FILES "${DOC_OUTPUT_DIR}")