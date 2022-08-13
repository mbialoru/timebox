add_subdirectory(lib)
add_subdirectory(test)
add_subdirectory(benchmark)
add_subdirectory(proto)

add_executable(
  ${PROJECT_NAME}
  ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/application.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/application.hpp)
target_include_directories(${PROJECT_NAME}
                           PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(
  ${PROJECT_NAME}
  PRIVATE lib${PROJECT_NAME}
          SDL2
          IMGUI
          Threads::Threads
          LibSerial::serial
          Boost::log
          Boost::log_setup)

if(ipo_supported)
  message(STATUS "IPO / LTO enabled for ${PROJECT_NAME}")
  set_target_properties(${PROJECT_NAME} PROPERTIES INTERPROCEDURAL_OPTIMIZATION
                                                   TRUE)
else()
  message(STATUS "IPO / LTO not supported: <${error}>")
endif()
