project(${CMAKE_PROJECT_NAME}_benchmark)

file(GLOB_RECURSE SRCS CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE HDRS CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp)

add_executable(${PROJECT_NAME} ${SRCS} ${HDRS})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        Threads::Threads
        ${CMAKE_PROJECT_NAME}_lib
        benchmark::benchmark
)

if( ipo_supported )
    message(STATUS "IPO / LTO enabled for ${PROJECT_NAME}")
    set_target_properties(${PROJECT_NAME} PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
else()
    message(STATUS "IPO / LTO not supported: <${error}>")
endif()