project(PROTO_W03_imgui_dx11)

add_executable(${PROJECT_NAME})

add_subdirectory(src)
add_subdirectory(include)

target_link_libraries(${PROJECT_NAME} PRIVATE project_options project_warnings)

target_link_system_libraries(${PROJECT_NAME} PRIVATE SDL2 IMGUI)
