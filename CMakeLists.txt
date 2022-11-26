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

project(
  timebox
  VERSION 0.0.15
  LANGUAGES C CXX
  DESCRIPTION
    "Synchronize system clock with hardware GPS interface attached by USB"
  HOMEPAGE_URL "https://gitlab.sudobash.pl/Au5m8btpUaUS5t3dpixE/time-box")
set(PROJECT_VERSION_ADDENDUM "InDev" CACHE STRING "Addendum to project version (i.e. RC)" FORCE)
string(TIMESTAMP BUILD_TIME "%Y-%m-%d %H:%M:%S UTC" UTC)

# detect target platform
if(NOT TARGET_PLATFORM)
  if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(TARGET_PLATFORM "WIN32")
  elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(TARGET_PLATFORM "LINUX")
  else()
    message(FATAL_ERROR "Unsupported platform: ${CMAKE_SYSTEM_NAME}")
  endif()
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
    string(SHA256 BUILD_HASH ${GIT_SHORT_SHA})
  else()
    message(WARNING ".git directory not found !")
    message(WARNING "Have you cloned/synced everything ?")
  endif()
else()
  message(WARNING "Git not found !")
endif()

dynamic_project_options(
  ENABLE_INTERPROCEDURAL_OPTIMIZATION
  ENABLE_BUILD_WITH_TIME_TRACE
  ENABLE_NATIVE_OPTIMIZATION
  ENABLE_CACHE
  ENABLE_DOXYGEN)
target_compile_features(project_options INTERFACE cxx_std_${CMAKE_CXX_STANDARD})

# boost vars
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
if (TARGET_PLATFORM MATCHES "LINUX")
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

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest
  GIT_BRANCH main
)
# FetchContent_MakeAvailable(googletest)
FetchContent_Declare(
  googlebenchmark
  GIT_REPOSITORY https://github.com/google/benchmark
  GIT_BRANCH main
)
FetchContent_Declare(
  sdl
  GIT_REPOSITORY https://github.com/libsdl-org/SDL
  GIT_BRANCH main
)
FetchContent_Declare(
  imgui
  GIT_REPOSITORY https://github.com/ocornut/imgui
  GIT_BRANCH master
)

# TODO: Choose if we make use of these packages
# FetchContent_Declare(
#   fmt
#   GIT_REPOSITORY https://github.com/fmtlib/fmt
#   GIT_BRANCH master)
# FetchContent_Declare(
#   inja
#   GIT_REPOSITORY https://github.com/pantor/inja
#   GIT_TAG master)

add_subdirectory(app)

# output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# profile guided optimization
set(ENABLE_PROFILE_GENERATE OFF CACHE BOOL "Enable profile generation")
set(ENABLE_PROFILE_USE OFF CACHE BOOL "Enable profile usage")
set(PROFILE_DIR "pgo" CACHE STRING "Profile directory" FORCE)
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