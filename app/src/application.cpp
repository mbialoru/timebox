#include "application.hpp"

AppContext InitializeContext()
{
  AppContext app_context;

  app_context.admin_privileges = CheckAdminPrivileges();
  app_context.using_docker = CheckIfUsingDocker();
  app_context.ntp_running = CheckNTPService();

  for (const auto &[key, value] : s_baud_conversion_map) { app_context.baud_rate_list.push_back(std::to_string(key)); }

  app_context.application_run = true;
  app_context.disabled_warning_popup = false;
  app_context.connection_established = false;

  app_context.display_about_dialog = false;
  app_context.display_connect_dialog = false;

  return app_context;
}

void MainDialog(AppContext &context)
{

  std::string window_title{ PROJECT_NAME };
  window_title[0] = toupper(window_title[0]);

  ImGuiWindowFlags window_flags = 0;
  window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
  window_flags |= ImGuiWindowFlags_NoCollapse;
  window_flags |= ImGuiWindowFlags_NoResize;
  window_flags |= ImGuiWindowFlags_NoMove;

  ImGui::Begin(window_title.c_str(), &context.application_run, window_flags);
  ImGui::SetWindowPos(ImVec2(0, 0));
  ImGui::SetWindowSize(ImVec2(WINDOW_HEIGHT, WINDOW_WIDTH));

  // Main dialog content
  if (context.connection_established) {
    ImGui::Text("Connection status:");
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "connected");
    ImGui::SameLine();
    ImGui::Text(context.serial_port.c_str());
    ImGui::Separator();

    if (context.p_clock_controller->time_difference_history.size() > 0
        && context.p_clock_controller->tick_history.size() > 0) {
      ImGui::Text("Clock difference %ld ms", context.p_clock_controller->time_difference_history.back());
      ImGui::Text("Kernel tick %lu (%.3f %%speed)",
        context.p_clock_controller->tick_history.back(),
        ((float)context.p_clock_controller->tick_history.back() - 10000) / 100 + 100);
      ImGui::Separator();
    }
  }

  else {
    ImGui::Text("Connection status:");
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "disconnected");
    ImGui::Separator();
  }

  if (context.p_clock_controller != nullptr && context.p_clock_controller->time_difference_history.size() > 0) {
    if (ImGui::Button("Save History")) {
      std::fstream output_file;
      output_file.open("timebox_history.log", std::ios::out);
      for (const auto entry : context.p_clock_controller->time_difference_history) {
        output_file << std::to_string(entry).c_str();
        output_file << "\n";
      }
      output_file.close();
    }
  }

  if (ImGui::Button("Connect")) { context.display_connect_dialog = true; }
  ImGui::SameLine();
  if (ImGui::Button("Quit")) { context.application_run = false; }

  ImGui::Dummy(ImVec2(0.0f, 20.0f));

  // Main dialog context menu
  if (ImGui::BeginPopupContextWindow()) {
    if (ImGui::MenuItem("Quit")) context.application_run = false;
    if (ImGui::MenuItem("About")) context.display_about_dialog = true;
    ImGui::EndPopup();
  }
  ImGui::End();
}

void ConnectDialog(AppContext &context)
{
  if (context.display_connect_dialog) {
    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoCollapse;

    ImGui::SetNextWindowSize(ImVec2(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2));
    ImGui::SetNextWindowPos(ImVec2(WINDOW_WIDTH / 4, WINDOW_HEIGHT / 4));

    static std::size_t current_item_index_port{ 0 };
    static std::size_t current_item_index_baud{ 0 };

    ImGui::Begin("Connect", &context.display_connect_dialog, window_flags);
    {
      // Baud rate choosing combo
      const char *preview_value_baud = context.baud_rate_list[current_item_index_baud].c_str();
      context.baud_rate = std::stoi(context.baud_rate_list[current_item_index_baud]);
      if (ImGui::BeginCombo("Baud rate", preview_value_baud)) {
        for (std::size_t i{ 0 }; i < context.baud_rate_list.size(); i++) {
          const bool is_selected = (current_item_index_baud == i);
          if (ImGui::Selectable(context.baud_rate_list[i].c_str(), is_selected)) { current_item_index_baud = i; }
          if (is_selected) {
            context.baud_rate = std::stoi(context.baud_rate_list[i]);
            ImGui::SetItemDefaultFocus();
          }
        }
        ImGui::EndCombo();
      }

      // Serial port choosing combo
      if (ImGui::Button("Scan ports")) { context.serial_port_list = GetSerialDevicesList(); }
      ImGui::SameLine();
      if (context.serial_port_list.size() > 0) {
        const char *preview_value_port = context.serial_port_list[current_item_index_port].c_str();
        context.serial_port = context.serial_port_list[current_item_index_port];
        if (ImGui::BeginCombo("##", preview_value_port)) {
          for (std::size_t i{ 0 }; i < context.serial_port_list.size(); i++) {
            const bool is_selected = (current_item_index_port == i);
            if (ImGui::Selectable(context.serial_port_list[i].c_str(), is_selected)) { current_item_index_port = i; }
            if (is_selected) {
              context.serial_port = context.serial_port_list[i];
              ImGui::SetItemDefaultFocus();
            }
          }
          ImGui::EndCombo();
        }
      }

      if (context.serial_port != "" && context.baud_rate != 0) {
        ImGui::Text("Port: ");
        ImGui::SameLine();
        ImGui::Text(context.serial_port.c_str());
        ImGui::Text("Baud: ");
        ImGui::SameLine();
        ImGui::Text(std::to_string(context.baud_rate).c_str());

        if (ImGui::Button("Connect")) {
          context.p_clock_controller = std::make_unique<ClockController>(0, 0.001);
          context.p_serial_reader = std::make_unique<SerialReader>(context.serial_port.c_str(),
            context.baud_rate,
            std::bind(&ClockController::AdjustClock, context.p_clock_controller.get(), std::placeholders::_1));
          context.connection_established = true;
        }

        if (context.connection_established) {
          if (ImGui::Button("Disconnect")) {
            context.p_serial_reader.reset();
            context.connection_established = false;
          }
        }
      }
    }
    ImGui::End();
  }
}