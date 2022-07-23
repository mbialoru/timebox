#include <string>
#include <stdexcept>
#include <boost/log/trivial.hpp>

class NotImplementedException : public std::exception
{
public:
  NotImplementedException(const char* msg =
    "Function or method not yet implemented")
  {
    this->msg = msg;
    BOOST_LOG_TRIVIAL(error) << msg;
  }
  const char* what() const noexcept
  {
    return this->msg.c_str();
  }

private:
  std::string msg{};
};

class TimexAcquisitionError : public std::exception
{
public:
  TimexAcquisitionError(const char* msg =
    "Error during acquisition of timex struct")
  {
    this->msg = msg;
    BOOST_LOG_TRIVIAL(error) << msg;
  }
  const char* what() const noexcept
  {
    return this->msg.c_str();
  }

private:
  std::string msg{};
};

class TimexChangeError : public std::exception
{
public:
  TimexChangeError(const char* msg = "Error during modifying timex struct")
  {
    this->msg = msg;
    BOOST_LOG_TRIVIAL(error) << msg;
  }
  const char* what() const noexcept
  {
    return this->msg.c_str();
  }

private:
  std::string msg{};
};