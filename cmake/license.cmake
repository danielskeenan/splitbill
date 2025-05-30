# Build the license files as HTML, RTF, and plain text
find_program(PANDOC_PATH NAMES pandoc)
if (NOT PANDOC_PATH)
    message(FATAL_ERROR "Could not find pandoc, required for building installers")
endif ()

list(APPEND CMAKE_CONFIGURE_DEPENDS ${PROJECT_SOURCE_DIR}/LICENSE.md)
message(STATUS "Creating license files")
execute_process(COMMAND ${PANDOC_PATH}
    "--standalone" "--embed-resources" "--metadata" "pagetitle='GNU GENERAL PUBLIC LICENSE, Version 3'" "--from" "commonmark" "--to" "html" "${PROJECT_SOURCE_DIR}/LICENSE.md"
    OUTPUT_FILE "${PROJECT_BINARY_DIR}/LICENSE.html"
    ENCODING "UTF8"
    )
execute_process(COMMAND ${PANDOC_PATH}
    "--standalone" "--embed-resources" "--from" "commonmark" "--to" "rtf" "${PROJECT_SOURCE_DIR}/LICENSE.md"
    OUTPUT_FILE "${PROJECT_BINARY_DIR}/LICENSE.rtf"
    ENCODING "UTF8"
    )
execute_process(COMMAND ${PANDOC_PATH}
    "--standalone" "--embed-resources" "--from" "commonmark" "--to" "plain" "${PROJECT_SOURCE_DIR}/LICENSE.md"
    OUTPUT_FILE "${PROJECT_BINARY_DIR}/LICENSE.txt"
    ENCODING "UTF8"
    )
