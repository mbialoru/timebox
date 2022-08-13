#ifndef UTILITIES_HPP
#define UTILITIES_HPP

#pragma once

#include <boost/algorithm/string.hpp>
#include <boost/asio.hpp>
#include <boost/log/trivial.hpp>
#include <chrono>
#include <filesystem>
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
std::chrono::system_clock::time_point ConvertStringToTimepoint(std::string);
std::string ConvertTimepointToString(std::chrono::system_clock::time_point);

typedef std::pair<std::string, std::chrono::system_clock::time_point> TimeboxReadout;

template<typename T> struct TimingDecorator;// Yes, this is necessary.
template<typename... Args> struct TimingDecorator<void(Args...)>
{
  TimingDecorator(std::function<void(Args...)> function) : m_function{ function } {}

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
  TimingDecorator(std::function<T(Args...)> function) : m_function{ function } {}

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

#endif// UTILITIES_HPP