#include "clockcontroller.hpp"

using namespace TimeBox;

ClockController::ClockController(const std::size_t t_minimal_delay,
  std::shared_ptr<PID<double>> t_pid,
  [[maybe_unused]] const double t_resolution)
  : BaseClockController(t_minimal_delay), mp_pid(std::move(t_pid))
{
  if (not CheckAdminPrivileges()) { throw InsufficientPermissionsError(); }
  UpdateProcessTokenPrivileges();

  BOOL enabled_legacy{ 0 };
  DWORD time_increment_legacy{ 0 };
  BOOST_LOG_TRIVIAL(debug) << "Reading initial system time adjustment";
  if (not GetSystemTimeAdjustment(&m_initial_adjustment_legacy, &time_increment_legacy, &enabled_legacy)) {
    BOOST_LOG_TRIVIAL(error) << "Failed to read system time adjustment, error code: "
                             << HRESULT_FROM_WIN32(GetLastError());
    return;
  }
  BOOST_LOG_TRIVIAL(debug) << "Obtained initial system time adjustment: "
                           << std::to_string(m_initial_adjustment_legacy);

  m_initial_adjustment = m_initial_adjustment_legacy;
  m_current_adjustment_legacy = m_initial_adjustment_legacy;
  std::ignore = QueryPerformanceFrequency(&m_performance_counter_frequency);
  BOOST_LOG_TRIVIAL(debug) << "System performance counter frequency: "
                           << std::to_string(m_performance_counter_frequency.QuadPart);
}

ClockController::~ClockController()
{
  BOOST_LOG_TRIVIAL(info) << "Reverting system clock adjustment to original value";
  BOOST_LOG_TRIVIAL(debug) << "Applied initial system time adjustment: " << std::to_string(m_initial_adjustment_legacy);
  if (not SetSystemTimeAdjustment(m_initial_adjustment_legacy, FALSE)) {
    BOOST_LOG_TRIVIAL(error) << "Failed to set system time adjustment, error code: "
                             << HRESULT_FROM_WIN32(GetLastError());
    return;
  }
}

void ClockController::AdjustClock(const TimeboxReadout t_readout)
{
  auto [time_string, time_stamp] = t_readout;
  auto now = std::chrono::system_clock::now();
  auto last_call_difference = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_last_call);
  if (std::llabs(last_call_difference.count()) < m_minimal_delay) {
    BOOST_LOG_TRIVIAL(warning) << "Too soon to last AdjustClock call ! " << last_call_difference.count() << " ms";
    return;
  }

  auto from_str = ConvertStringToTimepoint(time_string);
  auto time_difference = std::chrono::duration_cast<std::chrono::microseconds>(now - from_str);
  m_difference_history.push_back(time_difference);
  BOOST_LOG_TRIVIAL(debug) << "Clock difference is " << time_difference.count() << " microseconds";

  auto processing_time =
    std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - time_stamp);
  BOOST_LOG_TRIVIAL(debug) << "Processing time was " << processing_time.count() << " microseconds";

  mp_pid->UpdateLimited(static_cast<double>(time_difference.count()), 1);// For now we assume t_tick is always 1s (PPS)
  auto pid_output = mp_pid->GetOutputLimited();
  auto pid_output_raw = mp_pid->GetOutputRaw();
  BOOST_LOG_TRIVIAL(debug) << "PID output is " << pid_output;
  BOOST_LOG_TRIVIAL(debug) << "Raw PID output is " << pid_output_raw;
  SystemTimeAdjustmentWrapper(static_cast<long>(pid_output));
  m_last_call = now;
}

void ClockController::PrintCurrentClockAdjustments() const
{
  DWORD current_adjustment_legacy{ 0 };
  DWORD time_increment_legacy{ 0 };
  BOOL enabled_legacy{ 0 };
  HRESULT hresult_legacy{ S_OK };

  if (not GetSystemTimeAdjustment(&current_adjustment_legacy, &time_increment_legacy, &enabled_legacy)) {
    BOOST_LOG_TRIVIAL(error) << "Failed to read system time adjustment, error code: "
                             << HRESULT_FROM_WIN32(GetLastError());
  }

  std::stringstream message;
  message << "Adjustment: " << std::to_string(current_adjustment_legacy) << " ";
  message << "Increment: " << std::to_string(time_increment_legacy) << " ";
  message << "Enabled: " << std::to_string(enabled_legacy) << " ";
  message << "HResult: " << std::to_string(hresult_legacy);

  BOOST_LOG_TRIVIAL(info) << message.str();
}

void ClockController::SystemTimeAdjustmentWrapper(const long t_ppm_adjustment)
{
  // auto [lower_limit, upper_limit]{ mp_pid->GetLimits() };
  // if (t_ppm_adjustment >= (upper_limit * 1.05) || t_ppm_adjustment <= (lower_limit * 1.05)) {
  //   BOOST_LOG_TRIVIAL(error) << "PPM clock adjustment outside of operational range !";
  //   return;
  // }

  double scaling_factor{ static_cast<double>(m_performance_counter_frequency.QuadPart / m_micro_per_second) };
  DWORD adjustment_units{ static_cast<DWORD>(std::abs(t_ppm_adjustment * scaling_factor)) };

  BOOST_LOG_TRIVIAL(info) << "Adjusting system clock by " << std::to_string(t_ppm_adjustment) << " PPM ("
                          << ((t_ppm_adjustment >= 0) ? "+" : "-") << std::to_string(adjustment_units)
                          << " adjustment units";

  // NOTE: DWORD (unsigned long) size limitations, prevent wraparound
  // ref: https://learn.microsoft.com/en-us/openspecs/windows_protocols/ms-dtyp/262627d8-3418-4627-9218-4ffe110850b2
  if (t_ppm_adjustment > 0) {
    if ((m_current_adjustment_legacy + adjustment_units) >= std::numeric_limits<unsigned long>::max()) {
      m_current_adjustment_legacy = std::numeric_limits<unsigned long>::max();
    } else {
      m_current_adjustment_legacy += adjustment_units;
    }
  } else {
    if ((m_current_adjustment_legacy - adjustment_units) <= m_min_adjustment) {
      m_current_adjustment_legacy -= adjustment_units;
    } else {
      m_current_adjustment_legacy = m_min_adjustment;
    }
  }

  BOOST_LOG_TRIVIAL(debug) << "Current adjustment legacy " << std::to_string(m_current_adjustment_legacy);

  if (not SetSystemTimeAdjustment(m_current_adjustment_legacy, FALSE)) {
    BOOST_LOG_TRIVIAL(error) << "Failed to set system time adjustment, error code: "
                             << HRESULT_FROM_WIN32(GetLastError());
    return;
  }
  m_adjustment_history.push_back(t_ppm_adjustment);
}

HRESULT ClockController::UpdateProcessTokenPrivileges()
{
  HRESULT hresult;
  HANDLE process_token{ nullptr };
  TOKEN_PRIVILEGES token_privileges{ 0 };
  LUID luid;

  if (not LookupPrivilegeValue(NULL, SE_SYSTEMTIME_NAME, &luid)) {
    hresult = HRESULT_FROM_WIN32(GetLastError());
    BOOST_LOG_TRIVIAL(error) << "Failed to lookup privilege value, error code: " << hresult;
    return hresult;
  }

  // get token for our process
  if (not OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &process_token)) {
    hresult = HRESULT_FROM_WIN32(GetLastError());
    BOOST_LOG_TRIVIAL(error) << "Failed to open process token, error code: " << hresult;
    return hresult;
  }

  // enable SE_SYSTEMTIME_NAME privilege
  token_privileges.PrivilegeCount = 1;
  token_privileges.Privileges[0].Luid = luid;
  token_privileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

  if (not AdjustTokenPrivileges(process_token, FALSE, &token_privileges, sizeof(token_privileges), NULL, NULL)) {
    BOOST_LOG_TRIVIAL(error) << "Failed to adjust process token privileges, error code: "
                             << HRESULT_FROM_WIN32(GetLastError());
  } else {
    hresult = S_OK;
    BOOST_LOG_TRIVIAL(debug) << "Added SYSTEMTIME privilege to the process token";
  }

  if (process_token != nullptr) { CloseHandle(process_token); }
  return hresult;
}