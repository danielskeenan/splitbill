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
find_library(BOOST_LOCALE NAMES boost_locale boost_locale-mt libboost_locale libboost_locale-mt)

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
target_link_libraries(splitbill ${BOOST_LOCALE})
target_include_directories(splitbill PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

# Qt packaging
if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
#    target_compile_options(splitbill PRIVATE "/MDd")
    target_link_options(splitbill PRIVATE "/NODEFAULTLIB:library")
    # Bundle Qt runtime
    include("${CMAKE_CURRENT_LIST_DIR}/../../cmake/Windeployqt.cmake")
    windeployqt(splitbill bin)
endif ()
