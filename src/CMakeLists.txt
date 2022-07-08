add_subdirectory(lib)

add_executable(${CMAKE_PROJECT_NAME} main.cpp)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE ${PROJECT_NAME_LIB})

if( ipo_supported )
    message(STATUS "IPO / LTO enabled for ${CMAKE_PROJECT_NAME}")
    set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
else()
    message(STATUS "IPO / LTO not supported: <${error}>")
endif()