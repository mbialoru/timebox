add_library(lib${PROJECT_NAME} STATIC "")

set_target_properties(lib${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX PREFIX
                                                                        "")

add_subdirectory(include)
add_subdirectory(src)

target_link_libraries(lib${PROJECT_NAME} PRIVATE project_options
                                                 project_warnings)

target_link_system_libraries(
  lib${PROJECT_NAME}
  PRIVATE
  fmt::fmt
  Threads::Threads
  Boost::log
  Boost::log_setup
  Boost::date_time)
