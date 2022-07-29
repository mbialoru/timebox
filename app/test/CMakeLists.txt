include(GoogleTest)
enable_testing()

set(CTEST_TEST_TIMEOUT 20) # if triggered, log files won't be written

file(GLOB_RECURSE SRCS CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_executable(${PROJECT_NAME}_test ${SRCS})
target_link_libraries(${PROJECT_NAME}_test
    PRIVATE
        lib${CMAKE_PROJECT_NAME}_static
        libserial_static
        gtest
        gtest_main
        gmock
        gmock_main
)

if( ipo_supported )
    message(STATUS "IPO / LTO enabled for ${PROJECT_NAME}_test")
    set_target_properties(${PROJECT_NAME}_test PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
else()
    message(STATUS "IPO / LTO not supported: <${error}>")
endif()

gtest_discover_tests(${PROJECT_NAME}_test)