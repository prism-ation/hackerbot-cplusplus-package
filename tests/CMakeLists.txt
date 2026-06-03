add_executable(${PROJECT_NAME}Tests
    unit/test_hackerbot.cpp
    unit/test_protocol.cpp
    unit/test_core.cpp
    unit/test_transport.cpp)

target_link_libraries(${PROJECT_NAME}Tests
    PRIVATE
        ${PROJECT_NAME}_lib
        GTest::gtest_main
)

target_compile_features(${PROJECT_NAME}Tests PRIVATE cxx_std_17)

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME}Tests)