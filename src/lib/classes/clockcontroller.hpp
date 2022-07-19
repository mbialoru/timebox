#ifndef CLOCKCONTROLLER_HPP
#define CLOCKCONTROLLER_HPP

#pragma once

#include <ctime>
#include <chrono>
#include <memory>
#include <sys/timex.h>
#include <boost/date_time.hpp>

#include "pid.hpp"
#include "exceptions.hpp"

class ClockController
{
public:
  ClockController(char);
  ~ClockController();
  void Trigger(std::string);

private:
  char clock_mode;
  timex original, modified;
  std::unique_ptr<PID<double>> pid;

  void AdjustKernelTick(unsigned);
  double ClockDifference(std::string,
    std::chrono::time_point<std::chrono::system_clock>);

  // TODO: Some container to store differences in PPS time and system clock time
  // Way to calculate these differences
  // Way to connect that with embedded PID object
  // Way to influence system clock speed (tick for unix - timex)
  // Way to access current data (history and last difference)
  // Preferably time measurements done on a separate thread ?
  // No, we will make a trigger method, which will do all the work
};


#endif // CLOCKCONTROLLER_HPP