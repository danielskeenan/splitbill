add_library(splitbill_lib STATIC
    Bill.h Bill.cpp)

find_package(Boost COMPONENTS date_time REQUIRED)
find_path(DATE_TIME_PATH "boost/date_time/gregorian/gregorian.hpp")
target_include_directories(splitbill_lib PUBLIC ${DATE_TIME_PATH})
target_link_libraries(splitbill_lib PRIVATE Boost::date_time)
