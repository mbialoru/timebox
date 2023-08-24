#include "appcontext.hpp"

using namespace TimeBox;

AppContext::AppContext()
{
  admin_privileges = check_admin_privileges();
  application_run = true;
  baud_rate = 0;
  connection_established = false;
  disabled_warning_popup = false;
  display_about_dialog = false;
  display_connect_dialog = false;
  main_window_name = std::string(BuildInformation().PROJECT_NAME);
  ntp_running = check_ntp_status();
  using_docker = check_if_using_docker();

  for (const auto &baud_rate : baud_rate_list) { baud_rate_string_list.push_back(std::to_string(baud_rate)); }

  std::make_shared<PID<double>>(0.0, 0.0, 0.0, 0.0).swap(p_pid);
};

void AppContext::Reset()
{
  p_serial_reader.reset();
  p_clock_controller.reset();
}