#ifndef WINCLOCKCONTROLLER_HPP
#define WINCLOCKCONTROLLER_HPP

#pragma once

#include <boost/log/trivial.hpp>
#include <limits>
#include <sysinfoapi.h>
#include <windows.h>

#include "clockcontroller.hpp"
#include "pid.hpp"

namespace TimeBox {

// NOTE: This class could be later expanded with precise variant for Windows10
class WinClockController final : public ClockController
{
public:
  WinClockController(std::size_t, std::shared_ptr<PID<double>>, double);
  ~WinClockController();

  void AdjustClock(TimeboxReadout) override;

private:
  HRESULT UpdateProcessTokenPrivileges();
  void SystemTimeAdjustmentWrapper(long);
  void PrintCurrentClockAdjustments() const;

  DWORD m_initial_adjustment_legacy{ 0UL };
  DWORD m_current_adjustment_legacy{ 0UL };
  LARGE_INTEGER m_performance_counter_frequency{ 0L };
  static constexpr DWORD m_min_adjustment{ 0UL };
  static constexpr DWORD m_micro_per_second{ 1000000UL };
  std::shared_ptr<PID<double>> mp_pid;
  std::vector<long> m_adjustment_history;
};

}// namespace TimeBox

#endif// WINCLOCKCONTROLLER_HPP