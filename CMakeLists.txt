cmake_minimum_required(VERSION 3.20)

project(Hackerbot
    VERSION 0.1.0
    DESCRIPTION "Hackerbot C++ package"
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(GTest REQUIRED)
find_package(Threads REQUIRED)

add_library(${PROJECT_NAME}_lib
    src/hackerbot/Hackerbot.cpp
    src/protocol/CommandCodec.cpp
    src/protocol/ResponseParser.cpp
    src/transport/SerialTransport.cpp)

target_include_directories(${PROJECT_NAME}_lib
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_BINARY_DIR}/generated)

target_compile_features(${PROJECT_NAME}_lib PUBLIC cxx_std_17)
target_link_libraries(${PROJECT_NAME}_lib
    PUBLIC
        Threads::Threads)

add_executable(${PROJECT_NAME}
    src/main.cpp)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        ${PROJECT_NAME}_lib)

enable_testing()

add_subdirectory(tests)

find_package(Doxygen)
if(DOXYGEN_FOUND)
    set(DOXYGEN_INPUT_FILE ${PROJECT_SOURCE_DIR}/Doxyfile.in)
    set(DOXYGEN_OUTPUT_FILE ${PROJECT_BINARY_DIR}/generated/Doxyfile)

    configure_file(${DOXYGEN_INPUT_FILE} ${DOXYGEN_OUTPUT_FILE} @ONLY)

    add_custom_target(docs
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUTPUT_FILE}
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        COMMENT "Generate Doxygen API documentation"
        VERBATIM)
else()
    message(STATUS "Doxygen not found, 'docs' target will not be available")
endif()