add_subdirectory(SDL)

target_disable_cpp_check(SDL2)
target_disable_vs_analysis(SDL2)
target_disable_clang_tidy(SDL2)
target_disable_static_analysis(SDL2)

add_subdirectory(fmt)

target_disable_cpp_check(fmt)
target_disable_vs_analysis(fmt)
target_disable_clang_tidy(fmt)
target_disable_static_analysis(fmt)

project(IMGUI)
set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/imgui)
add_library(IMGUI STATIC)

find_package(OpenGL REQUIRED)
if (OPENGL_FOUND)
    message(STATUS "OpenGL found")
    message(STATUS "include dir: ${OPENGL_INCLUDE_DIR}")
    message(STATUS "link libraries: ${OPENGL_LIBRARIES}")
else (OPENGL_FOUND)
    message(STATUS "OpenGL not found")
endif()

target_link_libraries(IMGUI PRIVATE SDL2 ${OPENGL_LIBRARIES} INTERFACE -ldl)
set_property(TARGET IMGUI PROPERTY CXX_STANDARD 17)

target_sources( IMGUI
    PRIVATE
        ${IMGUI_DIR}/imgui_demo.cpp
        ${IMGUI_DIR}/imgui_draw.cpp
        ${IMGUI_DIR}/imgui_tables.cpp
        ${IMGUI_DIR}/imgui_widgets.cpp
        ${IMGUI_DIR}/imgui.cpp
        ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
        ${IMGUI_DIR}/backends/imgui_impl_sdl.cpp
)

target_include_directories( IMGUI
    PUBLIC
        ${IMGUI_DIR}
        ${IMGUI_DIR}/backends
        ${SDL2_DIR}/include
)

target_disable_cpp_check(IMGUI)
target_disable_vs_analysis(IMGUI)
target_disable_clang_tidy(IMGUI)
target_disable_static_analysis(IMGUI)