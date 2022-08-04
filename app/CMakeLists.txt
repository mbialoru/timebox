add_subdirectory(lib)
add_subdirectory(test)
add_subdirectory(benchmark)
add_subdirectory(proto)

add_executable(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)
target_link_libraries(
  ${PROJECT_NAME} PRIVATE lib${CMAKE_PROJECT_NAME}_static SDL2 IMGUI
                          libserial_static Boost::log Boost::log_setup)

if(ipo_supported)
  message(STATUS "IPO / LTO enabled for ${PROJECT_NAME}")
  set_target_properties(${PROJECT_NAME} PROPERTIES INTERPROCEDURAL_OPTIMIZATION
                                                   TRUE)
else()
  message(STATUS "IPO / LTO not supported: <${error}>")
endif()
