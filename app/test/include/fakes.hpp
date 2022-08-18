#ifndef TEST_FAKES_HPP
#define TEST_FAKES_HPP

#include "defines.hpp"
#include "serialreader.hpp"
#include "threadwrapper.hpp"
#include "utilities.hpp"

class FakeSerialReader : public TimeBox::ThreadWrapper
{
private:
  std::function<void(TimeBox::TimeboxReadout)> m_callback = nullptr;
  void Work() override
  {
    auto now = std::chrono::system_clock::now();
    now -= std::chrono::milliseconds(200);
    m_callback(TimeBox::TimeboxReadout{ TimeBox::ConvertTimepointToString(now), now });
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }

public:
  FakeSerialReader(const char *, std::size_t, std::function<void(TimeBox::TimeboxReadout)> t_callback)
    : TimeBox::ThreadWrapper::ThreadWrapper("FakeSerialReader")
  {
    m_callback = t_callback;
    m_is_paused = false;
  };
  ~FakeSerialReader() = default;
};

#endif// TEST_FAKES_HPP