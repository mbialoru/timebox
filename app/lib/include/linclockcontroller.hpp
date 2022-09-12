#ifndef LINCLOCKCONTROLLER_HPP
#define LINCLOCKCONTROLLER_HPP

#pragma once

#include <boost/log/trivial.hpp>
#include <ctime>
#include <future>
#include <memory>
#include <sys/timex.h>

#include "clockcontroller.hpp"
#include "pid.hpp"

namespace TimeBox {

class LinClockController final : public ClockController
{
public:
  LinClockController(char, std::shared_ptr<PID<double>>, double, std::size_t = 500);
  ~LinClockController();
  void AdjustClock(TimeboxReadout) override;

private:
  timex GetTimex() const;
  timex GetSystemTimex() const;
  void SetSystemTimex(timex *) const;
  bool AdjtimexWrapper(timex *) const;
  void AdjustKernelTick(std::size_t);

  timex m_timex;
  char m_clock_mode;
  std::size_t m_original_tick;
  std::shared_ptr<PID<double>> mp_pid;
  std::vector<std::size_t> m_tick_history;
};

}// namespace TimeBox

#endif// LINCLOCKCONTROLLER_HPP