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

  virtual void adjust_clock(TimeboxReadout) = 0;

  long get_initial_adjustment() const;

  std::vector<long> get_adjustment_history() const;

  std::vector<std::chrono::system_clock::duration> get_difference_history() const;

protected:
  long m_initial_adjustment;

  std::chrono::system_clock::time_point m_last_call;

  std::size_t m_minimal_delay;

  std::vector<long> m_adjustment_history;

  std::vector<std::chrono::system_clock::duration> m_difference_history;
};

}// namespace TimeBox

#endif// BASE_CLOCKCONTROLLER_HPP