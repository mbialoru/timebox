#include <SDL.h>
#include <boost/log/trivial.hpp>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl.h>

#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL_opengles2.h>
#else
#include <SDL_opengl.h>
#endif

#include <fstream>

#include "clockcontroller.hpp"
#include "config.hpp"
#include "serialreader.hpp"
#include "utilities.hpp"

int main(int argc, const char *argv[])
{
  // Setup SDL
  // (Some versions of SDL before <2.0.10 appears to have performance/stalling issues on a minority of Windows systems,
  // depending on whether SDL_INIT_GAMECONTROLLER is enabled or disabled.. updating to latest version of SDL is
  // recommended!)
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0) {
    BOOST_LOG_TRIVIAL(error) << SDL_GetError();
    return EXIT_FAILURE;
  }

  // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
  // GL ES 2.0 + GLSL 100
  const char *glsl_version = "#version 100";
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#elif defined(__APPLE__)
  // GL 3.2 Core + GLSL 150
  const char *glsl_version = "#version 150";
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);// Always required on Mac
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#else
  // GL 3.0 + GLSL 130
  const char *glsl_version = "#version 130";
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#endif

  // Create window with graphics context
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
  SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI);

  std::string window_title{ PROJECT_NAME };
  window_title[0] = toupper(window_title[0]);

  SDL_Window *window = SDL_CreateWindow(
    window_title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_HEIGHT, WINDOW_WIDTH, window_flags);
  SDL_GLContext gl_context = SDL_GL_CreateContext(window);
  SDL_GL_MakeCurrent(window, gl_context);
  SDL_GL_SetSwapInterval(1);// Enable vsync

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();

  // Setup Platform/Renderer backends
  ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
  ImGui_ImplOpenGL3_Init(glsl_version);

  // Variables for application
  std::string serial_port;
  std::size_t baud_rate;

  std::unique_ptr<ClockController> p_clock_controller;
  std::unique_ptr<SerialReader> p_serial_reader;

  // Variables for ImGui
  static const Uint32 max_fps{ 20 };
  static Uint32 last_frametime, this_frametime;

  static bool admin_privileges = CheckAdminPrivileges();
  static bool ntp_running = CheckNTPService();
  static bool using_docker = CheckIfUsingDocker();

  static bool disabled_warning_popup{ false };
  static bool display_connection_dialog{ false };
  static bool connected{ false };
  static bool show_about{ false };
  static bool app_run{ true };

  std::vector<std::string> serial_port_list;
  std::vector<std::string> baud_rate_list;
  for (const auto &[key, value] : s_baud_conversion_map) { baud_rate_list.push_back(std::to_string(key)); }

  // Main loop
  while (app_run) {
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
      if (event.type == SDL_QUIT) app_run = true;
      if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE
          && event.window.windowID == SDL_GetWindowID(window))
        app_run = false;
    }

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    // Main dialog
    {
      ImGuiWindowFlags window_flags = 0;
      window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
      window_flags |= ImGuiWindowFlags_NoCollapse;
      window_flags |= ImGuiWindowFlags_NoResize;
      window_flags |= ImGuiWindowFlags_NoMove;

      ImGui::Begin(window_title.c_str(), &app_run, window_flags);
      ImGui::SetWindowPos(ImVec2(0, 0));
      ImGui::SetWindowSize(ImVec2(WINDOW_HEIGHT, WINDOW_WIDTH));

      // Main dialog content
      if (connected) {
        ImGui::Text("Connection status:");
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "connected");
        ImGui::SameLine();
        ImGui::Text(serial_port.c_str());
        ImGui::Separator();

        if (p_clock_controller->time_difference_history.size() > 0 && p_clock_controller->tick_history.size() > 0) {
          ImGui::Text("Clock difference %ld ms", p_clock_controller->time_difference_history.back());
          ImGui::Text("Kernel tick %lu (%.3f %%speed)",
            p_clock_controller->tick_history.back(),
            ((float)p_clock_controller->tick_history.back() - 10000) / 100 + 100);
          ImGui::Separator();
        }
      }

      else {
        ImGui::Text("Connection status:");
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "disconnected");
        ImGui::Separator();
      }

      if (p_clock_controller != nullptr && p_clock_controller->time_difference_history.size() > 0) {
        if (ImGui::Button("Save History")) {
          std::fstream output_file;
          output_file.open("timebox_history.log", std::ios::out);
          for (const auto entry : p_clock_controller->time_difference_history) {
            output_file << std::to_string(entry).c_str();
            output_file << "\n";
          }
          output_file.close();
        }
      }

      if (ImGui::Button("Connect")) { display_connection_dialog = true; }
      ImGui::SameLine();
      if (ImGui::Button("Quit")) { app_run = false; }

      ImGui::Dummy(ImVec2(0.0f, 20.0f));

      // Main dialog context menu
      if (ImGui::BeginPopupContextWindow()) {
        if (ImGui::MenuItem("Quit")) app_run = false;
        if (ImGui::MenuItem("About")) show_about = true;
        ImGui::EndPopup();
      }
      ImGui::End();
    }

    // Warning popup
    if ((!admin_privileges || ntp_running || using_docker) && !disabled_warning_popup) {
      ImGui::SetNextWindowBgAlpha(0.35f);
      ImGui::SetNextWindowSize(ImVec2(WINDOW_WIDTH / 2, 200));
      ImGui::SetNextWindowPos(ImVec2(WINDOW_WIDTH / 4, WINDOW_HEIGHT / 2 - 100));

      ImGuiWindowFlags window_flags = 0;
      window_flags |= ImGuiWindowFlags_NoDecoration;
      window_flags |= ImGuiWindowFlags_NoFocusOnAppearing;

      ImGui::Begin("Warning", (bool *)NULL, window_flags);
      ImGui::Text("Warning");
      ImGui::Separator();

      if (!admin_privileges) { ImGui::TextWrapped("Program is not running with administrator privileges"); }
      if (ntp_running) { ImGui::TextWrapped("NTP service is running"); }
      if (using_docker) { ImGui::TextWrapped("Application is running in docker container"); }

      if (ImGui::BeginPopupContextWindow()) {
        if (ImGui::MenuItem("Disable warning")) disabled_warning_popup = true;
        ImGui::EndPopup();
      }
      ImGui::End();
    }

    // About popup
    if (show_about) {
      ImGui::SetNextWindowBgAlpha(0.35f);
      ImGui::SetNextWindowSize(ImVec2(WINDOW_WIDTH * 0.75, 200));
      ImGui::SetNextWindowPos(ImVec2((WINDOW_WIDTH - WINDOW_WIDTH * 0.75) / 2, WINDOW_HEIGHT / 2 - 100));

      ImGuiWindowFlags window_flags = 0;
      window_flags |= ImGuiWindowFlags_NoDecoration;
      window_flags |= ImGuiWindowFlags_NoFocusOnAppearing;

      ImGui::Begin("About", (bool *)NULL, window_flags);
      ImGui::Text("About");
      ImGui::Separator();

      ImGui::TextWrapped(
        "Frametime %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
      ImGui::TextWrapped(std::string(BUILD_INFO).c_str());
      ImGui::TextWrapped("dear imgui says hello! (%s) (%d)", IMGUI_VERSION, IMGUI_VERSION_NUM);

      if (ImGui::BeginPopupContextWindow()) {
        if (ImGui::MenuItem("Close")) show_about = false;
        ImGui::EndPopup();
      }
      ImGui::End();
    }

    // Connection dialog
    if (display_connection_dialog) {
      ImGuiWindowFlags window_flags = 0;
      window_flags |= ImGuiWindowFlags_NoCollapse;

      ImGui::SetNextWindowSize(ImVec2(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2));
      ImGui::SetNextWindowPos(ImVec2(WINDOW_WIDTH / 4, WINDOW_HEIGHT / 4));

      static std::size_t current_item_index_port{ 0 };
      static std::size_t current_item_index_baud{ 0 };

      ImGui::Begin("Connect", &display_connection_dialog, window_flags);
      {
        // Baud rate choosing combo
        const char *preview_value_baud = baud_rate_list[current_item_index_baud].c_str();
        baud_rate = std::stoi(baud_rate_list[current_item_index_baud]);
        if (ImGui::BeginCombo("Baud rate", preview_value_baud)) {
          for (std::size_t i{ 0 }; i < baud_rate_list.size(); i++) {
            const bool is_selected = (current_item_index_baud == i);
            if (ImGui::Selectable(baud_rate_list[i].c_str(), is_selected)) { current_item_index_baud = i; }
            if (is_selected) {
              baud_rate = std::stoi(baud_rate_list[i]);
              ImGui::SetItemDefaultFocus();
            }
          }
          ImGui::EndCombo();
        }

        // Serial port choosing combo
        if (ImGui::Button("Scan ports")) { serial_port_list = GetSerialDevicesList(); }
        ImGui::SameLine();
        if (serial_port_list.size() > 0) {
          const char *preview_value_port = serial_port_list[current_item_index_port].c_str();
          serial_port = serial_port_list[current_item_index_port];
          if (ImGui::BeginCombo("##", preview_value_port)) {
            for (std::size_t i{ 0 }; i < serial_port_list.size(); i++) {
              const bool is_selected = (current_item_index_port == i);
              if (ImGui::Selectable(serial_port_list[i].c_str(), is_selected)) { current_item_index_port = i; }
              if (is_selected) {
                serial_port = serial_port_list[i];
                ImGui::SetItemDefaultFocus();
              }
            }
            ImGui::EndCombo();
          }
        }

        if (serial_port != "" && baud_rate != 0) {
          ImGui::Text("Port: ");
          ImGui::SameLine();
          ImGui::Text(serial_port.c_str());
          ImGui::Text("Baud: ");
          ImGui::SameLine();
          ImGui::Text(std::to_string(baud_rate).c_str());

          if (ImGui::Button("Connect")) {
            p_clock_controller = std::make_unique<ClockController>(0, 0.001);
            p_serial_reader = std::make_unique<SerialReader>(serial_port.c_str(),
              baud_rate,
              std::bind(&ClockController::AdjustClock, p_clock_controller.get(), std::placeholders::_1));
            connected = true;
          }

          if (connected) {
            if (ImGui::Button("Disconnect")) {
              p_serial_reader.reset();
              connected = false;
            }
          }
        }
      }
      ImGui::End();
    }

    // Rendering - This is our Viewport
    ImGui::Render();
    glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(window);

    // FPS limiter
    if (this_frametime - last_frametime < 1000 / max_fps) SDL_Delay(1000 / max_fps - this_frametime + last_frametime);
  }

  // Cleanup
  p_serial_reader.reset();
  p_clock_controller.reset();

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();

  SDL_GL_DeleteContext(gl_context);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return EXIT_SUCCESS;
}
