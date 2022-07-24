#ifndef CLOCKCONTROLLER_HPP
#define CLOCKCONTROLLER_HPP

#pragma once

#include <ctime>
#include <chrono>
#include <memory>
#include <future>
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
  std::atomic<std::chrono::system_clock::time_point> last_call;

private:
  char clock_mode;
  short resolution_power;
  timex original, modified;
  std::unique_ptr<PID<double>> pid;

  timex GetTimex();
  bool SetTimex(timex*);
  std::size_t ClockDifference();
  void AdjustKernelTick(unsigned);
  short NormalizeTickValue(short);
};


#endif // CLOCKCONTROLLER_HPP