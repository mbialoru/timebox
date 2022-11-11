#include "winclockcontroller.hpp"

using namespace TimeBox;

WinClockController::WinClockController(const std::size_t t_minimal_delay,
  std::shared_ptr<PID<double>> t_pid,
  const double t_resolution)
  : ClockController(t_minimal_delay), mp_pid(std::move(t_pid))
{
  GetInitialParameters();
  static_cast<void>(QueryPerformanceCounter(&m_performance_counter_frequency));
  BOOST_LOG_TRIVIAL(debug) << "System performance counter frequency: "
                           << std::to_string(m_performance_counter_frequency.QuadPart);
}

WinClockController::~WinClockController()
{
  BOOST_LOG_TRIVIAL(info) << "Reverting system clock adjustment to original value";
  if (!SetSystemTimeAdjustment(m_initial_adjustment_legacy, FALSE)) {
    BOOST_LOG_TRIVIAL(error) << "Failed to set system time adjustment " << HRESULT_FROM_WIN32(GetLastError());
    return;
  }
}

void WinClockController::AdjustClock(const TimeboxReadout t_readout)
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

  mp_pid->UpdateLimited(static_cast<double>(diff.count()), 1);// For now we assume t_tick is always 1s (PPS)
  auto pid_output = mp_pid->GetOutputLimited();
  auto pid_output_raw = mp_pid->GetOutputRaw();
  BOOST_LOG_TRIVIAL(debug) << "PID output is " << pid_output;
  BOOST_LOG_TRIVIAL(debug) << "Raw PID output is " << pid_output_raw;
  SystemTimeAdjustmentWrapper(static_cast<long>(pid_output));
  m_last_call = now;
}

void WinClockController::GetInitialParameters() const
{
  if (!GetSystemTimeAdjustment(&m_initial_adjustment_legacy, 0, 0)) {
    BOOST_LOG_TRIVIAL(error) << "Failed to read system time adjustment" << HRESULT_FROM_WIN32(GetLastError());
    return;
  }
  m_current_adjustment_legacy = m_initial_adjustment_legacy;
}

void WinClockController::LogCurrentClockAdjustments()
{
  DWORD current_adjustment_legacy{ 0 };
  DWORD time_increment_legacy{ 0 };
  BOOL enabled_legacy{ 0 };
  HRESULT hresult_legacy{ S_OK };

  if (!GetSystemTimeAdjustment(&current_adjustment_legacy, &time_increment_legacy, &enabled_legacy)) {
    BOOST_LOG_TRIVIAL(error) << "Failed to read system time adjustment" << HRESULT_FROM_WIN32(GetLastError());
  }

  std::stringstream message;
  message << "Adjustment: " << std::to_string(current_adjustment_legacy) << " ";
  message << "Increment: " << std::to_string(time_increment_legacy) << " ";
  message << "Enabled: " << std::to_string(enabled_legacy) << " ";
  message << "HResult: " << std::to_string(hresult_legacy);

  BOOST_LOG_TRIVIAL(info) << message.str();
}

void WinClockController::SystemTimeAdjustmentWrapper(const long t_ppm_adjustment)
{
  if (t_ppm_adjustment > 1000 || t_ppm_adjustment < -1000) {
    BOOST_LOG_TRIVIAL(error) << "PPM clock adjustment outside of operational range !";
    return;
  }

  auto adjustment_units = static_cast<DWORD>(
    static_cast<double>(t_ppm_adjustment * m_performance_counter_frequency.QuadPart / m_micro_per_second));

  BOOST_LOG_TRIVIAL(info) << "Adjusting system clock by " << ((t_ppm_adjustment > 0) ? "+" : "-")
                          << std::to_string(t_ppm_adjustment) << " PPM";

  m_current_adjustment_legacy += adjustment_units;

  if (!SetSystemTimeAdjustment(m_current_adjustment_legacy, FALSE)) {
    BOOST_LOG_TRIVIAL(error) << "Failed to set system time adjustment " << HRESULT_FROM_WIN32(GetLastError());
    return;
  }

  m_adjustment_history.push_back(t_ppm_adjustment);
}