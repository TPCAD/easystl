# add_executable(pair pair_test.cpp)
# target_include_directories(pair PRIVATE ../include ../3rdlib/googletest/googletest/include)
#
# target_link_libraries(pair PRIVATE GTest::gtest_main)
# gtest_discover_tests(pair)


# add_executable(vector vector_test.cpp)
# target_include_directories(vector PRIVATE ../include ../3rdlib/googletest/googletest/include)
# target_link_libraries(vector PRIVATE GTest::gtest_main)
# gtest_discover_tests(vector)

add_executable(basic_string basic_string_test.cpp)
target_include_directories(basic_string PRIVATE ../include ../3rdlib/googletest/googletest/include)
target_link_libraries(basic_string PRIVATE GTest::gtest_main)
gtest_discover_tests(basic_string)
