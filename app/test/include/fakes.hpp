#ifndef TEST_FAKES_HPP
#define TEST_FAKES_HPP

#include "clockcontroller.hpp"
#include "defines.hpp"
#include "serialreader.hpp"
#include "utilities.hpp"

namespace TimeBox {

class FakeSerialReader : public SerialReader
{
public:
  FakeSerialReader(const char *, std::size_t, std::function<void(TimeboxReadout)> t_callback)
    : SerialReader(std::move(t_callback))
  {
    m_is_paused = false;
  };

  ~FakeSerialReader()
  {
    WipeSerialBuffer();
    m_callback = nullptr;
    m_is_paused = true;
  };

  bool QuerryDevice(std::string) override { return true; };

private:
  void InitializeSerial(const char *, std::size_t) override{};
  void Work() override
  {
    auto now = std::chrono::system_clock::now() - std::chrono::milliseconds(200);
    m_callback(TimeboxReadout{ ConvertTimepointToString(now) + ".0", now });
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
};

class FakeClockController : public ClockController
{
public:
  FakeClockController() : ClockController(500){};
  ~FakeClockController() = default;

  void AdjustClock(TimeboxReadout) override{};
  std::chrono::system_clock::duration CalculateClockDifference() override{};
};

}// namespace TimeBox

#endif// TEST_FAKES_HPP