find_package(Git QUIET)
if(GIT_FOUND)
  message(STATUS "Found git")
  if(EXISTS "${PROJECT_SOURCE_DIR}/.git")
    message(
    STATUS "Executable will get SHA and branch used")
    execute_process(
      COMMAND git -C ${CMAKE_SOURCE_DIR} rev-parse HEAD
      OUTPUT_VARIABLE GIT_SHA
      OUTPUT_STRIP_TRAILING_WHITESPACE)
    execute_process(
      COMMAND git -C ${CMAKE_SOURCE_DIR} branch --show-current
      OUTPUT_VARIABLE GIT_BRANCH
      OUTPUT_STRIP_TRAILING_WHITESPACE)
    string(SUBSTRING "${GIT_SHA}" 0 8 GIT_SHORT_SHA)
    set(GIT_BRANCH ${GIT_BRANCH})
    set(GIT_SHORT_SHA ${GIT_SHORT_SHA})
    message(STATUS "GIT SHA: ${GIT_SHA}")
    message(STATUS "GIT SHORT SHA: ${GIT_SHORT_SHA}")
    message(STATUS "GIT BRANCH: ${GIT_BRANCH}")
    string(SHA256 BUILD_HASH ${GIT_SHORT_SHA})
  else()
    message(WARNING ".git directory not found !")
    message(WARNING "Have you cloned/synced everything ?")
  endif()
else()
  message(WARNING "Git not found !")
endif()