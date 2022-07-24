#include "clockcontroller.hpp"

ClockController::ClockController(char clock_mode, double resolution)
{
  this->clock_mode = clock_mode;
  this->resolution_power = std::floor(std::log10(resolution));

  // 0 is the aimed at value of difference between system clock and PPS
  pid = std::make_unique<PID<double>>(1.2, 1.0, 0.001, 0);

  bool acquired = false;

  // Stupid way of doing that.
  for (size_t i = 0; i < 10; i++)
  {
    if (adjtimex(&original) != 0)
    {
      sleep(1);
      continue;
    }
    else
      acquired = true;
  }

  if (not acquired)
    throw TimexAcquisitionError();

  modified = original;
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
  // TODO: Here code to modify timex and try to apply it
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
  auto from_str = TimepointFromString(time_str);
  auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(now - from_str);
  timediff_history.push_back(diff.count());
  BOOST_LOG_TRIVIAL(debug) << "Clock difference is " << diff.count();

  pid->update(diff.count(), 1); // For now we assume tick time is always 1s
  auto output = pid->get_output();
  BOOST_LOG_TRIVIAL(debug) << "PID output is " << output;
}