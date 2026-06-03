add_executable(hackerbot_tests
    unit/test_hackerbot.cpp)

target_link_libraries(hackerbot_tests
    PRIVATE
        ${PROJECT_NAME}_lib
        GTest::gtest_main)

target_compile_features(hackerbot_tests PRIVATE cxx_std_17)

add_test(NAME hackerbot_tests COMMAND hackerbot_tests)