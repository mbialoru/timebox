#include "clockcontroller.hpp"
#include <cmath>

using namespace TimeBox;

ClockController::ClockController(const std::size_t t_minimal_delay,
  std::shared_ptr<PID<double>> t_pid,
  [[maybe_unused]] const double t_resolution)
  : BaseClockController(t_minimal_delay), mp_pid(std::move(t_pid))
{
  if (not check_admin_privileges()) { throw InsufficientPermissionsError(); }
  update_process_token();

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

void ClockController::adjust_clock(const TimeboxReadout t_readout)
{
  static std::chrono::system_clock::time_point s_last_time_stamp;
  auto [time_string, time_stamp]{ t_readout };
  auto now{ std::chrono::system_clock::now() };
  auto last_call_difference{ std::chrono::duration_cast<std::chrono::milliseconds>(now - m_last_call) };
  if (std::llabs(last_call_difference.count()) < m_minimal_delay) {
    BOOST_LOG_TRIVIAL(warning) << "Too soon to last adjust_clock call ! " << last_call_difference.count() << " ms";
    return;
  }

  auto from_str{ string_to_timepoint(time_string) };
  auto time_difference{ std::chrono::duration_cast<std::chrono::microseconds>(now - from_str) };
  auto time_stamp_diff{ std::chrono::duration_cast<std::chrono::milliseconds>(time_stamp - s_last_time_stamp) };

  if (std::isnan(time_difference.count())) { throw std::runtime_error("Encountered clock difference as NaN !"); }
  if (std::isnan(time_stamp_diff.count())) { throw std::runtime_error("Encountered time stamp difference as NaN !"); }

  m_difference_history.push_back(time_difference);
  BOOST_LOG_TRIVIAL(debug) << "Time stamp difference is " << time_stamp_diff.count() << " milliseconds";
  BOOST_LOG_TRIVIAL(debug) << "Clock difference is " << time_difference.count() << " microseconds";

  auto processing_time{ std::chrono::duration_cast<std::chrono::microseconds>(
    std::chrono::system_clock::now() - time_stamp) };
  BOOST_LOG_TRIVIAL(debug) << "Processing time was " << processing_time.count() << " microseconds";

  double time_stamp_diff_seconds{ static_cast<double>(time_stamp_diff.count() / 1000) };

  // Ideally only first tick should have some weird values - hence ternary style operator if
  mp_pid->update_limited(
    static_cast<double>(time_difference.count()), (time_stamp_diff_seconds >= 0) ? time_stamp_diff_seconds : 1.0);
  auto pid_output = mp_pid->get_output_limited();
  auto pid_output_raw = mp_pid->get_output_raw();
  BOOST_LOG_TRIVIAL(debug) << "PID output is " << pid_output;
  BOOST_LOG_TRIVIAL(debug) << "Raw PID output is " << pid_output_raw;
  system_time_adjustment_wrapper(static_cast<long>(pid_output));
  m_last_call = now;
  s_last_time_stamp = time_stamp;
}

HRESULT ClockController::update_process_token()
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

void ClockController::print_current_clock_adjustments() const
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

void ClockController::system_time_adjustment_wrapper(const long t_ppm_adjustment)
{
  auto [lower_limit, upper_limit]{ mp_pid->get_limits() };
  if (t_ppm_adjustment > upper_limit || t_ppm_adjustment < lower_limit) {
    BOOST_LOG_TRIVIAL(error) << "PPM clock adjustment outside of operational range !";
    return;
  }

  double scaling_factor{ static_cast<double>(m_performance_counter_frequency.QuadPart / SM_MICRO_PER_SECOND) };
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
    if ((m_current_adjustment_legacy - adjustment_units) <= SM_MINIMAL_ADJUSTMENT) {
      m_current_adjustment_legacy -= adjustment_units;
    } else {
      m_current_adjustment_legacy = SM_MINIMAL_ADJUSTMENT;
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