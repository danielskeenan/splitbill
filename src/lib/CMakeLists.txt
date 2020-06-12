add_library(splitbill_lib STATIC
    Bill.h Bill.cpp)

find_library(BOOST_DATE_TIME NAMES boost_date_time boost_date_time-mt libboost_date_time libboost_date_time-mt)
target_include_directories(splitbill_lib PUBLIC "${CONAN_INCLUDE_DIRS_BOOST}")
target_link_libraries(splitbill_lib PRIVATE ${BOOST_DATE_TIME})
