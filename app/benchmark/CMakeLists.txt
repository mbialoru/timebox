file(GLOB_RECURSE SRCS CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_executable(${PROJECT_NAME}_benchmark ${SRCS})
target_link_libraries(
  ${PROJECT_NAME}_benchmark
  PRIVATE project_options project_warnings lib${CMAKE_PROJECT_NAME}
          Threads::Threads benchmark::benchmark)
