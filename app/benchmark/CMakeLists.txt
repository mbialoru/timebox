add_executable(${PROJECT_NAME}_benchmark)

add_subdirectory(include)
add_subdirectory(src)

target_link_libraries(${PROJECT_NAME}_benchmark PRIVATE project_options
                                                        project_warnings)

target_link_system_libraries(${PROJECT_NAME}_benchmark PRIVATE Threads::Threads
                             benchmark::benchmark)

DisableAnalyzers(${PROJECT_NAME}_benchmark)