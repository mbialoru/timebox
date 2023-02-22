project(PROTO_W02_winmain)

add_executable(${PROJECT_NAME})

add_subdirectory(src)
add_subdirectory(include)

target_link_libraries(${PROJECT_NAME} PRIVATE project_options project_warnings)

target_link_system_libraries(${PROJECT_NAME} PRIVATE Boost::headers)

# Required for MinGW acceptance of wWinMain
target_link_options(${PROJECT_NAME} PRIVATE -municode)
