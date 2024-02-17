# Qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
find_path(QSETTINGSCONTAINER_INCLUDE_DIRS "QSettingsContainer.h")

qt_add_executable(splitbill WIN32 MACOSX_BUNDLE
    main.cpp
    AboutDialog.h
    AboutDialog.cpp
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

target_link_libraries(splitbill PUBLIC splitbill_lib Qt::Widgets)
target_include_directories(splitbill PRIVATE ${QSETTINGSCONTAINER_INCLUDE_DIRS})
target_compile_definitions(splitbill PUBLIC -DQT_NO_KEYWORDS)

install(TARGETS splitbill
        BUNDLE DESTINATION .
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
if (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    file(COPY "${PROJECT_SOURCE_DIR}/resources/app-icon.icns" DESTINATION "${PROJECT_BINARY_DIR}/meta")
    file(RENAME "${PROJECT_BINARY_DIR}/meta/app-icon.icns" "${PROJECT_BINARY_DIR}/meta/splitbill.icns")
    target_sources(splitbill PRIVATE "${PROJECT_BINARY_DIR}/meta/splitbill.icns")
    set_source_files_properties("${PROJECT_BINARY_DIR}/meta/splitbill.icns" PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
endif ()
if (NOT CMAKE_SYSTEM_NAME STREQUAL "Linux")
    qt_generate_deploy_app_script(
            TARGET ${PROJECT_NAME}
            OUTPUT_SCRIPT DEPLOY_SCRIPT
            NO_UNSUPPORTED_PLATFORM_ERROR
    )
    install(SCRIPT ${DEPLOY_SCRIPT})
endif ()
