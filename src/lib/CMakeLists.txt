add_library(splitbill_lib STATIC
    Bill.cpp
    )
target_include_directories(splitbill_lib PRIVATE ${PROJECT_SOURCE_DIR}/include/lib)

find_package(Boost REQUIRED COMPONENTS date_time)
target_link_libraries(splitbill_lib PUBLIC Boost::date_time)
