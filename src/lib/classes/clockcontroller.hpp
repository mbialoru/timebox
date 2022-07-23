#ifndef CLOCKCONTROLLER_HPP
#define CLOCKCONTROLLER_HPP

#pragma once

#include <ctime>
#include <chrono>
#include <memory>
#include <sys/timex.h>
#include <boost/date_time.hpp>
#include <boost/log/trivial.hpp>

#include "pid.hpp"
#include "exceptions.hpp"

class ClockController
{
public:
  ClockController(char, double);
  ~ClockController();
  void AdjustKernelTick(unsigned);

private:
  char clock_mode;
  short resolution_power;
  timex original, modified;
  std::unique_ptr<PID<double>> pid;

  short NormalizeTickValue(short);

  // TODO: Some container to store differences in PPS time and system clock time
  // Way to calculate these differences
  // Way to connect that with embedded PID object
  // Way to influence system clock speed (tick for unix - timex)
  // Way to access current data (history and last difference)
  // Preferably time measurements done on a separate thread ?
  // No, we will make a trigger method, which will do all the work
};


#endif // CLOCKCONTROLLER_HPP