#include <SDL.h>
#include <SDL_syswm.h>
#include <boost/log/trivial.hpp>
#include <d3d11.h>
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_sdl.h>

#include "application.hpp"
#include "config.hpp"
#include "utilities.hpp"

using namespace TimeBox;

// main()
int wWinMain(HINSTANCE, HINSTANCE, PWSTR, INT)
{
  // Setup SDL
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0) {
    BOOST_LOG_TRIVIAL(error) << SDL_GetError();
    return EXIT_FAILURE;
  }

  // From 2.0.18: Enable native IME.
#ifdef SDL_HINT_IME_SHOW_UI
  SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
#endif

  // Create window
  SDL_WindowFlags window_flags = static_cast<SDL_WindowFlags>(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
  SDL_Window *window = SDL_CreateWindow(std::string(BuildInformation::PROJECT_NAME).c_str(),
    SDL_WINDOWPOS_CENTERED,
    SDL_WINDOWPOS_CENTERED,
    WINDOW_HEIGHT,
    WINDOW_WIDTH,
    window_flags);
  SDL_SysWMinfo wm_info;
  SDL_VERSION(&wm_info.version);
  SDL_GetWindowWMInfo(window, &wm_info);
  HWND window_handle{ wm_info.info.win.window };

  // Initialize Direct3D
  D3DContext d3d_context;
  if (not CreateDeviceD3D(window_handle, d3d_context)) {
    DestroyDeviceD3D(d3d_context);
    BOOST_LOG_TRIVIAL(error) << "Failed to create D3D device !";
    return EXIT_FAILURE;
  }

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();

  // Setup Platform/Renderer backends
  ImGui_ImplSDL2_InitForD3D(window);
  ImGui_ImplDX11_Init(d3d_context.p_d3d_device, d3d_context.p_d3d_device_context);

  // Variables for ImGui
  static const Uint32 max_fps{ 20 };
  static Uint32 last_frametime, this_frametime;

  // Application context
  AppContext context = InitializeContext();

  // Main loop
  while (context.application_run) {
    // FPS limiter
    last_frametime = this_frametime;
    this_frametime = SDL_GetTicks();

    // Poll and handle events (inputs, window resize, etc.)
    // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your
    // inputs.
    // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite
    // your copy of the mouse data.
    // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or
    // clear/overwrite your copy of the keyboard data. Generally you may always pass all inputs to dear imgui, and hide
    // them from your application based on those two flags.
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      ImGui_ImplSDL2_ProcessEvent(&event);
      if (event.type == SDL_QUIT) context.application_run = true;
      if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE
          && event.window.windowID == SDL_GetWindowID(window))
        context.application_run = false;
    }

    // Start the Dear ImGui frame
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    // GUI parts
    MainDialog(context);
    ConnectDialog(context);
    WarningPopup(context);
    AboutDialog(context);

    // Rendering - This is our Viewport
    ImGui::Render();
    d3d_context.p_d3d_device_context->OMSetRenderTargets(1, &d3d_context.p_render_target_view, NULL);
    d3d_context.p_d3d_device_context->ClearRenderTargetView(d3d_context.p_render_target_view, NULL);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    d3d_context.p_swap_chain->Present(1, 0);// VSync

    // FPS limiter
    if (this_frametime - last_frametime < 1000 / max_fps) SDL_Delay(1000 / max_fps - this_frametime + last_frametime);
  }

  // Cleanup
  DestroyContext(context);

  ImGui_ImplDX11_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();

  DestroyDeviceD3D(d3d_context);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return EXIT_SUCCESS;
}
