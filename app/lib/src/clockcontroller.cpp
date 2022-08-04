#include "clockcontroller.hpp"

ClockController::ClockController(char clock_mode, double resolution, long int minimalDelay)
{
  m_clock_mode = clock_mode;
  m_resolution_power = std::floor(std::log10(resolution));
  m_minimal_delay = minimalDelay;

  // 0 is the aimed value of difference between system clock and PPS
  mp_pid = std::make_unique<PID<double>>(1.2, 1.0, 0.001, 0);
  mp_pid->SetLimits(9000, 11000);

  BOOST_LOG_TRIVIAL(trace) << "Retrieving timex from kernel";

  std::future<timex> res = std::async(std::launch::async, std::bind(&ClockController::GetSystemTimex, this));
  m_original = res.get();

  BOOST_LOG_TRIVIAL(trace) << "Success retrieving timex from kernel";

  m_modified = m_original;
  m_modified.modes |= ADJ_TICK;
  last_call = std::chrono::system_clock::now();
}

ClockController::~ClockController() { SetSystemTimex(&m_original); }

timex ClockController::GetModifiedTimex() { return m_modified; }

timex ClockController::GetOriginalTimex() { return m_original; }

void ClockController::SetSystemTimex(timex *t) { OperateOnTimex(t); }

void ClockController::AdjustKernelTick(std::size_t tick)
{
  BOOST_LOG_TRIVIAL(debug) << "Adjusting kernel tick to " << tick;
  tick_history.push_back(tick);
  m_modified.tick = tick;

  if (not CheckAdminPrivileges()) throw InsufficientPermissionsError();

  std::ignore = std::async(
    std::launch::async, std::bind(&ClockController::SetSystemTimex, this, std::placeholders::_1), &m_modified);

  BOOST_LOG_TRIVIAL(debug) << "Successfully changed kernel tick to " << tick;
}

void ClockController::AdjustClock(std::string time_str)
{
  auto now = std::chrono::system_clock::now();
  auto tmp_diff = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_call.load());
  if (tmp_diff.count() < m_minimal_delay) {
    BOOST_LOG_TRIVIAL(warning) << "Too soon to last AdjustClock call ! " << tmp_diff.count() << " ms";
    return;
  }

  auto from_str = ConvertTimepointToString(time_str);
  auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(now - from_str);
  time_difference_history.push_back(diff.count());
  BOOST_LOG_TRIVIAL(debug) << "Clock difference is " << diff.count() << " milliseconds";

  mp_pid->Update(diff.count(), 1);// For now we assume tick is always 1s (PPS)
  auto pid_output = mp_pid->GetOutputLimited();
  BOOST_LOG_TRIVIAL(debug) << "PID output is " << pid_output;
  AdjustKernelTick(pid_output);
  last_call.store(now);
}

bool ClockController::OperateOnTimex(timex *tm)
{
  bool success{ false };
  std::size_t attempts{ 0 };
  while (not success) {
    if (attempts == 100) throw TimexOperationError();

    success = !(bool)adjtimex(tm);
    attempts++;

    switch (errno) {
    case 0:
      break;
    case EPERM:
      BOOST_LOG_TRIVIAL(warning) << "Operation not permitted when acquiring timex";
      break;
    case EINVAL:
      BOOST_LOG_TRIVIAL(warning) << "Invalid argument when acquiring timex";
      break;
    default:
      BOOST_LOG_TRIVIAL(warning) << "errno when acquiring timex " << errno;
      break;
    }
  }
  BOOST_LOG_TRIVIAL(debug) << "Operation took " << attempts << " attempts";
  return success;
}

timex ClockController::GetSystemTimex()
{
  timex ret{ 0 };
  OperateOnTimex(&ret);
  return ret;
}