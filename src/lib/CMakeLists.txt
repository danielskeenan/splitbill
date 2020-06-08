add_library(splitbill_lib STATIC
    Bill.h Bill.cpp)

#find_package(Boost REQUIRED)
target_include_directories(splitbill_lib PUBLIC "${CONAN_INCLUDE_DIRS_BOOST}")
target_link_libraries(splitbill_lib PRIVATE CONAN_LIB::boost_libboost_date_time)
