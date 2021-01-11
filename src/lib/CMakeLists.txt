add_library(splitbill_lib STATIC
    Bill.cpp
    Money.cpp
    )
target_include_directories(splitbill_lib PRIVATE ${PROJECT_SOURCE_DIR}/include/lib)

find_package(Boost REQUIRED COMPONENTS date_time)
target_link_libraries(splitbill_lib PUBLIC Boost::date_time)

# Generate the currency header
set(_CURRENCY_HEADER_PATH "${PROJECT_SOURCE_DIR}/include/lib/Currency.h")
set(ISO_4217_XML_PATH "" CACHE FILEPATH "Path to ISO 4217 currency information. This can be downloaded from https://www.currency-iso.org/en/home/tables/table-a1.html. If this is empty, it will be downloaded at configure time.")
if (NOT EXISTS "${ISO_4217_XML_PATH}")
    if ("${ISO_4217_XML_PATH}" STREQUAL "")
        set(ISO_4217_XML_PATH "${PROJECT_BINARY_DIR}/resources/iso_4217.xml" CACHE FILEPATH "" FORCE)
    endif ()
    message(STATUS "Downloading ISO 4217 Currency info to ${ISO_4217_XML_PATH}")
    file(DOWNLOAD "https://www.currency-iso.org/dam/downloads/lists/list_one.xml" "${ISO_4217_XML_PATH}"
        SHOW_PROGRESS
        TLS_VERIFY On
        STATUS _ISO_4217_XML_STATUS
        )
    list(GET _ISO_4217_XML_STATUS 0 _ISO_4217_XML_STATUS_CODE)
    list(GET _ISO_4217_XML_STATUS 1 _ISO_4217_XML_STATUS_MESSAGE)
    if (NOT ${_ISO_4217_XML_STATUS_CODE} EQUAL 0)
        message(FATAL_ERROR "Could not download ISO 4217 Currency file: ${_ISO_4217_XML_STATUS_MESSAGE}. Try downloading it manually and set ISO_4217_XML_PATH.")
    endif ()

endif ()
set(GENERATE_CURRENCY_MAP_SCRIPT "${PROJECT_SOURCE_DIR}/scripts/generate_currency_map.py")
list(APPEND CMAKE_CONFIGURE_DEPENDS "${GENERATE_CURRENCY_MAP_SCRIPT}")
find_package(Python3 REQUIRED COMPONENTS Interpreter)
message(STATUS "Generating currency header")
execute_process(
    COMMAND "${Python3_EXECUTABLE}" "${GENERATE_CURRENCY_MAP_SCRIPT}" "--from-file=${ISO_4217_XML_PATH}" "--out=${_CURRENCY_HEADER_PATH}"
    RESULT_VARIABLE _GENERATE_CURRENCY_HEADER_ERROR
)
if (${_GENERATE_CURRENCY_HEADER_ERROR})
    message(FATAL_ERROR "The currency header could not be generated")
endif ()
