project(PROTO_02_regex)

add_executable(${PROJECT_NAME})

add_subdirectory(src)
add_subdirectory(include)

target_link_libraries(${PROJECT_NAME} PRIVATE project_options project_warnings)

target_link_system_libraries(${PROJECT_NAME} PRIVATE gtest gtest_main)

gtest_discover_tests(${PROJECT_NAME})
