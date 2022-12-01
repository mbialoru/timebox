#include "mock.hpp"

using namespace TimeBox;

MockClockController::MockClockController()
  : BaseClockController(500), m_time_adjustment(15600),
    mp_pid(std::move(std::make_shared<PID<double>>(1.0, 0.5, 1.0, 0))), m_timepoint(std::chrono::system_clock::now())
{
  mp_pid->SetLimits(-1000, 1000);
}

MockClockController::~MockClockController() = default;

void MockClockController::AdjustClock(TimeboxReadout t_readout)
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
  m_time_adjustment += static_cast<DWORD>(pid_output);
  m_last_call = now;
}
