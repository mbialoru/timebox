#ifndef COMMON_HPP
#define COMMON_HPP

#pragma once

#include <boost/algorithm/string.hpp>
#include <boost/log/trivial.hpp>
#include <chrono>
#include <map>

#include "exceptions.hpp"

namespace TimeBox {

extern std::map<int, int> s_baud_conversion_map;
typedef std::pair<std::string, std::chrono::system_clock::time_point> TimeboxReadout;

bool CheckAdminPrivileges();
// TODO: These functions just ask to be wrapped into a class
bool CheckNTPService();
void PauseNTPService();
void StartNTPService();
std::vector<std::string> GetSerialDevicesList();
std::size_t ConvertBaudRate(std::size_t);
std::chrono::system_clock::time_point ConvertStringToTimepoint(std::string);
std::string ConvertTimepointToString(std::chrono::system_clock::time_point);

}// namespace TimeBox

#endif// COMMON_HPP