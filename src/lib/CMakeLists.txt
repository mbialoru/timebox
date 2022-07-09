file(GLOB_RECURSE SRCS CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE HDRS CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp)

add_library(${PROJECT_LIB} SHARED STATIC ${SRCS} ${HDRS})
set_target_properties(${PROJECT_LIB} PROPERTIES LINKER_LANGUAGE CXX)

if( ipo_supported )
    message(STATUS "IPO / LTO enabled for ${PROJECT_LIB}")
    set_target_properties(${PROJECT_LIB} PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
else()
    message(STATUS "IPO / LTO not supported: <${error}>")
endif()

target_link_libraries(${PROJECT_LIB}
    PUBLIC
        Boost::log
        Boost::program_options
        Boost::log_setup)