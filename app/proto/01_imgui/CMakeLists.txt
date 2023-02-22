project(01_imgui)

add_executable(${PROJECT_NAME})

add_subdirectory(src)
add_subdirectory(include)

target_link_libraries(${PROJECT_NAME} PRIVATE project_options project_warnings)

target_link_system_libraries(${PROJECT_NAME} PRIVATE SDL2 IMGUI)

gtest_discover_tests(${PROJECT_NAME})
