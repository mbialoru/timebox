#include "clockcontroller.hpp"

ClockController::ClockController()
{
  // 0 is the aimed at value of difference between system clock and PPS
  pid = std::make_unique<PID<double>>(1.2, 1.0, 0.001, 0);

  if (adjtimex(&buffer) != 0)
    throw TimexAcquisitionError();

  
}