#ifndef EXCEPTIONS_HPP
#define EXCEPTIONS_HPP

#pragma once

#include <boost/log/trivial.hpp>
#include <stdexcept>
#include <string>

namespace TimeBox {

class NotImplementedException : public std::exception
{
public:
  explicit NotImplementedException(const char *tp_msg = "Function, method or feature not yet implemented")
    : m_msg(tp_msg)
  {
    BOOST_LOG_TRIVIAL(error) << m_msg;
  }
  const char *what() const noexcept { return m_msg.c_str(); }

private:
  std::string m_msg{};
};

class TimexOperationError : public std::exception
{
public:
  explicit TimexOperationError(const char *tp_msg = "Error during operation with timex struct") : m_msg(tp_msg)
  {
    BOOST_LOG_TRIVIAL(error) << m_msg;
    BOOST_LOG_TRIVIAL(error) << "errno " << errno;
  }
  const char *what() const noexcept { return m_msg.c_str(); }

private:
  std::string m_msg{};
};

class InsufficientPermissionsError : public std::exception
{
public:
  explicit InsufficientPermissionsError(const char *tp_msg = "Operation not permitted") : m_msg(tp_msg)
  {
    BOOST_LOG_TRIVIAL(error) << m_msg;
  }
  const char *what() const noexcept { return m_msg.c_str(); }

private:
  std::string m_msg{};
};

class DeviceDirectoryNotExist : public std::exception
{
public:
  explicit DeviceDirectoryNotExist(
    const char *tp_msg = "Device directory doesn't exist - check platform and do not run in docker")
    : m_msg(tp_msg)
  {
    BOOST_LOG_TRIVIAL(error) << m_msg;
  }
  const char *what() const noexcept { return m_msg.c_str(); }

private:
  std::string m_msg{};
};

class NTPServiceOperationError : public std::exception
{
public:
  explicit NTPServiceOperationError(const char *tp_msg = "Could not perform operation with NTP Service") : m_msg(tp_msg)
  {
    BOOST_LOG_TRIVIAL(error) << m_msg;
  }
  const char *what() const noexcept { return m_msg.c_str(); }

private:
  std::string m_msg{};
};

}// namespace TimeBox

#endif// EXCEPTIONS_HPP