#ifndef CLOCKCONTROLLER_HPP
#define CLOCKCONTROLLER_HPP

#pragma once

#include <chrono>

#include "exceptions.hpp"
#include "utilities.hpp"

namespace TimeBox {

class ClockController
{
public:
  ClockController(std::size_t);
  virtual ~ClockController() = default;
  virtual void AdjustClock(TimeboxReadout) = 0;
  virtual std::chrono::system_clock::duration CalculateClockDifference() = 0;

protected:
  std::size_t m_minimal_delay;
  std::chrono::system_clock::time_point m_last_call;
  std::vector<std::chrono::system_clock::duration> time_difference_history;
};

}// namespace TimeBox

#endif// CLOCKCONTROLLER_HPP