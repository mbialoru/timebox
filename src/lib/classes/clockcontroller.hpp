#ifndef CLOCKCONTROLLER_HPP
#define CLOCKCONTROLLER_HPP

#pragma once

#include <ctime>
#include <memory>
#include <sys/timex.h>
#include "pid.hpp"
#include "exceptions.hpp"

class ClockController
{
public:
  ClockController();
  ~ClockController() = default;
  void Trigger(char*);

private:
  int clock_mode;
  timex buffer;
  struct tm curr { 0 };
  struct tm last { 0 };
  std::unique_ptr<PID<double>> pid;
  void AdjustKernelTick(unsigned);


  // TODO: Some container to store differences in PPS time and system clock time
  // Way to calculate these differences
  // Way to connect that with embedded PID object
  // Way to influence system clock speed (tick for unix - timex)
  // Way to access current data (history and last difference)
  // Preferably time measurements done on a separate thread ?
  // No, we will make a trigger method, which will do all the work
};


#endif // CLOCKCONTROLLER_HPP