#include "timebox.hpp"

using namespace TimeBox;

void TimeBox::SaveHistoryToFile(std::unique_ptr<ClockController> tp_clock_controller)
{
  if (tp_clock_controller != nullptr && !tp_clock_controller->GetDifferenceHistory().empty()) {
    std::fstream output_file;
    output_file.open("timebox_history.log", std::ios::out);
    for (const auto entry : tp_clock_controller->GetDifferenceHistory()) {
      output_file << std::to_string(entry.count()).c_str() << "\n";
    }
    output_file.close();
  }
}

void TimeBox::MainLoop(SDL_Window *tp_sdl_window, AppContext &tr_app_context, D3DContext &tr_d3d_context)
{
  static const uint32_t s_max_fps{ 20 };

  while (tr_app_context.application_run) {
    // FPS limiter
    static uint32_t s_last_frametime, s_this_frametime;
    s_last_frametime = s_this_frametime;
    s_this_frametime = SDL_GetTicks();

    HandleSDLEvent(tp_sdl_window, tr_app_context);

    // Start the Dear ImGui frame
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    HandleGUI(tr_app_context);
    Render(tr_d3d_context);

    // FPS limiter
    if (s_this_frametime - s_last_frametime < 1000 / s_max_fps)
      SDL_Delay(1000 / s_max_fps - s_this_frametime + s_last_frametime);
  }

  // Cleanup
  tr_app_context.Reset();
  Cleanup(tp_sdl_window, tr_d3d_context);
}