#ifndef BASE_CLOCKCONTROLLER_HPP
#define BASE_CLOCKCONTROLLER_HPP

#pragma once

#include <chrono>

#include "utilities.hpp"

namespace TimeBox {

class BaseClockController
{
public:
  BaseClockController(std::size_t) noexcept;
  virtual ~BaseClockController() = default;
  virtual void AdjustClock(TimeboxReadout) = 0;
  long GetInitialAdjustment() const;
  std::vector<std::chrono::system_clock::duration> GetDifferenceHistory() const;
  std::vector<long> GetAdjustmentHistory() const;

protected:
  long m_initial_adjustment;
  std::size_t m_minimal_delay;
  std::chrono::system_clock::time_point m_last_call;
  std::vector<std::chrono::system_clock::duration> m_difference_history;
  std::vector<long> m_adjustment_history;
};

}// namespace TimeBox

#endif// BASE_CLOCKCONTROLLER_HPP