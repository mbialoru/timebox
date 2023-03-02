#ifndef APPCONTEXT_HPP
#define APPCONTEXT_HPP

#pragma once

#include "clockcontroller.hpp"
#include "config.hpp"
#include "serialinterface.hpp"
#include "utilities.hpp"

class AppContext
{
public:
  AppContext();
  ~AppContext() = default;

  void Reset();

  // Application variables
  bool ntp_running;
  bool using_docker;
  bool admin_privileges;

  std::size_t baud_rate;
  std::string serial_port;
  std::string main_window_name;

  std::vector<std::string> baud_rate_string_list;
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

#endif// APPCONTEXT_HPP