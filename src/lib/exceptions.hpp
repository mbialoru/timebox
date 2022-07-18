#include <string>
#include <stdexcept>

class NotImplementedException : public std::exception
{
public:
  NotImplementedException(const char* msg = "Function or method not yet implemented")
  {
    this->msg = msg;
  }
  const char* what() const noexcept
  {
    return this->msg.c_str();
  }

private:
  std::string msg{};
};