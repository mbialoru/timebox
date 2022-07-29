cmake_minimum_required(VERSION 3.18 FATAL_ERROR)

project(timebox VERSION 0.0.1 LANGUAGES C CXX
    DESCRIPTION ""
    HOMEPAGE_URL ""
    )

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Strongly encouraged to enable this globally to avoid conflicts between
# -Wpedantic being enabled and -std=c++20 and -std=gnu++20 for example
# when compiling with PCH enabled
set(CMAKE_CXX_EXTENSIONS OFF)

# Flags
set(CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS}-march=native")
set(CMAKE_CXX_FLAGS_DEBUG
    "${CMAKE_CXX_FLAGS_DEBUG} -Wall -ggdb3 -DDEBUG -fno-omit-frame-pointer -fsanitize=address")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
set(CMAKE_LINKER_FLAGS_DEBUG
    "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")

# Add project_options v0.20.0
# https://github.com/cpp-best-practices/project_options
include(FetchContent)
FetchContent_Declare(_project_options URL
    https://github.com/aminya/project_options/archive/refs/tags/v0.24.0.zip)
FetchContent_MakeAvailable(_project_options)
include(${_project_options_SOURCE_DIR}/Index.cmake)

# This variable is set by project() in CMake 3.21+
string(COMPARE EQUAL "${CMAKE_SOURCE_DIR}" "${PROJECT_SOURCE_DIR}"
    PROJECT_IS_TOP_LEVEL)
if(PROJECT_IS_TOP_LEVEL)
  # Consider the CTest module, which creates targets and options!
  # Only needed if you want to enable submissions to a CDash server.
  include(CTest)
endif()

set(GIT_SHA
    "Unknown"
    CACHE STRING "SHA this build was generated from")
string(SUBSTRING "${GIT_SHA}" 0 8 GIT_SHORT_SHA)

include(${_project_options_SOURCE_DIR}/src/DynamicProjectOptions.cmake)
# Initialize project_options variable related to this project
# This overwrites `project_options` and sets `project_warnings`
# uncomment the options to enable them:
dynamic_project_options(
    # This is a list of headers to pre-compile, here are some common ones
    <vector>
    <string>
    # ENABLE_CONAN     # Enable when ready to package
    # CONAN_OPTIONS    # Extra options to pass to conan
    # MSVC_WARNINGS    # Override the defaults for the MSVC warnings
    # CLANG_WARNINGS   # Override the defaults for the CLANG warnings
    # GCC_WARNINGS     # Override the defaults for the GCC warnings
    CPPCHECK_OPTIONS
    --enable=style,performance,warning,portability
    --inline-suppr
    # We cannot act on a bug/missing feature of cppcheck
    --suppress=cppcheckError
    --suppress=internalAstError
    # if a file does not have an internalAstError, we get an unmatchedSuppression error
    --suppress=unmatchedSuppression
    --suppress=passedByValue
    --suppress=syntaxError
    --inconclusive
)

target_compile_features(project_options INTERFACE cxx_std_${CMAKE_CXX_STANDARD})
add_library(myproject::project_options INTERFACE IMPORTED)
add_library(myproject::project_warnings INTERFACE IMPORTED)

include(CheckIPOSupported)
check_ipo_supported(RESULT ipo_supported OUTPUT error)

# required packages
find_package(Threads REQUIRED)
find_package(Boost REQUIRED COMPONENTS log program_options log_setup date_time)
find_package(GTest REQUIRED)
find_package(benchmark REQUIRED)
find_package(PkgConfig REQUIRED)

# Boost vars
set( Boost_USE_STATIC_LIBS        OFF )
set( Boost_USE_MULTITHREADED      ON )
set( Boost_USE_STATIC_RUNTIME     OFF )
set( BOOST_ALL_DYN_LINK           ON ) # force dynamic linking for all libraries

add_subdirectory(app)
add_subdirectory(proto)
add_subdirectory(submodules)

if (CMAKE_BUILD_TYPE MATCHES "^[Rr]elease")
option(BUILD_DOC "Build documentation" ON)
find_package(Doxygen)
if (DOXYGEN_FOUND)
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
    message("Doxygen build started")
    add_custom_target( doc_doxygen ALL
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM )
else (DOXYGEN_FOUND)
message("Doxygen need to be installed to generate the doxygen documentation")
endif (DOXYGEN_FOUND)
endif()

set(CPACK_PACKAGE_FILE_NAME
    "${CMAKE_PROJECT_NAME}-${CMAKE_PROJECT_VERSION}-${GIT_SHORT_SHA}-${CMAKE_SYSTEM_NAME}-${CMAKE_BUILD_TYPE}-${CMAKE_CXX_COMPILER_ID}-${CMAKE_CXX_COMPILER_VERSION}"
)

include(CPack)