file(GLOB_RECURSE SRCS CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_executable(${PROJECT_NAME}_benchmark ${SRCS})
target_link_libraries(
  ${PROJECT_NAME}_benchmark PRIVATE project_options project_warnings
                                    Threads::Threads benchmark::benchmark)
target_include_directories(${PROJECT_NAME}_benchmark
                           PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_disable_cpp_check(${PROJECT_NAME}_benchmark)
target_disable_vs_analysis(${PROJECT_NAME}_benchmark)
target_disable_clang_tidy(${PROJECT_NAME}_benchmark)
target_disable_static_analysis(${PROJECT_NAME}_benchmark)
