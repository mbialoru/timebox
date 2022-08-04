#ifndef UTILS_HPP
#define UTILS_HPP

#pragma once

#include <boost/algorithm/string.hpp>
#include <boost/asio.hpp>
#include <boost/log/trivial.hpp>
#include <chrono>
#include <filesystem>
#include <iostream>
#include <map>
#include <termios.h>
#include <unistd.h>

static std::map<int, int> s_baud_conversion_map{ { 0, B0 },
  { 50, B50 },
  { 75, B75 },
  { 110, B110 },
  { 134, B134 },
  { 150, B150 },
  { 200, B200 },
  { 300, B300 },
  { 600, B600 },
  { 1200, B1200 },
  { 1800, B1800 },
  { 2400, B2400 },
  { 4800, B4800 },
  { 9600, B9600 },
  { 19200, B19200 },
  { 38400, B38400 } };

bool CheckSudo();
bool CheckAdminPrivileges();
bool CheckIfUsingDocker();
bool CheckNTPService();
std::vector<std::string> GetSerialDevicesList();
void PrintTimex(timex &);
std::size_t ConvertBaudRate(int);
std::chrono::system_clock::time_point ConvertTimepointToString(std::string);
std::string ConvertStringToTimepoint(std::chrono::system_clock::time_point);

#endif// UTILS_HPP