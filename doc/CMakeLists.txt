find_program(SPHINXBUILD_PROG sphinx-build)
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
    COMMAND ${SPHINXBUILD_PROG} -b html -j auto -c "${CMAKE_CURRENT_BINARY_DIR}" "${DOC_SOURCE_DIR}" "${DOC_OUTPUT_DIR}"
    BYPRODUCTS "${DOC_OUTPUT_DIR}")
