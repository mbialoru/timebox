configure_file(${CMAKE_CURRENT_SOURCE_DIR}/../config.hpp.in
               ${CMAKE_CURRENT_SOURCE_DIR}/config.hpp)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
