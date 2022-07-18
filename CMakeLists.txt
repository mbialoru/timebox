cmake_minimum_required(VERSION 3.18 FATAL_ERROR)

project(timebox LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "-march=native")
set(CMAKE_CXX_FLAGS_DEBUG "-ggdb3 -Wall -Werror -Wno-unused-variable -DDEBUG -fsanitize=address")

set(PROJECT_HEADERS_DIR ${PROJECT_SOURCE_DIR}/src/lib)
set(PROJECT_CLASSES_DIR ${PROJECT_SOURCE_DIR}/src/lib/classes)

include(CheckIPOSupported)
check_ipo_supported(RESULT ipo_supported OUTPUT error)

# required packages
find_package(Threads REQUIRED)
find_package(Boost REQUIRED COMPONENTS log program_options log_setup)
find_package(GTest REQUIRED)
find_package(benchmark REQUIRED)
find_package(PkgConfig REQUIRED)

include_directories(${PROJECT_HEADERS_DIR})
include_directories(${PROJECT_CLASSES_DIR})

add_subdirectory(submodules)
add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(benchmark)
add_subdirectory(proto)

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