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
#include "utilities.hpp"

class ClockController
{
public:
  ClockController(char, double, long int = 500);
  ~ClockController();
  timex GetTimex();
  void AdjustClock(std::string);

  std::vector<std::size_t> tick_history, time_difference_history;
  std::atomic<std::chrono::system_clock::time_point> last_call;

private:
  timex GetSystemTimex();
  void SetSystemTimex(timex *);
  bool OperateOnTimex(timex *);
  void AdjustKernelTick(std::size_t);
  std::size_t CalculateClockDifference();

  timex m_timex;
  char m_clock_mode;
  std::size_t m_resolution_power;
  long int m_minimal_delay;
  std::size_t m_original_tick;
  std::unique_ptr<PID<double>> mp_pid;
};

#endif// CLOCKCONTROLLER_HPP