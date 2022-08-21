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

namespace TimeBox {

class ClockController
{
public:
  ClockController(double, char, std::shared_ptr<PID<double>>, long int = 500);
  ~ClockController();
  timex GetTimex() const;
  void AdjustClock(TimeboxReadout);
  std::vector<std::size_t> tick_history;
  std::vector<long> time_difference_history;
  std::chrono::system_clock::time_point last_call;

private:
  timex GetSystemTimex() const;
  void SetSystemTimex(timex *) const;
  bool OperateOnTimex(timex *) const;
  void AdjustKernelTick(std::size_t);
  std::size_t CalculateClockDifference();

  timex m_timex;
  char m_clock_mode;
  long int m_minimal_delay;
  std::size_t m_original_tick;
  std::size_t m_resolution_power;
  std::shared_ptr<PID<double>> mp_pid;
};

}// namespace TimeBox

#endif// CLOCKCONTROLLER_HPP