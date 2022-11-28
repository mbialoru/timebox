#ifndef TEST_FAKES_HPP
#define TEST_FAKES_HPP

#include "baseclockcontroller.hpp"
#include "defines.hpp"
#include "pid.hpp"
#include "serialreader.hpp"
#include "utilities.hpp"

namespace TimeBox {

class FakeSerialReader final : public SerialReader
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
    m_conditon_variable.notify_one();
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
};

class FakeClockController final : public ClockController
{
public:
  FakeClockController()
    : ClockController(500), m_tick(10000), m_timepoint(std::chrono::system_clock::now()),
      mp_pid(std::move(std::make_shared<PID<double>>(1.0, 0.5, 1.0, 0)))
  {
    mp_pid->SetLimits(9000, 10000);
  }

  ~FakeClockController() = default;

  void AdjustClock(TimeboxReadout t_readout) override
  {
    auto [time_string, time_stamp] = t_readout;
    auto now = std::chrono::system_clock::now();
    auto last_call_difference = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_last_call);
    if (last_call_difference.count() < m_minimal_delay) {
      BOOST_LOG_TRIVIAL(warning) << "Too soon to last AdjustClock call ! " << last_call_difference.count() << " ms";
      return;
    }

    auto from_str = ConvertStringToTimepoint(time_string);
    auto diff = now - from_str;
    time_difference_history.push_back(diff);
    BOOST_LOG_TRIVIAL(debug) << "Clock difference is " << diff.count() << " nanoseconds";

    auto processing_time = std::chrono::system_clock::now() - time_stamp;
    BOOST_LOG_TRIVIAL(debug) << "Processing time was " << processing_time.count() << " nanoseconds";

    mp_pid->UpdateLimited(static_cast<double>(diff.count()), 1);
    auto pid_output = mp_pid->GetOutputLimited();
    auto pid_output_raw = mp_pid->GetOutputRaw();
    BOOST_LOG_TRIVIAL(debug) << "PID output is " << pid_output;
    BOOST_LOG_TRIVIAL(debug) << "Raw PID output is " << pid_output_raw;
    m_tick = pid_output;
    m_last_call = now;
  }

  std::size_t m_tick;
  std::shared_ptr<PID<double>> mp_pid;
  std::chrono::system_clock::time_point m_timepoint;
};

}// namespace TimeBox

#endif// TEST_FAKES_HPP