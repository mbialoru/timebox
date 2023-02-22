project(PROTO_W01_clockcontrol)

add_executable(${PROJECT_NAME})

add_subdirectory(src)
add_subdirectory(include)

target_link_libraries(${PROJECT_NAME} PRIVATE project_options project_warnings
                                              libtimebox)

target_link_system_libraries(${PROJECT_NAME} PRIVATE Boost::headers gtest
                             gtest_main)

gtest_discover_tests(${PROJECT_NAME})
