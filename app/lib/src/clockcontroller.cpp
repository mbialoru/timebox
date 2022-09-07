#include "clockcontroller.hpp"

using namespace TimeBox;

ClockController::ClockController(const std::size_t t_minimal_delay) noexcept
  : m_minimal_delay(t_minimal_delay), m_last_call(std::chrono::system_clock::now())
{}