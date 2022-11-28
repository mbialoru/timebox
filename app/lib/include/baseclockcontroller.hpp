#ifndef BASECLOCKCONTROLLER_HPP
#define BASECLOCKCONTROLLER_HPP

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

protected:
  std::size_t m_minimal_delay;
  std::chrono::system_clock::time_point m_last_call;
  std::vector<std::chrono::system_clock::duration> time_difference_history;
};

}// namespace TimeBox

#endif// BASECLOCKCONTROLLER_HPP