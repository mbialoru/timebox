#include <boost/log/trivial.hpp>

#include "timebox.hpp"

using namespace TimeBox;

// Initialize Contexts
static D3DContext s_d3d_context;
static AppContext s_app_context;

// main()
int wWinMain(HINSTANCE, HINSTANCE, PWSTR, INT)
{
  // Initialize application window
  initialize_sdl();

  SDL_Window* p_sdl_window{ create_sdl_window(std::string(BuildInformation::PROJECT_NAME), WINDOW_HEIGHT, WINDOW_WIDTH) };
  HWND window_handle{ win32_windows_handle(p_sdl_window) };

  // Initialize Direct3D
  if (not create_d3d_device(window_handle, s_d3d_context)) {
    destroy_d3d_device(s_d3d_context);
    BOOST_LOG_TRIVIAL(error) << "Failed to create D3D device !";

    return EXIT_FAILURE;
  }

  // Initialize Dear ImGui
  initialize_imgui(p_sdl_window, s_d3d_context);

  // Main loop
  main_loop(p_sdl_window, s_app_context, s_d3d_context);

  return EXIT_SUCCESS;
}
