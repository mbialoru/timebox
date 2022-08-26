add_subdirectory(lib)
add_subdirectory(test)
add_subdirectory(benchmark)
add_subdirectory(proto)

file(GLOB_RECURSE SRCS CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

add_executable(${PROJECT_NAME} ${SRCS})
target_include_directories(${PROJECT_NAME}
                           PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(
  ${PROJECT_NAME}
  PRIVATE project_options
          project_warnings
          lib${PROJECT_NAME}
          SDL2
          IMGUI
          Threads::Threads
          LibSerial::serial
          Boost::log
          Boost::log_setup)
