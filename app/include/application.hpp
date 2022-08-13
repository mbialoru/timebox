#include <imgui.h>

#include "clockcontroller.hpp"
#include "config.hpp"
#include "serialreader.hpp"
#include "utilities.hpp"

struct AppContext
{
  // Application variables
  static bool ntp_running;
  static bool using_docker;
  static bool admin_privileges;

  std::vector<std::string> baud_rate_list;
  std::vector<std::string> serial_port_list;

  std::size_t baud_rate;
  std::string serial_port;

  std::unique_ptr<ClockController> p_clock_controller;
  std::unique_ptr<SerialReader> p_serial_reader;

  // ImGUI variables
  static bool application_run;
  static bool disabled_warning_popup;
  static bool connection_established;

  static bool display_about_dialog;
  static bool display_connect_dialog;
};

AppContext InitializeContext();

void MainDialog(AppContext &);
void ConnectDialog(AppContext &);
void InformationPopup(AppContext &);