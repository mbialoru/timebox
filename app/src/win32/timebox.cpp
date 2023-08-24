#include "timebox.hpp"

using namespace TimeBox;

void TimeBox::main_loop(SDL_Window *tp_sdl_window, AppContext &tr_app_context, D3DContext &tr_d3d_context)
{
  while (tr_app_context.application_run) {
    // FPS limiter
    static uint32_t s_last_frametime, s_this_frametime;

    s_last_frametime = s_this_frametime;
    s_this_frametime = SDL_GetTicks();

    handle_sdl_event(tp_sdl_window, tr_app_context);

    // start the Dear ImGui frame
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    handle_gui(tr_app_context);
    render(tr_d3d_context);

    // FPS limiter
    if (s_this_frametime - s_last_frametime < 1000 / MAX_FPS)
      SDL_Delay(1000 / MAX_FPS - s_this_frametime + s_last_frametime);
  }

  // cleanup
  tr_app_context.Reset();

  cleanup(tp_sdl_window, tr_d3d_context);
}