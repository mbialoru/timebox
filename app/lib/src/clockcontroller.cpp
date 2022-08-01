#include "clockcontroller.hpp"

ClockController::ClockController(char clock_mode, double resolution)
{
  m_clock_mode = clock_mode;
  m_resolution_power = std::floor(std::log10(resolution));

  // 0 is the aimed value of difference between system clock and PPS
  mp_pid = std::make_unique<PID<double>>(1.2, 1.0, 0.001, 0);

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
  auto kernel_tick = NormalizeTickValue(tick);
  tick_history.push_back(kernel_tick);
  BOOST_LOG_TRIVIAL(debug) << "Adjusting kernel tick to " << tick;
  m_modified.tick = tick;

  if (not CheckAdminPrivileges()) throw InsufficientPermissionsError();

  std::future<void> _ = std::async(
    std::launch::async, std::bind(&ClockController::SetSystemTimex, this, std::placeholders::_1), &m_modified);

  BOOST_LOG_TRIVIAL(debug) << "Successfully changed kernel tick to " << tick;
}

std::size_t ClockController::NormalizeTickValue(std::size_t tick)
{
  if (tick > 11000) {
    return 11000;
  } else if (tick < 9000) {
    return 9000;
  } else {
    return tick;
  }
}

void ClockController::AdjustClock(std::string time_str)
{
  auto now = std::chrono::system_clock::now();
  auto tmp_diff = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_call.load());
  if (tmp_diff.count() < 500) {
    BOOST_LOG_TRIVIAL(warning) << "Too soon to last AdjustClock call ! " << tmp_diff.count() << " ms";
    return;
  }

  auto from_str = ConvertTimepointToString(time_str);
  auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(now - from_str);
  time_difference_history.push_back(diff.count());
  BOOST_LOG_TRIVIAL(debug) << "Clock difference is " << diff.count();

  mp_pid->Update(diff.count(), 1);// For now we assume tick is always 1s (PPS)
  auto output = mp_pid->GetOutput();
  BOOST_LOG_TRIVIAL(debug) << "PID output is " << output;
  AdjustKernelTick(output);
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