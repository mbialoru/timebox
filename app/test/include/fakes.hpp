#ifndef TEST_FAKES_HPP
#define TEST_FAKES_HPP

#include "defines.hpp"
#include "serialreader.hpp"
#include "threadwrapper.hpp"

class FakeSerialReader : public ThreadWrapper
{
private:
  std::function<void(std::string)> m_callback = nullptr;
  void Work() override
  {
    auto now = std::chrono::system_clock::now();
    now -= std::chrono::milliseconds(200);
    m_callback(ConvertStringToTimepoint(now));
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }

public:
  FakeSerialReader(const char *, std::size_t, std::function<void(std::string)> callback)
    : ThreadWrapper::ThreadWrapper("FakeSerialReader")
  {
    m_callback = callback;
    m_is_paused = false;
  };
  ~FakeSerialReader() = default;
};

#endif// TEST_FAKES_HPP