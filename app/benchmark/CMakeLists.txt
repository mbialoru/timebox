file(GLOB_RECURSE SRCS CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_executable(${PROJECT_NAME}_benchmark ${SRCS})
target_link_libraries(
  ${PROJECT_NAME}_benchmark PRIVATE lib${CMAKE_PROJECT_NAME} Threads::Threads
                                    benchmark::benchmark)

if(ipo_supported)
  message(STATUS "IPO / LTO enabled for ${PROJECT_NAME}")
  set_target_properties(${PROJECT_NAME}_benchmark
                        PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
else()
  message(STATUS "IPO / LTO not supported: <${error}>")
endif()
