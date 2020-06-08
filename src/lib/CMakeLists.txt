add_library(splitbill_lib STATIC
    Bill.h Bill.cpp)

find_package(Boost COMPONENTS date_time REQUIRED)
target_include_directories(splitbill_lib PUBLIC ${Boost_INCLUDE_DIRS})
target_link_libraries(splitbill_lib PRIVATE Boost::date_time)
