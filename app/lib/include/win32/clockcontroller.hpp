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
  ClockController(std::size_t, std::shared_ptr<PID<double>>, double);
  ~ClockController();

  void adjust_clock(TimeboxReadout) override;

private:
  HRESULT update_process_token();

  void print_current_clock_adjustments() const;
  void system_time_adjustment_wrapper(long);

  DWORD m_current_adjustment_legacy{ 0UL };
  DWORD m_initial_adjustment_legacy{ 0UL };

  LARGE_INTEGER m_performance_counter_frequency{ 0L };

  static constexpr DWORD SM_MICRO_PER_SECOND{ 1000000UL };
  static constexpr DWORD SM_MINIMAL_ADJUSTMENT{ 0UL };

  std::shared_ptr<PID<double>> mp_pid;
};

}// namespace TimeBox

#endif// CLOCKCONTROLLER_HPP