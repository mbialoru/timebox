include(GoogleTest)
enable_testing()

set(CTEST_TEST_TIMEOUT 20) # if triggered, log files won't be written

file(GLOB_RECURSE SRCS CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE HDRS CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp)

add_executable(${PROJECT_NAME}_test ${HDRS} ${SRCS})
target_include_directories(${PROJECT_NAME}_test
                           PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(
  ${PROJECT_NAME}_test
  PRIVATE lib${PROJECT_NAME}
          LibSerial::serial
          gtest
          gtest_main
          gmock
          gmock_main
          Boost::log)

if(ipo_supported)
  message(STATUS "IPO / LTO enabled for ${PROJECT_NAME}_test")
  set_target_properties(${PROJECT_NAME}_test
                        PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
else()
  message(STATUS "IPO / LTO not supported: <${error}>")
endif()

gtest_discover_tests(${PROJECT_NAME}_test)
