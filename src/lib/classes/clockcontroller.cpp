#include "clockcontroller.hpp"

ClockController::ClockController(char clock_mode)
{
  this->clock_mode = clock_mode;

  // 0 is the aimed at value of difference between system clock and PPS
  pid = std::make_unique<PID<double>>(1.2, 1.0, 0.001, 0);

  if (adjtimex(&original) != 0)
    throw TimexAcquisitionError();
  modified = original;
}

ClockController::~ClockController()
{
  adjtimex(&original); // We need to make sure this happens
}

double ClockController::ClockDifference(std::string time_str,
  std::chrono::time_point<std::chrono::system_clock> sys_time)
{
  throw NotImplementedException();
}