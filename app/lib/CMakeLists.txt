file(GLOB_RECURSE SRCS CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE HDRS CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp)

# Exclude platform specific files from build
if(UNIX)
list(FILTER HDRS EXCLUDE REGEX "win.*\\.hpp$")
list(FILTER SRCS EXCLUDE REGEX "win.*\\.cpp$")
elseif(WIN32)
list(FILTER HDRS EXCLUDE REGEX "lin.*\\.hpp$")
list(FILTER SRCS EXCLUDE REGEX "lin.*\\.cpp$")
message("${HDRS}")
endif()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.hpp.in
               ${CMAKE_CURRENT_SOURCE_DIR}/include/config.hpp)

add_library(lib${PROJECT_NAME} STATIC ${SRCS})
set_target_properties(lib${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX PREFIX "")
target_include_directories(lib${PROJECT_NAME}
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(lib${PROJECT_NAME} PRIVATE project_options
                                                 project_warnings)
if(UNIX)
  target_link_system_libraries(
    lib${PROJECT_NAME}
    PRIVATE
    fmt::fmt
    Threads::Threads
    Boost::log
    Boost::log_setup
    Boost::date_time
    LibSerial::serial)
elseif(WIN32)
  target_link_system_libraries(
    lib${PROJECT_NAME}
    PRIVATE
    fmt::fmt
    Threads::Threads
    Boost::log
    Boost::log_setup
    Boost::date_time)
endif()
