project(${CMAKE_PROJECT_NAME}_test)

include(GoogleTest)
enable_testing()

set(CTEST_TEST_TIMEOUT 20) # if triggered, log files won't be written

file(GLOB_RECURSE SRCS CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE HDRS CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp)

add_executable(${PROJECT_NAME} ${SRCS} ${HDRS})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_PROJECT_NAME}_lib
        gtest
        gtest_main
)

if( ipo_supported )
    message(STATUS "IPO / LTO enabled for ${PROJECT_NAME}")
    set_target_properties(${PROJECT_NAME} PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
else()
    message(STATUS "IPO / LTO not supported: <${error}>")
endif()

gtest_discover_tests(${PROJECT_NAME})