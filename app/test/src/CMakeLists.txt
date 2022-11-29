file(GLOB SRCS CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
target_sources(${PROJECT_NAME}_test PRIVATE ${SRCS})

if(TARGET_PLATFORM MATCHES "LINUX")
  add_subdirectory(linux)
elseif(TARGET_PLATFORM MATCHES "WIN32")
  add_subdirectory(win32)
endif()
