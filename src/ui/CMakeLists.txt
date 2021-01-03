# Qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
#set(CMAKE_AUTOUIC ON)
find_package(Qt5 COMPONENTS Widgets REQUIRED)
set(autouic_options
    -tr _
    -include trans.h
    )

# Boost Locale
find_package(Boost REQUIRED COMPONENTS locale)

# Config
configure_file(config.h.in config.h)

add_executable(splitbill WIN32 MACOSX_BUNDLE
    config.h
    trans.h
    main.cpp
    Settings.h
    BillLineDelegate.h
    BillLineDelegate.cpp
    BillLineModel.h
    BillLineModel.cpp
    MainWindow.h
    MainWindow.cpp
    PersonListDelegate.h
    PersonListDelegate.cpp
    PersonListModel.h
    PersonListModel.cpp
    SettingsDialog.h
    SettingsDialog.cpp
    SplitViewModel.h
    SplitViewModel.cpp
    )
set_target_properties(splitbill PROPERTIES
    MACOSX_BUNDLE_BUNDLE_NAME splitbill
    MACOSX_BUNDLE_BUNDLE_VERSION ${CMAKE_PROJECT_VERSION}
    MACOSX_BUNDLE_GUI_IDENTIFIER "${PROJECT_AUTHOR_DOMAIN}.${CMAKE_PROJECT_NAME}"
    MACOSX_BUNDLE_ICON_FILE splitbill.icns
    MACOSX_BUNDLE_LONG_VERSION_STRING ${CMAKE_PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${CMAKE_PROJECT_VERSION})

target_link_libraries(splitbill splitbill_lib)
target_link_libraries(splitbill Qt5::Widgets)
target_link_libraries(splitbill Boost::locale)
target_include_directories(splitbill PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

# Qt packaging
if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_link_options(splitbill PRIVATE "/NODEFAULTLIB:library")
    # Bundle Qt runtime
    if (BUILD_PACKAGE)
        include("${PROJECT_SOURCE_DIR}/cmake/Windeployqt.cmake")
        windeployqt(splitbill bin)
    endif ()
elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    file(COPY "${PROJECT_SOURCE_DIR}/resources/app-icon.icns" DESTINATION "${PROJECT_BINARY_DIR}/meta")
    file(RENAME "${PROJECT_BINARY_DIR}/meta/app-icon.icns" "${PROJECT_BINARY_DIR}/meta/splitbill.icns")
    target_sources(splitbill PRIVATE "${PROJECT_BINARY_DIR}/meta/splitbill.icns")
    set_source_files_properties("${PROJECT_BINARY_DIR}/meta/splitbill.icns" PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
    if (BUILD_PACKAGE)
        find_program(MACDEPLOYQT_PROG macdeployqt)
        add_custom_command(TARGET splitbill POST_BUILD
            COMMENT "Running macdeployqt"
            COMMAND ${MACDEPLOYQT_PROG} ARGS $<TARGET_BUNDLE_DIR:splitbill> -verbose=1)
    endif ()
endif ()
