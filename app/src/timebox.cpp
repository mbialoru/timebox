#include "timebox.hpp"

using namespace TimeBox;

void TimeBox::MainLoop(SDL_Window *tp_sdl_window, AppContext &tr_app_context, D3DContext &tr_d3d_context)
{
  uint32_t last_frametime, this_frametime;

  while (tr_app_context.application_run) {
    // FPS limiter
    last_frametime = this_frametime;
    this_frametime = SDL_GetTicks();

    HandleSDLEvent(tp_sdl_window, tr_app_context);

    // Start the Dear ImGui frame
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    HandleGUI(tr_app_context);
    Render(tr_d3d_context);

    // FPS limiter
    if (this_frametime - last_frametime < 1000 / s_max_fps)
      SDL_Delay(1000 / s_max_fps - this_frametime + last_frametime);
  }

  // Cleanup
  tr_app_context.Reset();
  Cleanup(tp_sdl_window, tr_d3d_context);
}