include(GoogleTest)
enable_testing()

set(CTEST_TEST_TIMEOUT 20) # if triggered, log files won't be written

file(GLOB_RECURSE SRCS CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE HDRS CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp)

add_executable(${PROJECT_NAME}_test ${HDRS} ${SRCS})
target_include_directories(${PROJECT_NAME}_test
                           PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_compile_options(${PROJECT_NAME}_test PRIVATE "--coverage")
target_link_options(${PROJECT_NAME}_test PRIVATE "--coverage")

target_link_libraries(
  ${PROJECT_NAME}_test PRIVATE project_options project_warnings
                               lib${PROJECT_NAME})

target_link_system_libraries(
  ${PROJECT_NAME}_test
  PRIVATE
  LibSerial::serial
  gtest
  gtest_main
  gmock
  gmock_main
  Boost::log
  Boost::log_setup)

gtest_discover_tests(${PROJECT_NAME}_test)
