include(GoogleTest)
enable_testing()

set(CTEST_TEST_TIMEOUT 20) # if triggered, log files won't be written

add_executable(${PROJECT_NAME}_proto)

add_subdirectory(include)
add_subdirectory(src)

target_link_libraries(
  ${PROJECT_NAME}_proto PRIVATE project_options project_warnings
                                lib${PROJECT_NAME})

target_link_system_libraries(
  ${PROJECT_NAME}_proto PRIVATE Boost::headers Threads::Threads gtest
  gtest_main
  # IMGUI SDL2
)

gtest_discover_tests(${PROJECT_NAME}_proto)
