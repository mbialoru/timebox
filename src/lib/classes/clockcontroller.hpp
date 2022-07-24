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
#include "utils.hpp"
#include "exceptions.hpp"

class ClockController
{
public:
  ClockController(char, double);
  ~ClockController();
  void AdjustClock(std::string);
  std::vector<std::size_t> tick_history, timediff_history;

private:
  char clock_mode;
  short resolution_power;
  timex original, modified;
  std::unique_ptr<PID<double>> pid;

  std::size_t ClockDifference();
  void AdjustKernelTick(unsigned);
  short NormalizeTickValue(short);
};


#endif // CLOCKCONTROLLER_HPP