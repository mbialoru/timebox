include(GoogleTest)
enable_testing()

set(CTEST_TEST_TIMEOUT 20) # if triggered, log files won't be written

file(GLOB_RECURSE SRCS CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_executable(${PROJECT_NAME}_proto ${SRCS})
target_include_directories(${PROJECT_NAME}_proto
                           PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(
  ${PROJECT_NAME}_proto
  PRIVATE Threads::Threads
          lib${PROJECT_NAME}
          gtest
          gtest_main
          IMGUI
          SDL2
          LibSerial::serial)

gtest_discover_tests(${PROJECT_NAME}_proto)
