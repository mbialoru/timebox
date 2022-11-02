#ifndef UTILITIES_HPP
#define UTILITIES_HPP

#pragma once

#include <boost/algorithm/string.hpp>
#include <boost/log/trivial.hpp>
#include <chrono>
#include <filesystem>
#include <map>

#include "exceptions.hpp"

#if defined(__unix__)
#include <termios.h>
#include <unistd.h>
#elif defined(_WIN64) && !defined(__CYGWIN__)
#include <initguid.h>
#include <windows.h>
#endif

namespace TimeBox {

#if defined(__unix__)
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
#elif defined(_WIN64) && !defined(__CYGWIN__)
static std::map<int, int> s_baud_conversion_map{ { 110, CBR_110 },
  { 300, CBR_300 },
  { 600, CBR_600 },
  { 1200, CBR_1200 },
  { 2400, CBR_2400 },
  { 4800, CBR_4800 },
  { 9600, CBR_9600 },
  { 14400, CBR_14400 },
  { 19200, CBR_19200 },
  { 38400, CBR_38400 },
  { 56000, CBR_56000 },
  { 57600, CBR_57600 },
  { 115200, CBR_115200 },
  { 128000, CBR_128000 },
  { 115200, CBR_115200 },
  { 256000, CBR_256000 } };
#endif

#if defined(__unix__)
bool CheckSudo();
bool CheckIfUsingDocker();
#elif defined(_WIN64) && !defined(__CYGWIN__)
void WindowsErrorDebugLog(const char *, const char * = NULL);
void GetAvailableComPorts();
#endif

bool CheckAdminPrivileges();
bool CheckNTPService();
void PauseNTPService();
void StartNTPService();
std::vector<std::string> GetSerialDevicesList();
std::size_t ConvertBaudRate(std::size_t);
std::chrono::system_clock::time_point ConvertStringToTimepoint(std::string);
std::string ConvertTimepointToString(std::chrono::system_clock::time_point);

typedef std::pair<std::string, std::chrono::system_clock::time_point> TimeboxReadout;

template<typename T> struct TimingDecorator;// Yes, this is necessary.
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

#endif// UTILITIES_HPP