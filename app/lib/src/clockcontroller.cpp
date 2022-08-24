#include "clockcontroller.hpp"

using namespace TimeBox;

ClockController::ClockController(const double t_resolution,
  const char t_clock_mode,
  std::shared_ptr<PID<double>> t_pid,
  const long int t_minimal_delay)
  : mp_pid(t_pid), m_clock_mode(t_clock_mode), m_minimal_delay(t_minimal_delay)
{
  m_resolution_power = static_cast<std::size_t>(std::floor(std::log10(t_resolution)));

  BOOST_LOG_TRIVIAL(debug) << "Retrieving timex from kernel";

  std::future<timex> res{ std::async(std::launch::async, std::bind(&ClockController::GetSystemTimex, this)) };
  m_timex = res.get();
  m_original_tick = static_cast<std::size_t>(m_timex.tick);

  BOOST_LOG_TRIVIAL(debug) << "Success retrieving timex from kernel";

  m_timex.modes |= ADJ_TICK;
  last_call = std::chrono::system_clock::now();
}

ClockController::~ClockController()
{
  BOOST_LOG_TRIVIAL(debug) << "Rolling back kernel tick to original value";
  m_timex.tick = static_cast<long>(m_original_tick);
  std::ignore =
    std::async(std::launch::async, std::bind(&ClockController::SetSystemTimex, this, std::placeholders::_1), &m_timex);
}

timex ClockController::GetTimex() const { return m_timex; }

void ClockController::SetSystemTimex(timex *t_timex) const { OperateOnTimex(t_timex); }

void ClockController::AdjustKernelTick(const std::size_t t_tick)
{
  BOOST_LOG_TRIVIAL(debug) << "Adjusting kernel tick to " << t_tick;
  tick_history.push_back(t_tick);
  m_timex.tick = static_cast<long>(t_tick);

  if (not CheckAdminPrivileges()) throw InsufficientPermissionsError();

  std::ignore =
    std::async(std::launch::async, std::bind(&ClockController::SetSystemTimex, this, std::placeholders::_1), &m_timex);

  BOOST_LOG_TRIVIAL(debug) << "Successfully changed kernel tick to " << t_tick;
}

void ClockController::AdjustClock(const TimeboxReadout t_readout)
{
  auto [time_string, time_stamp] = t_readout;
  auto now = std::chrono::system_clock::now();
  auto last_call_difference = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_call);
  if (last_call_difference.count() < m_minimal_delay) {
    BOOST_LOG_TRIVIAL(warning) << "Too soon to last AdjustClock call ! " << last_call_difference.count() << " ms";
    return;
  }

  auto from_str = ConvertStringToTimepoint(time_string);
  auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(now - from_str);
  time_difference_history.push_back(diff.count());
  BOOST_LOG_TRIVIAL(debug) << "Clock difference is " << diff.count() << " milliseconds";

  auto processing_time = std::chrono::system_clock::now() - time_stamp;
  BOOST_LOG_TRIVIAL(debug) << "Processing time was " << processing_time.count() << " nanoseconds";

  mp_pid->UpdateLimited(static_cast<double>(diff.count()), 1);// For now we assume t_tick is always 1s (PPS)
  auto pid_output = mp_pid->GetOutputLimited();
  auto pid_output_raw = mp_pid->GetOutputRaw();
  BOOST_LOG_TRIVIAL(debug) << "PID output is " << pid_output;
  BOOST_LOG_TRIVIAL(debug) << "Raw PID output is " << pid_output_raw;
  AdjustKernelTick(static_cast<std::size_t>(pid_output));
  last_call = now;
}

bool ClockController::OperateOnTimex(timex *t_tm) const
{
  bool success{ false };
  std::size_t attempt{ 0 };
  while (not success) {
    if (attempt == 100) throw TimexOperationError();

    success = !static_cast<bool>(adjtimex(t_tm));
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

timex ClockController::GetSystemTimex() const
{
  timex ret{};
  OperateOnTimex(&ret);
  return ret;
}