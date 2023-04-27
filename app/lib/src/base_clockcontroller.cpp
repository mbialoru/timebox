#include "base_clockcontroller.hpp"

using namespace TimeBox;

BaseClockController::BaseClockController(const std::size_t t_minimal_delay) noexcept
  : m_minimal_delay(t_minimal_delay), m_last_call(std::chrono::system_clock::now())
{}

long BaseClockController::GetInitialAdjustment() const { return m_initial_adjustment; }

std::vector<std::chrono::system_clock::duration> BaseClockController::GetDifferenceHistory() const
{
  return m_difference_history;
}

std::vector<long> BaseClockController::GetAdjustmentHistory() const { return m_adjustment_history; }
