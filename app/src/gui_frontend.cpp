#include "gui_frontend.hpp"

using namespace TimeBox;

void TimeBox::CenterWindow(std::size_t t_height, std::size_t t_width)
{
  if (t_height > WINDOW_HEIGHT || t_width > WINDOW_WIDTH) {
    BOOST_LOG_TRIVIAL(warning) << "Window size too big, shrinking.";
    if (t_height > WINDOW_HEIGHT) { t_height = WINDOW_HEIGHT; }
    if (t_width > WINDOW_WIDTH) { t_width = WINDOW_WIDTH; }
  }

  ImGui::SetNextWindowPos(ImVec2((WINDOW_HEIGHT - t_height) / 2, (WINDOW_WIDTH - t_width) / 2));
}

void TimeBox::HandleGUI(AppContext &tr_app_context)
{
  MainDialog(tr_app_context);
  ConnectDialog(tr_app_context);
  WarningPopup(tr_app_context);
  AboutDialog(tr_app_context);
}

void TimeBox::MainDialog(AppContext &tr_context)
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

  ImGui::Begin(window_title.c_str(), &tr_context.application_run, window_flags);
  ImGui::SetWindowPos(ImVec2(0, 0));
  ImGui::SetWindowSize(ImVec2(WINDOW_HEIGHT, WINDOW_WIDTH));

  // Main dialog content
  if (tr_context.connection_established) {
    ImGui::Text("Connection status:");
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "connected");
    ImGui::SameLine();
    ImGui::Text(tr_context.serial_port.c_str());
    ImGui::Separator();

    // TODO: Bring out time difference and adjustment history
    if (!tr_context.p_clock_controller->get_difference_history().empty()
        && !tr_context.p_clock_controller->get_adjustment_history().empty()) {
      ImGui::Text("Clock difference %ld ms", tr_context.p_clock_controller->get_difference_history().back());
      ImGui::Text("Clock adjustment %lu (%.3f %%speed)",
        tr_context.p_clock_controller->get_adjustment_history().back(),
        (static_cast<double>(tr_context.p_clock_controller->get_adjustment_history().back())
          - tr_context.p_clock_controller->get_initial_adjustment())
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
  if (ImGui::Button("Apply")) { tr_context.p_pid->set_terms(term_p, term_i, term_d); }
  ImGui::SameLine();
  if (ImGui::Button("Reset")) {
    term_p = 0.0;
    term_i = 0.0;
    term_d = 0.0;
    tr_context.p_pid->set_terms(term_p, term_i, term_d);
  }
  ImGui::SameLine();
  if (ImGui::Button("Auto")) { throw NotImplementedException(); }
  ImGui::SameLine();
  ImGui::Spacing();
  ImGui::SameLine();
  auto [p, i, d] = tr_context.p_pid->get_terms();
  ImGui::Text("P: %.3f I: %.3f D: %.3f", p, i, d);
  ImGui::Separator();

  // TODO: Fix history saving to file and implement autosave every X steps
  // if (t_context.p_clock_controller != nullptr && !t_context.p_clock_controller->get_difference_history().empty()) {
  //   if (ImGui::Button("Save History")) {
  //     std::fstream output_file;
  //     output_file.open("timebox_history.log", std::ios::out);
  //     for (const auto entry : t_context.p_clock_controller->get_difference_history()) {
  //       output_file << std::to_string(entry.count()).c_str();
  //       output_file << "\n";
  //     }
  //     output_file.close();
  //   }
  // }

  if (ImGui::Button("Connect")) { tr_context.display_connect_dialog = true; }
  ImGui::SameLine();
  if (ImGui::Button("Quit")) { tr_context.application_run = false; }

  // Main dialog context menu
  if (ImGui::BeginPopupContextWindow()) {
    if (ImGui::MenuItem("Quit")) tr_context.application_run = false;
    if (ImGui::MenuItem("About")) tr_context.display_about_dialog = true;
    ImGui::EndPopup();
  }
  ImGui::End();
}

void TimeBox::ConnectDialog(AppContext &tr_context)
{
  if (tr_context.display_connect_dialog) {
    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoCollapse;

    ImGui::SetNextWindowSize(ImVec2(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2));
    CenterWindow(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);

    ImGui::Begin("Connect", &tr_context.display_connect_dialog, window_flags);
    {
      // Baud rate choosing combo
      static std::size_t current_item_index_baud{ 0 };
      const char *preview_value_baud = tr_context.baud_rate_string_list[current_item_index_baud].c_str();
      tr_context.baud_rate = std::stoul(tr_context.baud_rate_string_list[current_item_index_baud]);
      if (ImGui::BeginCombo("Baud rate", preview_value_baud)) {
        for (std::size_t i{ 0 }; i < tr_context.baud_rate_string_list.size(); ++i) {
          const bool is_selected = (current_item_index_baud == i);
          if (ImGui::Selectable(tr_context.baud_rate_string_list[i].c_str(), is_selected)) {
            current_item_index_baud = i;
          }
          if (is_selected) {
            tr_context.baud_rate = std::stoul(tr_context.baud_rate_string_list[i]);
            ImGui::SetItemDefaultFocus();
          }
        }
        ImGui::EndCombo();
      }

      // Serial port choosing combo
      if (ImGui::Button("Scan ports")) { tr_context.serial_port_list = get_serial_devices_list(); }
      ImGui::SameLine();
      if (!tr_context.serial_port_list.empty()) {
        static std::size_t current_item_index_port{ 0 };
        const char *preview_value_port = tr_context.serial_port_list[current_item_index_port].c_str();
        tr_context.serial_port = tr_context.serial_port_list[current_item_index_port];
        if (ImGui::BeginCombo("##", preview_value_port)) {
          for (std::size_t i{ 0 }; i < tr_context.serial_port_list.size(); ++i) {
            const bool is_selected = (current_item_index_port == i);
            if (ImGui::Selectable(tr_context.serial_port_list[i].c_str(), is_selected)) { current_item_index_port = i; }
            if (is_selected) {
              tr_context.serial_port = tr_context.serial_port_list[i];
              ImGui::SetItemDefaultFocus();
            }
          }
          ImGui::EndCombo();
        }
      }

      if (not tr_context.serial_port.empty() && tr_context.baud_rate != 0) {
        ImGui::Text("%s", "Port: ");
        ImGui::SameLine();
        ImGui::Text("%s", tr_context.serial_port.c_str());
        ImGui::Text("Baud: ");
        ImGui::SameLine();
        ImGui::Text("%s", std::to_string(tr_context.baud_rate).c_str());

        if (ImGui::Button("Connect")) {
          tr_context.p_clock_controller = std::make_unique<ClockController>(0, tr_context.p_pid, 0.001);

          // Limit PID settings +/- 10% speed
          auto initial_adjustment{ tr_context.p_clock_controller->get_initial_adjustment() };
          tr_context.p_pid->set_limits(initial_adjustment * 0.9, initial_adjustment * 1.1);

          tr_context.p_serial_reader = std::make_unique<SerialInterface>(
            std::bind(&ClockController::adjust_clock, tr_context.p_clock_controller.get(), std::placeholders::_1));

          try {
            tr_context.p_serial_reader->open(tr_context.serial_port, tr_context.baud_rate);
          } catch (const std::exception &e) {
            BOOST_LOG_TRIVIAL(error) << e.what();
            throw e;
          }
          tr_context.connection_established = true;
        }

        if (tr_context.connection_established) {
          if (ImGui::Button("Disconnect")) {
            if (tr_context.p_serial_reader->is_open()) {
              tr_context.p_serial_reader->flush_io_buffers();
              tr_context.p_serial_reader.reset();
            }
            tr_context.connection_established = false;
          }
        }
      }
    }
    ImGui::End();
  }
}

void TimeBox::WarningPopup(AppContext &tr_context)
{
  if ((!tr_context.admin_privileges || tr_context.ntp_running || tr_context.using_docker)
      && !tr_context.disabled_warning_popup) {

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

    if (!tr_context.admin_privileges) { ImGui::TextWrapped("Program is not running with administrator privileges"); }
    if (tr_context.ntp_running) { ImGui::TextWrapped("NTP service is running"); }
    if (tr_context.using_docker) { ImGui::TextWrapped("Application is running in docker container"); }

    if (ImGui::BeginPopupContextWindow()) {
      if (ImGui::MenuItem("Disable warning")) { tr_context.disabled_warning_popup = true; }
      ImGui::EndPopup();
    }
    ImGui::End();
  }
}

void TimeBox::AboutDialog(AppContext &tr_context)
{
  if (tr_context.display_about_dialog) {
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
    ImGui::TextWrapped("Project: %s", std::string(BuildInformation().PROJECT_NAME).c_str());
    ImGui::TextWrapped("Version: %s %s",
      std::string(BuildInformation().PROJECT_VERSION).c_str(),
      std::string(BuildInformation().PROJECT_VERSION_ADDENDUM).c_str());
    ImGui::TextWrapped("Branch: %s", std::string(BuildInformation().GIT_BRANCH).c_str());
    ImGui::TextWrapped("Commit: %s", std::string(BuildInformation().GIT_SHORT_SHA).c_str());
    ImGui::TextWrapped("Compiler: %s %s",
      std::string(BuildInformation().COMPILER).c_str(),
      std::string(BuildInformation().COMPILER_VERSION).c_str());
    ImGui::TextWrapped("Platform: %s", std::string(BuildInformation().PLATFORM).c_str());
    ImGui::TextWrapped("Build: %s %s",
      std::string(BuildInformation().BUILD_TYPE).c_str(),
      std::string(BuildInformation().BUILD_DATE).c_str());
    ImGui::TextWrapped("dear imgui says hello! (%s) (%d)", IMGUI_VERSION, IMGUI_VERSION_NUM);

    if (ImGui::BeginPopupContextWindow()) {
      if (ImGui::MenuItem("Close")) { tr_context.display_about_dialog = false; }
      ImGui::EndPopup();
    }
    ImGui::End();
  }
}