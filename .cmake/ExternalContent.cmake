function(DisableAnalyzers target_name)
  message(STATUS "Disabling static analysis for target ${target_name}")
  target_disable_cpp_check(${target_name})
  target_disable_vs_analysis(${target_name})
  target_disable_clang_tidy(${target_name})
  target_disable_static_analysis(${target_name})
endfunction(DisableAnalyzers)

function(FetchExternalContent repo_url git_tag content_uid)
  message(STATUS "Fetching external repository ${content_uid}")
  FetchContent_Declare(
    "${content_uid}"
    GIT_REPOSITORY "${repo_url}"
    GIT_TAG "${git_tag}"

    # Commands are left empty so that we only clone source and perform no build
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
  )
  FetchContent_GetProperties(${content_uid})
  if(NOT ${content_uid}_POPULATED)
    FetchContent_Populate(${content_uid})
    add_subdirectory(${${content_uid}_SOURCE_DIR} ${${content_uid}_BINARY_DIR} EXCLUDE_FROM_ALL)
  endif()
  FetchContent_MakeAvailable(${content_uid})
endfunction(FetchExternalContent)

FetchExternalContent(https://github.com/google/googletest release-1.12.1 googletest)
DisableAnalyzers(gtest)
DisableAnalyzers(gtest_main)
DisableAnalyzers(gmock)
DisableAnalyzers(gmock_main)

FetchExternalContent(https://github.com/google/benchmark v1.7.1 googlebenchmark)
DisableAnalyzers(benchmark)

FetchExternalContent(https://github.com/libsdl-org/SDL release-2.26.0 sdl)
DisableAnalyzers(SDL2)

FetchExternalContent(https://github.com/fmtlib/fmt 9.1.0 fmt)
DisableAnalyzers(fmt)

FetchExternalContent(https://github.com/pantor/inja v3.3.0 inja)
DisableAnalyzers(inja)

FetchContent_Declare(
  imgui
  GIT_REPOSITORY https://github.com/ocornut/imgui
  GIT_TAG v1.89.1
)
include(imgui)
DisableAnalyzers(IMGUI)