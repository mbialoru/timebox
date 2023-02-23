file(GLOB SRCS CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
target_sources(${PROJECT_NAME} PRIVATE ${SRCS})
target_link_options(${PROJECT_NAME} PRIVATE -municode)
