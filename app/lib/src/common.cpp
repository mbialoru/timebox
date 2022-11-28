#include "common.hpp"

using namespace TimeBox;

std::size_t ConvertBaudRate(const std::size_t t_baud)
{
  auto search = s_baud_conversion_map.find(static_cast<int>(t_baud));
  if (search != s_baud_conversion_map.end()) {
    return static_cast<std::size_t>(search->second);
  } else {
    throw std::invalid_argument("Invalid baud rate !");
  }
}

std::chrono::system_clock::time_point ConvertStringToTimepoint(const std::string t_time_string)
{
  std::vector<std::string> tmp;
  time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  struct tm tm = *std::localtime(&now);

  boost::split(tmp, t_time_string, boost::is_any_of(":."));

  tm.tm_hour = std::stoi(tmp[0]);
  tm.tm_min = std::stoi(tmp[1]);
  tm.tm_sec = std::stoi(tmp[2]);

  auto res = std::chrono::system_clock::from_time_t(std::mktime(&tm));
  res += std::chrono::duration<int, std::milli>(std::stoi(tmp[3]) * 10);

  return res;
}

std::string ConvertTimepointToString(const std::chrono::system_clock::time_point t_timepoint)
{
  time_t tmp = std::chrono::system_clock::to_time_t(t_timepoint);
  struct tm tm = *std::localtime(&tmp);
  std::string res{ std::to_string(tm.tm_hour) + ":" + std::to_string(tm.tm_min) + ":" + std::to_string(tm.tm_sec) };

  return res;
}