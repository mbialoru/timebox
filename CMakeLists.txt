cmake_minimum_required(VERSION 3.18 FATAL_ERROR)
set(CMAKE_POLICY_DEFAULT_CMP0135 NEW) # Disable CMP0135 warning

# Not ideal to use this global variable, but necessary to make sure that tooling
# and projects use the same version
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 17)

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

# Change this to FALSE if you want to disable warnings_as_errors in developer
# mode
set(WARNINGS_AS_ERRORS_DEVELOPER_DEFAULT FALSE)

# Add project_options
# https://github.com/aminya/project_options
include(FetchContent)
FetchContent_Declare(
  project_options
  GIT_REPOSITORY https://github.com/aminya/project_options.git
  GIT_TAG v0.24.1)
FetchContent_MakeAvailable(project_options)
include(${project_options_SOURCE_DIR}/Index.cmake)
include(${project_options_SOURCE_DIR}/src/DynamicProjectOptions.cmake)

# Setup vcpkg - should be called before defining project()
run_vcpkg()

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

# # This variable is set by project() in CMake 3.21+
string(COMPARE EQUAL "${CMAKE_SOURCE_DIR}" "${PROJECT_SOURCE_DIR}"
               PROJECT_IS_TOP_LEVEL)

if(PROJECT_IS_TOP_LEVEL)
  # Consider the CTest module, which creates targets and options! Only needed if
  # you want to enable submissions to a CDash server.
  # include(CTest)
endif()

# Add Git SHA
find_package(Git QUIET)
if(GIT_FOUND)
  message(STATUS "Found git")
  if(EXISTS "${PROJECT_SOURCE_DIR}/.git")
    message(
    STATUS "Executable will get SHA and branch used")
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
    message(STATUS "GIT SHA: ${GIT_SHA}")
    message(STATUS "GIT SHORT SHA: ${GIT_SHORT_SHA}")
    message(STATUS "GIT BRANCH: ${GIT_BRANCH}")
  else()
    message(WARNING ".git directory not found !")
    message(WARNING "Have you cloned/synced everything ?")
  endif()
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

if (UNIX)
  dynamic_project_options(
    ENABLE_INTERPROCEDURAL_OPTIMIZATION
    ENABLE_BUILD_WITH_TIME_TRACE
    ENABLE_NATIVE_OPTIMIZATION
    ENABLE_SANITIZER_ADDRESS
    ENABLE_SANITIZER_UNDEFINED_BEHAVIOR
    ENABLE_CACHE
    ENABLE_CPPCHECK
    ENABLE_CLANG_TIDY
    ENABLE_DOXYGEN
    CPPCHECK_OPTIONS
    --enable=style,performance,warning,portability
    --inline-suppr
    --suppress=cppcheckError
    --suppress=internalAstError
    --suppress=unmatchedSuppression
    --suppress=passedByValue
    --suppress=syntaxError
    --inconclusive)
elseif(WIN32)
    dynamic_project_options(
    ENABLE_INTERPROCEDURAL_OPTIMIZATION
    ENABLE_BUILD_WITH_TIME_TRACE
    ENABLE_NATIVE_OPTIMIZATION
    ENABLE_CACHE
    ENABLE_CPPCHECK
    ENABLE_CLANG_TIDY
    ENABLE_DOXYGEN
    CPPCHECK_OPTIONS
    --enable=style,performance,warning,portability
    --inline-suppr
    --suppress=cppcheckError
    --suppress=internalAstError
    --suppress=unmatchedSuppression
    --suppress=passedByValue
    --suppress=syntaxError
    --inconclusive)
endif()

target_compile_features(project_options INTERFACE cxx_std_${CMAKE_CXX_STANDARD})

if(UNIX)
  # add file to find libserial with cmake
  list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/submodules")
endif()

# boost vars
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
if (UNIX)
  set(BOOST_ALL_DYN_LINK ON)
  set(Boost_USE_STATIC_LIBS OFF)
elseif(WIN32)
  set(BOOST_ALL_DYN_LINK OFF)
  set(Boost_USE_STATIC_LIBS ON)
endif()

# required packages
find_package(Threads REQUIRED)
find_package(Boost REQUIRED COMPONENTS log log_setup date_time)

if (UNIX)
  find_package(LibSerial REQUIRED)
  find_package(PkgConfig REQUIRED)
endif()

add_subdirectory(submodules)
add_subdirectory(app)

# profile guided optimization
set(ENABLE_PROFILE_GENERATE OFF CACHE BOOL "Enable profile generation")
set(ENABLE_PROFILE_USE OFF CACHE BOOL "Enable profile usage")
set(PROFILE_DIR "prof" CACHE STRING "Profile directory" FORCE)

if(${CMAKE_CXX_COMPILER_ID} MATCHES "GNU")
  if(ENABLE_PROFILE_GENERATE)
    add_compile_options(-fprofile-generate=${PROFILE_DIR})
    add_link_options(-fprofile-generate=${PROFILE_DIR})
    if(ENABLE_PROFILE_USE)
      message(WARNING "Both profile generate and profile use enabled")
    endif()
  elseif(ENABLE_PROFILE_USE)
    add_compile_options(-fprofile-use=${PROFILE_DIR})
    add_link_options(-fprofile-use=${PROFILE_DIR})
  endif()
elseif(ENABLE_PROFILE_GENERATE OR ENABLE_PROFILE_USE)
  message(WARNING "Profile Guided Optimization implemented only for GCC")
endif()

if(CMAKE_SKIP_INSTALL_RULES)
  return()
elseif(NOT PROJECT_IS_TOP_LEVEL)
  return()
endif()

# add other targets that you want installed here, be default we just package the
# one executable we know we want to ship
package_project(TARGETS ${PROJECT_NAME} project_options project_warnings)

set(CPACK_PACKAGE_FILE_NAME
  "${CMAKE_PROJECT_NAME}-${CMAKE_PROJECT_VERSION}-${GIT_SHORT_SHA}-${CMAKE_SYSTEM_NAME}-${CMAKE_BUILD_TYPE}-${CMAKE_CXX_COMPILER_ID}-${CMAKE_CXX_COMPILER_VERSION}"
)

include(CPack)
