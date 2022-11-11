#ifndef WINCLOCKCONTROLLER_HPP
#define WINCLOCKCONTROLLER_HPP

#pragma once

#include <boost/log/trivial.hpp>
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
  void GetInitialParameters() const;
  void SystemTimeAdjustmentWrapper(long);
  void LogCurrentClockAdjustments();

  DWORD m_initial_adjustment_legacy{ 0 };
  DWORD m_current_adjustment_legacy{ 0 };
  LARGE_INTEGER m_performance_counter_frequency{ 0 };
  static constexpr DWORD m_micro_per_second{ 1000000 };
  std::shared_ptr<PID<double>> mp_pid;
  std::vector<long> m_adjustment_history;
};

}// namespace TimeBox

#endif// WINCLOCKCONTROLLER_HPP