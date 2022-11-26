file(GLOB SRCS CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
file(GLOB HDRS CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp)

# Exclude platform specific files from build
if(TARGET_PLATFORM MATCHES "LINUX")
  file(GLOB PLATFORM_SRCS CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/linux/*.cpp)
  file(GLOB PLATFORM_HDRS CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/include/linux/*.hpp)
elseif(TARGET_PLATFORM MATCHES "WIN32")
  file(GLOB PLATFORM_SRCS CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/win32/*.cpp)
  file(GLOB PLATFORM_HDRS CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/include/win32/*.hpp)
endif()
list(APPEND SRCS ${PLATFORM_SRCS})
list(APPEND HDRS ${PLATFORM_HDRS})

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.hpp.in
               ${CMAKE_CURRENT_SOURCE_DIR}/include/config.hpp)

add_library(lib${PROJECT_NAME} STATIC ${SRCS})
set_target_properties(lib${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX PREFIX "")

target_include_directories(lib${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

if(TARGET_PLATFORM MATCHES "LINUX")
  target_include_directories(lib${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/linux)
elseif(TARGET_PLATFORM MATCHES "WIN32")
  target_include_directories(lib${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/win32)
endif()

target_link_libraries(lib${PROJECT_NAME} PRIVATE project_options
                                                 project_warnings)
target_link_system_libraries(
  lib${PROJECT_NAME}
  PRIVATE
  fmt::fmt
  Threads::Threads
  Boost::log
  Boost::log_setup
  Boost::date_time)