function(DisableAnalyzers target_name)
  message(STATUS "Disabling static analysis for target ${target_name}")
  target_disable_cpp_check(${target_name})
  target_disable_vs_analysis(${target_name})
  target_disable_clang_tidy(${target_name})
  target_disable_static_analysis(${target_name})
endfunction(DisableAnalyzers)

function(SearchCMakeListsForCopy content_uid)
  message(
    STATUS "Looking for matching CMakeLists for ${content_uid} in module path")
  if(EXISTS "${CMAKE_MODULE_PATH}/CMakeLists_${content_uid}.txt")
    message(STATUS "Found matching CMakeLists for ${content_uid}")
    file(COPY_FILE "${CMAKE_MODULE_PATH}/CMakeLists_${content_uid}.txt"
         "${${content_uid}_SOURCE_DIR}/CMakeLists.txt")
    message(STATUS "Copied CMakeLists.txt from module path for ${content_uid}")
  else()
    message(STATUS "Failed to find mathing CMakeLists file for ${content_uid}")
  endif()
endfunction(SearchCMakeListsForCopy)

function(FetchExternalContent repo_url git_tag content_uid)
  message(STATUS "Fetching external repository ${content_uid}")
  FetchContent_Declare(
    "${content_uid}"
    GIT_REPOSITORY "${repo_url}"
    GIT_TAG "${git_tag}"
    # Commands are left empty so that we only clone source and perform no build
    CONFIGURE_COMMAND "" BUILD_COMMAND "" INSTALL_COMMAND "")
  FetchContent_GetProperties(${content_uid})
  if(NOT ${content_uid}_POPULATED)
    FetchContent_Populate(${content_uid})
    # Support repositories with external CMakeLists.txt (not in fetched content)
    if(NOT EXISTS "${${content_uid}_SOURCE_DIR}/CMakeLists.txt")
      searchcmakelistsforcopy(${content_uid})
    endif()
    add_subdirectory(${${content_uid}_SOURCE_DIR} ${${content_uid}_BINARY_DIR}
                     EXCLUDE_FROM_ALL)
  endif()
  FetchContent_MakeAvailable(${content_uid})
endfunction(FetchExternalContent)
