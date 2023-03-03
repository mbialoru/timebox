target_include_directories(lib${PROJECT_NAME}
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

if(TARGET_PLATFORM MATCHES "LINUX")
  add_subdirectory(linux)
elseif(TARGET_PLATFORM MATCHES "WIN32")
  add_subdirectory(win32)
endif()
