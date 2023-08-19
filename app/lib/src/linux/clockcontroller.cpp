#include "clockcontroller.hpp"

using namespace TimeBox;

ClockController::ClockController(const char t_clock_mode,
  std::shared_ptr<PID<double>> t_pid,
  [[maybe_unused]] const double t_resolution,
  const std::size_t t_minimal_delay)
  : ClockController(t_minimal_delay), m_clock_mode(t_clock_mode), mp_pid(std::move(t_pid))
{
  BOOST_LOG_TRIVIAL(debug) << "Retrieving timex from kernel";

  std::future<timex> res{ std::async(std::launch::async, std::bind(&ClockController::get_system_timex, this)) };
  m_timex = res.get();
  m_original_tick = static_cast<std::size_t>(m_timex.tick);

  BOOST_LOG_TRIVIAL(debug) << "Success retrieving timex from kernel";

  m_timex.modes |= ADJ_TICK;
};

ClockController::~ClockController()
{
  BOOST_LOG_TRIVIAL(debug) << "Rolling back kernel tick to original value";
  m_timex.tick = static_cast<long>(m_original_tick);
  std::ignore =
    std::async(std::launch::async, std::bind(&ClockController::set_system_timex, this, std::placeholders::_1), &m_timex);
}

timex ClockController::get_timex() const { return m_timex; }

void ClockController::set_system_timex(timex *tp_timex) const { adjtimex_wrapper(tp_timex); }

void ClockController::adjust_kernel_tick(const std::size_t t_tick)
{
  BOOST_LOG_TRIVIAL(debug) << "Adjusting kernel tick to " << t_tick;
  m_tick_history.push_back(t_tick);
  m_timex.tick = static_cast<long>(t_tick);

  if (not check_admin_privileges()) { throw InsufficientPermissionsError(); }

  std::ignore =
    std::async(std::launch::async, std::bind(&ClockController::set_system_timex, this, std::placeholders::_1), &m_timex);

  BOOST_LOG_TRIVIAL(debug) << "Successfully changed kernel tick to " << t_tick;
}

bool ClockController::adjtimex_wrapper(timex *tp_tm) const
{
  bool success{ false };
  std::size_t attempt{ 0 };
  while (not success) {
    if (attempt == 100) { throw TimexOperationError(); }

    success = !static_cast<bool>(adjtimex(tp_tm));
    attempt++;

    switch (errno) {
    case 0:
      BOOST_LOG_TRIVIAL(debug) << "Success operating on timex";
      break;
    case EAGAIN:
      BOOST_LOG_TRIVIAL(debug) << "EAGAIN - trying again";
      success = false;
      continue;
    case EPERM:
      BOOST_LOG_TRIVIAL(warning) << "Operation not permitted when acquiring timex";
      break;
    case EINVAL:
      BOOST_LOG_TRIVIAL(warning) << "Invalid argument when acquiring timex";
      break;
    default:
      BOOST_LOG_TRIVIAL(warning) << "errno when operating on timex " << errno;
      break;
    }
  }
  BOOST_LOG_TRIVIAL(debug) << "Operation took " << attempt << " attempts";
  return success;
}

void ClockController::adjust_clock(const TimeboxReadout t_readout)
{
  auto [time_string, time_stamp] = t_readout;
  auto now = std::chrono::system_clock::now();
  auto last_call_difference = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_last_call);
  if (last_call_difference.count() < m_minimal_delay) {
    BOOST_LOG_TRIVIAL(warning) << "Too soon to last adjust_clock call ! " << last_call_difference.count() << " ms";
    return;
  }

  auto from_str = string_to_timepoint(time_string);
  auto diff = now - from_str;
  m_difference_history.push_back(diff);
  BOOST_LOG_TRIVIAL(debug) << "Clock difference is " << diff.count() << " nanoseconds";

  auto processing_time = std::chrono::system_clock::now() - time_stamp;
  BOOST_LOG_TRIVIAL(debug) << "Processing time was " << processing_time.count() << " nanoseconds";

  mp_pid->update_limited(static_cast<double>(diff.count()), 1);// For now we assume t_tick is always 1s (PPS)
  auto pid_output = mp_pid->get_output_limited();
  auto pid_output_raw = mp_pid->get_output_raw();
  BOOST_LOG_TRIVIAL(debug) << "PID output is " << pid_output;
  BOOST_LOG_TRIVIAL(debug) << "Raw PID output is " << pid_output_raw;
  adjust_kernel_tick(static_cast<std::size_t>(pid_output));
  m_last_call = now;
}

timex ClockController::get_system_timex() const
{
  timex ret{};
  adjtimex_wrapper(&ret);
  return ret;
}