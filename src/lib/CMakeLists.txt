add_library(splitbill_lib STATIC
    Bill.h Bill.cpp)

find_package(Boost COMPONENTS Date_Time REQUIRED)
target_link_libraries(splitbill_lib PRIVATE Boost::date_time)
