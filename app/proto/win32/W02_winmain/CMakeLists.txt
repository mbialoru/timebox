project(W02_winmain)

add_executable(${PROJECT_NAME})

add_subdirectory(src)
add_subdirectory(include)

target_link_libraries(${PROJECT_NAME} PRIVATE project_options project_warnings)

target_link_system_libraries(${PROJECT_NAME} PRIVATE Boost::headers)

gtest_discover_tests(${PROJECT_NAME})
