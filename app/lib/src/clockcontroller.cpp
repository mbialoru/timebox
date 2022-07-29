#include "clockcontroller.hpp"

ClockController::ClockController(char clock_mode, double resolution)
{
  this->clock_mode = clock_mode;
  this->resolution_power = std::floor(std::log10(resolution));

  // 0 is the aimed value of difference between system clock and PPS
  pid = std::make_unique<PID<double>>(1.2, 1.0, 0.001, 0);

  BOOST_LOG_TRIVIAL(trace) << "Retrieving timex from kernel";

  std::future<timex> ret = std::async(std::launch::async,
    std::bind(&ClockController::GetTimex, this));
  original = ret.get();

  BOOST_LOG_TRIVIAL(trace) << "Success in retrieving timex from kernel";

  modified = original;
  modified.modes |= ADJ_TICK;
  last_call = std::chrono::system_clock::now();
}

ClockController::~ClockController()
{
  // TODO: Here code to re-adjust kernel tick to old values
  if (adjtimex(&original) != 0)
    BOOST_LOG_TRIVIAL(error) << "Could not reset kernel tick value back to original!";
}

void ClockController::AdjustKernelTick(unsigned tick)
{
  tick = NormalizeTickValue(tick);
  tick_history.push_back(tick);
  BOOST_LOG_TRIVIAL(debug) << "Adjusting kernel tick to " << tick;
  modified.tick = tick;

  std::future<bool> ret = std::async(std::launch::async, std::bind(
    &ClockController::SetTimex, this, std::placeholders::_1), &modified);

  bool is_changed = ret.get();

  if (is_changed)
    BOOST_LOG_TRIVIAL(debug) << "Successfully changed kernel tick to " << tick;
}

short ClockController::NormalizeTickValue(short tick)
{
  if (tick > 11000) { return 11000; }
  else if (tick < 9000) { return 9000; }
  else { return tick; }
}

void ClockController::AdjustClock(std::string time_str)
{
  auto now = std::chrono::system_clock::now();
  auto tmp = last_call.load();
  auto tmp_diff = std::chrono::duration_cast<std::chrono::milliseconds>(now - tmp);
  if (tmp_diff.count() < 500)
  {
    BOOST_LOG_TRIVIAL(warning) << "Too soon to last AdjustClock call !";
    return;
  }

  auto from_str = TimepointFromString(time_str);
  auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(now - from_str);
  timediff_history.push_back(diff.count());
  BOOST_LOG_TRIVIAL(debug) << "Clock difference is " << diff.count();

  pid->update(diff.count(), 1); // For now we assume tick is always 1s (PPS)
  auto output = pid->get_output();
  BOOST_LOG_TRIVIAL(debug) << "PID output is " << output;
  AdjustKernelTick(output);
  last_call.store(now);
}

timex ClockController::getModifiedTimex()
{
  return modified;
}

timex ClockController::getOriginalTimex()
{
  return original;
}

timex ClockController::GetTimex()
{
  // TODO: On Fedora it doesn't work from container for some reason ...
  timex ret;
  thread_local bool success = false;
  while (not success)
  {
    success = (bool)adjtimex(&ret);
    if (errno == EPERM)
      BOOST_LOG_TRIVIAL(error) << "Operation not permitted when acquiring timex";
    if (errno == EINVAL)
      BOOST_LOG_TRIVIAL(warning) << "Invalid argument when acquiring timex";
  }
  return ret;
}

bool ClockController::SetTimex(timex* t)
{
  return true;
  // bool success = false;
  // while (not success)
  //   success = (bool)adjtimex(t);
  // return success;
}