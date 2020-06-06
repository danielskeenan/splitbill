include(GoogleTest)

add_executable(splitbill_lib_test
    BillTest.cpp)
target_link_libraries(splitbill_lib_test gtest gtest_main splitbill_lib)

gtest_discover_tests(splitbill_lib_test)
