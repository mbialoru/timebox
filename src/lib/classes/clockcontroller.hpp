#ifndef CLOCKCONTROLLER_HPP
#define CLOCKCONTROLLER_HPP

#pragma once

#include <vector>
#include <ctime>
#include "pid.hpp"

class ClockController
{
public:
  ClockController();
  ~ClockController() = default;

private:
  struct tm curr { 0 };
  struct tm last { 0 };
  // TODO: Some container to store differences in PPS time and system clock time
  // Way to calculate these differences
  // Way to connect that with embedded PID object
  // Way to influence system clock speed (tick for unix - timex)
  // Way to access current data (history and last difference)
  // Preferably time measurements done on a separate thread ?
};


#endif // CLOCKCONTROLLER_HPP