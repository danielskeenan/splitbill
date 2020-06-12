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
find_library(BOOST_LOCALE NAMES boost_locale libboost_locale)

# Config
configure_file(config.h.in config.h)

add_executable(splitbill WIN32
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
