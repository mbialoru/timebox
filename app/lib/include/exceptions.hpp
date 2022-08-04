#ifndef EXCEPTIONS_HPP
#define EXCEPTIONS_HPP

#pragma once

#include <boost/log/trivial.hpp>
#include <stdexcept>
#include <string>

class NotImplementedException : public std::exception
{
public:
  NotImplementedException(const char *msg = "Function or method not yet implemented")
  {
    this->msg = msg;
    BOOST_LOG_TRIVIAL(error) << msg;
  }
  const char *what() const noexcept { return this->msg.c_str(); }

private:
  std::string msg{};
};

class TimexOperationError : public std::exception
{
public:
  TimexOperationError(const char *msg = "Error during operation with timex struct")
  {
    this->msg = msg;
    BOOST_LOG_TRIVIAL(error) << msg;
    BOOST_LOG_TRIVIAL(error) << "errno " << errno;
  }
  const char *what() const noexcept { return this->msg.c_str(); }

private:
  std::string msg{};
};

class InsufficientPermissionsError : public std::exception
{
public:
  InsufficientPermissionsError(const char *msg = "Operation not permitted")
  {
    this->msg = msg;
    BOOST_LOG_TRIVIAL(error) << msg;
  }
  const char *what() const noexcept { return this->msg.c_str(); }

private:
  std::string msg{};
};

#endif// EXCEPTIONS_HPP