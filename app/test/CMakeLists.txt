include(GoogleTest)
enable_testing()

set(CTEST_TEST_TIMEOUT 20) # if triggered, log files won't be written

add_executable(${PROJECT_NAME}_test)
add_subdirectory(include)
add_subdirectory(src)

target_compile_options(${PROJECT_NAME}_test PRIVATE "--coverage")
target_link_options(${PROJECT_NAME}_test PRIVATE "--coverage")

target_link_libraries(
  ${PROJECT_NAME}_test PRIVATE project_options project_warnings
                               lib${PROJECT_NAME})
target_link_system_libraries(
  ${PROJECT_NAME}_test
  PRIVATE
  GTest::gtest
  GTest::gtest_main
  GTest::gmock
  GTest::gmock_main
  Boost::log
  Boost::log_setup)

gtest_discover_tests(${PROJECT_NAME}_test)
