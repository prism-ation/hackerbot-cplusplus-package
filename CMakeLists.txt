cmake_minimum_required(VERSION 3.20)

project(Hackerbot
    VERSION 0.1.0
    DESCRIPTION "Hackerbot C++ package"
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(CTest)
find_package(Threads REQUIRED)

set(Boost_NO_BOOST_CMAKE ON)

# Cross builds must not pick host Boost from MSYS/Windows include paths.
if(CMAKE_CROSSCOMPILING AND DEFINED CMAKE_SYSROOT AND NOT CMAKE_SYSROOT STREQUAL "")
    set(Boost_NO_SYSTEM_PATHS ON)
endif()
find_package(Boost REQUIRED COMPONENTS)

add_library(${PROJECT_NAME}_lib
    src/hackerbot/Hackerbot.cpp
    src/protocol/CommandCodec.cpp
    src/protocol/ResponseParser.cpp
    src/transport/SerialTransport.cpp
    src/core/CoreService.cpp
    src/base/BaseDriver.cpp
    src/base/BaseFacade.cpp
    src/base/MapsService.cpp)

target_include_directories(${PROJECT_NAME}_lib
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_BINARY_DIR}/generated)

target_compile_features(${PROJECT_NAME}_lib PUBLIC cxx_std_17)
target_link_libraries(${PROJECT_NAME}_lib
    PUBLIC
        Threads::Threads)

if(WIN32)
    target_link_libraries(${PROJECT_NAME}_lib
        PUBLIC
            ws2_32)
endif()

add_executable(${PROJECT_NAME}
    src/main.cpp)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        ${PROJECT_NAME}_lib)

if(BUILD_TESTING)
    find_package(GTest REQUIRED)
    enable_testing()
    add_subdirectory(tests)
endif()

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