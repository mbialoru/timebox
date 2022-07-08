include(GoogleTest)
enable_testing()

set(CTEST_TEST_TIMEOUT 20) # if triggered, log files won't be written

file(GLOB_RECURSE SRCS CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE HDRS CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp)

add_executable(${PROJECT_NAME_TEST} ${SRCS} ${HDRS})
target_link_libraries(${PROJECT_NAME_TEST}
    PRIVATE
        ${PROJECT_NAME_LIB}
        gtest
        gtest_main
)

if( ipo_supported )
    message(STATUS "IPO / LTO enabled for ${PROJECT_NAME_TEST}")
    set_target_properties(${PROJECT_NAME_TEST} PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
else()
    message(STATUS "IPO / LTO not supported: <${error}>")
endif()

gtest_discover_tests(${PROJECT_NAME_TEST})