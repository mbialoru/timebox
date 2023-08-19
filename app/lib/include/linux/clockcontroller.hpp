#ifndef CLOCKCONTROLLER_HPP
#define CLOCKCONTROLLER_HPP

#pragma once

#include <boost/log/trivial.hpp>
#include <ctime>
#include <future>
#include <memory>
#include <sys/timex.h>

#include "base_clockcontroller.hpp"
#include "pid.hpp"

namespace TimeBox {

class ClockController final : public BaseClockController
{
public:
  ClockController(char, std::shared_ptr<PID<double>>, double, std::size_t = 500);
  ~ClockController();

  void adjust_clock(TimeboxReadout) override;

private:
  bool adjtimex_wrapper(timex*) const;

  timex get_system_timex() const;
  timex get_timex() const;

  void adjust_kernel_tick(std::size_t);
  void set_system_timex(timex *) const;

  timex m_timex;

  char m_clock_mode;

  std::size_t m_original_tick;

  std::shared_ptr<PID<double>> mp_pid;

  std::vector<std::size_t> m_tick_history;
};

}// namespace TimeBox

#endif// CLOCKCONTROLLER_HPP