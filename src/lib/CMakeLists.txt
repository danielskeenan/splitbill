add_library(splitbill_lib STATIC
    Bill.h Bill.cpp)

find_package(Boost REQUIRED COMPONENTS date_time)
target_link_libraries(splitbill_lib PUBLIC Boost::date_time)
