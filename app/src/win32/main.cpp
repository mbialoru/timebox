#include <SDL.h>
#include <SDL_syswm.h>
#include <boost/log/trivial.hpp>
#include <d3d11.h>
#include <imgui_impl_sdl.h>

#include "appcontext.hpp"
#include "gui_backend.hpp"
#include "gui_frontend.hpp"

using namespace TimeBox;

// Initialize D3D Context
static D3DContext s_d3d_context;

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
  SDL_WindowFlags window_flags = static_cast<SDL_WindowFlags>(SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI);
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
  if (not CreateDeviceD3D(window_handle, s_d3d_context)) {
    DestroyDeviceD3D(s_d3d_context);
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
  ImGui_ImplDX11_Init(s_d3d_context.p_d3d_device, s_d3d_context.p_d3d_device_context);

  // Application context
  AppContext context;

  // Main loop
  while (context.application_run) {
    // FPS limiter
    s_last_frametime = s_this_frametime;
    s_this_frametime = SDL_GetTicks();

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
    s_d3d_context.p_d3d_device_context->OMSetRenderTargets(1, &(s_d3d_context.p_render_target_view), NULL);
    s_d3d_context.p_d3d_device_context->ClearRenderTargetView(
      s_d3d_context.p_render_target_view, clear_color_with_alpha);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    s_d3d_context.p_swap_chain->Present(1, 0);// VSync

    // FPS limiter
    if (s_this_frametime - s_last_frametime < 1000 / s_max_fps)
      SDL_Delay(1000 / s_max_fps - s_this_frametime + s_last_frametime);
  }

  // Cleanup
  context.Reset();
  ImGui_ImplDX11_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();

  DestroyDeviceD3D(s_d3d_context);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return EXIT_SUCCESS;
}
