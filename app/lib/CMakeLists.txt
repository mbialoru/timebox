file(GLOB_RECURSE SRCS CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE HDRS CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.hpp.in
               ${CMAKE_CURRENT_SOURCE_DIR}/include/config.hpp)

add_library(lib${PROJECT_NAME} STATIC ${SRCS})
set_target_properties(lib${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX PREFIX
                                                                        "")
target_include_directories(lib${PROJECT_NAME}
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

if(ipo_supported)
  message(STATUS "IPO / LTO enabled for lib${PROJECT_NAME}")
  set_target_properties(lib${PROJECT_NAME}
                        PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
else()
  message(STATUS "IPO / LTO not supported: <${error}>")
endif()

target_link_libraries(
  lib${PROJECT_NAME} PRIVATE Threads::Threads Boost::log Boost::date_time
                             LibSerial::serial)
