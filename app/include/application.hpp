#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#pragma once

#include <imgui.h>

#include "clockcontroller.hpp"
#include "config.hpp"
#include "serialreader.hpp"
#include "utilities.hpp"

namespace TimeBox {

struct AppContext
{
  // Application variables
  bool ntp_running;
  bool using_docker;
  bool admin_privileges;

  std::size_t baud_rate;
  std::string serial_port;
  std::string main_window_name;

  std::vector<std::string> baud_rate_list;
  std::vector<std::string> serial_port_list;

  std::unique_ptr<TimeBox::ClockController> p_clock_controller;
  std::unique_ptr<TimeBox::SerialInterface> p_serial_reader;
  std::shared_ptr<TimeBox::PID<double>> p_pid;

  // ImGUI variables
  bool application_run;
  bool disabled_warning_popup;
  bool connection_established;

  bool display_about_dialog;
  bool display_connect_dialog;
};

AppContext InitializeContext();
void DestroyContext(AppContext &);
void SaveHistoryToFile(std::unique_ptr<TimeBox::ClockController>);
void CenterWindow(std::size_t, std::size_t);

// Dialog windows
void MainDialog(AppContext &);
void ConnectDialog(AppContext &);
void AboutDialog(AppContext &);

// Popups
void WarningPopup(AppContext &);

}// namespace TimeBox

#endif// APPLICATION_HPP