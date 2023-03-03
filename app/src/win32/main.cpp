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
  InitializeSDL();
  SDL_Window *p_sdl_window{ CreateSDLWindow(std::string(BuildInformation::PROJECT_NAME), WINDOW_HEIGHT, WINDOW_WIDTH) };
  HWND window_handle{ Win32WindowHandle(p_sdl_window) };

  // Initialize Direct3D
  if (not CreateDeviceD3D(window_handle, s_d3d_context)) {
    DestroyDeviceD3D(s_d3d_context);
    BOOST_LOG_TRIVIAL(error) << "Failed to create D3D device !";
    return EXIT_FAILURE;
  }

  // Initialize Dear ImGui
  InitializeImGUI(p_sdl_window, s_d3d_context);

  // Main loop
  MainLoop(p_sdl_window, s_app_context, s_d3d_context);

  return EXIT_SUCCESS;
}
