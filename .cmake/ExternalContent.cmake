include(utilities)

fetchexternalcontent(https://github.com/google/googletest release-1.12.1
                     googletest)
disableanalyzers(gtest)
disableanalyzers(gtest_main)
disableanalyzers(gmock)
disableanalyzers(gmock_main)

fetchexternalcontent(https://github.com/google/benchmark v1.7.1 googlebenchmark)
disableanalyzers(benchmark)

fetchexternalcontent(https://github.com/libsdl-org/SDL release-2.26.0 sdl)
disableanalyzers(SDL2)

fetchexternalcontent(https://github.com/fmtlib/fmt 9.1.0 fmt)
disableanalyzers(fmt)

fetchexternalcontent(https://github.com/pantor/inja v3.3.0 inja)
disableanalyzers(inja)

fetchexternalcontent(https://github.com/ocornut/imgui v1.89.1 imgui)
disableanalyzers(IMGUI)
