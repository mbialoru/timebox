#include "application.hpp"

using namespace TimeBox;

AppContext TimeBox::InitializeContext()
{
  AppContext context;

  context.admin_privileges = CheckAdminPrivileges();
  context.using_docker = CheckIfUsingDocker();
  context.ntp_running = CheckNTPService();

  context.baud_rate = 0;
  for (const auto &baud_rate : baud_rate_list) { context.baud_rate_string_list.push_back(std::to_string(baud_rate)); }

  context.application_run = true;
  context.disabled_warning_popup = false;
  context.connection_established = false;

  context.display_about_dialog = false;
  context.display_connect_dialog = false;

  context.main_window_name = std::string(BuildInformation().PROJECT_NAME);

  std::make_shared<PID<double>>(0.0, 0.0, 0.0, 0.0).swap(context.p_pid);

  return context;
}

bool TimeBox::CreateDeviceD3D(HWND t_hwnd, D3DContext t_context)
{
  // Setup swap chain
  DXGI_SWAP_CHAIN_DESC sd;
  ZeroMemory(&sd, sizeof(sd));
  sd.BufferCount = 2;
  sd.BufferDesc.Width = 0;
  sd.BufferDesc.Height = 0;
  sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
  sd.BufferDesc.RefreshRate.Numerator = 60;
  sd.BufferDesc.RefreshRate.Denominator = 1;
  sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
  sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
  sd.OutputWindow = t_hwnd;
  sd.SampleDesc.Count = 1;
  sd.SampleDesc.Quality = 0;
  sd.Windowed = TRUE;
  sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

  UINT createDeviceFlags = 0;
  // createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
  D3D_FEATURE_LEVEL featureLevel;
  const D3D_FEATURE_LEVEL featureLevelArray[2] = {
    D3D_FEATURE_LEVEL_11_0,
    D3D_FEATURE_LEVEL_10_0,
  };
  if (D3D11CreateDeviceAndSwapChain(NULL,
        D3D_DRIVER_TYPE_HARDWARE,
        NULL,
        createDeviceFlags,
        featureLevelArray,
        2,
        D3D11_SDK_VERSION,
        &sd,
        &t_context.p_swap_chain,
        &t_context.p_d3d_device,
        &featureLevel,
        &t_context.p_d3d_device_context)
      != S_OK)
    return false;

  CreateRenderTarget(t_context);
  return true;
}

void TimeBox::DestroyDeviceD3D(D3DContext t_context)
{
  DestroyRenderTarget(t_context);
  if (t_context.p_swap_chain) {
    t_context.p_swap_chain->Release();
    t_context.p_swap_chain = NULL;
  }
  if (t_context.p_d3d_device_context) {
    t_context.p_d3d_device_context->Release();
    t_context.p_d3d_device_context = NULL;
  }
  if (t_context.p_d3d_device) {
    t_context.p_d3d_device->Release();
    t_context.p_d3d_device = NULL;
  }
}

void TimeBox::CreateRenderTarget(D3DContext t_context)
{
  ID3D11Texture2D *pBackBuffer;
  t_context.p_swap_chain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
  t_context.p_d3d_device->CreateRenderTargetView(pBackBuffer, NULL, &t_context.p_render_target_view);
  pBackBuffer->Release();
}

void TimeBox::DestroyRenderTarget(D3DContext t_context)
{
  if (t_context.p_render_target_view) {
    t_context.p_render_target_view->Release();
    t_context.p_render_target_view = NULL;
  }
}

void TimeBox::DestroyContext(AppContext &t_context)
{
  t_context.p_serial_reader.reset();
  t_context.p_clock_controller.reset();
}

void TimeBox::CenterWindow(std::size_t t_height, std::size_t t_width)
{
  if (t_height > WINDOW_HEIGHT || t_width > WINDOW_WIDTH) {
    BOOST_LOG_TRIVIAL(warning) << "Window size too big, shrinking.";
    if (t_height > WINDOW_HEIGHT) { t_height = WINDOW_HEIGHT; }
    if (t_width > WINDOW_WIDTH) { t_width = WINDOW_WIDTH; }
  }

  ImGui::SetNextWindowPos(ImVec2((WINDOW_HEIGHT - t_height) / 2, (WINDOW_WIDTH - t_width) / 2));
}

void TimeBox::MainDialog(AppContext &t_context)
{
  std::string window_title{ BuildInformation().PROJECT_NAME };
  window_title[0] = toupper(window_title[0]);

  static float term_p{ 0.0 };
  static float term_i{ 0.0 };
  static float term_d{ 0.0 };

  ImGuiWindowFlags window_flags = 0;
  window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
  window_flags |= ImGuiWindowFlags_NoCollapse;
  window_flags |= ImGuiWindowFlags_NoResize;
  window_flags |= ImGuiWindowFlags_NoMove;

  ImGui::Begin(window_title.c_str(), &t_context.application_run, window_flags);
  ImGui::SetWindowPos(ImVec2(0, 0));
  ImGui::SetWindowSize(ImVec2(WINDOW_HEIGHT, WINDOW_WIDTH));

  // Main dialog content
  if (t_context.connection_established) {
    ImGui::Text("Connection status:");
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "connected");
    ImGui::SameLine();
    ImGui::Text(t_context.serial_port.c_str());
    ImGui::Separator();

    // TODO: Bring out time difference and adjustment history
    if (!t_context.p_clock_controller->GetDifferenceHistory().empty()
        && !t_context.p_clock_controller->GetAdjustmentHistory().empty()) {
      ImGui::Text("Clock difference %ld ms", t_context.p_clock_controller->GetDifferenceHistory().back());
      ImGui::Text("Clock adjustment %lu (%.3f %%speed)",
        t_context.p_clock_controller->GetAdjustmentHistory().back(),
        (static_cast<double>(t_context.p_clock_controller->GetAdjustmentHistory().back())
          - t_context.p_clock_controller->GetInitialAdjustment())
            / 100
          + 100);
      ImGui::Separator();
    }
  }

  else {
    ImGui::Text("Connection status:");
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "disconnected");
    ImGui::Separator();
  }

  // PID terms setting panel
  ImGuiSliderFlags flags = ImGuiSliderFlags_AlwaysClamp;
  ImGui::Text("PID Control");
  ImGui::SliderFloat("P term", &term_p, -10.0f, 10.0f, "%.3f", flags);
  ImGui::SliderFloat("I term", &term_i, -10.0f, 10.0f, "%.3f", flags);
  ImGui::SliderFloat("D term", &term_d, -10.0f, 10.0f, "%.3f", flags);
  if (ImGui::Button("Apply")) { t_context.p_pid->SetTerms(term_p, term_i, term_d); }
  ImGui::SameLine();
  if (ImGui::Button("Reset")) {
    term_p = 0.0;
    term_i = 0.0;
    term_d = 0.0;
    t_context.p_pid->SetTerms(term_p, term_i, term_d);
  }
  ImGui::SameLine();
  if (ImGui::Button("Auto")) { throw NotImplementedException(); }
  ImGui::SameLine();
  ImGui::Spacing();
  ImGui::SameLine();
  auto [p, i, d] = t_context.p_pid->GetTerms();
  ImGui::Text("P: %.3f I: %.3f D: %.3f", p, i, d);
  ImGui::Separator();

  // TODO: Fix history saving to file and implement autosave every X steps
  if (t_context.p_clock_controller != nullptr && !t_context.p_clock_controller->GetDifferenceHistory().empty()) {
    if (ImGui::Button("Save History")) {
      std::fstream output_file;
      output_file.open("timebox_history.log", std::ios::out);
      for (const auto entry : t_context.p_clock_controller->GetDifferenceHistory()) {
        output_file << std::to_string(entry.count()).c_str();
        output_file << "\n";
      }
      output_file.close();
    }
  }

  if (ImGui::Button("Connect")) { t_context.display_connect_dialog = true; }
  ImGui::SameLine();
  if (ImGui::Button("Quit")) { t_context.application_run = false; }

  // Main dialog context menu
  if (ImGui::BeginPopupContextWindow()) {
    if (ImGui::MenuItem("Quit")) t_context.application_run = false;
    if (ImGui::MenuItem("About")) t_context.display_about_dialog = true;
    ImGui::EndPopup();
  }
  ImGui::End();
}

void TimeBox::ConnectDialog(AppContext &t_context)
{
  if (t_context.display_connect_dialog) {
    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoCollapse;

    ImGui::SetNextWindowSize(ImVec2(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2));
    CenterWindow(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);

    static std::size_t current_item_index_port{ 0 };
    static std::size_t current_item_index_baud{ 0 };

    ImGui::Begin("Connect", &t_context.display_connect_dialog, window_flags);
    {
      // Baud rate choosing combo
      const char *preview_value_baud = t_context.baud_rate_string_list[current_item_index_baud].c_str();
      t_context.baud_rate = std::stoul(t_context.baud_rate_string_list[current_item_index_baud]);
      if (ImGui::BeginCombo("Baud rate", preview_value_baud)) {
        for (std::size_t i{ 0 }; i < t_context.baud_rate_string_list.size(); ++i) {
          const bool is_selected = (current_item_index_baud == i);
          if (ImGui::Selectable(t_context.baud_rate_string_list[i].c_str(), is_selected)) {
            current_item_index_baud = i;
          }
          if (is_selected) {
            t_context.baud_rate = std::stoul(t_context.baud_rate_string_list[i]);
            ImGui::SetItemDefaultFocus();
          }
        }
        ImGui::EndCombo();
      }

      // Serial port choosing combo
      if (ImGui::Button("Scan ports")) { t_context.serial_port_list = GetSerialDevicesList(); }
      ImGui::SameLine();
      if (!t_context.serial_port_list.empty()) {
        const char *preview_value_port = t_context.serial_port_list[current_item_index_port].c_str();
        t_context.serial_port = t_context.serial_port_list[current_item_index_port];
        if (ImGui::BeginCombo("##", preview_value_port)) {
          for (std::size_t i{ 0 }; i < t_context.serial_port_list.size(); ++i) {
            const bool is_selected = (current_item_index_port == i);
            if (ImGui::Selectable(t_context.serial_port_list[i].c_str(), is_selected)) { current_item_index_port = i; }
            if (is_selected) {
              t_context.serial_port = t_context.serial_port_list[i];
              ImGui::SetItemDefaultFocus();
            }
          }
          ImGui::EndCombo();
        }
      }

      if (t_context.serial_port != "" && t_context.baud_rate != 0) {
        ImGui::Text("Port: ");
        ImGui::SameLine();
        ImGui::Text(t_context.serial_port.c_str());
        ImGui::Text("Baud: ");
        ImGui::SameLine();
        ImGui::Text(std::to_string(t_context.baud_rate).c_str());

        if (ImGui::Button("Connect")) {
          t_context.p_clock_controller = std::make_unique<ClockController>(0, t_context.p_pid, 0.001);
          t_context.p_serial_reader = std::make_unique<SerialInterface>(
            std::bind(&ClockController::AdjustClock, t_context.p_clock_controller.get(), std::placeholders::_1));

          try {
            t_context.p_serial_reader->Open(t_context.serial_port, t_context.baud_rate);
          } catch (const std::exception &e) {
            BOOST_LOG_TRIVIAL(error) << e.what();
            throw e;
          }

          t_context.connection_established = true;
        }

        if (t_context.connection_established) {
          if (ImGui::Button("Disconnect")) {
            t_context.p_serial_reader.reset();
            t_context.connection_established = false;
          }
        }
      }
    }
    ImGui::End();
  }
}

void TimeBox::WarningPopup(AppContext &t_context)
{
  if ((!t_context.admin_privileges || t_context.ntp_running || t_context.using_docker)
      && !t_context.disabled_warning_popup) {

    ImGuiWindowFlags window_flags{ 0 };
    window_flags |= ImGuiWindowFlags_NoDecoration;
    window_flags |= ImGuiWindowFlags_NoFocusOnAppearing;
    window_flags |= ImGuiWindowFlags_NoMove;

    ImGui::SetNextWindowBgAlpha(0.35f);
    ImGui::Begin("Warning", static_cast<bool *>(nullptr), window_flags);
    ImGui::SetWindowSize(ImVec2(WINDOW_WIDTH / 2, 200));
    ImGui::SetWindowPos(ImVec2(WINDOW_WIDTH / 4, WINDOW_HEIGHT / 2 - 100));

    ImGui::Text("Warning");
    ImGui::Separator();

    if (!t_context.admin_privileges) { ImGui::TextWrapped("Program is not running with administrator privileges"); }
    if (t_context.ntp_running) { ImGui::TextWrapped("NTP service is running"); }
    if (t_context.using_docker) { ImGui::TextWrapped("Application is running in docker container"); }

    if (ImGui::BeginPopupContextWindow()) {
      if (ImGui::MenuItem("Disable warning")) { t_context.disabled_warning_popup = true; }
      ImGui::EndPopup();
    }
    ImGui::End();
  }
}

void TimeBox::AboutDialog(AppContext &t_context)
{
  if (t_context.display_about_dialog) {
    ImGui::SetNextWindowBgAlpha(0.35f);
    ImGui::SetNextWindowSize(ImVec2(WINDOW_WIDTH * 0.75, 200));
    ImGui::SetNextWindowPos(ImVec2((WINDOW_WIDTH - WINDOW_WIDTH * 0.75) / 2, WINDOW_HEIGHT / 2 - 100));

    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoDecoration;
    window_flags |= ImGuiWindowFlags_NoFocusOnAppearing;

    ImGui::Begin("About", static_cast<bool *>(nullptr), window_flags);
    ImGui::Text("About");
    ImGui::Separator();

    ImGui::TextWrapped("Frametime %.3f ms/frame (%.1f FPS)",
      static_cast<double>(1000.0f / ImGui::GetIO().Framerate),
      static_cast<double>(ImGui::GetIO().Framerate));
    ImGui::TextWrapped(std::string(BuildInformation().PROJECT_NAME).c_str());
    ImGui::TextWrapped("Version: ",
      std::string(BuildInformation().PROJECT_VERSION).c_str(),
      " ",
      std::string(BuildInformation().PROJECT_VERSION_ADDENDUM).c_str());
    ImGui::TextWrapped("Branch: ", std::string(BuildInformation().GIT_BRANCH).c_str());
    ImGui::TextWrapped("Commit: ", std::string(BuildInformation().GIT_SHORT_SHA).c_str());
    ImGui::TextWrapped("Compiler: ",
      std::string(BuildInformation().COMPILER).c_str(),
      " ",
      std::string(BuildInformation().COMPILER_VERSION).c_str());
    ImGui::TextWrapped("Platform: ", std::string(BuildInformation().PLATFORM).c_str());
    ImGui::TextWrapped("Build: ",
      std::string(BuildInformation().BUILD_TYPE).c_str(),
      " ",
      std::string(BuildInformation().BUILD_DATE).c_str());
    ImGui::TextWrapped("Build Hash: ", std::string(BuildInformation().BUILD_HASH).c_str());
    ImGui::TextWrapped("dear imgui says hello! (%s) (%d)", IMGUI_VERSION, IMGUI_VERSION_NUM);

    if (ImGui::BeginPopupContextWindow()) {
      if (ImGui::MenuItem("Close")) { t_context.display_about_dialog = false; }
      ImGui::EndPopup();
    }
    ImGui::End();
  }
}