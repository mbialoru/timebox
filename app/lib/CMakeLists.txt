file(GLOB_RECURSE SRCS CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.hpp.in
               ${CMAKE_CURRENT_SOURCE_DIR}/include/config.hpp)

add_library(lib${PROJECT_NAME}_static STATIC ${SRCS})
set_target_properties(lib${PROJECT_NAME}_static PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(lib${PROJECT_NAME}_static
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

if(ipo_supported)
  message(STATUS "IPO / LTO enabled for lib${PROJECT_NAME}_static")
  set_target_properties(lib${PROJECT_NAME}_static
                        PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
else()
  message(STATUS "IPO / LTO not supported: <${error}>")
endif()

target_link_libraries(
  lib${PROJECT_NAME}_static PRIVATE Threads::Threads Boost::log
                                    Boost::date_time libserial_static)
