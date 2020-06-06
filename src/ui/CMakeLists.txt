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
find_package(Boost COMPONENTS locale REQUIRED)

# Config
configure_file(config.h.in config.h)

add_executable(splitbill
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
target_link_libraries(splitbill Boost::locale)
target_include_directories(splitbill PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
