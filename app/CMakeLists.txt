add_subdirectory(lib)
add_subdirectory(test)
add_subdirectory(benchmark)

if(ENABLE_DEVELOPER_MODE)
  add_subdirectory(proto)
endif()

add_executable(${PROJECT_NAME})
add_subdirectory(include)
add_subdirectory(src)

target_link_libraries(${PROJECT_NAME} PRIVATE project_options project_warnings
                                              lib${PROJECT_NAME})

target_link_system_libraries(
  ${PROJECT_NAME}
  PRIVATE
  SDL2
  IMGUI
  fmt::fmt
  Threads::Threads
  Boost::log
  Boost::log_setup)
