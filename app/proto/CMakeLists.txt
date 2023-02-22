include(GoogleTest)
enable_testing()

set(CTEST_TEST_TIMEOUT 20) # if triggered, log files won't be written

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/proto)

if(TARGET_PLATFORM MATCHES "LINUX")
  add_subdirectory(linux)
elseif(TARGET_PLATFORM MATCHES "WIN32")
  add_subdirectory(win32)
endif()

add_subdirectory(00_experiment)
add_subdirectory(01_imgui)
add_subdirectory(02_regex)
add_subdirectory(03_timefromstr)
