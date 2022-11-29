cmake_minimum_required(VERSION 3.18 FATAL_ERROR)
set(CMAKE_POLICY_DEFAULT_CMP0135 NEW) # Disable CMP0135 warning
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/.cmake")
include(EnablePGO)

# set output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# not ideal to use this global variable, but necessary to make sure that tooling
# and projects use the same version
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 17)

# strongly encouraged to enable this globally to avoid conflicts between
# -Wpedantic being enabled and -std=c++20 and -std=gnu++20 for example when
# compiling with PCH enabled
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# note: by default ENABLE_DEVELOPER_MODE is True This means that all analysis
# (sanitizers, static analysis) is enabled and all warnings are treated as
# errors if you want to switch this behavior, change TRUE to FALSE
set(ENABLE_DEVELOPER_MODE
    TRUE
    CACHE BOOL "Enable 'developer mode'")
set(WARNINGS_AS_ERRORS_DEVELOPER_DEFAULT FALSE)

# project_options
include(FetchContent)
FetchContent_Declare(
  project_options
  GIT_REPOSITORY https://github.com/aminya/project_options.git
  GIT_TAG v0.24.1)
FetchContent_MakeAvailable(project_options)
include(${project_options_SOURCE_DIR}/Index.cmake)
include(${project_options_SOURCE_DIR}/src/DynamicProjectOptions.cmake)

project(
  timebox
  VERSION 0.0.15
  LANGUAGES C CXX
  DESCRIPTION
    "Synchronize system clock with hardware GPS interface attached by USB"
  HOMEPAGE_URL "https://gitlab.sudobash.pl/Au5m8btpUaUS5t3dpixE/time-box")
set(PROJECT_VERSION_ADDENDUM
    "InDev"
    CACHE STRING "Addendum to project version (i.e. RC)" FORCE)
string(TIMESTAMP BUILD_TIME "%Y-%m-%d %H:%M:%S UTC" UTC)

# detect target platform
if(NOT TARGET_PLATFORM)
  if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(TARGET_PLATFORM "WIN32")
    add_compile_definitions(WIN32_LEAN_AND_MEAN NOMINMAX)
  elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(TARGET_PLATFORM "LINUX")
  else()
    message(FATAL_ERROR "Unsupported platform: ${CMAKE_SYSTEM_NAME}")
  endif()
endif()

# grab git information
include(GitVars)

dynamic_project_options(
  ENABLE_INTERPROCEDURAL_OPTIMIZATION ENABLE_BUILD_WITH_TIME_TRACE
  ENABLE_NATIVE_OPTIMIZATION ENABLE_CACHE ENABLE_DOXYGEN)
target_compile_features(project_options INTERFACE cxx_std_${CMAKE_CXX_STANDARD})

# boost vars
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
if(TARGET_PLATFORM MATCHES "LINUX")
  set(BOOST_ALL_DYN_LINK ON)
  set(Boost_USE_STATIC_LIBS OFF)
elseif(TARGET_PLATFORM MATCHES "WIN32")
  set(BOOST_ALL_DYN_LINK OFF)
  set(Boost_USE_STATIC_LIBS ON)
endif()

# required packages
find_package(Threads REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(Boost REQUIRED COMPONENTS headers log log_setup date_time)
include(ExternalContent)

# include application
add_subdirectory(app)
