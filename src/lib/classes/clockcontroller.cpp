#include "clockcontroller.hpp"

ClockController::ClockController(char clock_mode, double resolution)
{
  this->clock_mode = clock_mode;
  this->resolution_power = std::floor(std::log10(resolution));

  // 0 is the aimed at value of difference between system clock and PPS
  pid = std::make_unique<PID<double>>(1.2, 1.0, 0.001, 0);

  if (adjtimex(&original) != 0)
    throw TimexAcquisitionError();
  modified = original;
}

ClockController::~ClockController()
{
  if (adjtimex(&original) != 0)
    BOOST_LOG_TRIVIAL(error) << "Could not reset kernel tick value back to original!";
}

void ClockController::AdjustKernelTick(unsigned tick)
{
  NormalizeTickValue(tick);
}

short ClockController::NormalizeTickValue(short tick)
{
  if (tick > 11000) { return 11000; }
  else if (tick < 9000) { return 9000; }
  else { return tick; }
}