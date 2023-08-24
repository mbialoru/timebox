#ifndef CLOCKCONTROLLER_HPP
#define CLOCKCONTROLLER_HPP

#pragma once

#include <boost/log/trivial.hpp>
#include <limits>
#include <sysinfoapi.h>
#include <windows.h>

#include "base_clockcontroller.hpp"
#include "pid.hpp"

namespace TimeBox {

// NOTE: This class could be later expanded with precise variant for Windows10
class ClockController final : public BaseClockController
{
public:
  explicit ClockController(const std::size_t t_minimal_delay, std::shared_ptr<PID<double>> t_pid);
  ~ClockController();

  void adjust_clock(const TimeboxReadout t_readout) override;

private:
  HRESULT update_process_token();

  void print_current_clock_adjustments() const;
  void system_time_adjustment_wrapper(const long t_adjustment);

  DWORD m_current_adjustment_legacy{ 0UL };
  DWORD m_initial_adjustment_legacy{ 0UL };
  DWORD m_lowest_adjustment_legacy{ 0UL };
  DWORD m_highest_adjustment_legacy{ 0UL };

  LARGE_INTEGER m_performance_counter_frequency{ 0L };

  std::shared_ptr<PID<double>> mp_pid;
};

}// namespace TimeBox

#endif// CLOCKCONTROLLER_HPP