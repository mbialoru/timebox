#ifndef UTILITIES_COMMON_HPP
#define UTILITIES_COMMON_HPP

#pragma once

#include <boost/algorithm/string.hpp>
#include <boost/log/trivial.hpp>
#include <chrono>
#include <map>
#include <regex>

#include "exceptions.hpp"

namespace TimeBox {

extern std::map<int, int> baud_conversion_map;
typedef std::pair<std::string, std::chrono::system_clock::time_point> TimeboxReadout;
const std::string correct_serial_readout_regex{ "^[0-9]{1,2}\\:[0-9]{1,2}\\:[0-9]{1,2}\\.[0-9]{1,2}\\n?$" };

bool CheckAdminPrivileges();
// TODO: These functions just ask to be wrapped into a class
bool CheckNTPService();
void PauseNTPService();
void StartNTPService();
std::vector<std::string> GetSerialDevicesList();
std::size_t ConvertBaudRate(std::size_t);
std::chrono::system_clock::time_point ConvertStringToTimepoint(std::string);
std::string ConvertTimepointToString(std::chrono::system_clock::time_point);

template<typename T> struct TimingDecorator;
template<typename... Args> struct TimingDecorator<void(Args...)>
{
  explicit TimingDecorator(std::function<void(Args...)> function) : m_function{ function } {}

  long int operator()(Args... args)
  {
    auto pre_call = std::chrono::system_clock::now();
    m_function(args...);
    auto post_call = std::chrono::system_clock::now();
    std::chrono::duration time_diff = std::chrono::duration_cast<std::chrono::milliseconds>(post_call - pre_call);
    return time_diff.count();
  }

  std::function<void(Args...)> m_function;
};

template<typename T, typename... Args> struct TimingDecorator<T(Args...)>
{
  explicit TimingDecorator(std::function<T(Args...)> function) : m_function{ function } {}

  std::pair<T, long int> operator()(Args... args)
  {
    auto pre_call = std::chrono::system_clock::now();
    T result = m_function(args...);
    auto post_call = std::chrono::system_clock::now();
    std::chrono::duration time_diff = std::chrono::duration_cast<std::chrono::milliseconds>(post_call - pre_call);
    return std::make_pair<T, long int>(std::move(result), time_diff.count());
  }

  std::function<T(Args...)> m_function;
};

template<typename... Args> auto WrapTimingDecorator(void (*function)(Args...))
{
  return TimingDecorator<void(Args...)>(std::function<void(Args...)>(function));
}

template<typename T, typename... Args> auto WrapTimingDecorator(T (*function)(Args...))
{
  return TimingDecorator<T(Args...)>(std::function<T(Args...)>(function));
}

}// namespace TimeBox

#endif// UTILITIES_COMMON_HPP