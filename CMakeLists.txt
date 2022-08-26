cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

# Not ideal to use this global variable, but necessary to make sure that tooling
# and projects use the same version
set(CMAKE_CXX_STANDARD 20)

# strongly encouraged to enable this globally to avoid conflicts between
# -Wpedantic being enabled and -std=c++20 and -std=gnu++20 for example when
# compiling with PCH enabled
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Note: by default ENABLE_DEVELOPER_MODE is True This means that all analysis
# (sanitizers, static analysis) is enabled and all warnings are treated as
# errors if you want to switch this behavior, change TRUE to FALSE
set(ENABLE_DEVELOPER_MODE
    TRUE
    CACHE BOOL "Enable 'developer mode'")

# Change this to false if you want to disable warnings_as_errors in developer
# mode
set(OPT_WARNINGS_AS_ERRORS_DEVELOPER_DEFAULT FALSE)

# uncomment to enable vcpkg: # Setup vcpkg - should be called before defining
# project() run_vcpkg()

project(
  timebox
  VERSION 0.1.0
  LANGUAGES C CXX
  DESCRIPTION
    "Synchronize system clock with hardware GPS interface attached by USB"
  HOMEPAGE_URL "https://gitlab.sudobash.pl/Au5m8btpUaUS5t3dpixE/time-box")

# Output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Add project_options
# https://github.com/aminya/project_options
include(FetchContent)
FetchContent_Declare(
  _project_options
  URL https://github.com/aminya/project_options/archive/refs/tags/v0.24.1.zip)
FetchContent_MakeAvailable(_project_options)
include(${_project_options_SOURCE_DIR}/Index.cmake)

# # This variable is set by project() in CMake 3.21+
string(COMPARE EQUAL "${CMAKE_SOURCE_DIR}" "${PROJECT_SOURCE_DIR}"
               PROJECT_IS_TOP_LEVEL)

if(PROJECT_IS_TOP_LEVEL)
  # Consider the CTest module, which creates targets and options! Only needed if
  # you want to enable submissions to a CDash server.
  include(CTest)
endif()

# Add Git SHA
find_package(Git)
find_program(GIT_FOUND git)
if(GIT_FOUND)
  message(
    STATUS "Found Git (${GIT_FOUND}), executable will get SHA and branch used")
  execute_process(
    COMMAND git -C ${CMAKE_SOURCE_DIR} rev-parse HEAD
    OUTPUT_VARIABLE GIT_SHA
    OUTPUT_STRIP_TRAILING_WHITESPACE)
  execute_process(
    COMMAND git -C ${CMAKE_SOURCE_DIR} branch --show-current
    OUTPUT_VARIABLE GIT_BRANCH
    OUTPUT_STRIP_TRAILING_WHITESPACE)
  string(SUBSTRING "${GIT_SHA}" 0 8 GIT_SHORT_SHA)
  set(GIT_BRANCH ${GIT_BRANCH})
  set(GIT_SHORT_SHA ${GIT_SHORT_SHA})
  message(STATUS "GIT SHA = ${GIT_SHA}")
  message(STATUS "GIT SHORT SHA = ${GIT_SHORT_SHA}")
  message(STATUS "GIT BRANCH = ${GIT_BRANCH}")
else()
  message(WARNING "Git not found !")
endif()

get_property(BUILDING_MULTI_CONFIG GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if(BUILDING_MULTI_CONFIG)
  if(NOT CMAKE_BUILD_TYPE)
    # Make sure that all supported configuration types have their associated
    # conan packages available. You can reduce this list to only the
    # configuration types you use, but only if one is not forced-set on the
    # command line for VS
    message(TRACE "Setting up multi-config build types")
    set(CMAKE_CONFIGURATION_TYPES
        Debug Release RelWithDebInfo MinSizeRel
        CACHE STRING "Enabled build types" FORCE)
  else()
    message(TRACE "User chose a specific build type, so we are using that")
    set(CMAKE_CONFIGURATION_TYPES
        ${CMAKE_BUILD_TYPE}
        CACHE STRING "Enabled build types" FORCE)
  endif()
endif()

include(${_project_options_SOURCE_DIR}/src/DynamicProjectOptions.cmake)
dynamic_project_options(
  ENABLE_INTERPROCEDURAL_OPTIMIZATION
  ENABLE_NATIVE_OPTIMIZATION
  ENABLE_DOXYGEN
  ENABLE_SANITIZER_ADDRESS
  ENABLE_CPPCHECK
  ENABLE_PCH
  PCH_HEADERS
  <vector>
  <string>
  CPPCHECK_OPTIONS
  --enable=style,performance,warning,portability
  --inline-suppr
  --suppress=cppcheckError
  --suppress=internalAstError
  --suppress=unmatchedSuppression
  --suppress=passedByValue
  --suppress=syntaxError
  --inconclusive)

target_compile_features(project_options INTERFACE cxx_std_${CMAKE_CXX_STANDARD})

# add file to find libserial with cmake
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/submodules")

# required packages
find_package(Threads REQUIRED)
find_package(Boost REQUIRED COMPONENTS log log_setup date_time)
find_package(GTest REQUIRED)
find_package(benchmark REQUIRED)
find_package(LibSerial REQUIRED)
find_package(PkgConfig REQUIRED)

# Boost vars
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
set(BOOST_ALL_DYN_LINK ON) # force dynamic linking for all libraries

add_subdirectory(app)
add_subdirectory(submodules)

# If MSVC is being used, and ASAN is enabled, we need to set the debugger environment
# so that it behaves well with MSVC's debugger, and we can run the target from visual studio
if(MSVC)
  get_all_installable_targets(all_targets)
  message("all_targets=${all_targets}")
  set_target_properties(${all_targets} PROPERTIES VS_DEBUGGER_ENVIRONMENT "PATH=$(VC_ExecutablePath_x64);%PATH%")
endif()

# set the startup project for the "play" button in MSVC
set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT intro)

if(CMAKE_SKIP_INSTALL_RULES)
  return()
elseif(NOT PROJECT_IS_TOP_LEVEL)
  return()
endif()

# Add other targets that you want installed here, be default we just package the
# one executable we know we want to ship
package_project(TARGETS timebox project_options project_warnings)

set(CPACK_PACKAGE_FILE_NAME
    "${CMAKE_PROJECT_NAME}-${CMAKE_PROJECT_VERSION}-${GIT_SHORT_SHA}-${CMAKE_SYSTEM_NAME}-${CMAKE_BUILD_TYPE}-${CMAKE_CXX_COMPILER_ID}-${CMAKE_CXX_COMPILER_VERSION}"
)

include(CPack)
