# Raspberry Pi 5 cross-compilation toolchain for CMake.
#
# Usage example:
#   cmake -S . -B build-rpi -DCMAKE_TOOLCHAIN_FILE=toolchain-rpi.cmake \
#     -DRPI_TOOLCHAIN_PREFIX=aarch64-linux-gnu \
#     -DRPI_SYSROOT=/path/to/raspberry-pi-sysroot

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_VERSION 1)
set(CMAKE_SYSTEM_PROCESSOR aarch64)

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

if(NOT DEFINED RPI_TOOLCHAIN_PREFIX)
    if(CMAKE_HOST_WIN32)
        set(RPI_TOOLCHAIN_PREFIX aarch64-none-linux-gnu)
    else()
        set(RPI_TOOLCHAIN_PREFIX aarch64-linux-gnu)
    endif()
endif()

if(NOT DEFINED RPI_TOOLCHAIN_BIN_DIR OR RPI_TOOLCHAIN_BIN_DIR STREQUAL "")
    set(RPI_TOOLCHAIN_BIN_DIR "")
endif()

if(NOT DEFINED RPI_SYSROOT OR RPI_SYSROOT STREQUAL "")
    if(CMAKE_HOST_WIN32 AND EXISTS "C:/sysroot_aarch64")
        set(RPI_SYSROOT "C:/sysroot_aarch64")
    endif()
endif()

set(RPI_TOOLCHAIN_SEARCH_PATHS /usr/bin /usr/local/bin)
if(CMAKE_HOST_WIN32)
    list(APPEND RPI_TOOLCHAIN_SEARCH_PATHS
        C:/msys64/usr/bin
        C:/msys64/mingw64/bin
        C:/msys64/ucrt64/bin
        C:/msys64/clang64/bin
        C:/arm-gnu-toolchain/bin
        C:/ProgramData/chocolatey/bin)
endif()

# Help find_package(Boost) when cross-compiling with an RPi sysroot (Windows, Linux, or WSL host).
if(DEFINED RPI_SYSROOT AND NOT RPI_SYSROOT STREQUAL "")
    set(_RPI_BOOST_ROOT "${RPI_SYSROOT}/usr")
elseif(CMAKE_HOST_WIN32)
    set(_RPI_BOOST_ROOT "C:/sysroot_aarch64/usr")
endif()

if(DEFINED _RPI_BOOST_ROOT AND EXISTS "${_RPI_BOOST_ROOT}/include/boost")
    set(Boost_ROOT "${_RPI_BOOST_ROOT}" CACHE PATH "Boost root in cross sysroot" FORCE)
    set(Boost_INCLUDE_DIR "${_RPI_BOOST_ROOT}/include" CACHE PATH "Boost include dir in cross sysroot" FORCE)
    set(BOOST_ROOT "${_RPI_BOOST_ROOT}" CACHE PATH "Boost root in cross sysroot" FORCE)
    set(BOOST_INCLUDEDIR "${_RPI_BOOST_ROOT}/include" CACHE PATH "Boost include dir in cross sysroot" FORCE)
    if(EXISTS "${_RPI_BOOST_ROOT}/lib/aarch64-linux-gnu")
        set(BOOST_LIBRARYDIR "${_RPI_BOOST_ROOT}/lib/aarch64-linux-gnu" CACHE PATH "Boost library dir in cross sysroot" FORCE)
    elseif(EXISTS "${_RPI_BOOST_ROOT}/lib")
        set(BOOST_LIBRARYDIR "${_RPI_BOOST_ROOT}/lib" CACHE PATH "Boost library dir in cross sysroot" FORCE)
    endif()
    if(DEFINED BOOST_LIBRARYDIR AND NOT BOOST_LIBRARYDIR STREQUAL "")
        set(Boost_LIBRARY_DIR "${BOOST_LIBRARYDIR}" CACHE PATH "Boost library dir in cross sysroot" FORCE)
    endif()
    set(Boost_DIR "" CACHE PATH "Disable host BoostConfig package path for cross builds" FORCE)
    set(Boost_NO_SYSTEM_PATHS ON CACHE BOOL "Disable system path lookup for Boost" FORCE)
    set(Boost_NO_BOOST_CMAKE ON CACHE BOOL "Disable BoostConfig.cmake and use FindBoost" FORCE)
    list(APPEND CMAKE_PREFIX_PATH "${_RPI_BOOST_ROOT}")
    list(APPEND CMAKE_INCLUDE_PATH "${_RPI_BOOST_ROOT}/include")
    if(DEFINED BOOST_LIBRARYDIR AND NOT BOOST_LIBRARYDIR STREQUAL "")
        list(APPEND CMAKE_LIBRARY_PATH "${BOOST_LIBRARYDIR}")
    endif()
endif()

find_program(RPI_C_COMPILER
    NAMES ${RPI_TOOLCHAIN_PREFIX}-gcc
    HINTS ${RPI_TOOLCHAIN_BIN_DIR}
    PATHS ${RPI_TOOLCHAIN_SEARCH_PATHS}
    NO_DEFAULT_PATH)

find_program(RPI_CXX_COMPILER
    NAMES ${RPI_TOOLCHAIN_PREFIX}-g++
    HINTS ${RPI_TOOLCHAIN_BIN_DIR}
    PATHS ${RPI_TOOLCHAIN_SEARCH_PATHS}
    NO_DEFAULT_PATH)

find_program(RPI_ASM_COMPILER
    NAMES ${RPI_TOOLCHAIN_PREFIX}-gcc
    HINTS ${RPI_TOOLCHAIN_BIN_DIR}
    PATHS ${RPI_TOOLCHAIN_SEARCH_PATHS}
    NO_DEFAULT_PATH)

if(NOT RPI_C_COMPILER)
    message(FATAL_ERROR "Could not find ${RPI_TOOLCHAIN_PREFIX}-gcc. Install the aarch64 cross toolchain or set RPI_TOOLCHAIN_BIN_DIR.")
endif()

if(NOT RPI_CXX_COMPILER)
    message(FATAL_ERROR "Could not find ${RPI_TOOLCHAIN_PREFIX}-g++. Install the aarch64 g++ cross package or set RPI_TOOLCHAIN_BIN_DIR.")
endif()

set(CMAKE_C_COMPILER ${RPI_C_COMPILER})
set(CMAKE_CXX_COMPILER ${RPI_CXX_COMPILER})
set(CMAKE_ASM_COMPILER ${RPI_ASM_COMPILER})

if(DEFINED RPI_SYSROOT AND NOT RPI_SYSROOT STREQUAL "")
    set(CMAKE_SYSROOT ${RPI_SYSROOT})
    set(CMAKE_FIND_ROOT_PATH ${RPI_SYSROOT})
    set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
    set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
    set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
else()
    # No sysroot was provided: allow host and prefixed paths (e.g. /usr/include, /usr/aarch64-linux-gnu).
    set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY BOTH)
    set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE BOTH)
    set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE BOTH)
endif()

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)