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
  NotImplementedException(const char *t_msg = "Function or method not yet implemented")
  {
    m_msg = t_msg;
    BOOST_LOG_TRIVIAL(error) << m_msg;
  }
  const char *what() const noexcept { return m_msg.c_str(); }

private:
  std::string m_msg{};
};

class TimexOperationError : public std::exception
{
public:
  TimexOperationError(const char *t_msg = "Error during operation with timex struct")
  {
    m_msg = t_msg;
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
  InsufficientPermissionsError(const char *t_msg = "Operation not permitted")
  {
    m_msg = t_msg;
    BOOST_LOG_TRIVIAL(error) << m_msg;
  }
  const char *what() const noexcept { return m_msg.c_str(); }

private:
  std::string m_msg{};
};

}// namespace TimeBox

#endif// EXCEPTIONS_HPP