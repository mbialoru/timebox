#ifndef CLOCKCONTROLLER_HPP
#define CLOCKCONTROLLER_HPP

#pragma once

#include <sys/timex.h>

#include <boost/date_time.hpp>
#include <boost/log/trivial.hpp>
#include <chrono>
#include <ctime>
#include <future>
#include <memory>

#include "exceptions.hpp"
#include "pid.hpp"
#include "utils.hpp"

class ClockController
{
public:
  ClockController(char, double);
  ~ClockController();
  timex GetOriginalTimex();
  timex GetModifiedTimex();
  void AdjustClock(std::string);

  std::vector<std::size_t> tick_history, time_difference_history;
  std::atomic<std::chrono::system_clock::time_point> last_call;

private:
  timex GetSystemTimex();
  void SetSystemTimex(timex *);
  bool OperateOnTimex(timex *);
  std::size_t CalculateClockDifference();
  void AdjustKernelTick(std::size_t);
  std::size_t NormalizeTickValue(std::size_t);

  char m_clock_mode;
  std::size_t m_resolution_power;
  timex m_original, m_modified;
  std::unique_ptr<PID<double>> mp_pid;
};

#endif// CLOCKCONTROLLER_HPP