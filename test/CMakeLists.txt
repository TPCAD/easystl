add_executable(pair pair_test.cpp)
target_include_directories(pair PRIVATE ../include)

# target_link_libraries(pair PRIVATE GTest::GTest)

add_test(NAME pair COMMAND $<TARGET_FILE:pair>)
