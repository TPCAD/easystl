# add_executable(pair pair_test.cpp)
# target_include_directories(pair PRIVATE ../include ../3rdlib/googletest/googletest/include)
#
# target_link_libraries(pair PRIVATE GTest::gtest_main)
# gtest_discover_tests(pair)


add_executable(vector vector_test.cpp)
target_include_directories(vector PRIVATE ../include ../3rdlib/googletest/googletest/include)
target_link_libraries(vector PRIVATE GTest::gtest_main)
gtest_discover_tests(vector)
